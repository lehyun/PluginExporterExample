#include "V8PCH.h"

PRAGMA_DISABLE_SHADOW_VARIABLE_WARNINGS

#if !PLATFORM_LINUX
#include "JavascriptIsolate.h"
#include "JavascriptContext.h"
#include "SocketSubSystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "NetworkMessage.h"
#include "Misc/DefaultValueHelper.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#endif
#include <thread>
#if PLATFORM_WINDOWS
#include "HideWindowsPlatformTypes.h"
#endif

#define DEBUG_V8_DEBUGGER 1

#include "Translator.h"
#endif

#if V8_MAJOR_VERSION == 5 && V8_MINOR_VERSION < 5
#	define V8_Debug_ProcessDebugMessages(isolate) Debug::ProcessDebugMessages()
#	define V8_Debug_SetMessageHandler(isolate,fn) Debug::SetMessageHandler(fn)
#else
#	define V8_Debug_ProcessDebugMessages(isolate) Debug::ProcessDebugMessages(isolate)
#	define V8_Debug_SetMessageHandler(isolate,fn) Debug::SetMessageHandler(isolate,fn)
#endif

using namespace v8;

#if WITH_EDITOR && !PLATFORM_LINUX
#include "TickableEditorObject.h"

namespace 
{
	//@HACK : to be multi-threaded and support multiple sockets
	FSocket* MainSocket{ nullptr };

	void RegisterDebuggerSocket(FSocket* Socket)
	{
		MainSocket = Socket;
	}

	void UnregisterDebuggerSocket(FSocket* Socket)
	{
		MainSocket = nullptr;
	}

	void SendTo(FSocket* Socket_, const char* msg)
	{
		FSimpleAbstractSocket_FSocket Socket(Socket_);

		auto send = [&](const char* msg) {
			Socket.Send(reinterpret_cast<const uint8*>(msg), strlen(msg));
		};

		send(TCHAR_TO_UTF8(*FString::Printf(TEXT("Content-Length: %d\r\n"), strlen(msg))));
		send("\r\n");
		send(msg);

#if DEBUG_V8_DEBUGGER
		UE_LOG(Javascript, Log, TEXT("Reply : %s"), UTF8_TO_TCHAR(msg));
#endif
	}

	void Broadcast(const char* msg)
	{
		if (MainSocket)
		{
			SendTo(MainSocket,msg);
		}
#if DEBUG_V8_DEBUGGER
		UE_LOG(Javascript, Log, TEXT("Broadcast : %s"), UTF8_TO_TCHAR(msg));
#endif
	}
}

// deprecated
class FDebugger : public IJavascriptDebugger, public FTickableEditorObject
{
public:
	struct FClientData : public Debug::ClientData
	{
		FClientData(FDebugger* InDebugger, FSocket* InSocket)
		: Debugger(InDebugger), Socket_(InSocket)
		{
			Debugger->Busy.Increment();
		}

		~FClientData()
		{
			Debugger->Busy.Decrement();
		}

		FDebugger* Debugger;
		FSocket* Socket_;

		void Send(const char* msg)
		{
			SendTo(Socket_, msg);
		}
	};

	std::thread thread;
	Isolate* isolate_;
	Persistent<Context> context_;

	Local<Context> context() { return Local<Context>::New(isolate_, context_); }

	virtual void Tick(float DeltaTime) override
	{
		Isolate::Scope isolate_scope(isolate_);
		HandleScope handle_scope(isolate_);
		Context::Scope debug_scope(context());

		V8_Debug_ProcessDebugMessages(isolate_);
	}

	virtual bool IsTickable() const override
	{
		return true;
	}

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FV8Debugger, STATGROUP_Tickables);
	}	

	FDebugger(int32 InPort, Local<Context> context)
	{
		isolate_ = context->GetIsolate();
		context_.Reset(isolate_,context);

		StopRequested.Reset();
		StopAck.Reset();

		Install();

		thread = std::thread([this, InPort]{Main(InPort); });				
	}

	static FString DumpJSON(Handle<Value> value,int32 depth = 1) 
	{
		if (depth <= 0) return TEXT("<deep>");

		if (value.IsEmpty()) return TEXT("(empty)");
		if (!value->IsObject()) return StringFromV8(value);
		auto Obj = value.As<Object>();
		auto Names = Obj->GetOwnPropertyNames();
		auto len = Names->Length();
		TArray<FString> values;
		for (decltype(len) Index = 0; Index < len; ++Index)
		{
			auto Name = Names->Get(Index);
			auto Value = Obj->Get(Name);
			values.Add(FString::Printf(TEXT("%s : %s"), *StringFromV8(Name), *DumpJSON(Value,depth-1)));
		}
		return FString::Printf(TEXT("{%s}"), *FString::Join(values,TEXT(",")));
	}


	void Install()
	{
		V8_Debug_SetMessageHandler(isolate_,[](const Debug::Message& message){
			FClientData* data = static_cast<FClientData*>(message.GetClientData());
			auto json = message.GetJSON();

			UE_LOG(Javascript, Log, TEXT("V8 message : %s %s"), message.IsEvent() ? TEXT("(event)") : TEXT(""), message.IsResponse() ? TEXT("(response)") : TEXT("") );	

			if (json.IsEmpty())
			{
				UE_LOG(Javascript, Error, TEXT("Not a json"));
			}
			else
			{
				String::Utf8Value utf8(json);
				auto str = *utf8;

				if (data)
				{
					data->Send(str);
				}
				else
				{
					Broadcast(str);
				}
			}
		});

#if DEBUG_V8_DEBUGGER
		// HL: 이슈1: 첫번째 파라미터 isolate_ 빠졌음
		Debug::SetDebugEventListener(isolate_,[](const v8::Debug::EventDetails& event_details) {
			static const TCHAR* EventTypes[] = { 
				TEXT("Break"), 
				TEXT("Exception"), 
				TEXT("NewFunction"), 
				TEXT("BeforeCompile"), 
				TEXT("AfterCompile"), 
				TEXT("CompileError"), 
				TEXT("PromiseEvent"), 
				TEXT("AsyncTaskEvent") 
			};
			UE_LOG(Javascript, Log, TEXT("DebugEvent Event(%s) State(%s) Data(%s)"), 
				EventTypes[event_details.GetEvent()],
				*DumpJSON(event_details.GetExecutionState()),
				*DumpJSON(event_details.GetEventData())
				);
		});
#endif
	}

	void Uninstall()
	{		
		Context::Scope context_scope(context());
		V8_Debug_SetMessageHandler(isolate_,nullptr);
	}

	~FDebugger()
	{
		Uninstall();

		context_.Reset();
	}

	virtual void Destroy() override
	{
		Stop();		

		delete this;
	}

	void Stop()
	{
		StopRequested.Set(true);

		if (WorkingOn)
		{
			WorkingOn->Close();
		}

		while (!StopAck.GetValue())
		{
			V8_Debug_ProcessDebugMessages(isolate_);
		}

		thread.join();
	}	

	FSocket* WorkingOn{ nullptr };
	
	bool Main(int32 Port)
	{
		auto SocketSubsystem = ISocketSubsystem::Get();
		if (!SocketSubsystem) return ReportError("No socket subsystem");

		auto Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("V8 debugger"));
		if (!Socket) return ReportError("Failed to create socket");

		auto Cleanup_Socket = [&]{Socket->Close(); SocketSubsystem->DestroySocket(Socket); };
		
		// listen on any IP address
		auto ListenAddr = SocketSubsystem->GetLocalBindAddr(*GLog);

		ListenAddr->SetPort(Port);
		Socket->SetReuseAddr();

		// bind to the address
		if (!Socket->Bind(*ListenAddr)) return ReportError("Failed to bind socket", Cleanup_Socket );

		if (!Socket->Listen(16)) return ReportError("Failed to listen", Cleanup_Socket );
				
		int32 port = Socket->GetPortNo();
		check((Port == 0 && port != 0) || port == Port);

		ListenAddr->SetPort(port);		

		while (!StopRequested.GetValue())
		{
			bool bReadReady = false;

			// check for incoming connections
			if (Socket->HasPendingConnection(bReadReady) && bReadReady)
			{
				auto ClientSocket = Socket->Accept(TEXT("Remote V8 connection"));

				if (!ClientSocket) continue;				

				RegisterDebuggerSocket(ClientSocket);

				UE_LOG(Javascript, Log, TEXT("V8 Debugger session start"));

				WorkingOn = ClientSocket;

				Logic(ClientSocket);

				WorkingOn = nullptr;

				// waiting for commands to be processed.
				WaitForPendingCommands();

				UE_LOG(Javascript, Log, TEXT("V8 Debugger session stop"));

				UnregisterDebuggerSocket(ClientSocket);
				
				// HL: 이슈2: 응답을 보낼 충분한 시간을 기다리지 않고 바로 소켓을 닫는 이슈
				ClientSocket->Close();
				SocketSubsystem->DestroySocket(ClientSocket);
			}
			else
			{
				FPlatformProcess::Sleep(0.25f);
			}		
		}

		Cleanup_Socket();		

		StopAck.Set(true);

		return true;
	}

	bool WaitForPendingCommands(int32 MaxCount = 5)
	{
		int32 Count = 0;
		int32 Remain = Busy.GetValue();
		while (Remain != 0 && Count < MaxCount)
		{
#if DEBUG_V8_DEBUGGER
			UE_LOG(Javascript, Display, TEXT("Waiting for commands to be processed: %d"), Busy.GetValue());
#endif
			FPlatformProcess::Sleep(0.25f);

			if (Remain != Busy.GetValue()) 
			{
				Count = 0;
				Remain = Busy.GetValue();
			}
			else Count++;
		}
		return Count < MaxCount;
	}
	bool WaitForIncomingData(FSocket* InSocket, int32 MaxCount = 5)
	{
		int32 Count = 0;
		uint32 PendingDataSize = 0;
		while (!InSocket->HasPendingData(PendingDataSize) && Count < MaxCount)
		{
#if DEBUG_V8_DEBUGGER
			UE_LOG(Javascript, Display, TEXT("Waiting for incoming data"));
#endif
			FPlatformProcess::Sleep(0.25f);
			Count++;
		}
		return Count < MaxCount;
	}

	bool Logic(FSocket* InSocket)
	{
		FSimpleAbstractSocket_FSocket Socket(InSocket);

		auto send = [&](const char* msg) {
			Socket.Send(reinterpret_cast<const uint8*>(msg), strlen(msg));
		};

		auto read_line = [&](FString& line) {
			const auto buffer_size = 1024;
			ANSICHAR buffer[buffer_size];
			for (int pos = 0; pos < buffer_size; pos++)			
			{
				char ch;
				if (!Socket.Receive(reinterpret_cast<uint8*>(&ch), 1)) return false;

				if (pos && buffer[pos - 1] == '\r' && ch == '\n')
				{
					buffer[pos - 1] = '\0';
					line = UTF8_TO_TCHAR(buffer);
					return true;
				}
				else if (ch == '\n')
				{
					buffer[pos] = '\0';
					line = UTF8_TO_TCHAR(buffer);
					return true;
				}
				buffer[pos] = ch;				
			}
			return false;
		};

		auto command = [&](const FString& request) {			
			Debug::SendCommand(isolate_, (uint16_t*)*request, request.Len(), new FClientData(this,InSocket));
		};

		auto process = [&](const FString& content) {
#if DEBUG_V8_DEBUGGER
			UE_LOG(Javascript, Display, TEXT("Received: %s"), *content);
#endif
			command(content);
			return content.Find(TEXT("\"type\":\"request\",\"command\":\"disconnect\"}")) == INDEX_NONE;
		};

		auto bye = [&] {
			command(TEXT("{\"seq\":1,\"type:\":\"request\",\"command\":\"disconnect\"}"));
		};

		const uint32 PROTOCOL_VERSION = 1;

		if (!WaitForIncomingData(InSocket)) return ReportError("timeout");

		FString Line;
		bool Cont = read_line(Line);
		if (!Cont) return ReportError("failed to read data");
		if (Line.IsEmpty()) return true;	// exit

		if (Line.Contains("GET"))
		{
#if DEBUG_V8_DEBUGGER
			UE_LOG(Javascript, Display, TEXT("GET >>>"));
#endif
			FString Router;
			TArray<FString> Words;
			if (Line.ParseIntoArray(Words, TEXT(" ")) > 1)
				Router = Words[1];

			while (Cont)
			{
#if DEBUG_V8_DEBUGGER
				UE_LOG(Javascript, Display, TEXT("Header: %s"), *Line);
#endif
				// read next line
				Cont = read_line(Line) && !Line.IsEmpty();
			}

#if DEBUG_V8_DEBUGGER
			UE_LOG(Javascript, Display, TEXT("Header End."));
#endif

			if (!Router.IsEmpty())
			{
				if (Router == "/json/version")
				{
					FString Header;
					FString Body;

					Body.Append(FString::Printf(TEXT("{\"V8-Version\":\"%s\",\"Protocol-Version\":%d,\"Embedding-Host\":\"UnrealEngine\"}"), ANSI_TO_TCHAR(v8::V8::GetVersion()), PROTOCOL_VERSION));

					Header.Append(TEXT("HTTP/1.1 200 OK\r\n"));
					Header.Append(FString::Printf(TEXT("Content-Length: %d\r\n"), Body.Len()));
					Header.Append(TEXT("Content-Type: application/json\r\n"));
					Header.Append(TEXT("Connection: closed\r\n"));
					Header.Append(TEXT("\r\n"));

					send(TCHAR_TO_UTF8(*Header));
					send(TCHAR_TO_UTF8(*Body));
				}
			}
		}
		else 
		{
#if DEBUG_V8_DEBUGGER
			UE_LOG(Javascript, Display, TEXT("CMD >>>"));
#endif

			send("Type: connect\r\n");
			send("V8-Version: ");
			send(v8::V8::GetVersion());
			send("\r\n");
			send("Protocol-Version: 1\r\n");
			send("Embedding-Host: UnrealEngine\r\n");
			send("Content-Length: 0\r\n");
			send("\r\n");

			int32 ContentLength = -1;
			while (!StopRequested.GetValue() && Cont)
			{
#if DEBUG_V8_DEBUGGER
				UE_LOG(Javascript, Display, TEXT("Header: %s"), *Line);
#endif

				FString Left, Right;
				if (Line.Split(TEXT(":"), &Left, &Right))
				{
					// VSCode 1.23.1 버그대응
					if (Left.Contains(TEXT("Content-Length")))
					//if (Left == TEXT("Content-Length"))
					{
						if (!FDefaultValueHelper::ParseInt(Right, ContentLength)) return ReportError("invalid content length");
					}
				}
				else
				{
#if DEBUG_V8_DEBUGGER
					UE_LOG(Javascript, Display, TEXT("Header End."));
#endif

					if (ContentLength <= 0) return ReportError("invalid content length < 0");
					if (!WaitForIncomingData(InSocket)) return ReportError("timeout");

					char* Buffer = new char[ContentLength + 1];
					if (!Buffer) break;

					if (!Socket.Receive(reinterpret_cast<uint8*>(Buffer), ContentLength)) break;

					Buffer[ContentLength] = '\0';

					// VSCode 1.23.1 버그대응
					if (Buffer[ContentLength - 1] == '"')
						Buffer[ContentLength - 1] = '\0';
					
					if (!process(Buffer)) return ReportError("failed to process");
				}

				if (!WaitForIncomingData(InSocket)) return ReportError("timeout");
				// read next line
				Cont = read_line(Line);
			}

			bye();
		}
		
		return true;
	}	

	bool ReportError(const char* Error)
	{
		UE_LOG(Javascript, Error, TEXT("%s"), UTF8_TO_TCHAR(Error));

		// always false
		return false;
	}

	template <typename Fn>
	bool ReportError(const char* Error, Fn&& fn)
	{		
		fn();
		return ReportError(Error);
	}

	FThreadSafeCounter StopAck;
	FThreadSafeCounter StopRequested;	
	FThreadSafeCounter Busy;
};

IJavascriptDebugger* IJavascriptDebugger::Create(int32 InPort,Local<Context> InContext)
{
	return new FDebugger(InPort,InContext);
}
#else
IJavascriptDebugger* IJavascriptDebugger::Create(int32 InPort, Local<Context> InContext)
{
	return nullptr;
} 
#endif

PRAGMA_ENABLE_SHADOW_VARIABLE_WARNINGS

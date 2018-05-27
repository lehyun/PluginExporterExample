#include "JavascriptWebSocketModule.h"

class FJavascripWebSocketModule : public IJavascriptWebSocketModule
{
	// Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface

};


void FJavascripWebSocketModule::StartupModule()
{
	FName NAME_JavascriptCmd("JavascriptCmd");
	GLog->Log(NAME_JavascriptCmd, ELogVerbosity::Log, TEXT("JavascripWebSocketModule started"));
}


void FJavascripWebSocketModule::ShutdownModule()
{

}

IMPLEMENT_MODULE(FJavascripWebSocketModule, JavascriptWebSocket);
DEFINE_LOG_CATEGORY(LogWebsocket);

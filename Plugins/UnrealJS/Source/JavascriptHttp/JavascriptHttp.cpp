#include "IJavascriptHttpModule.h"
#include "Misc/OutputDeviceRedirector.h"

class FJavascripHttpModule : public IJavascriptHttpModule
{
	// Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface

};


void FJavascripHttpModule::StartupModule()
{
	FName NAME_JavascriptCmd("JavascriptCmd");
	GLog->Log(NAME_JavascriptCmd, ELogVerbosity::Log, TEXT("JavascripHttpModule started"));
}


void FJavascripHttpModule::ShutdownModule()
{

}

IMPLEMENT_MODULE(FJavascripHttpModule, JavascriptHttp);
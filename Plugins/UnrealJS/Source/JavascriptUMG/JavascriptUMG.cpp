#include "IJavascriptUMGModule.h"
#include "Misc/OutputDeviceRedirector.h"

class FJavascripUMGModule : public IJavascriptUMGModule
{
	// Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End IModuleInterface

};


void FJavascripUMGModule::StartupModule()
{
	FName NAME_JavascriptCmd("JavascriptCmd");
	GLog->Log(NAME_JavascriptCmd, ELogVerbosity::Log, TEXT("JavascripUMGModule started"));
}


void FJavascripUMGModule::ShutdownModule()
{

}

IMPLEMENT_MODULE(FJavascripUMGModule, JavascriptUMG);
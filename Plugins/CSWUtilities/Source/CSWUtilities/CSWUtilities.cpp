// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CSWUtilities.h"

/* USE THIS IF DEVELOPING A PLUGIN */
#define LOCTEXT_NAMESPACE "FCSWUtilitiesModule"

void FCSWUtilitiesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FCSWUtilitiesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSWUtilitiesModule, CSWUtilities)

/* USE THIS IF DEVELOPING A GAME MODULE*/
/*
// 1. FDefaultGameModuleImpl과 FDefaultModuleImpl의 차이는 무엇일까?
IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, CSWUtilities, "CSWUtilities");
*/
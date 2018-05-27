// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

/* USE THIS IF DEVELOPING A PLUGIN */
#include "CoreMinimal.h"
#include "ModuleManager.h"


class FCSWUtilitiesModule : public IModuleInterface
{
public:

	// IModuleInterface implementation 
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

/* USE THIS IF DEVELOPING A GAME MODULE */
/*
#include "ModuleManager.h"
*/
// Fill out your copyright notice in the Description page of Project Settings.

#include "CSWBlueprintUtilities.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "Json.h"
#include "JsonObjectConverter.h"

void UCSWBlueprintUtilities::GetAllActorsWithComponent(
	const UObject* WorldContextObject,
	const TSubclassOf<UActorComponent> Component,
	TArray<AActor*>& OutActors,
	TArray<UActorComponent*>& OutActorComponents)
{
	if (!Component || !WorldContextObject) return;

	OutActors.Empty();
	OutActorComponents.Empty();

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (World)
	{
		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			UActorComponent* ActorComponent = Actor->GetComponentByClass(Component);
			if (Actor->IsValidLowLevelFast() && !Actor->IsPendingKill() && ActorComponent)
			{
				OutActors.Add(Actor);
				OutActorComponents.Add(ActorComponent);
			}
		}
	}
}

int32 UCSWBlueprintUtilities::Plus(int32 A, int32 B)
{
	return A + B;
}

int32 UCSWBlueprintUtilities::Minus(int32 A, int32 B)
{
	return A - B;
}

bool UCSWBlueprintUtilities::GetHyunLee(FHyunLee& OutHyunLee)
{
	OutHyunLee.myInt = 2;
	OutHyunLee.myFloat = 2.22;
	OutHyunLee.myString = TEXT("Hyun Lee");
	return true;
}

FString UCSWBlueprintUtilities::HyunLeeToJsonString(const FHyunLee& InHyunLee)
{
	FString OutJson;
	bool result = FJsonObjectConverter::UStructToJsonObjectString(FHyunLee::StaticStruct(), &InHyunLee, OutJson, 0, 0);
	//if (result) { UE_LOG(LogClass, Display, TEXT("%s"), *OutJson); }
	//else { UE_LOG(LogClass, Error, TEXT("HyunLeeToJsonString() failed.")); }
	return OutJson;
}
bool UCSWBlueprintUtilities::HyunLeeToJsonString_v2(const FHyunLee& InHyunLee, FString& OutJson, int32& OutInt, bool& OutBool)
{
	OutInt = 10;
	OutBool = true;
	bool result = FJsonObjectConverter::UStructToJsonObjectString(FHyunLee::StaticStruct(), &InHyunLee, OutJson, 0, 0);
	//if (result) { UE_LOG(LogClass, Display, TEXT("%s"), *OutJson); }
	//else { UE_LOG(LogClass, Error, TEXT("HyunLeeToJsonString() failed.")); }
	return result;
}

bool UCSWBlueprintUtilities::HyunLeeToJsonString_v3(const FHyunLee& InHyunLee, FReturnParam& Out)
{
	Out.OutFloat = 1982.02;
	Out.OutInt = 10;
	Out.OutBool = true;
	return FJsonObjectConverter::UStructToJsonObjectString(FHyunLee::StaticStruct(), &InHyunLee, Out.OutString, 0, 0);
}

bool UCSWBlueprintUtilities::HyunLeeToJsonString_v4(const FHyunLee& InHyunLee, TArray<FReturnParam>& Out)
{
	FReturnParam Data;
	Data.OutFloat = 1982.02;
	Data.OutInt = 10;
	Data.OutBool = FJsonObjectConverter::UStructToJsonObjectString(FHyunLee::StaticStruct(), &InHyunLee, Data.OutString, 0, 0);
	Out.Add(Data);
	return Data.OutBool;
}

bool UCSWBlueprintUtilities::GetVector(FVector2D& OutVector)
{
	OutVector.X = 1.0f;
	OutVector.Y = 1.0f;
	return true;
}
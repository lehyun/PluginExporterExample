// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSWBlueprintUtilities.generated.h"


// 1. 핫 리로드 가능. 하지만, Unload / Reload 비활성화. 
// UObject(UBlueprintFunctionLibrary)를 상속받은 클래스를 포함하기 때문.
UCLASS()
class CSWUTILITIES_API UCSWBlueprintUtilities : public UBlueprintFunctionLibrary
// 2. 핫 리로드 가능. Unload / Reload 버튼도 활성화.
//class CSWUTILITIES_API UCSWBlueprintUtilities
{
	GENERATED_BODY()
	
	
public:
	/**
	* Find all Actors in the world with the specified component
	* This is a slow operation, use with caution e.g. do not use every frame
	* @param Component			Component to find. Must be specified or result array will be empty.
	* @param OutActors			Output array of Actors that have specified component.
	* @param OutActorComponent	Output array of Actor Components that belongs to the array of actors
	*/
	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary", meta = (DisplayName="CSW: Get All Actors With Components", WorldContext="WorldContextObject", DeterminesOutputType = "Component", DynamicOutputParam = "OutActorComponents"))
	static void GetAllActorsWithComponent(const UObject* WorldContextObject,
		const TSubclassOf<UActorComponent> Component,
		TArray<AActor*>& OutActors, 
		TArray<UActorComponent*>& OutActorComponents);

	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary", meta = (DisplayName = "CSW: Plus"))
	static int32 Plus(int32 A, int32 B);

	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary", meta = (DisplayName = "CSW: Minus"))
	static int32 Minus(int32 A, int32 B);
};

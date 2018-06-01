// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CSWBlueprintUtilities.generated.h"

USTRUCT(BlueprintType)
struct FHyunLee
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	float myFloat;
	UPROPERTY()
	int myInt;
	UPROPERTY()
	FString myString;

public:

	/** Default constructor (no initialization). */
	FORCEINLINE FHyunLee() { }
};

USTRUCT(BlueprintType)
struct FReturnParam
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float OutFloat;
	UPROPERTY()
	int OutInt;
	UPROPERTY()
	FString OutString;
	UPROPERTY()
	bool OutBool;

public:

	/** Default constructor (no initialization). */
	FORCEINLINE FReturnParam() { }
};


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

	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary")
	static bool GetHyunLee(FHyunLee& OutHyunLee);

	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary")
	static FString HyunLeeToJsonString(const FHyunLee& InHyunLee);

	// USTRUCT 타입만 Ref로 가능하게 되어 있으므로 동작하지 않는다.
	// 참고: static Local<Value> CallFunction(Isolate* isolate, Local<Value> self, UFunction* Function, UObject* Object, Fn&& GetArg)
	// 항상 컨테이너가 필요한건가??
	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary")
	static bool HyunLeeToJsonString_v2(const FHyunLee& InHyunLee, FString& OutJson, int32& OutInt, bool& OutBool);

	// 컨테이너에 쌓여있는건 잘되는거 확인
	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary")
	static bool HyunLeeToJsonString_v3(const FHyunLee& InHyunLee, FReturnParam& Out);

	// 배열은?
	// 역시나 동작하지 않음
	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary")
	static bool HyunLeeToJsonString_v4(const FHyunLee& InHyunLee, TArray<FReturnParam>& Out);

	// 엔진 구조체는 USTRUCT가 없어도 프로퍼티를 가지는듯. ue.d.ts에도 자동으로 포함됨.
	// 역시나 동작함.
	UFUNCTION(BlueprintPure, Category = "CSW|Utilities|BlueprintLibrary")
	static bool GetVector(FVector2D& OutVector);
};

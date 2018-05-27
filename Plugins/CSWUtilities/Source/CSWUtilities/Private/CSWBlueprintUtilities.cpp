// Fill out your copyright notice in the Description page of Project Settings.

#include "CSWBlueprintUtilities.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

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

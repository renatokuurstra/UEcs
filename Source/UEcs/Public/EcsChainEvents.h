// Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EcsSystem.h"
#include "EcsEventElement.h"
#include "EcsChainEvents.generated.h"


USTRUCT(BlueprintType)
struct FChainEventData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|Chain Events")
	bool bIsUpdateSystems = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|Chain Events", meta = (EditCondition = "bIsUpdateSystems"))
	float UpdateFreqSec = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|Chain Events")
	TArray<TScriptInterface<IEcsEventElement>> Elements;
};

/**
 * Standard ECS Chain Event Names
 */
struct UECS_API FEcsChainEventNames
{
	static const FName BeginPlay;
	static const FName EndPlay;

	static const FName PrePhysics;
	static const FName DuringPhysics;
	static const FName PostPhysics;
	static const FName PostUpdate;
};

USTRUCT(BlueprintType)
struct UECS_API FEcsChainEvents
{
	GENERATED_BODY()

public:
	FEcsChainEvents();

	void EnsureDefaultEvents();

	UPROPERTY(EditAnywhere, Category = "ECS|Chain Events")
	TMap<FName, FChainEventData> ChainEvents;
};

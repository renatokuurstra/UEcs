// Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EcsSystem.h"
#include "EcsSystemEvents.generated.h"

USTRUCT(BlueprintType)
struct FSystemArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite)
	TArray<UEcsSystem*> Systems;
};

/**
 * 
 */
UCLASS()
class UECS_API UEcsSystemEvents : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ECS|System Events")
	TMap<FName, FSystemArray> SystemsEvents;
};

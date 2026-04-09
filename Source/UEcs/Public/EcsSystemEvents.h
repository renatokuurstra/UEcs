// Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EcsSystem.h"
#include "EcsEventElement.h"
#include "EcsSystemEvents.generated.h"


USTRUCT(BlueprintType)
struct FSystemEventData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|System Events")
	bool bIsUpdateSystems = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|System Events", meta = (EditCondition = "bIsUpdateSystems"))
	float UpdateFreqSec = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|System Events")
	TArray<TScriptInterface<IEcsEventElement>> Elements;
};

/**
 * Standard ECS System Event Names
 */
struct UECS_API FEcsSystemEventNames
{
	static const FName BeginPlay;
	static const FName EndPlay;
};

/**
 * 
 */
UCLASS()
class UECS_API UEcsSystemEvents : public UDataAsset
{
	GENERATED_BODY()

public:
	UEcsSystemEvents();

	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditAnywhere, Category = "ECS|System Events")
	TMap<FName, FSystemEventData> SystemsEvents;
};

//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

#include "CoreMinimal.h"
#include "EcsSystem.h"
#include "EcsEventElement.h"

#include "TestSimpleEcsSystem.generated.h"

UCLASS(EditInlineNew, DefaultToInstanced)
class UECSTESTS_API UTestSimpleEcsSystem : public UEcsSystem
{
	GENERATED_BODY()
public:
	UTestSimpleEcsSystem()
		: TickCount(0)
	{
	}

	virtual void Update_Implementation(float DeltaTime) override
	{
		++TickCount;
	}

	virtual void Initialize_Implementation(AEcsContext* InContext) override
	{
		Super::Initialize_Implementation(InContext);
		bIsInitialized = true;
	}

	virtual void Deinitialize_Implementation() override
	{
		Super::Deinitialize_Implementation();
		bIsDeinitialized = true;
	}

	UPROPERTY()
	bool bIsInitialized = false;

	UPROPERTY()
	bool bIsDeinitialized = false;

	UFUNCTION(BlueprintCallable, Category = "ECS|System|Tests")
	int32 GetTickCount() const { return TickCount; }

	UFUNCTION(BlueprintCallable, Category = "ECS|System|Tests")
	void ResetTickCount() { TickCount = 0; }

private:
	UPROPERTY()
	int32 TickCount;
};

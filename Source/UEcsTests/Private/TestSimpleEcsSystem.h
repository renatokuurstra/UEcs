#pragma once

#include "CoreMinimal.h"
#include "EcsSystem.h"

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

	virtual void Update(float /*DeltaTime*/) override
	{
		++TickCount;
	}

	UFUNCTION(BlueprintCallable, Category = "ECS|System|Tests")
	int32 GetTickCount() const { return TickCount; }

	UFUNCTION(BlueprintCallable, Category = "ECS|System|Tests")
	void ResetTickCount() { TickCount = 0; }

private:
	UPROPERTY()
	int32 TickCount;
};

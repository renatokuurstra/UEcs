#pragma once

#include "SpeedComp.generated.h"

// Simple float component with implicit conversion
USTRUCT(BlueprintType)
struct UECS_API FSpeedComp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|Components")
	float Value;

	FSpeedComp()
		: Value(0.f)
	{
	}

	explicit FSpeedComp(float InValue)
		: Value(InValue)
	{
	}

	operator float() const { return Value; }
	FSpeedComp& operator=(const float& InValue) { Value = InValue; return *this; }
};

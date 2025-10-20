//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
#pragma once


#include "DirectionComp.generated.h"

// Component wrapping a direction vector with implicit conversion
USTRUCT(BlueprintType)
struct UECS_API FDirectionComp
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|Components")
	FVector Value;

	FDirectionComp()
		: Value(FVector::ForwardVector)
	{
	}

	explicit FDirectionComp(const FVector& InValue)
		: Value(InValue)
	{
	}

	operator FVector() const { return Value; }
	FDirectionComp& operator=(const FVector& InValue) { Value = InValue; return *this; }
};

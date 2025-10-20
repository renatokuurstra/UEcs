//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
#pragma once

#include "PositionComp.generated.h"

USTRUCT(BlueprintType)
struct UECS_API FPositionComp
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ECS|Components")
	FVector Value;

	FPositionComp()
		: Value(FVector::ZeroVector)
	{
	}

	explicit FPositionComp(const FVector& InValue)
		: Value(InValue)
	{
	}

	operator FVector() const { return Value; }
	FPositionComp& operator=(const FVector& InValue) { Value = InValue; return *this; }
};

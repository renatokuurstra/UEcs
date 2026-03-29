// Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#include "EcsSystemEvents.h"

const FName FEcsSystemEventNames::BeginPlay = FName(TEXT("BeginPlay"));
const FName FEcsSystemEventNames::EndPlay = FName(TEXT("EndPlay"));

UEcsSystemEvents::UEcsSystemEvents()
{
	SystemsEvents.FindOrAdd(FEcsSystemEventNames::BeginPlay);
	SystemsEvents.FindOrAdd(FEcsSystemEventNames::EndPlay);
}

void UEcsSystemEvents::PostLoad()
{
	Super::PostLoad();
	SystemsEvents.FindOrAdd(FEcsSystemEventNames::BeginPlay);
	SystemsEvents.FindOrAdd(FEcsSystemEventNames::EndPlay);
}

#if WITH_EDITOR
void UEcsSystemEvents::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SystemsEvents.FindOrAdd(FEcsSystemEventNames::BeginPlay);
	SystemsEvents.FindOrAdd(FEcsSystemEventNames::EndPlay);
}
#endif

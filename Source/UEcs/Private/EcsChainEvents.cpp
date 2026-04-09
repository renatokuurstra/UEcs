// Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#include "EcsChainEvents.h"

const FName FEcsChainEventNames::BeginPlay = FName(TEXT("BeginPlay"));
const FName FEcsChainEventNames::EndPlay = FName(TEXT("EndPlay"));

UEcsChainEvents::UEcsChainEvents()
{
	ChainEvents.FindOrAdd(FEcsChainEventNames::BeginPlay);
	ChainEvents.FindOrAdd(FEcsChainEventNames::EndPlay);
}

void UEcsChainEvents::PostLoad()
{
	Super::PostLoad();
	ChainEvents.FindOrAdd(FEcsChainEventNames::BeginPlay);
	ChainEvents.FindOrAdd(FEcsChainEventNames::EndPlay);
}

#if WITH_EDITOR
void UEcsChainEvents::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ChainEvents.FindOrAdd(FEcsChainEventNames::BeginPlay);
	ChainEvents.FindOrAdd(FEcsChainEventNames::EndPlay);
}
#endif

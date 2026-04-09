// Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#include "EcsChainEvents.h"

const FName FEcsChainEventNames::BeginPlay = FName(TEXT("BeginPlay"));
const FName FEcsChainEventNames::EndPlay = FName(TEXT("EndPlay"));
const FName FEcsChainEventNames::PrePhysics = FName(TEXT("PrePhysics"));
const FName FEcsChainEventNames::DuringPhysics = FName(TEXT("DuringPhysics"));
const FName FEcsChainEventNames::PostPhysics = FName(TEXT("PostPhysics"));
const FName FEcsChainEventNames::PostUpdate = FName(TEXT("PostUpdate"));

FEcsChainEvents::FEcsChainEvents()
{
	EnsureDefaultEvents();
}

void FEcsChainEvents::EnsureDefaultEvents()
{
	ChainEvents.FindOrAdd(FEcsChainEventNames::BeginPlay);
	ChainEvents.FindOrAdd(FEcsChainEventNames::EndPlay);
	ChainEvents.FindOrAdd(FEcsChainEventNames::PrePhysics);
	ChainEvents.FindOrAdd(FEcsChainEventNames::DuringPhysics);
	ChainEvents.FindOrAdd(FEcsChainEventNames::PostPhysics);
	ChainEvents.FindOrAdd(FEcsChainEventNames::PostUpdate);
}

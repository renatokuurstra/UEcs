//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#include "EcsSystem.h"

void UEcsSystem::Initialize_Implementation(AEcsContext* InContext)
{
	Context = InContext;
	if (Context)
	{
		EntitiesRegistry = &Context->GetRegistry();
	}
}

void UEcsSystem::Deinitialize_Implementation()
{
	EntitiesRegistry = nullptr;
	Context = nullptr;
}

entt::registry& UEcsSystem::GetRegistry() const
{
	check(EntitiesRegistry);
	return *EntitiesRegistry;
}

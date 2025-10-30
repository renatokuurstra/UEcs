//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

// Core UE types and containers
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "entt/entt.hpp"
#include "Engine/DataTable.h" // FTableRowBase
#include "Containers/ArrayView.h"
#include <type_traits>

class UScriptStruct;

#include "EcsSystem.generated.h"

/**
 * Base class for ECS Systems
 */
UCLASS(Abstract)
class UECS_API UEcsSystem : public UObject
{
	GENERATED_BODY()
public:
	
	// Systems declare their component types in the constructor or via properties
	// Forcing users to openly declare the components they are using
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ECS|System", meta = (AllowPrivateAccess = "true"))
	TArray<UScriptStruct*> RequiredComponents;
    
	virtual void Initialize(entt::registry& InRegistry) { EntitiesRegistry = &InRegistry;}
	
	// Allow context to cleanup references on EndPlay
	virtual void Deinitialize() { EntitiesRegistry = nullptr; }

	//This should be called by the system design to update all entities.
	UFUNCTION(BlueprintCallable, Category = "ECS|System")
	virtual void Update(float DeltaTime = 0.0f) {}
    
protected:
	
	// Provide controlled access to the registry for systems that need to spawn/maintain entities
	entt::registry& GetRegistry() const
	{
		check(EntitiesRegistry);
		return *EntitiesRegistry;
	}

	// Register a component type (called in subclass constructor)
	template<typename T>
	void RegisterComponent()
	{
		// Ensure T provides StaticStruct at compile time; non-USTRUCT types will fail to compile here.
		RequiredComponents.Add(T::StaticStruct());
	}

	//Give user access to a view of components openly declared in Register components(or defaults).
	template<typename... TComponents>
	auto GetView()
	{
		static_assert(sizeof...(TComponents) > 0, "Must specify at least one component type");

#if WITH_EDITOR || UE_BUILD_DEBUG
		// Validation only in editor/debug builds
		auto ValidateComponent = [this]<typename T>()
		{
			UScriptStruct* ComponentStruct = T::StaticStruct();
			checkf(RequiredComponents.Contains(ComponentStruct),
				TEXT("System '%s' attempted to access undeclared component '%s'. Register it in the constructor using RegisterComponent<%s>()"),
				*GetName(),
				*ComponentStruct->GetName(),
				*ComponentStruct->GetName());
		};
		
		// Validate each component type
		(ValidateComponent.template operator()<TComponents>(), ...);
#endif

		// Return EnTT's view directly
		return EntitiesRegistry->view<TComponents...>();
	}

private:
	// Not owned; set by Initialize() and cleared by Deinitialize().
	entt::registry* EntitiesRegistry = nullptr;
};

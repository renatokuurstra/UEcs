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
    
	virtual void Initialize(entt::registry& InRegistry) { Registry = &InRegistry;}
	
	// Allow context to cleanup references on EndPlay
	virtual void Deinitialize() { Registry = nullptr; }

	//This should be called by the system design to update all entities.
	UFUNCTION(BlueprintCallable, Category = "ECS|System")
	virtual void Update(float DeltaTime = 0.0f) {}
    
protected:
	
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
		return Registry->view<TComponents...>();
	}

	// Returns a non-owning view over the contiguous component storage for TComponent.
	// Components are expected to be stored by value in EnTT. Do NOT pass pointer types here.
	template<typename TComponent>
	TArrayView<TComponent> GetComponentArray()
	{
		static_assert(std::is_same_v<TComponent, std::decay_t<TComponent>>, 
			"Component type must not be a reference or cv-qualified");
		static_assert(!std::is_pointer_v<TComponent>, "Component type must be value-backed (no pointers)");

#if WITH_EDITOR || UE_BUILD_DEBUG
		// Validation only in editor/Debug builds
		UScriptStruct* ComponentStruct = TComponent::StaticStruct();
		checkf(RequiredComponents.Contains(ComponentStruct),
			TEXT("System '%s' attempted to access undeclared component '%s'. Register it in the constructor using RegisterComponent<%s>()"),
			*GetName(),
			*ComponentStruct->GetName(),
			*ComponentStruct->GetName());
#endif

		// Get the storage for this component type (value-backed)
		auto& Storage = Registry->storage<TComponent>();

		//TODO Verify Storage.raw()
		//this is weird, storage.raw should return a *, but instead it return a **
		//this happens regardless of type, maybe shenanigans with templates?
		auto Raw =  Storage.size() ? *Storage.raw() : nullptr;
		const int32 Count = static_cast<int32>(Storage.size());
		return TArrayView<TComponent>(Raw, Count);
	}

private:
	// Not owned; set by Initialize() and cleared by Deinitialize().
	entt::registry* Registry = nullptr;
};

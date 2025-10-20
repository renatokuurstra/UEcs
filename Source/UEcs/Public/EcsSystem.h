//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.

#pragma once

// Core UE types and containers
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "entt/entt.hpp"
#include "Engine/DataTable.h" // FTableRowBase

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
	virtual void Update(float DeltaTime) {}
    
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

	// Returns a pointer to the beginning of the contiguous component storage
	// and the number of components in the array
	template<typename TComponent>
	auto GetComponentArray()
	{
		static_assert(std::is_same_v<TComponent, std::decay_t<TComponent>>, 
			"Component type must not be a reference or cv-qualified");

#if WITH_EDITOR || UE_BUILD_DEBUG
		// Validation only in editor/Debug builds
		UScriptStruct* ComponentStruct = TComponent::StaticStruct();
		checkf(RequiredComponents.Contains(ComponentStruct),
			TEXT("System '%s' attempted to access undeclared component '%s'. Register it in the constructor using RegisterComponent<%s>()"),
			*GetName(),
			*ComponentStruct->GetName(),
			*ComponentStruct->GetName());
#endif

		// Get the storage for this component type
		auto& storage = Registry->storage<TComponent>();
		
		// Return pointer to raw array and size
		// Using specific wrapper.
		struct ComponentArrayView
		{
			TComponent* Data;
			size_t Size;
			
			// Convenience methods
			TComponent* begin() { return Data; }
			TComponent* end() { return Data + Size; }
			const TComponent* begin() const { return Data; }
			const TComponent* end() const { return Data + Size; }
			TComponent& operator[](size_t Index) { return Data[Index]; }
			const TComponent& operator[](size_t Index) const { return Data[Index]; }
			bool IsEmpty() const { return Size == 0; }
		};
		
		if (storage.size() > 0)
		{
			return ComponentArrayView{ storage.raw(), storage.size() };
		}
		else
		{
			return ComponentArrayView{ nullptr, 0 };
		}
	}

private:
	entt::registry* Registry;
};

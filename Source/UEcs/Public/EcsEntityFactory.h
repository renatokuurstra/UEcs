
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "entt/entt.hpp"

#include "EcsEntityFactory.generated.h"

/**
 * Base class for ECS Entity Factories with UObject compatibility
 */
UCLASS(Abstract)
class UECS_API UEcsEntityFactory : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(entt::registry& InRegistry)
	{
		Registry = &InRegistry;
	}
	
	/**
	 * Public entry point - calls the user-defined BuildEntities implementation
	 */
	UFUNCTION(BlueprintCallable, Category = "ECS|Factory")
	void BuildEntities()
	{
		check(Registry);
		BuildEntitiesInternal();
	}
	
protected:
	/**
	 * Override this method to build your entities
	 */
	virtual void BuildEntitiesInternal() {};
	
	/**
	 * Access the registry
	 */
	entt::registry* GetRegistry() { return Registry; }
	
	/**
	 * Helper: Build an entity with specified components
	 */
	template<typename... TComponents>
	void BuildEntity(const TComponents&... Components)
	{
		check(Registry);
		entt::entity Entity = Registry->create();
		(Registry->emplace<TComponents>(Entity, Components), ...);
	}
	
	/**
	 * Helper: Build an entity with default-constructed components
	 */
	template<typename... TComponents>
	void CreateDefaultEntity()
	{
		check(Registry);
		entt::entity Entity = Registry->create();
		(Registry->emplace<TComponents>(Entity), ...);
	}
	
private:
	entt::registry* Registry = nullptr;
};
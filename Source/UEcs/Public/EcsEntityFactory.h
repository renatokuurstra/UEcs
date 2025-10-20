
//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
#pragma once

#include "CoreMinimal.h"
#include "entt/entt.hpp"

/**
 * Base class for ECS Entity Factories without UObject
 */
struct FEcsEntityFactory
{
public:
	virtual ~FEcsEntityFactory() = default;

	virtual void Initialize(entt::registry& InRegistry)
	{
		Registry = &InRegistry;
	}

	/**
	 * Public entry point - calls the user-defined BuildEntities implementation
	 */
	void BuildEntities(int32 Count = 1)
	{
		check(Registry);
		BuildEntitiesInternal(Count);
	}

protected:
	/** Trait to detect presence of StaticStruct() => likely a USTRUCT */
	template<typename T>
	struct THasStaticStruct
	{
	private:
		template<typename U>
		static auto Test(int) -> decltype(&U::StaticStruct, std::true_type{});
		template<typename>
		static auto Test(...) -> std::false_type;
	public:
		static constexpr bool Value = decltype(Test<T>(0))::value;
	};

	/**
	 * Override this method to build your entities
	 */
	virtual void BuildEntitiesInternal(int32 Count) {}

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
		// Ensure all components are USTRUCTs by requiring T::StaticStruct
		static_assert((THasStaticStruct<TComponents>::Value && ...), "All TComponents must be USTRUCTs (have StaticStruct)");
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
		static_assert((THasStaticStruct<TComponents>::Value && ...), "All TComponents must be USTRUCTs (have StaticStruct)");
		check(Registry);
		entt::entity Entity = Registry->create();
		(Registry->emplace<TComponents>(Entity), ...);
	}

private:
	entt::registry* Registry = nullptr;
};
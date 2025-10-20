//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
// Tests for UEcs::EcsEntityFactory

#include "CoreMinimal.h"
#include "CQTest.h"
#include "entt/entt.hpp"
#include "SimpleEntityFactory.h"

TEST_CLASS(UEcs_EcsEntityFactory, "UEcs.EntityFactory")
{
	BEFORE_ALL() {}
	BEFORE_EACH() {}
	AFTER_EACH() {}
	AFTER_ALL() {}

	TEST_METHOD(Creates_One_Default_Entity_DefaultParam)
	{
		entt::registry Registry;
		FSimpleEntityFactory Factory;
		Factory.Initialize(Registry);
		Factory.BuildEntities(); // default should be 1

		// Check counts
		ASSERT_THAT(IsTrue(Registry.storage<FSpeedComp>().size() == 1));
		ASSERT_THAT(IsTrue(Registry.storage<FPositionComp>().size() == 1));
		ASSERT_THAT(IsTrue(Registry.storage<FDirectionComp>().size() == 1));

		// Check default values
		auto View = Registry.view<FSpeedComp, FPositionComp, FDirectionComp>();
		ASSERT_THAT(IsTrue(View.size_hint() == 1));
		entt::entity Found = entt::null;
		for (auto Entity : View) { Found = Entity; break; }
		ASSERT_THAT(IsTrue(Found != entt::null));

		const FSpeedComp& Speed = Registry.get<FSpeedComp>(Found);
		const FPositionComp& Pos = Registry.get<FPositionComp>(Found);
		const FDirectionComp& Dir = Registry.get<FDirectionComp>(Found);

		ASSERT_THAT(IsTrue(FMath::IsNearlyEqual(Speed.Value, 0.f)));
		ASSERT_THAT(IsTrue(Pos.Value.Equals(FVector::ZeroVector)));
		ASSERT_THAT(IsTrue(Dir.Value.Equals(FVector::ForwardVector)));
	}

	TEST_METHOD(Creates_Entity_With_Parameters)
	{
		// Local factory that builds one entity with explicit component values
		struct FParamEntityFactory : public FEcsEntityFactory
		{
			virtual void BuildEntitiesInternal(int32 Count) override
			{
				for (int32 i = 0; i < Count; ++i)
				{
					BuildEntity(
						FSpeedComp(10.f),
						FPositionComp(FVector(1.f, 2.f, 3.f)),
						FDirectionComp(FVector(0.f, 1.f, 0.f))
					);
				}
			}
		};

		entt::registry Registry;
		FParamEntityFactory Factory;
		Factory.Initialize(Registry);
		Factory.BuildEntities(1);

		ASSERT_THAT(IsTrue(Registry.storage<FSpeedComp>().size() == 1));
		entt::entity E = *Registry.view<FSpeedComp, FPositionComp, FDirectionComp>().begin();
		const FSpeedComp& Speed = Registry.get<FSpeedComp>(E);
		const FPositionComp& Pos = Registry.get<FPositionComp>(E);
		const FDirectionComp& Dir = Registry.get<FDirectionComp>(E);

		ASSERT_THAT(IsTrue(FMath::IsNearlyEqual(Speed.Value, 10.f)));
		ASSERT_THAT(IsTrue(Pos.Value.Equals(FVector(1.f, 2.f, 3.f))));
		ASSERT_THAT(IsTrue(Dir.Value.Equals(FVector(0.f, 1.f, 0.f))));
	}
};

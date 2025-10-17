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

	TEST_METHOD(Creates_Three_Default_Entities)
	{
		entt::registry Registry;
		USimpleEntityFactory* Factory = NewObject<USimpleEntityFactory>();
		Factory->Initialize(Registry);
		Factory->BuildEntities();

		const std::size_t SpeedCompCount = Registry.storage<FSpeedComp>().size();
		ASSERT_THAT(IsTrue(SpeedCompCount == 3));
	}
};

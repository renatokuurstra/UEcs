// Tests for AEcsContext

#include "CoreMinimal.h"
#include "CQTest.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "EcsContext.h"
#include "TestSimpleEcsSystem.h"

TEST_CLASS(UEcs_EcsContext, "UEcs.EcsContext")
{
	BEFORE_ALL() {}
	BEFORE_EACH() {}
	AFTER_EACH() {}
	AFTER_ALL() {}

	TEST_METHOD(Can_Create_World_And_Spawn_Single_Context)
	{
		// Create a transient game world
		UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, FName(TEXT("EcsContextTestWorld")));
		ASSERT_THAT(IsTrue(World != nullptr));

		// Spawn one context actor
		FActorSpawnParameters Params;
		Params.ObjectFlags |= RF_Transient;
		AEcsContext* Context = World->SpawnActor<AEcsContext>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
		ASSERT_THAT(IsTrue(Context != nullptr));

		// Reference the test system type (ensure it compiles and can be instantiated)
		UTestSimpleEcsSystem* TestSys = NewObject<UTestSimpleEcsSystem>(Context);
		ASSERT_THAT(IsTrue(TestSys != nullptr));

		// Count AEcsContext actors in this world
		int32 Count = 0;
		for (TActorIterator<AEcsContext> It(World); It; ++It)
		{
			++Count;
		}
		ASSERT_THAT(IsTrue(Count == 1));

		// Cleanup
		World->DestroyWorld(false);
	}
};

//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
// Tests for AEcsContext

#include "CoreMinimal.h"
#include "CQTest.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "EcsContext.h"
#include "EcsSystemEvents.h"
#include "TestSimpleEcsSystem.h"

TEST_CLASS(UEcs_EcsContext, "UEcs.EcsContext")
{
	TObjectPtr<UWorld> World;
	TObjectPtr<AEcsContext> Context;
	TObjectPtr<UTestSimpleEcsSystem> TestSys;
	
	BEFORE_ALL() {}
	BEFORE_EACH()
	{
		// Create a transient game world
		World = UWorld::CreateWorld(EWorldType::Game, false, FName(TEXT("EcsContextTestWorld")));
		ASSERT_THAT(IsTrue(World != nullptr, "Transient world should be successfully created"));

		// Spawn one context actor
		FActorSpawnParameters Params;
		Params.ObjectFlags |= RF_Transient;
		Context = World->SpawnActor<AEcsContext>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
		ASSERT_THAT(IsTrue(Context != nullptr, "EcsContext actor should be successfully spawned"));
		
		// Reference the test system type (ensure it compiles and can be instantiated)
		TestSys = NewObject<UTestSimpleEcsSystem>(Context);
		ASSERT_THAT(IsTrue(TestSys != nullptr, "Test system should be successfully created"));
	}
	AFTER_EACH()
	{
		// Cleanup
		World->DestroyWorld(false);
		World = nullptr;
		Context = nullptr;
		TestSys = nullptr;
	}
	AFTER_ALL() {}

	TEST_METHOD(Context_Supports_Nested_Contexts)
	{
		// Spawn a child context
		FActorSpawnParameters Params;
		Params.ObjectFlags |= RF_Transient;
		AEcsContext* ChildContext = World->SpawnActor<AEcsContext>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
		ASSERT_THAT(IsTrue(ChildContext != nullptr, "Child EcsContext should be successfully spawned"));

		// Add a test system to the child context
		UTestSimpleEcsSystem* ChildSys = NewObject<UTestSimpleEcsSystem>(ChildContext);
		ChildContext->AddElementToTickGroup(TG_PrePhysics, MakeEcsEventElement(ChildSys));

		// Add child context to parent context
		Context->AddElementToTickGroup(TG_PrePhysics, MakeEcsEventElement(ChildContext));

		// Manually trigger BeginPlay to initialize everything
		Context->ExecuteEvent(FEcsSystemEventNames::BeginPlay);
		
		ASSERT_THAT(IsTrue(ChildSys->bIsInitialized, "Child system should be initialized after parent's BeginPlay"));

		// Tick parent context
		IEcsEventElement::Execute_Update(Context, 0.1f);
		
		// Child system should have been ticked via ChildContext::Update_Implementation
		ASSERT_THAT(AreEqual(1, ChildSys->GetTickCount(), "Child system should have ticked exactly once"));

		// Deinitialize parent
		IEcsEventElement::Execute_Deinitialize(Context);
		ASSERT_THAT(IsTrue(ChildSys->bIsDeinitialized, "Child system should be deinitialized when parent is deinitialized"));
	}
};

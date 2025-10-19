#include "EcsContext.h"

#include "EcsSystem.h"
#include "Engine/World.h"

// FContextTickFunction implementation
void AEcsContext::FContextTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Owner)
	{
		Owner->TickGroupUpdate(DeltaTime, MyGroup);
	}
}

AEcsContext::AEcsContext()
{
	PrimaryActorTick.bCanEverTick = false; // we use custom tick functions per group

}

void AEcsContext::BeginPlay()
{
	Super::BeginPlay();

	// Initialize all systems with our registry
	auto InitializeSystems = [this](TArray<UEcsSystem*>& Systems)
	{
		for (UEcsSystem* Sys : Systems)
		{
			if (Sys)
			{
				Sys->Initialize(Registry);
			}
		}
	};

	InitializeSystems(PrePhysicsSystems);
	InitializeSystems(DuringPhysicsSystems);
	InitializeSystems(PostPhysicsSystems);
	InitializeSystems(PostUpdateSystems);

	InitialiseTickFunctions();

	
	// Register tick functions only if needed
	auto* Level = GetLevel(); // owner for registration

	if (PrePhysicsSystems.Num() > 0)
	{
		PrePhysicsTick.SetTickFunctionEnable(true);
		PrePhysicsTick.RegisterTickFunction(Level);
	}
	if (DuringPhysicsSystems.Num() > 0)
	{
		DuringPhysicsTick.SetTickFunctionEnable(true);
		DuringPhysicsTick.RegisterTickFunction(Level);
	}
	if (PostPhysicsSystems.Num() > 0)
	{
		PostPhysicsTick.SetTickFunctionEnable(true);
		PostPhysicsTick.RegisterTickFunction(Level);
	}
	if (PostUpdateSystems.Num() > 0)
	{
		PostUpdateTick.SetTickFunctionEnable(true);
		PostUpdateTick.RegisterTickFunction(Level);
	}
}

void AEcsContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PrePhysicsTick.UnRegisterTickFunction();
	DuringPhysicsTick.UnRegisterTickFunction();
	PostPhysicsTick.UnRegisterTickFunction();
	PostUpdateTick.UnRegisterTickFunction();

	Super::EndPlay(EndPlayReason);
}

void AEcsContext::InitialiseTickFunctions()
{
	// Initialize tick functions
	if (PrePhysicsSystems.Num() > 0)
	{
		PrePhysicsTick.Owner = this;
		PrePhysicsTick.MyGroup = TG_PrePhysics;
		PrePhysicsTick.TickGroup = TG_PrePhysics;
		PrePhysicsTick.bCanEverTick = true;
		PrePhysicsTick.SetTickFunctionEnable(false);
	}

	if (DuringPhysicsSystems.Num() > 0)
	{
	
		DuringPhysicsTick.Owner = this;
		DuringPhysicsTick.MyGroup = TG_DuringPhysics;
		DuringPhysicsTick.TickGroup = TG_DuringPhysics;
		DuringPhysicsTick.bCanEverTick = true;
		DuringPhysicsTick.SetTickFunctionEnable(false);
	}

	if (PostPhysicsSystems.Num() > 0)
	{
		PostPhysicsTick.Owner = this;
		PostPhysicsTick.MyGroup = TG_PostPhysics;
		PostPhysicsTick.TickGroup = TG_PostPhysics;
		PostPhysicsTick.bCanEverTick = true;
		PostPhysicsTick.SetTickFunctionEnable(false);
	}

	if (PostUpdateSystems.Num() > 0)
	{
		PostUpdateTick.Owner = this;
		PostUpdateTick.MyGroup = TG_PostUpdateWork;
		PostUpdateTick.TickGroup = TG_PostUpdateWork;
		PostUpdateTick.bCanEverTick = true;
		PostUpdateTick.SetTickFunctionEnable(false);
	}
}

void AEcsContext::TickGroupUpdate(float DeltaTime, ETickingGroup Group)
{
	TArray<UEcsSystem*>* SystemsPtr = nullptr;
	switch (Group)
	{
		case TG_PrePhysics: SystemsPtr = &PrePhysicsSystems; break;
		case TG_DuringPhysics: SystemsPtr = &DuringPhysicsSystems; break;
		case TG_PostPhysics: SystemsPtr = &PostPhysicsSystems; break;
		case TG_PostUpdateWork: SystemsPtr = &PostUpdateSystems; break;
		default: break;
	}

	if (!SystemsPtr) return;

	for (UEcsSystem* Sys : *SystemsPtr)
	{
		if (Sys)
		{
			Sys->Update(DeltaTime);
		}
	}
}

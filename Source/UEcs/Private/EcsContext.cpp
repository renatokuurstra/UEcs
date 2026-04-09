//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
#include "EcsContext.h"

#include "EcsSystem.h"
#include "EcsSystemEvents.h"
#include "Engine/World.h"
#include "TimerManager.h"

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

void AEcsContext::ExecuteEvent(const FName& EventName)
{
	if(EcsSystemsEvents)
	{
		if (const FSystemEventData* EventData = EcsSystemsEvents->SystemsEvents.Find(EventName))
		{
			for (const TScriptInterface<IEcsEventElement>& Element : EventData->Elements)
			{
				if (Element)
				{
					// For initialization/deinitialization events, we might want to call those specifically,
					// but generally Execute_Update is used as the generic "trigger" for event elements.
					IEcsEventElement::Execute_Update(Element.GetObject(), 0.0f);
				}
			}
		}
	}

	// Propagate to nested contexts in all tick groups
	auto PropagateToNested = [this, EventName](TArray<TScriptInterface<IEcsEventElement>>& Elements)
	{
		for (const TScriptInterface<IEcsEventElement>& Element : Elements)
		{
			if (Element)
			{
				if (AEcsContext* NestedContext = Cast<AEcsContext>(Element.GetObject()))
				{
					NestedContext->ExecuteEvent(EventName);
				}
			}
		}
	};

	PropagateToNested(PrePhysicsElements);
	PropagateToNested(DuringPhysicsElements);
	PropagateToNested(PostPhysicsElements);
	PropagateToNested(PostUpdateElements);
}

void AEcsContext::BeginPlay()
{
	Super::BeginPlay();

	// Initialize all elements with our context
	auto InitializeElements = [this](TArray<TScriptInterface<IEcsEventElement>>& Elements)
	{
		for (TScriptInterface<IEcsEventElement>& Element : Elements)
		{
			if (Element)
			{
				IEcsEventElement::Execute_Initialize(Element.GetObject(), this);
			}
		}
	};

	InitializeElements(PrePhysicsElements);
	InitializeElements(DuringPhysicsElements);
	InitializeElements(PostPhysicsElements);
	InitializeElements(PostUpdateElements);

	if (EcsSystemsEvents)
	{
		for (auto& Pair : EcsSystemsEvents->SystemsEvents)
		{
			InitializeElements(Pair.Value.Elements);

			if (Pair.Value.bIsUpdateSystems && Pair.Value.UpdateFreqSec > 0.0f)
			{
				FTimerHandle& Handle = EventTimers.FindOrAdd(Pair.Key);
				const FName EventName = Pair.Key;
				GetWorldTimerManager().SetTimer(Handle, [this, EventName]()
				{
					ExecuteEvent(EventName);
				}, Pair.Value.UpdateFreqSec, true);
			}
		}
	}

	InitialiseTickFunctions();

	ExecuteEvent(FEcsSystemEventNames::BeginPlay);

	
	// Register tick functions only if needed
	auto* Level = GetLevel(); // owner for registration

	if (PrePhysicsElements.Num() > 0)
	{
		PrePhysicsTick.SetTickFunctionEnable(true);
		PrePhysicsTick.RegisterTickFunction(Level);
	}
	if (DuringPhysicsElements.Num() > 0)
	{
		DuringPhysicsTick.SetTickFunctionEnable(true);
		DuringPhysicsTick.RegisterTickFunction(Level);
	}
	if (PostPhysicsElements.Num() > 0)
	{
		PostPhysicsTick.SetTickFunctionEnable(true);
		PostPhysicsTick.RegisterTickFunction(Level);
	}
	if (PostUpdateElements.Num() > 0)
	{
		PostUpdateTick.SetTickFunctionEnable(true);
		PostUpdateTick.RegisterTickFunction(Level);
	}
}

void AEcsContext::Initialize_Implementation(AEcsContext* InContext)
{
	// Ensure we're properly set up when used as a nested context
	InitialiseTickFunctions();
}

void AEcsContext::Update_Implementation(float DeltaTime)
{
	TickGroupUpdate(DeltaTime, TG_PrePhysics);
	TickGroupUpdate(DeltaTime, TG_DuringPhysics);
	TickGroupUpdate(DeltaTime, TG_PostPhysics);
	TickGroupUpdate(DeltaTime, TG_PostUpdateWork);

	// Update all event-based systems that are marked as update systems
	if (EcsSystemsEvents)
	{
		for (auto& Pair : EcsSystemsEvents->SystemsEvents)
		{
			if (Pair.Value.bIsUpdateSystems && Pair.Value.UpdateFreqSec <= 0.0f)
			{
				ExecuteEvent(Pair.Key);
			}
		}
	}
}

void AEcsContext::Deinitialize_Implementation()
{
	// Similar to EndPlay but for non-actor contexts (if any) or nested usage
	ExecuteEvent(FEcsSystemEventNames::EndPlay);
	
	auto CleanupElements = [](TArray<TScriptInterface<IEcsEventElement>>& Elements)
	{
		for (int32 i = Elements.Num() - 1; i >= 0; --i)
		{
			if (Elements[i])
			{
				IEcsEventElement::Execute_Deinitialize(Elements[i].GetObject());
			}
		}
		Elements.Empty();
	};

	CleanupElements(PrePhysicsElements);
	CleanupElements(DuringPhysicsElements);
	CleanupElements(PostPhysicsElements);
	CleanupElements(PostUpdateElements);
}

void AEcsContext::AddElementToTickGroup(ETickingGroup Group, TScriptInterface<IEcsEventElement> Element)
{
	if (!Element) return;

	TArray<TScriptInterface<IEcsEventElement>>* ElementsPtr = nullptr;
	switch (Group)
	{
		case TG_PrePhysics: ElementsPtr = &PrePhysicsElements; break;
		case TG_DuringPhysics: ElementsPtr = &DuringPhysicsElements; break;
		case TG_PostPhysics: ElementsPtr = &PostPhysicsElements; break;
		case TG_PostUpdateWork: ElementsPtr = &PostUpdateElements; break;
		default: break;
	}

	if (ElementsPtr)
	{
		ElementsPtr->Add(Element);
		IEcsEventElement::Execute_Initialize(Element.GetObject(), this);
		
		// If we're already playing, we might need to register the tick function if it wasn't already
		if (HasActorBegunPlay())
		{
			InitialiseTickFunctions();
			// And register it if it's the first element
			if (ElementsPtr->Num() == 1)
			{
				FTickFunction* TickFunc = nullptr;
				switch (Group)
				{
					case TG_PrePhysics: TickFunc = &PrePhysicsTick; break;
					case TG_DuringPhysics: TickFunc = &DuringPhysicsTick; break;
					case TG_PostPhysics: TickFunc = &PostPhysicsTick; break;
					case TG_PostUpdateWork: TickFunc = &PostUpdateTick; break;
					default: break;
				}
				if (TickFunc && !TickFunc->IsTickFunctionRegistered())
				{
					TickFunc->SetTickFunctionEnable(true);
					TickFunc->RegisterTickFunction(GetLevel());
				}
			}
		}
	}
}

void AEcsContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Deinitialize_Implementation();

	// Stop ticking
	PrePhysicsTick.UnRegisterTickFunction();
	DuringPhysicsTick.UnRegisterTickFunction();
	PostPhysicsTick.UnRegisterTickFunction();
	PostUpdateTick.UnRegisterTickFunction();

	if (EcsSystemsEvents)
	{
		for (auto& Pair : EcsSystemsEvents->SystemsEvents)
		{
			for (int32 i = Pair.Value.Elements.Num() - 1; i >= 0; --i)
			{
				if (Pair.Value.Elements[i])
				{
					IEcsEventElement::Execute_Deinitialize(Pair.Value.Elements[i].GetObject());
				}
			}
		}

		for (auto& Pair : EventTimers)
		{
			GetWorldTimerManager().ClearTimer(Pair.Value);
		}
		EventTimers.Empty();
	}

	// Dispose of all entities/components and reset the registry
	Registry.clear();
	Registry = entt::registry{};

	Super::EndPlay(EndPlayReason);
}

void AEcsContext::InitialiseTickFunctions()
{
	UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] AEcsContext::InitialiseTickFunctions called on %s"), *GetName());
	// Initialize tick functions
	PrePhysicsTick.Owner = this;
	PrePhysicsTick.MyGroup = TG_PrePhysics;
	PrePhysicsTick.TickGroup = TG_PrePhysics;
	PrePhysicsTick.bCanEverTick = true;
	PrePhysicsTick.SetTickFunctionEnable(false);

	DuringPhysicsTick.Owner = this;
	DuringPhysicsTick.MyGroup = TG_DuringPhysics;
	DuringPhysicsTick.TickGroup = TG_DuringPhysics;
	DuringPhysicsTick.bCanEverTick = true;
	DuringPhysicsTick.SetTickFunctionEnable(false);

	PostPhysicsTick.Owner = this;
	PostPhysicsTick.MyGroup = TG_PostPhysics;
	PostPhysicsTick.TickGroup = TG_PostPhysics;
	PostPhysicsTick.bCanEverTick = true;
	PostPhysicsTick.SetTickFunctionEnable(false);

	PostUpdateTick.Owner = this;
	PostUpdateTick.MyGroup = TG_PostUpdateWork;
	PostUpdateTick.TickGroup = TG_PostUpdateWork;
	PostUpdateTick.bCanEverTick = true;
	PostUpdateTick.SetTickFunctionEnable(false);
}

void AEcsContext::TickGroupUpdate(float DeltaTime, ETickingGroup Group)
{
	TArray<TScriptInterface<IEcsEventElement>>* ElementsPtr = nullptr;
	switch (Group)
	{
		case TG_PrePhysics: ElementsPtr = &PrePhysicsElements; break;
		case TG_DuringPhysics: ElementsPtr = &DuringPhysicsElements; break;
		case TG_PostPhysics: ElementsPtr = &PostPhysicsElements; break;
		case TG_PostUpdateWork: ElementsPtr = &PostUpdateElements; break;
		default: break;
	}

	if (!ElementsPtr) return;

	for (TScriptInterface<IEcsEventElement>& Element : *ElementsPtr)
	{
		if (Element)
		{
			IEcsEventElement::Execute_Update(Element.GetObject(), DeltaTime);
		}
	}
}

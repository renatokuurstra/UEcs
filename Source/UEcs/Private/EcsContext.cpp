//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
#include "EcsContext.h"

#include "EcsSystem.h"
#include "EcsChainEvents.h"
#include "Engine/World.h"
#include "TimerManager.h"

// FContextTickFunction implementation
void AEcsContext::FContextTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Owner)
	{
		Owner->ExecuteEvent(EventName);
	}
}

AEcsContext::AEcsContext()
{
	PrimaryActorTick.bCanEverTick = false; // we use custom tick functions per group
}

void AEcsContext::ExecuteEvent(const FName& EventName)
{
	if (const FChainEventData* EventData = EcsChainEvents.ChainEvents.Find(EventName))
	{
		for (const TScriptInterface<IEcsEventElement>& Element : EventData->Elements)
		{
			if (Element)
			{
				IEcsEventElement::Execute_Update(Element.GetObject(), GetWorld() ? GetWorld()->GetDeltaSeconds() : 0.0f);
			}
		}
	}

	// Propagate to nested contexts in all events
	for (auto& Pair : EcsChainEvents.ChainEvents)
	{
		for (const TScriptInterface<IEcsEventElement>& Element : Pair.Value.Elements)
		{
			if (Element)
			{
				if (AEcsContext* NestedContext = Cast<AEcsContext>(Element.GetObject()))
				{
					NestedContext->ExecuteEvent(EventName);
				}
			}
		}
	}
}

void AEcsContext::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Pair : EcsChainEvents.ChainEvents)
	{
		for (TScriptInterface<IEcsEventElement>& Element : Pair.Value.Elements)
		{
			if (Element)
			{
				IEcsEventElement::Execute_Initialize(Element.GetObject(), this);
			}
		}

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

	InitialiseTickFunctions();

	ExecuteEvent(FEcsChainEventNames::BeginPlay);

	// Register tick functions only if they have elements
	auto* Level = GetLevel(); 
	for (auto& Pair : TickFunctions)
	{
		if (const FChainEventData* EventData = EcsChainEvents.ChainEvents.Find(Pair.Key))
		{
			if (EventData->Elements.Num() > 0)
			{
				Pair.Value->SetTickFunctionEnable(true);
				Pair.Value->RegisterTickFunction(Level);
			}
		}
	}
}

void AEcsContext::Initialize_Implementation(AEcsContext* InContext)
{
	// Ensure we're properly set up when used as a nested context
	InitialiseTickFunctions();
}

void AEcsContext::Update_Implementation(float DeltaTime)
{
	ExecuteEvent(FEcsChainEventNames::PrePhysics);
	ExecuteEvent(FEcsChainEventNames::DuringPhysics);
	ExecuteEvent(FEcsChainEventNames::PostPhysics);
	ExecuteEvent(FEcsChainEventNames::PostUpdate);

	// Update all event-based systems that are marked as update systems
	for (auto& Pair : EcsChainEvents.ChainEvents)
	{
		if (Pair.Value.bIsUpdateSystems && Pair.Value.UpdateFreqSec <= 0.0f)
		{
			ExecuteEvent(Pair.Key);
		}
	}
}

void AEcsContext::Deinitialize_Implementation()
{
	// Similar to EndPlay but for non-actor contexts (if any) or nested usage
	ExecuteEvent(FEcsChainEventNames::EndPlay);
	
	for (auto& Pair : EcsChainEvents.ChainEvents)
	{
		for (int32 i = Pair.Value.Elements.Num() - 1; i >= 0; --i)
		{
			if (Pair.Value.Elements[i])
			{
				IEcsEventElement::Execute_Deinitialize(Pair.Value.Elements[i].GetObject());
			}
		}
	}
}

void AEcsContext::AddElementToTickGroup(ETickingGroup Group, TScriptInterface<IEcsEventElement> Element)
{
	if (!Element) return;

	FName EventName;
	switch (Group)
	{
		case TG_PrePhysics: EventName = FEcsChainEventNames::PrePhysics; break;
		case TG_DuringPhysics: EventName = FEcsChainEventNames::DuringPhysics; break;
		case TG_PostPhysics: EventName = FEcsChainEventNames::PostPhysics; break;
		case TG_PostUpdateWork: EventName = FEcsChainEventNames::PostUpdate; break;
		default: break;
	}

	if (!EventName.IsNone())
	{
		FChainEventData& EventData = EcsChainEvents.ChainEvents.FindOrAdd(EventName);
		EventData.Elements.Add(Element);
		IEcsEventElement::Execute_Initialize(Element.GetObject(), this);
		
		// If we're already playing, we might need to register the tick function if it wasn't already
		if (HasActorBegunPlay())
		{
			InitialiseTickFunctions();
			FContextTickFunction** TickFuncPtr = TickFunctions.Find(EventName);
			if (TickFuncPtr && *TickFuncPtr)
			{
				FContextTickFunction& TickFunc = **TickFuncPtr;
				if (!TickFunc.IsTickFunctionRegistered())
				{
					TickFunc.SetTickFunctionEnable(true);
					TickFunc.RegisterTickFunction(GetLevel());
				}
			}
		}
	}
}

void AEcsContext::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Deinitialize_Implementation();

	// Stop ticking
	for (auto& Pair : TickFunctions)
	{
		Pair.Value->UnRegisterTickFunction();
		delete Pair.Value;
	}
	TickFunctions.Empty();

	for (auto& Pair : EventTimers)
	{
		GetWorldTimerManager().ClearTimer(Pair.Value);
	}
	EventTimers.Empty();

	// Dispose of all entities/components and reset the registry
	Registry.clear();
	Registry = entt::registry{};

	Super::EndPlay(EndPlayReason);
}

void AEcsContext::InitialiseTickFunctions()
{
	UE_LOG(LogTemp, Log, TEXT("[DEBUG_LOG] AEcsContext::InitialiseTickFunctions called on %s"), *GetName());
	
	auto SetupTick = [this](FName EventName, ETickingGroup Group)
	{
		if (TickFunctions.Contains(EventName)) return;

		FContextTickFunction* TickFunc = new FContextTickFunction(this, EventName);
		TickFunc->TickGroup = Group;
		TickFunc->bCanEverTick = true;
		TickFunc->SetTickFunctionEnable(false);
		TickFunctions.Add(EventName, TickFunc);
	};

	SetupTick(FEcsChainEventNames::PrePhysics, TG_PrePhysics);
	SetupTick(FEcsChainEventNames::DuringPhysics, TG_DuringPhysics);
	SetupTick(FEcsChainEventNames::PostPhysics, TG_PostPhysics);
	SetupTick(FEcsChainEventNames::PostUpdate, TG_PostUpdateWork);
}

void AEcsContext::TickGroupUpdate(float DeltaTime, ETickingGroup Group)
{
	// This function is now deprecated in favor of ExecuteEvent called from ExecuteTick
}

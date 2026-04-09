//Copyright (c) 2025 Renato Kuurstra. Licensed under the MIT License. See LICENSE file in the project root for details.
#pragma once

#include "CoreMinimal.h"
#include "EcsChainEvents.h"
#include "GameFramework/Actor.h"
#include "entt/entt.hpp"
#include "EcsEventElement.h"
#include "EcsContext.generated.h"


class UEcsSystem;

/**
 * AEcsContext
 * - Owns a private EnTT registry
 * - Holds arrays of UEcsSystem instances per tick group
 * - Updates those systems in the corresponding engine tick group
 */
UCLASS(BlueprintType)
class UECS_API AEcsContext : public AActor, public IEcsEventElement
{
	GENERATED_BODY()
public:
	AEcsContext();

	UFUNCTION(BlueprintCallable, Category = "ECS|Systems")
	void ExecuteEvent(const FName& EventName);

	virtual void Initialize_Implementation(AEcsContext* InContext) override;
	virtual void Update_Implementation(float DeltaTime) override;
	virtual void Deinitialize_Implementation() override;

	void AddElementToTickGroup(ETickingGroup Group, TScriptInterface<IEcsEventElement> Element);

	entt::registry& GetRegistry() { return Registry; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category="ECS|Chain Events")
	FEcsChainEvents EcsChainEvents;
	
private:
	void InitialiseTickFunctions();

	
	// Internal dispatcher for tick functions
	void TickGroupUpdate(float DeltaTime, ETickingGroup Group);

	// The ECS Registry (private and automatic)
	entt::registry Registry;

	TMap<FName, FTimerHandle> EventTimers;

	// Custom tick functions for each group

	struct FContextTickFunction : public FTickFunction
	{
		AEcsContext* Owner = nullptr;
		FName EventName;

		FContextTickFunction() : Owner(nullptr), EventName(NAME_None) {}
		FContextTickFunction(AEcsContext* InOwner, FName InEventName) : Owner(InOwner), EventName(InEventName) {}

		virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
		virtual FString DiagnosticMessage() override { return TEXT("AEcsContext::FContextTickFunction"); }
		virtual FName DiagnosticContext(bool bDetailed) override { return FName(TEXT("AEcsContext")); }
	};

	TMap<FName, FContextTickFunction*> TickFunctions;
};

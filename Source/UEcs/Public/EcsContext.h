#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "entt/entt.hpp"
#include "EcsContext.generated.h"


class UEcsSystem;

/**
 * AEcsContext
 * - Owns a private EnTT registry
 * - Holds arrays of UEcsSystem instances per tick group
 * - Updates those systems in the corresponding engine tick group
 */
UCLASS()
class UECS_API AEcsContext : public AActor
{
	GENERATED_BODY()
public:
	AEcsContext();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void InitialiseTickFunctions();
	
	// Internal dispatcher for tick functions
	void TickGroupUpdate(float DeltaTime, ETickingGroup Group);

	// The ECS Registry (private and automatic)
	entt::registry Registry;

	// Arrays of systems per tick group
	// Instanced so they are owned by this actor when added in editor/blueprints.
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "ECS|Systems")
	TArray<UEcsSystem*> PrePhysicsSystems;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "ECS|Systems")
	TArray<UEcsSystem*> DuringPhysicsSystems;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "ECS|Systems")
	TArray<UEcsSystem*> PostPhysicsSystems;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "ECS|Systems")
	TArray<UEcsSystem*> PostUpdateSystems;

	// Custom tick functions for each group

	struct FContextTickFunction : public FTickFunction
	{
		AEcsContext* Owner = nullptr;
		ETickingGroup MyGroup = TG_PrePhysics;

		virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
		virtual FString DiagnosticMessage() override { return TEXT("AEcsContext::FContextTickFunction"); }
		virtual FName DiagnosticContext(bool bDetailed) override { return FName(TEXT("AEcsContext")); }
	};


	FContextTickFunction PrePhysicsTick;

	FContextTickFunction DuringPhysicsTick;

	FContextTickFunction PostPhysicsTick;

	FContextTickFunction PostUpdateTick;
};

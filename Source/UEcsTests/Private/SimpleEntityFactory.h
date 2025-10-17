#pragma once

#include "CoreMinimal.h"
#include "EcsEntityFactory.h"
#include "Components/SpeedComp.h"
#include "Components/PositionComp.h"
#include "Components/DirectionComp.h"

#include "SimpleEntityFactory.generated.h"

UCLASS()
class UECSTESTS_API USimpleEntityFactory : public UEcsEntityFactory
{
	GENERATED_BODY()
	
protected:
	virtual void BuildEntitiesInternal() override
	{
		// Specify component types explicitly when calling CreateDefaultEntity
		CreateDefaultEntity<FSpeedComp, FPositionComp, FDirectionComp>();
		CreateDefaultEntity<FSpeedComp, FPositionComp, FDirectionComp>();
		CreateDefaultEntity<FSpeedComp, FPositionComp, FDirectionComp>();
	}
};
#pragma once

#include "CoreMinimal.h"
#include "EcsEntityFactory.h"
#include "Components/SpeedComp.h"
#include "Components/PositionComp.h"
#include "Components/DirectionComp.h"

struct FSimpleEntityFactory : public FEcsEntityFactory
{
protected:
	virtual void BuildEntitiesInternal(int32 Count) override
	{
		// Build Count default entities with the three components
		for (int32 i = 0; i < Count; ++i)
		{
			CreateDefaultEntity<FSpeedComp, FPositionComp, FDirectionComp>();
		}
	}
};
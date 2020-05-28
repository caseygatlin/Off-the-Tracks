#pragma once

#include "TrainAndRail/Structs/S_TrackRegion.h"

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackTriggerRegion.generated.h"

class ATrackTrigger;

USTRUCT(BlueprintType)
struct FS_TrackTriggerRegion
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FS_TrackRegion Region;

    UPROPERTY()
        ATrackTrigger* Trigger;
};

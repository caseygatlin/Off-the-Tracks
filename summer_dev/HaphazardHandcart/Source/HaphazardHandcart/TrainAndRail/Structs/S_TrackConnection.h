#pragma once

#include "TrainAndRail/E_TrainAndRail.h"

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackConnection.generated.h"

// forward declare
class ATrainTrack;

USTRUCT(BlueprintType)
struct FS_TrackConnection
{
	GENERATED_BODY()

	UPROPERTY()
		ATrainTrack * Track;
	UPROPERTY()
		int32 ConnectionID;
	UPROPERTY()
		E_TrackSplineLocation Location;
};

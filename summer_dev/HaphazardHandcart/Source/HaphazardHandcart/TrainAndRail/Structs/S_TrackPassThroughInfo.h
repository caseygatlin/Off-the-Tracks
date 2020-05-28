#pragma once

#include "TrainAndRail/TrainTrack.h"
#include "TrainAndRail/Structs/S_TrackSpline.h"

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackPassThroughInfo.generated.h"

USTRUCT(BlueprintType)
struct FS_TrackPassThroughInfo
{
	GENERATED_BODY()

	UPROPERTY()
		ATrainTrack * Track;
	UPROPERTY()
		FS_TrackSpline TrackSpline;
	UPROPERTY()
		float StartDistance;
	UPROPERTY()
		float EndDistance;
	UPROPERTY()
		bool Invert;
};

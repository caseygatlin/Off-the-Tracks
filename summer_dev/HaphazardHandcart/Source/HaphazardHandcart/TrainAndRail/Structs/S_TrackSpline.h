#pragma once

#include "TrainAndRail/Structs/S_TrackConnectionPoint.h"

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackSpline.generated.h"

USTRUCT(BlueprintType)
struct FS_TrackSpline
{
	GENERATED_BODY()

	UPROPERTY()
		USplineComponent * Spline;
	UPROPERTY()
		FS_TrackConnectionPoint StartConnectionPoint;
	UPROPERTY()
		int32 StartConnectionIndex;
	UPROPERTY()
		FS_TrackConnectionPoint EndConnectionPoint;
	UPROPERTY()
		int32 EndConnectionIndex;
};

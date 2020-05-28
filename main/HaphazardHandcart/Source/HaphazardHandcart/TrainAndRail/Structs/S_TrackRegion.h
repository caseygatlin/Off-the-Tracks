#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackRegion.generated.h"

USTRUCT(BlueprintType)
struct FS_TrackRegion
{
	GENERATED_BODY()

	UPROPERTY()
		float StartDistance;
	UPROPERTY()
		float EndDistance;
	UPROPERTY()
		int32 TrackSplineIndex;
};

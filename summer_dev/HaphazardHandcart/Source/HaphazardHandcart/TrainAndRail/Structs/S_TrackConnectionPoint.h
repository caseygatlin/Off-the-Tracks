#pragma once

#include "TrainAndRail/Structs/S_TrackConnection.h"

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackConnectionPoint.generated.h"

USTRUCT(BlueprintType)
struct FS_TrackConnectionPoint
{
	GENERATED_BODY()

	UPROPERTY()
		TArray<FS_TrackConnection> Connections;
};

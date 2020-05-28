#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ObjectMacros.h"
#include "S_TrackTimedIgnore.generated.h"

USTRUCT(BlueprintType)
struct FS_TrackTimedIgnore
{
	GENERATED_BODY()

	UPROPERTY()
		AActor * ActorToIgnore;
	UPROPERTY()
		float Timeout;
	UPROPERTY()
		bool NeverExpire;
};

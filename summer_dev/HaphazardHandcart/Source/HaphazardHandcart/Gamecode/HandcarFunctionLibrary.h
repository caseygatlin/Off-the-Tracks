#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HandcarFunctionLibrary.generated.h"

class AJumpTrigger;
class ALeanJunctionSwitchTrigger;
class ATrainTrack;

UCLASS()
class HAPHAZARDHANDCART_API UHandcarFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "Handcar|FunctionLibrary")
		static void GetAllPossibleTrackLengths(ATrainTrack * i_CurrentTrack, float i_CurrentTravelDistance, FString i_CurrentPath, TMap<FString, int32> & o_TrackLengths);

	UFUNCTION(BlueprintCallable, Category = "Handcar|FunctionLibrary")
		static bool const TrackHasBranchingTrigger(ATrainTrack * i_CurrentTrack, ALeanJunctionSwitchTrigger *& o_FoundJunctionTrigger, AJumpTrigger *& o_FoundJumpTrigger);

};

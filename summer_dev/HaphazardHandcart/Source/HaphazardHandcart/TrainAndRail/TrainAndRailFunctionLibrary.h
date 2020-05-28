#pragma once

#include "TrainAndRail/TrainTrack.h"
#include "TrainAndRail/Structs/S_TrackPassThroughInfo.h"
#include "TrainAndRail/Structs/S_TrackSpline.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TrainAndRailFunctionLibrary.generated.h"

class USplineComponent;

UCLASS()
class HAPHAZARDHANDCART_API UTrainAndRailFunctionLibrary : public UBlueprintFunctionLibrary
{

	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static inline FName GetTrackIgnoreTag() { return "ChooChooIgnore"; }

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static TArray<USplineComponent*> GetUsableSplinesFromTrack(const ATrainTrack * i_Track);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static FVector GetStartLocationOfSpline(const USplineComponent * i_SplineComp);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static FVector GetEndLocationOfSpline(const USplineComponent * i_SplineComp);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static float GetDistanceBetweenLocations(FVector & i_PointA, FVector & i_PointB);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static bool IsValidTrackSplineIndex(ATrainTrack * i_Track, int32 i_SplineIndex);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static bool HasTrackSplines(ATrainTrack * i_Track);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static int32 GetClosestSplinePoint(const USplineComponent * i_SplineComp, TArray<int32> i_IgnorePoints, FVector & i_Location);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static float GetClosestDistanceOnSpline(const USplineComponent * i_SplineComp, FVector & i_WorldLocation, int32 i_DistanceSampleCount, bool & o_ValidDistance);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static float GetClosestDistanceBetweenSplinePoints(const USplineComponent * i_SplineComp, int32 i_PointA, int32 i_PointB, FVector & i_WorldLocation, int32 i_NumDistanceSamples);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static TArray<FS_TrackPassThroughInfo> FindLocationOnTracks(ATrainTrack * i_StartingTrack, const FS_TrackSpline & i_StartingTrackSpline, float i_StartingDistance,
			float i_RequestedDistance, bool i_Invert, TArray<FS_TrackPassThroughInfo> & i_CurrentPassThroughInfo,
			ATrainTrack *& o_EndTrack, FS_TrackSpline & o_EndTrackSpline, float & o_EndDistance, bool & o_EndReached, bool & o_InvertedDirOfTravel);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|FunctionLibrary")
		static inline FName GetInternalCompTag() { return "ChooChooAutoAdded"; }

};
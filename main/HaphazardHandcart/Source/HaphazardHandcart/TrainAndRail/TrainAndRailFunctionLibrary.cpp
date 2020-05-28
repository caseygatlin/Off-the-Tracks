#include "TrainAndRailFunctionLibrary.h"

#include "Components/SplineComponent.h"
#include "EngineGlobals.h"
#include "Math/UnrealMathUtility.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#include <limits>

UTrainAndRailFunctionLibrary::UTrainAndRailFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TArray<USplineComponent*> UTrainAndRailFunctionLibrary::GetUsableSplinesFromTrack(const ATrainTrack * i_Track)
{
	TArray<USplineComponent*> SplineComponents;
	i_Track->GetComponents<USplineComponent>(SplineComponents);

	TArray<USplineComponent*> UsableSplineComps;
	for (USplineComponent * SplineComp : SplineComponents)
	{
		if (!SplineComp->ComponentHasTag(GetTrackIgnoreTag()))
			UsableSplineComps.Add(SplineComp);
	}

	return UsableSplineComps;
}

FVector UTrainAndRailFunctionLibrary::GetStartLocationOfSpline(const USplineComponent * i_SplineComp)
{
	return i_SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
}

FVector UTrainAndRailFunctionLibrary::GetEndLocationOfSpline(const USplineComponent * i_SplineComp)
{
	int32 index = i_SplineComp->GetNumberOfSplinePoints() - 1;
	return i_SplineComp->GetLocationAtSplinePoint(index, ESplineCoordinateSpace::World);
}

float UTrainAndRailFunctionLibrary::GetDistanceBetweenLocations(FVector & i_PointA, FVector & i_PointB)
{
	return (i_PointA - i_PointB).Size();
}

bool UTrainAndRailFunctionLibrary::IsValidTrackSplineIndex(ATrainTrack * i_Track, int32 i_SplineIndex)
{
	return i_SplineIndex < i_Track->GetTrackSplines().Num();
}

bool UTrainAndRailFunctionLibrary::HasTrackSplines(ATrainTrack * i_Track)
{
	return i_Track->GetTrackSplines().Num() > 0;
}

int32 UTrainAndRailFunctionLibrary::GetClosestSplinePoint(const USplineComponent * i_SplineComp, TArray<int32> i_IgnorePoints, FVector & i_Location)
{
	int32 returnIndex = -1;

	int32 numSplinePoints = i_SplineComp->GetNumberOfSplinePoints();

	// if we have enough spline points
	if (numSplinePoints >= 2)
	{
		float distanceToPoint = std::numeric_limits<float>::max();
		// iterate through spline points to find closest spline point
		for (int32 i = 0; i < numSplinePoints; i++)
		{
			// if we aren't ignoring this point
			if (!i_IgnorePoints.Contains(i))
			{
				FVector splinePointLoc = i_SplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				float distanceBetweenCur = GetDistanceBetweenLocations(i_Location, splinePointLoc);

				if (distanceBetweenCur < distanceToPoint)
				{
					distanceToPoint = distanceBetweenCur;
					returnIndex = i;
				}
			}
		}
	}

	return returnIndex;
}

float UTrainAndRailFunctionLibrary::GetClosestDistanceOnSpline(const USplineComponent * i_SplineComp, FVector & i_WorldLocation, 
	int32 i_DistanceSampleCount, bool & o_ValidDistance)
{
    // Make sure DistanceSampleCount is non-negative,
    // Set to zero and print debug if so
    if (i_DistanceSampleCount < 0)
    {
#ifdef UE_BUILD_DEBUG
        FString errorText = "Invalid distance sample count: ";          // Invalid distance
        errorText.AppendInt(i_DistanceSampleCount);                     // [DistanceSampleCount]
        errorText.Append(FString(", ensure value is a non-negative.")); // Make sure it's non-negative
        errorText.Append(FString(" Setting value to zero..."));         // Setting value to zero.

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
#endif

        i_DistanceSampleCount = 0;
    }

	float lowestDistance = -1;

	// get the closest two spline points to the location
	TArray<int32> ignorePoints;
	int32 pointA = GetClosestSplinePoint(i_SplineComp, ignorePoints, i_WorldLocation);

	ignorePoints.Add(pointA);
	int32 pointB = GetClosestSplinePoint(i_SplineComp, ignorePoints, i_WorldLocation);

	// are both points valid
	if (pointA != -1 && pointB != -1)
	{
		// get the closest location between the 2 points
		lowestDistance = GetClosestDistanceBetweenSplinePoints(i_SplineComp, pointA, pointB, i_WorldLocation, i_DistanceSampleCount);
		o_ValidDistance = true;
	}

	return lowestDistance;
}

float UTrainAndRailFunctionLibrary::GetClosestDistanceBetweenSplinePoints(const USplineComponent * i_SplineComp, int32 i_PointA, int32 i_PointB, FVector & i_WorldLocation,
	int32 i_NumDistanceSamples)
{
	if (i_NumDistanceSamples < 0)
	{
		i_NumDistanceSamples = 0;
	}

	// lowest point as the start point, highest as the end
	int32 endPoint = (i_PointA > i_PointB) ? i_PointA : i_PointB;
	int32 startPoint = (endPoint == i_PointB) ? i_PointA : i_PointB;

	float startDistance = i_SplineComp->GetDistanceAlongSplineAtSplinePoint(startPoint);
	float endDistance = i_SplineComp->GetDistanceAlongSplineAtSplinePoint(endPoint);

	float distanceSample = (endDistance - startDistance) / static_cast<float>(i_NumDistanceSamples);
	float currentSampleDistance = startDistance;

	// Get the location at the distance and check it against our closest dsitance
	// update and keep checking unitl we hit the end
	float returnDistance = std::numeric_limits<float>::max();
	float distanceToClosestPoint = std::numeric_limits<float>::max();

	for (int32 i = 0; i < i_NumDistanceSamples; i++)
	{
		currentSampleDistance = (distanceSample * i) + startDistance;

		FVector locationAtIndex = i_SplineComp->GetLocationAtDistanceAlongSpline(currentSampleDistance, ESplineCoordinateSpace::World);
		float distanceBetweenAtIndex = GetDistanceBetweenLocations(locationAtIndex, i_WorldLocation);
		if (distanceBetweenAtIndex <= distanceToClosestPoint)
		{
			distanceToClosestPoint = distanceBetweenAtIndex;
			returnDistance = currentSampleDistance;
		}
	}
	return returnDistance;
}

TArray<FS_TrackPassThroughInfo> UTrainAndRailFunctionLibrary::FindLocationOnTracks(ATrainTrack * i_StartingTrack, const FS_TrackSpline & i_StartingTrackSpline, 
	float i_StartingDistance, float i_RequestedDistance, bool i_Invert, TArray<FS_TrackPassThroughInfo> & i_CurrentPassThroughInfo,
	ATrainTrack *& o_EndTrack, FS_TrackSpline & o_EndTrackSpline, float & o_EndDistance, bool & o_EndReached, bool & o_InvertedDirOfTravel)
{
	// invert requested distance
	if (i_Invert)
		i_RequestedDistance *= -1;

	// Set the spline length
	float startTrackSplineLen = i_StartingTrackSpline.Spline->GetSplineLength();

	// If distance is +, then it's going forward down the tracks
	if (i_RequestedDistance < 0)
	{
		if (i_StartingDistance >= FMath::Abs(i_RequestedDistance))
		{
			// create pass through entry for this track
			FS_TrackPassThroughInfo newPassThroughInfo = FS_TrackPassThroughInfo();
			newPassThroughInfo.Track = i_StartingTrack;
			newPassThroughInfo.TrackSpline = i_StartingTrackSpline;
			float lowest = (i_StartingDistance < i_StartingDistance + i_RequestedDistance) ? i_StartingDistance : i_StartingDistance + i_RequestedDistance;
			float highest = (i_StartingDistance > i_StartingDistance + i_RequestedDistance) ? i_StartingDistance : i_StartingDistance + i_RequestedDistance;
			newPassThroughInfo.StartDistance = lowest;
			newPassThroughInfo.EndDistance = highest;
			newPassThroughInfo.Invert = i_Invert;
			i_CurrentPassThroughInfo.Add(newPassThroughInfo);

			// send back final location
			o_EndTrack = i_StartingTrack;
			o_EndTrackSpline = i_StartingTrackSpline;
			o_EndDistance = i_StartingDistance + i_RequestedDistance;
			o_EndReached = true;
			o_InvertedDirOfTravel = i_Invert;
			return i_CurrentPassThroughInfo;
		}
		else
		{
			// Create the pass through info, inverted travel can lead to high numbers at the start, so use the fun to make sure order is preserved
			FS_TrackPassThroughInfo newPassThroughInfo = FS_TrackPassThroughInfo();
			newPassThroughInfo.Track = i_StartingTrack;
			newPassThroughInfo.TrackSpline = i_StartingTrackSpline;
			float clampedDistance = FMath::Clamp(i_RequestedDistance + i_StartingDistance, 0.0f, startTrackSplineLen);
			float lowest = (i_StartingDistance < clampedDistance) ? i_StartingDistance : clampedDistance;
			float highest = (i_StartingDistance > clampedDistance) ? i_StartingDistance : clampedDistance;
			newPassThroughInfo.StartDistance = lowest;
			newPassThroughInfo.EndDistance = highest;
			newPassThroughInfo.Invert = i_Invert;
			i_CurrentPassThroughInfo.Add(newPassThroughInfo);

			//Update the required distance with the distance traveled on this spline
			i_RequestedDistance += i_StartingDistance;
			bool connectionFound = false;
			FS_TrackConnection connection = i_StartingTrack->PickConnectionToUse(i_StartingTrackSpline.StartConnectionPoint, false, connectionFound);

			// Here we are going backwards on a spline, so we expect the spline we are connecting into to be an end location,
			// if it's not, we need to invert the required distance and adjust our starting location on the connecting spline
			if (connectionFound)
			{
				bool doInvert = (connection.Location == E_TrackSplineLocation::Start);
				bool validTrackSpline = false;
				FS_TrackSpline trackSpline = connection.Track->GetTrackSplineByIndex(connection.ConnectionID, validTrackSpline);
				float newStartingDistance = doInvert ? 0.0f : trackSpline.Spline->GetSplineLength();

				// call this function recursevly
				return FindLocationOnTracks(connection.Track, trackSpline, newStartingDistance, i_RequestedDistance, doInvert, i_CurrentPassThroughInfo,
					o_EndTrack, o_EndTrackSpline, o_EndDistance, o_EndReached, o_InvertedDirOfTravel);
			}
			// no revelant info found
			else
			{
				// send back error info
				o_EndTrack = i_StartingTrack;
				o_EndTrackSpline = i_StartingTrackSpline;
				o_EndDistance = 0.0f;
				o_EndReached = false;
				o_InvertedDirOfTravel = i_Invert;
				return i_CurrentPassThroughInfo;
			}
		}
	}
	else
	{
		if (startTrackSplineLen >= i_RequestedDistance + i_StartingDistance)
		{
			// create pass through entry for this track
			FS_TrackPassThroughInfo newPassThroughInfo = FS_TrackPassThroughInfo();
			newPassThroughInfo.Track = i_StartingTrack;
			newPassThroughInfo.TrackSpline = i_StartingTrackSpline;
			float lowest = (i_StartingDistance < i_StartingDistance + i_RequestedDistance) ? i_StartingDistance : i_StartingDistance + i_RequestedDistance;
			float highest = (i_StartingDistance > i_StartingDistance + i_RequestedDistance) ? i_StartingDistance : i_StartingDistance + i_RequestedDistance;
			newPassThroughInfo.StartDistance = lowest;
			newPassThroughInfo.EndDistance = highest;
			newPassThroughInfo.Invert = i_Invert;
			i_CurrentPassThroughInfo.Add(newPassThroughInfo);

			// send back final location
			o_EndTrack = i_StartingTrack;
			o_EndTrackSpline = i_StartingTrackSpline;
			o_EndDistance = i_StartingDistance + i_RequestedDistance;
			o_EndReached = true;
			o_InvertedDirOfTravel = i_Invert;
			return i_CurrentPassThroughInfo;
		}
		else
		{
			// create pass through entry for this track
			FS_TrackPassThroughInfo newPassThroughInfo = FS_TrackPassThroughInfo();
			newPassThroughInfo.Track = i_StartingTrack;
			newPassThroughInfo.TrackSpline = i_StartingTrackSpline;
			float otherDistance = i_StartingDistance + (startTrackSplineLen - i_StartingDistance);
			float lowest = (i_StartingDistance < otherDistance) ? i_StartingDistance : otherDistance;
			float highest = (i_StartingDistance > otherDistance) ? i_StartingDistance : otherDistance;
			newPassThroughInfo.StartDistance = lowest;
			newPassThroughInfo.EndDistance = highest;
			newPassThroughInfo.Invert = i_Invert;
			i_CurrentPassThroughInfo.Add(newPassThroughInfo);

			// Update the required distance with the distance used on this spline
			i_RequestedDistance -= (startTrackSplineLen - i_StartingDistance);
			bool connectionFound = false;
			FS_TrackConnection connection = i_StartingTrack->PickConnectionToUse(i_StartingTrackSpline.EndConnectionPoint, false, connectionFound);

			if (connectionFound)
			{
				bool doInvert = (connection.Location == E_TrackSplineLocation::End);
				bool validTrackSpline = false;
				FS_TrackSpline trackSpline = connection.Track->GetTrackSplineByIndex(connection.ConnectionID, validTrackSpline);
				float newStartingDistance = doInvert ? trackSpline.Spline->GetSplineLength() : 0.0f;

				// call this function recursevly
				return FindLocationOnTracks(connection.Track, trackSpline, newStartingDistance, i_RequestedDistance, doInvert, i_CurrentPassThroughInfo,
					o_EndTrack, o_EndTrackSpline, o_EndDistance, o_EndReached, o_InvertedDirOfTravel);
			}
			else
			{
				// send back error info
				o_EndTrack = i_StartingTrack;
				o_EndTrackSpline = i_StartingTrackSpline;
				o_EndDistance = startTrackSplineLen;
				o_EndReached = false;
				o_InvertedDirOfTravel = i_Invert;
				return i_CurrentPassThroughInfo;
			}
		}
	}
}
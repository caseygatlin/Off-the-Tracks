// Fill out your copyright notice in the Description page of Project Settings.


#include "Bogie.h"
#include "TrainTrack.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SplineComponent.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/KismetMathLibrary.h"
#include "Structs/S_TrackConnectionPoint.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>


// Sets default values for this component's properties
UBogie::UBogie()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UBogie::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UBogie::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UBogie::ChangeTrack(ATrainTrack* i_Track, int32 i_TrackSplineIndex, float i_DistanceOnTrack)
{
    int numTrackSplines = i_Track->GetTrackSplines().Num();

    if (i_TrackSplineIndex < numTrackSplines)
    {
        // Set track and spline member variables
        _Track = i_Track;
        _SplineIndex = i_TrackSplineIndex;
        _SplineComponent = i_Track->GetTrackSplines()[_SplineIndex].Spline;
        _SplineLength = _SplineComponent->GetSplineLength();
		if (i_DistanceOnTrack > _SplineLength)
		{
			_SplineDistance = _SplineLength;
		}
		else
		{
			_SplineDistance = i_DistanceOnTrack;
		}
        _NeedsAlignment = false;
    }

#ifdef UE_BUILD_DEBUG
    else
    {
        // If spline index is out of range
        FString errorText = "Bogie asked to change to track: ";             // Asked to change track
        errorText.Append(UKismetSystemLibrary::GetDisplayName(i_Track));    // [Track name]
        errorText.Append(FString(" but spline index: "));                   // but spline index
        errorText.Append(FString::FromInt(i_TrackSplineIndex));             // [Index]
        errorText.Append(FString(" is out of bounds: "));                   // is out of bounds
        errorText.Append(FString::FromInt(numTrackSplines));                // [Bounds]

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
    }
#endif
}

void UBogie::TickMovement(float i_Speed, int32 i_UpdateID)
{
    _UpdateID = i_UpdateID;
    _PreviousSplineDistance = _SplineDistance;

    // Increase our distance on the spline, if the direction of travel is inverted, subtract the increase
	float speedDir = _InvertDirOfTravel ? i_Speed * -1 : i_Speed;
    _SplineDistance += speedDir;
}

FTransform UBogie::GetTransform() const
{
    FTransform returnTransform;
    FTransform transformAtDistance = _SplineComponent->GetTransformAtDistanceAlongSpline(_SplineDistance, ESplineCoordinateSpace::World);

    // If traveling against the spline, rotate the rotator by 180 to keep things looking correct
    if (_InvertDirOfTravel)
    {
        FRotator rotator = { 0.f, 0.f, 180.f };
        FRotator returnRotator = UKismetMathLibrary::ComposeRotators(rotator, transformAtDistance.GetRotation().Rotator());

        returnTransform.SetLocation(transformAtDistance.GetLocation());
        returnTransform.SetRotation(returnRotator.Quaternion());
        returnTransform.SetScale3D(transformAtDistance.GetScale3D());
    }
    // If the dir of travel isn't inverted, return the normal transform
    else
    {
        returnTransform = transformAtDistance;
    }

    return returnTransform;
}

float UBogie::GetDistanceToEndOfTrack(float i_Speed, bool& o_AtSplineEnd)
{
    // If the speed is negative
    if (i_Speed < 0.f)
    {
        // If direction of travel is inverted
        if (_InvertDirOfTravel)
        {
            o_AtSplineEnd = true;
            return (_SplineLength - _SplineDistance);
        }
        else
        {
            o_AtSplineEnd = false;
            return (_SplineDistance);
        }
    }
    else
    {
        // If direction of travel is inverted
        if (_InvertDirOfTravel)
        {
            o_AtSplineEnd = false;
            return (_SplineDistance);
        }
        else
        {
            o_AtSplineEnd = true;
            return (_SplineLength - _SplineDistance);
        }
    }
}

// Return the connection point for the track spline the bogie is currently on
FS_TrackConnectionPoint UBogie::GetConnectionPoint(bool i_GetEndPoint, bool& o_isEndPoint)
{
    o_isEndPoint = i_GetEndPoint;

    if (i_GetEndPoint)
    {
        return _Track->GetTrackSplines()[_SplineIndex].EndConnectionPoint;
    }
    else
    {
        return _Track->GetTrackSplines()[_SplineIndex].StartConnectionPoint;
    }

}

void UBogie::UpdateDistanceForEndOfClosedLoop(float i_Speed)
{
    _SplineDistance = i_Speed - _DistanceToEndOfTrack;
}

void UBogie::RestorePreviousDistance(int32 i_UpdateReverseID)
{
    if (i_UpdateReverseID == _UpdateID)
    {
        _SplineDistance = _PreviousSplineDistance;
    }
}

FVector UBogie::GetLocation() const
{
    return (GetTransform().GetLocation());
}

FRotator UBogie::GetRotation() const
{
    return (GetTransform().GetRotation().Rotator());
}

bool UBogie::IsSensorOrHelperBogie() const
{
    return _IsSensorOrHelperBogie;
}

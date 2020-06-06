// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/S_TrackConnectionPoint.h"
#include "Bogie.generated.h"

class USplineComponent;
class ATrainTrack;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HAPHAZARDHANDCART_API UBogie : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBogie();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Helper Methods

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		void ChangeTrack(ATrainTrack* i_Track, int32 i_TrackSplineIndex, float i_DistanceOnTrack);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		void TickMovement(float i_Speed, int32 i_UpdateID);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		FTransform GetTransform() const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		float GetDistanceToEndOfTrack(float i_Speed, bool& o_AtSplineEnd);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		FS_TrackConnectionPoint GetConnectionPoint(bool i_GetEndPoint, bool& o_isEndPoint);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		void UpdateDistanceForEndOfClosedLoop(float i_Speed);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		void RestorePreviousDistance(int32 i_UpdateReverseID);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		FVector GetLocation() const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		FRotator GetRotation() const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Bogie")
		bool IsSensorOrHelperBogie() const;

#pragma endregion

#pragma region Getters and Setters

	inline FString GetDebugName() const { return _DebugName; }
	inline int32 GetDistanceFromHandcar() const { return _DistanceFromHandcar; }
	inline bool GetIgnoreEndOfTrack() const { return _IgnoreEndOfTrack; }
	inline bool GetInvertDirOfTravel() const { return _InvertDirOfTravel; }
	inline bool GetNeedsAlignment() const { return  _NeedsAlignment; }
	ATrainTrack * GetTrack() const { return _Track; }
	inline USplineComponent * GetSplineComponent() const { return _SplineComponent; }
	inline float GetSplineDistance() const { return _SplineDistance; }
	inline int32 GetSplineIndex() const { return _SplineIndex; }

	void SetDebugName(FString i_DebugName) { _DebugName = i_DebugName; }
	void SetDistanceFromHandcar(int32 i_DistanceFromHandcar) { _DistanceFromHandcar = i_DistanceFromHandcar; }
	void SetIgnoreEndOfTrack(bool i_IgnoreEndOfTrack) { _IgnoreEndOfTrack = i_IgnoreEndOfTrack; }
	void SetInvertDirOfTravel(bool i_InvertDirOfTravel) { _InvertDirOfTravel = i_InvertDirOfTravel; }
	void SetIsSensorOrHelperBogie(bool i_IsSensorOrHelperBogie) { _IsSensorOrHelperBogie = i_IsSensorOrHelperBogie; }

#pragma endregion

private:

#pragma region Private Variables

#pragma region Components

	UPROPERTY(VisibleAnywhere, Category = Components)
		USplineComponent* _SplineComponent;

#pragma endregion

#pragma region Bogie Settings

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Distance on the current spline
		float _SplineDistance = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Length of the current spline
		float _SplineLength = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Index of the current spline in the current track’s splines
		int _SplineIndex = 0;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Distance on the current spline
		ATrainTrack* _Track;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Distance to the end of this track / spline
		float _DistanceToEndOfTrack = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		bool _NeedsAlignment = false;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Debug name used when printing info about this bogie
		FString _DebugName = FString(TEXT("Bogie"));

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// If set to true, this bogie will not fire events for the parent Handcar when it reaches the end of the track
		bool _IgnoreEndOfTrack = false;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Used by triggers and end of track notifications. If set to true, this bogie will be handled as a helper bogie 
		bool _IsSensorOrHelperBogie = false;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Used to indicate of this bogie is traveling against the spline. See docs for full details on this.
		bool _InvertDirOfTravel = false;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// How far this bogie should be from the Handcar. This is used for alignment and other distance / placement calculations
		float _DistanceFromHandcar = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "BoggieSettings")
		// Distance on the current spline
		float _PreviousSplineDistance = 0.0f;

#pragma endregion

	UPROPERTY(VisibleAnywhere)
		int32 _UpdateID = 0;

#pragma endregion

};

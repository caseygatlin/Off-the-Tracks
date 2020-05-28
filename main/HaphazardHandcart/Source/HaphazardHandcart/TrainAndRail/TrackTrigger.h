// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackTrigger.generated.h"


class AHandcar;
class ATrainCarriage;
class ATrainTrack;
class UBogie;

class UBillboardComponent;
class USphereComponent;

UCLASS()
class HAPHAZARDHANDCART_API ATrackTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrackTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Overide this for custom track triggers
	virtual void CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar);

#pragma region Editor Events

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditUndo() override;
	virtual void PostEditUndo() override;

	// Called after an actor has been moved in the editor
	virtual void PostEditMove(bool bFinished);
#endif // WITH_EDITOR

#pragma endregion

#pragma region Helper Methods

    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Initialization")
        void InitializeTrackTrigger();
	
    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Initialization")
        void AttachToTrackAndBuildRegions();
	
    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Enable & Disable")
        void DisableTrigger();
	
    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Enable & Disable")
        void EnableAndResetTrigger();

    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Enable Timer")
        void TickEnableTimer(const float& i_dt);
	
    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Enable Timer")
        void ResetEnableTimer();

    UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Track Trigger|Trigger Event")
        bool TriggerFireCheck(const UBogie* i_Bogie, AHandcar * i_Handcar);

#pragma endregion

#pragma region TriggerEventMethods

	DECLARE_EVENT_TwoParams(ATrackTrigger, FFireEvent, UBogie*, AHandcar*)
	FFireEvent& OnFireTrackTrigger() { return _FireTrackTriggerEvent; }

    UFUNCTION(BlueprintCallable, Category = "Track Trigger|Trigger Event")
        void FireTrackTrigger(UBogie * i_Bogie, AHandcar * i_Handcar);
	
    UFUNCTION(BlueprintCallable, Category = "Track Trigger|Trigger Event")
        void SubscribeToFireEvent(UBogie * i_Bogie, AHandcar * i_Handcar);

#pragma endregion

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBillboardComponent* m_EditorIconBB;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent* m_RadiusSphere;

#pragma endregion

#pragma region Track

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
		// Track this trigger is attached to.
		ATrainTrack* m_Track;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
		// Track spline index this trigger should attach to.
		int32 m_TrackSplineIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
		// Distance on track from the attach point in both directions for this trigger.
		float m_TriggerRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track")
		// Sample count to use when attaching to the track
		int32 m_DistanceSampleCount = 100;

#pragma endregion

#pragma region TriggerOptions

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// Bool indicating if this trigger should apply to all track splines.
		bool m_ApplyToAllTrackSplines = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// Bool indicating if this trigger is active or not.
		bool m_Active = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// If true, the trigger will not fire if the triggering bogie is a helper or sensor bogie.
		bool m_IgnoreHelperBogies = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// Trigger only once.
		bool m_TriggerOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// If true, the once triggered the trigger will become active again once the number of seconds set in ResetDelay has passed.
		bool m_UseEnableTimer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// Time in seconds between resets after the trigger has been fired. UseEnableTimer must be set to true for this to be used.
		float m_ResetDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Options")
		// If set to true, the trigger keeps track of which carriages have fired the trigger. When the trigger is reset, this will be reset.
		bool m_TriggerOncePerCarriage = false;

#pragma endregion

#pragma endregion

private:

#pragma region TriggerEvents

	FFireEvent _FireTrackTriggerEvent;

#pragma endregion

#pragma region Private Variables

#pragma region Track

	UPROPERTY(VisibleAnywhere, Category = "Track")
		// Distance on the attached spline, this is automatically set, do not edit manually.
		float _AutoSet_DistanceOnSpline = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Track")
		// Array of tracks this trigger is attached to.
		TArray<ATrainTrack*> _TracksWithRegions;

#pragma endregion

#pragma region TriggerOptions

	UPROPERTY(VisibleAnywhere, Category = "Trigger Options")
		// Internal count down used for reset. This is automatically set, do not edit manually.
		float _TimeRemainingForReset = 5.0f;

	UPROPERTY(VisibleAnywhere, Category = "Trigger Options")
		//Internal array used to track which carriages have fired the trigger. This is set automatically, do not edit manually.
		TArray<AHandcar*> _TriggeredHandcars;

#pragma endregion

#pragma endregion
};

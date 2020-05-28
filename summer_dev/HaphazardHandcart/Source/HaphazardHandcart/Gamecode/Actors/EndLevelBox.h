// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "EndLevelBox.generated.h"

class AEndTrack;
class UBoxComponent;
class UPrimitiveComponent;


UCLASS()
class HAPHAZARDHANDCART_API AEndLevelBox : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AEndLevelBox();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PreEditUndo() override;
    virtual void PostEditUndo() override;

    // Called after an actor has been moved in the editor
    virtual void PostEditMove(bool bFinished);
#endif // WITH_EDITOR

#pragma region Helper Methods

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Custom Actors|End Level Box|Blueprint Events")
        // Function to execute before end level delay.
        void PostDelayEndLevel();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Custom Actors|End Level Box|Blueprint Events")
        // Function to execute after end level delay.
        void PreDelayEndLevel(APlayerHandcar* i_PlayerHandcar);

    UFUNCTION(BlueprintCallable, Category = "Custom Actors|End Level Box|Triggers")
        // Triggers PreDelay, Delay, and PostDelay functions.
        // Called on overlap.
        void TriggerEndLevel(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable, Category = "Custom Actors|End Level Box|Level Loading")
        // Restarts the current level.
        void RestartLevel();

    UFUNCTION(BlueprintCallable, Category = "Custom Actors|End Level Box|Player Handcar")
        // Disables handcar input and applies brakes.
        void StopHandcar(APlayerHandcar* i_PlayerHandcar);

    UFUNCTION(BlueprintCallable, Category = "Custom Actors|End Level Box|Track")
        void AttachToTrack();

#pragma endregion

#pragma region Public Variables

#pragma region Components

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
        // The box collider.
        UBoxComponent* m_CollisionBox;

#pragma endregion

#pragma region Track Settings

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
        // The track to which this box trigger is attached.
        AEndTrack* m_AttachedTrack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
        // Index of the spline on the track to which this box trigger is attached.
        int32 m_AttachedTrackSplineIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
        // Number of distance samples to check when attaching this carriage to the track.
        int32 m_AttachDistanceSampleCount = 100;

#pragma endregion

#pragma region Delay

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delay")
        // The amount of time to delay before the level should move on.
        float m_EndLevelDelayTime = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delay")
        // Handle for the delay.
        FTimerHandle m_EndLevelDelayHandle;

#pragma endregion

#pragma region Delegates

    // Delegate called on enter overlap.
    FScriptDelegate m_OverlapDelegate;

#pragma endregion

#pragma endregion

private:

#pragma region Private Variables

#pragma region Track Settings

    UPROPERTY(VisibleAnywhere, Category = "Track Settings")
        // Distance on the spline. This is auto set, do not edit manually.
        float _AutoSet_DistanceOnSpline = 0.0f;

#pragma endregion

#pragma endregion

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DetachedCamera.generated.h"

class UCameraComponent;
class USpringArmComponent;
class APlayerHandcar;
class UBoxComponent;

UCLASS()
class HAPHAZARDHANDCART_API ADetachedCamera : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ADetachedCamera();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#if WITH_EDITOR

    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // Called after an actor has been moved in the editor
    virtual void PostEditMove(bool bFinished);

#endif // WITH_EDITOR


#pragma region Helper Functions

    UFUNCTION(BlueprintCallable, Category = "Custom Actors|Detached Camera|Tracking")
        // Adjusts position of AttachmentBox to track the specified actor.
        void TrackHandcar();

#pragma endregion
    
#pragma region Public Variables

#pragma region Components

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
        // The spring arm component.
        USpringArmComponent* m_SpringArm;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
        // The camera component.
        UCameraComponent* m_Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
        // Box collision that matches the position of the tracked object.
        UBoxComponent* m_AttachmentBox;

#pragma endregion

#pragma region Handcar

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handcar")
        // The object for the camera to track.
        APlayerHandcar* m_PlayerHandcar;

#pragma endregion

#pragma region Offset

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
        // Whether the camera should treat the tracked object as player 1 or not.
        // Flips the camera view.
        bool isPlayer1 = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
        // The vertical distance from the tracked object
        float m_VerticalOffset = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
        // The horizontal distance from the tracked object.
        float m_HorizontalOffset = 700.f;

#pragma endregion

#pragma endregion 

};

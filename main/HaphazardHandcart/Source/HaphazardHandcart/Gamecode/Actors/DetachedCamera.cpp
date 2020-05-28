// Fill out your copyright notice in the Description page of Project Settings.


#include "DetachedCamera.h"
#include "../Handcars/PlayerHandcar.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectGlobals.h"

// Sets default values
ADetachedCamera::ADetachedCamera()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create components
    m_AttachmentBox = CreateDefaultSubobject<UBoxComponent>("Attachment Box");
    m_SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
    m_Camera = CreateDefaultSubobject<UCameraComponent>("Camera");

    //Setup springarm
    m_SpringArm->SetupAttachment(m_AttachmentBox);
    m_SpringArm->TargetArmLength = m_HorizontalOffset;
    m_SpringArm->bInheritRoll = false;

    //Setup camera
    m_Camera->SetupAttachment(m_SpringArm);
    FRotator cameraRotation = FRotator(352.f, 0.f, 0.f);
    m_Camera->SetRelativeRotation(cameraRotation.Quaternion());

    // Set root component
    SetRootComponent(m_AttachmentBox);
}

// Called when the game starts or when spawned
void ADetachedCamera::BeginPlay()
{
    Super::BeginPlay();

    // Set camera to track player
    TrackHandcar();

	int32 numControllers = GetWorld()->GetNumPlayerControllers();

    // If this camera is player one's controller, set the first controller's view target to this camera.
    if (isPlayer1)
    {
        APlayerController* playerController = GetWorld()->GetFirstPlayerController();
        playerController->SetViewTarget(this);
    }

	// If there's only one controller, add another and set its view target to this camera.
    else if (numControllers == 1)
    {
        APlayerController* playerController = UGameplayStatics::CreatePlayer(GetWorld());
        playerController->SetViewTarget(this);
    }

	// If there's already two controllers, get the second player controller and set its view target to this camera.
	else if (numControllers == 2)
	{
		int32 iterationCount = 0;
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (iterationCount == 1)
			{
				APlayerController* playerController = Iterator->Get();
				playerController->SetViewTarget(this);
			}
			iterationCount++;
		}
	}
}

#if WITH_EDITOR

void ADetachedCamera::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

    // Set camera to track player
    TrackHandcar();
}

void ADetachedCamera::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

    // Set camera to track player
    TrackHandcar();
}

#endif // WITH_EDITOR

// Called every frame
void ADetachedCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Set camera to track player
    TrackHandcar();
}

// Tracks the specified actor at the specified offsets.
void ADetachedCamera::TrackHandcar()
{
    if (IsValid(m_PlayerHandcar))
    {
        float playerRotationAdjust = 0.f;

        if (!isPlayer1)
        {
            playerRotationAdjust = 180.f;
        }

        // Adjust the length of the spring arm to the horizontal offset
        m_SpringArm->TargetArmLength = m_HorizontalOffset;

        // Adjust position and rotation of the AttachmentBox to match i_Actor with the vertical offset
        FVector actorPos = m_PlayerHandcar->m_HandcarMesh->GetComponentLocation() +FVector::UpVector * m_VerticalOffset;
        FRotator actorRotator = m_PlayerHandcar->m_HandcarMesh->GetComponentRotation();
        actorRotator.Yaw += playerRotationAdjust;
        FQuat actorQuat = actorRotator.Quaternion();

        m_AttachmentBox->SetWorldLocation(actorPos);
        m_AttachmentBox->SetWorldRotation(actorQuat);
    }
}


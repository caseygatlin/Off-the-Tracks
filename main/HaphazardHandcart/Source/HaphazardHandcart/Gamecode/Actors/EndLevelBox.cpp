// Fill out your copyright notice in the Description page of Project Settings.


#include "EndLevelBox.h"
#include "Gamecode/Handcars/PlayerHandcar.h"
#include "Gamecode/Tracks/EndTrack.h"
#include "Gamecode/DebugFunctionLibrary.h"
#include "TrainAndRail/TrainAndRailFunctionLibrary.h"

#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "TimerManager.h"

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

// Sets default values
AEndLevelBox::AEndLevelBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Initialize collision box
    m_CollisionBox = CreateDefaultSubobject<UBoxComponent>("Collision Box");
    m_CollisionBox->SetGenerateOverlapEvents(true);
    m_CollisionBox->SetCollisionProfileName("OverlapAll");

    // Bind Trigger function to overlap event
    m_OverlapDelegate.BindUFunction(this, "TriggerEndLevel");
    m_CollisionBox->OnComponentBeginOverlap.AddUnique(m_OverlapDelegate);
    m_CollisionBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
    m_CollisionBox->ShapeColor = FColor(0, 255, 152, 255);
}

// Called when the game starts or when spawned
void AEndLevelBox::BeginPlay()
{
	Super::BeginPlay();
	
    // Add this actor to the handcar's collision ignore list
    APlayerHandcar* playerHandcar = Cast<APlayerHandcar>(UGameplayStatics::GetActorOfClass(this, APlayerHandcar::StaticClass()));
    bool neverExpire = true;
    playerHandcar->AddActorToCollisionIgnoreList(this, 0.f, neverExpire);
}

// Called every frame
void AEndLevelBox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

#if WITH_EDITOR

void AEndLevelBox::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    AttachToTrack();
}

void AEndLevelBox::PreEditUndo()
{
    Super::PreEditUndo();
}

void AEndLevelBox::PostEditUndo()
{
    Super::PostEditUndo();
}

void AEndLevelBox::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);
    AttachToTrack();
}

#endif // WITH_EDITOR



void AEndLevelBox::TriggerEndLevel(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Trigger end of level sequence if overlapping handcar
    if (UKismetMathLibrary::ClassIsChildOf(OtherActor->GetClass(), APlayerHandcar::StaticClass()))
    {
        APlayerHandcar* playerHandcar = Cast<APlayerHandcar>(OtherActor);

        // Trigger Pre Delay function
        PreDelayEndLevel(playerHandcar);

        // Set Post Delay function to happen after a delay
        GetWorld()->GetTimerManager().SetTimer(m_EndLevelDelayHandle, this, &AEndLevelBox::PostDelayEndLevel, m_EndLevelDelayTime);
    }
}

void AEndLevelBox::RestartLevel()
{
    // Open this level again
    FName levelName = UKismetStringLibrary::Conv_StringToName(UGameplayStatics::GetCurrentLevelName(this));
    UGameplayStatics::OpenLevel(this, levelName);
}

// Disable input and apply brakes
void AEndLevelBox::StopHandcar(APlayerHandcar* i_PlayerHandcar)
{
    APlayerController* playerController = GetWorld()->GetFirstPlayerController();

    i_PlayerHandcar->DisableInput(playerController);
    i_PlayerHandcar->ApplyOrReleaseBrakes(true);
}

void AEndLevelBox::AttachToTrack()
{
    // Check we have a track and that the track spline index is valid.
    if (IsValid(m_AttachedTrack))
    {
        bool isSplineIndexValid = UTrainAndRailFunctionLibrary::IsValidTrackSplineIndex(m_AttachedTrack, m_AttachedTrackSplineIndex);
        bool areThereSplines = m_AttachedTrack->GetTrackSplines().Num() > 0;

        if (isSplineIndexValid && areThereSplines)
        {
            bool couldRegisterTrigger = true; /*m_AttachedTrack->RegisterTrackTrigger(this);*/

            if (couldRegisterTrigger)
            {
                // Generate the spline array to use
                TArray<FS_TrackSpline>  trackSplines;
                bool allTracksLoaded = false;

                // Make sure m_TrackSplineIndex is non-negative for GetTrackSplineByIndex
                if (m_AttachedTrackSplineIndex < 0)
                {
                    m_AttachedTrackSplineIndex = 0;
                }

                bool isValidTrackSpline = false;
                trackSplines.Add(m_AttachedTrack->GetTrackSplineByIndex(m_AttachedTrackSplineIndex, isValidTrackSpline));
                allTracksLoaded = isValidTrackSpline;

                if (allTracksLoaded && trackSplines.Num() > 0)
                {
                    // Snap to the closest point on the spline
                    USplineComponent* startSpline = trackSplines[0].Spline;

                    // Make sure m_DistanceSampleCount is non-negative for GetClosestDistanceOnSpline
                    if (m_AttachDistanceSampleCount < 0)
                    {
                        m_AttachDistanceSampleCount = 0;
                    }

                    FVector actorPos = GetActorLocation();

                    bool isValidDistance = false;
                    _AutoSet_DistanceOnSpline = UTrainAndRailFunctionLibrary::GetClosestDistanceOnSpline(startSpline, actorPos, m_AttachDistanceSampleCount, isValidDistance);

                    if (isValidDistance)
                    {
                        // Snap the trigger onto the spline
                        FVector snapLocation = startSpline->GetLocationAtDistanceAlongSpline(_AutoSet_DistanceOnSpline, ESplineCoordinateSpace::Type::World);
                        FRotator snapRotation = startSpline->GetRotationAtDistanceAlongSpline(_AutoSet_DistanceOnSpline, ESplineCoordinateSpace::Type::World);

                        FVector snapNormal = startSpline->GetUpVectorAtDistanceAlongSpline(_AutoSet_DistanceOnSpline, ESplineCoordinateSpace::Type::World);

                        float halfBoxHeight = m_CollisionBox->GetScaledBoxExtent().Z / 2.f;
                        snapLocation += snapNormal * halfBoxHeight;

                        SetActorLocation(snapLocation, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
                        SetActorRotation(snapRotation.Quaternion());
                    }
                    else
                    {
                        // If we failed to find a valid location on the spline, print the info and disable the trigger
                        FString errorText = "Failed to get a valid distance when creating End Level Box: ";     // Invalid distance
                        errorText.Append(UKismetSystemLibrary::GetDisplayName(this));                           // [End Level Box name]
                        errorText.Append(FString(" on track: "));                                               // on track
                        errorText.Append(UKismetSystemLibrary::GetDisplayName(m_AttachedTrack));                // [Attached track name]

                        UDebugFunctionLibrary::DebugPrintScreen(errorText);
                    }
                }
                else
                {
                    // If we failed to get the track spline, disable this trigger and print a warning
                    FString errorText = "Invalid spline index to use for End Level Box: ";      // Invalid spline index
                    errorText.Append(UKismetSystemLibrary::GetDisplayName(this));               // [End Level Box name]
                    errorText.Append(FString(" on track: "));                                   // on track
                    errorText.Append(UKismetSystemLibrary::GetDisplayName(m_AttachedTrack));    // [Attached track name]

                    UDebugFunctionLibrary::DebugPrintScreen(errorText);
                }
            }
            else
            {
                FString errorText = "Failed to attach End Level Box: ";                     // Failed to attach trigger:
                errorText.Append(UKismetSystemLibrary::GetDisplayName(this));               // [End Level Box name]
                errorText.Append(FString(" to track: "));                                   // To Track
                errorText.Append(UKismetSystemLibrary::GetDisplayName(m_AttachedTrack));    // [Attached track name]

                UDebugFunctionLibrary::DebugPrintScreen(errorText);
            }
        }
        else
        {
            FString errorText = UKismetSystemLibrary::GetDisplayName(this);                         // [End Level Box name]
            errorText.Append(FString(" - End Level Box can't attach to track, spline index: "));    // can't attach to track...
            errorText.AppendInt(m_AttachedTrackSplineIndex);                                        // [spline index]
            errorText.Append(FString(" is invalid, track only has "));                              // is invalid, track only has
            errorText.AppendInt(m_AttachedTrack->GetTrackSplines().Num());                          // [number of splines]
            errorText.Append(FString(" track splines"));                                            // track splines

            UDebugFunctionLibrary::DebugPrintScreen(errorText);
        }
    }
}


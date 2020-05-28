// Fill out your copyright notice in the Description page of Project Settings.


#include "EndTrack.h"
#include "Gamecode/Actors/EndLevelBox.h"
#include "Gamecode/Components/EndLevelBoxComponent.h"

#include "Components/ChildActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"


AEndTrack::AEndTrack()
{
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> TrackMesh (TEXT(
        "StaticMesh'/Game/HandCarContent/Geometry/Tracks/S_Track.S_Track'"));

    m_TrackMesh = TrackMesh.Object;

    m_ChildEndLevelBox = CreateDefaultSubobject<UEndLevelBoxComponent>("Child Actor Component");
    m_ChildEndLevelBox->SetupAttachment(RootComponent);
}

void AEndTrack::BeginPlay()
{
    Super::BeginPlay();

    m_ChildEndLevelBox->AssignTrack(this);
    m_ChildEndLevelBox->AttachToTrack();
}

void AEndTrack::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

#if WITH_EDITOR
void AEndTrack::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    m_ChildEndLevelBox->AssignTrack(this);
    m_ChildEndLevelBox->AttachToTrack();
}

void AEndTrack::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);

    m_ChildEndLevelBox->AssignTrack(this);
    m_ChildEndLevelBox->AttachToTrack();
}

#endif // WITH_EDITOR

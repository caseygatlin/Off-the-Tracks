// Fill out your copyright notice in the Description page of Project Settings.


#include "EndLevelBoxComponent.h"
#include "Gamecode/Actors/EndLevelBox.h"


UEndLevelBoxComponent::UEndLevelBoxComponent()
{
}


void UEndLevelBoxComponent::BeginPlay()
{
    if (IsValid(GetChildActor()))
    {
        Cast<AEndLevelBox>(GetChildActor())->m_EndLevelDelayTime = m_EndLevelDelayTime;
    }
}

#if WITH_EDITOR
void UEndLevelBoxComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    AttachToTrack();
}

void UEndLevelBoxComponent::PostEditComponentMove(bool bFinished)
{
    Super::PostEditComponentMove(bFinished);
    AttachToTrack();
}
#endif // WITH_EDITOR


void UEndLevelBoxComponent::AssignTrack(AEndTrack* i_EndTrack)
{
    if (IsValid(GetChildActor()))
    {
        Cast<AEndLevelBox>(GetChildActor())->m_AttachedTrack = i_EndTrack;
        AttachToTrack();
    }
}

void UEndLevelBoxComponent::AttachToTrack()
{
    if (IsValid(GetChildActor()))
    {
        Cast<AEndLevelBox>(GetChildActor())->AttachToTrack();
        FVector childLocation = GetChildActor()->GetActorLocation();
        SetWorldLocation(childLocation);
        GetChildActor()->SetActorLocation(childLocation);
    }
}

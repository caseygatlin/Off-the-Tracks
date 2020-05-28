// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "Gamecode/Actors/EndLevelBox.h"
#include "EndLevelBoxComponent.generated.h"

class AEndTrack;
/**
 * 
 */
UCLASS()
class HAPHAZARDHANDCART_API UEndLevelBoxComponent : public UChildActorComponent
{
	GENERATED_BODY()
	

public:

    UEndLevelBoxComponent();

    virtual void BeginPlay() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // Called after an actor has been moved in the editor
    virtual void PostEditComponentMove(bool bFinished);

#endif // WITH_EDITOR

#pragma region Helper Methods

    UFUNCTION(BlueprintCallable, Category = "Custom Components|End Level Box Component|Track")
        // Assign the track of the child End Level Box
        void AssignTrack(AEndTrack* i_EndTrack);

    UFUNCTION(BlueprintCallable, Category = "Custom Components|End Level Box Component|Track")
        // Attach the child End Level Box to its assigned track
        void AttachToTrack();

#pragma endregion

#pragma region Public Variables

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "End Level Box|Delay")
        // The amount of time to delay before triggering the PostDelayEndLevel in EndLevelBox.
        float m_EndLevelDelayTime = 3.f;

#pragma endregion

};

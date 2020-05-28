// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrainAndRail/TrainTrack.h"
#include "EndTrack.generated.h"


class UEndLevelBoxComponent;

/**
 * 
 */

UCLASS()
class HAPHAZARDHANDCART_API AEndTrack : public ATrainTrack
{
	GENERATED_BODY()

public:

    // Sets default values for this actor's properties
    AEndTrack();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // Called after an actor has been moved in the editor
    virtual void PostEditMove(bool bFinished);

#endif // WITH_EDITOR

#pragma region Public Variables

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
        UEndLevelBoxComponent* m_ChildEndLevelBox;

#pragma endregion
	
};

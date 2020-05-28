// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrainAndRail/TrackTrigger.h"
#include "LeanJunctionSwitchTrigger.generated.h"

class ATrainTrack;

UCLASS()
class HAPHAZARDHANDCART_API ALeanJunctionSwitchTrigger : public ATrackTrigger
{
	GENERATED_BODY()

public:

	ALeanJunctionSwitchTrigger();

	// Called when an instance of this class is placed (in editor) or spawned.
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar) override;

#pragma region Variables

#pragma region Switch Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwitchOptions)
		// Left track to use
		ATrainTrack * m_LeftTrack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwitchOptions)
		// Center track to use
		ATrainTrack * m_CenterTrack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SwitchOptions)
		// Right track to use
		ATrainTrack * m_RightTrack;

#pragma endregion

#pragma endregion
};

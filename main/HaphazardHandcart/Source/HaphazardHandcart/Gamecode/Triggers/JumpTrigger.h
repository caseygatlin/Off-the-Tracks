// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrainAndRail/TrackTrigger.h"
#include "JumpTrigger.generated.h"

class ATrainTrack;

UCLASS()
class HAPHAZARDHANDCART_API AJumpTrigger : public ATrackTrigger
{
	GENERATED_BODY()

public:

	AJumpTrigger();

	virtual void CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar) override;

#pragma region Variables

#pragma region Switch Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
		// Factor to adjust horizontal speed in the air.
		// 50 is close to the speed before jumping.
		float m_AirSpeedFactor = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
		// Upward jump force. Should always be postive.
		float m_Upforce = 800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
		// Possible start tracks of each section we can land on
		TArray<ATrainTrack*> m_LandingTracks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
		// Minimum horizontal speed. Should always be postive.
		float m_MinSpeed = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
		// Medium horizontal speed. Should always be between min and max.
		float m_MidSpeed = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings")
		// Maximum horizontal speed. Should always be greater than m_MinSpeed.
		float m_MaxSpeed = 90;

#pragma endregion

#pragma endregion
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrainAndRail/TrackTrigger.h"
#include "LeanObstacleTrigger.generated.h"

class UPaperSprite;
class UPaperSpriteComponent;
class UStaticMeshComponent;

UCLASS()
class HAPHAZARDHANDCART_API ALeanObstacleTrigger : public ATrackTrigger
{
	GENERATED_BODY()

public:

	ALeanObstacleTrigger();

	// Called when an instance of this class is placed (in editor) or spawned.
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar) override;

#pragma region Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
		UStaticMeshComponent * m_Obstacle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
		UPaperSpriteComponent * m_EditorLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
		UPaperSpriteComponent * m_EditorCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
		UPaperSpriteComponent * m_EditorRight;

#pragma endregion

#pragma region Visiblity

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visiblity")
		// Whether the sprites are visible
		bool m_Visible = true;

#pragma endregion

#pragma region Slowdown

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowdown")
		// How long the player slows down for
		float m_SlowdownTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowdown")
		// How much this object slows down the player
		float m_SlowStrength = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowdown")
		// How much to shake the camera based on the handcar speed
		float m_CameraShakeStrength = 500.0f;

#pragma endregion

#pragma region Sound

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		// Sound to play when player is hit
		USoundBase * m_HitSound;

#pragma endregion

#pragma region Blocking

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocking")
		// Blocking left side of the track
		bool m_Left = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocking")
		// Blocking center part of the track
		bool m_Center = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocking")
		// Blocking right side of the track
		bool m_Right = false;

#pragma endregion

#pragma region Sprites

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprites")
		// Sprite used for blank sides of track
		UPaperSprite * m_BlankSprie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprites")
		// Sprite used for marked sides of track
		UPaperSprite * m_MarkedSprite;

#pragma endregion

#pragma endregion

private:

#pragma region Const Variables

	const FVector c_LeftSpriteLocation = FVector(-130.0f, 0.0f, 170.0f);
	const FVector c_CenterSpriteLocation = FVector(0.0f, 0.0f, 190.0f);
	const FVector c_RightSpriteLocation = FVector(130.0f, 0.0f, 170.0f);

#pragma endregion
};

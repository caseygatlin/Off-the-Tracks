#pragma once

#include "CoreMinimal.h"
#include "Gamecode/E_CustomEnums.h"
#include "GameFramework/Pawn.h"
#include "Cactus.generated.h"

class UBehaviorTree;
class USceneComponent;
class USkeletalMeshComponent;

UCLASS()
class HAPHAZARDHANDCART_API ACactus : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACactus();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent * m_CactusRootComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent * m_CactusMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent * m_RightSpikeLaunchLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent * m_LeftSpikeLaunchLocation;

#pragma endregion

#pragma region AI Settings

#pragma region Perception Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Perception|Sight")
		// Radius in which the cactus will attack the surronding player handcar
		float m_SightRadius = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Perception|Sight")
		// Radius in which the cactus will attack the surronding player handcar
		float m_LoseSightRadius = 15000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Perception|Sight")
		// Radius in which the cactus will attack the surronding player handcar
		float m_VisionAngle = 180.0f;

#pragma endregion

#pragma region Attack Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Attacking")
		// Speed to launch the spike at
		float m_SpikeLaunchSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Attacking")
		// Speed to launch the spike at
		float m_SpikeTimeToLive = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Attacking")
		// How long the spike slows down the player
		float m_SpikeSlowdownTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Attacking")
		// How much the spike slows down the player
		float m_SpikeSlowStrength = 300.0f;

#pragma endregion

#pragma endregion

#pragma region Debug Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugOptions")
		// If true any functions that need to print a message at run time will be allowed to do so.
		bool m_PrintDebugStrings = false;

#pragma endregion

#pragma endregion

#pragma region Getters and Setters

	UBehaviorTree * GetBehaviorTree() const { return _BehaviorTree; }

	E_CactusFireLane GetCactusFireLane() const { return _CactusFireLane; }

	void SetCactusFireLane(E_CactusFireLane i_CactusFireLane) { _CactusFireLane = i_CactusFireLane; }

	E_CactusFireState GetCactusFireState() const { return _CactusFireState; }

	void SetCactusFireState(E_CactusFireState i_CactusFireState) { _CactusFireState = i_CactusFireState; }

#pragma endregion

private:

#pragma region AI Settings

	UPROPERTY(EditAnywhere, Category = "AI Settings|BehaviorTree")
		// Behavior tree for the cactus
		UBehaviorTree * _BehaviorTree;

#pragma region Attack Settings

	UPROPERTY(VisibleAnywhere, Category = "AI Settings|Attacking")
		// The fire state the cactus is in
		// [RANDOM, AIMING]
		E_CactusFireState _CactusFireState = E_CactusFireState::RANDOM;

	UPROPERTY(VisibleAnywhere, Category = "AI Settings|Attacking")
		// The current lane cactus is firing at
		// [LEFT, RIGHT]
		E_CactusFireLane _CactusFireLane = E_CactusFireLane::LEFT;

#pragma endregion

#pragma endregion
};

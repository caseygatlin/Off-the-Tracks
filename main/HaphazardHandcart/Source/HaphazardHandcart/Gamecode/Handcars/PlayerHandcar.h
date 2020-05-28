#pragma once

#include "CoreMinimal.h"

#include "TrainAndRail/Handcar.h"
#include "Gamecode/E_CustomEnums.h"
#include "GenericTeamAgentInterface.h"

#include "PlayerHandcar.generated.h"

class AJumpTrigger;
class ADetachedCamera;

class UAIPerceptionStimuliSourceComponent;
class UBoxComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USceneComponent;
class UStaticMeshComponent;

/**
 *
 */
UCLASS()
class HAPHAZARDHANDCART_API APlayerHandcar : public AHandcar, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:	

	APlayerHandcar();

	// Called when the game starts or when spawned.
	virtual void BeginPlay() override;

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// TODO: PostEditChangeProperty for when someone edits values that should be clamped to [0, 1].
#if WITH_EDITOR
	virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Called after an actor has been moved in the editor
	virtual void PostEditMove(bool bFinished);
#endif // WITH_EDITOR

#pragma region Helper Methods

#pragma region Physics & Collision

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Physics & Collision")
		// Updates the mass of the object for movement.
		void UpdateDynamicMass();

#pragma endregion

#pragma region Pumping

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Pumping")
		// Updates the rotation of the handle mesh for the Handcar.
		void UpdateHandleRotation(float i_TargetRotation);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Pumping")
		// Keeps track of which handle is down based on controller input.
		// Updates pump timing variables.
		void Pump(const float i_AxisVal);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Pumping")
		// Pumps the cart for a percentage of the max pump time
		void ApplyPump(const float i_PumpStrength);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Pumping")
		// Checks if gyro input should trigger a pump
		void CheckInputAndPumpIfNeeded();

#pragma endregion

#pragma region Movement

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Force an input lockout on the player
		void SlowdownPlayer(const float i_SlowdownTimeAmount, float i_Strength);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Update the acceleration variable of the parent to reflect the LocalAccelerationTotal.
		void UpdateParentAcceleration();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Add an acceleration amount to be applied this frame.
		void AddAcceleration(float i_Acceleration);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Subtract an acceleration amount to be applied this frame.
		void SubtractAcceleration(float i_Acceleration);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Add a friction amount to be applied this frame.
		void AddFriction(float i_Friction);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Subtract a friction amount to be applied this frame.
		void SubtractFriction(float i_Friction);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Update the friction variable of the parent to reflect the LocalFrictionTotal.
		void UpdateParentFriction();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Movement")
		// Update the friction variable of the parent to reflect the LocalFrictionTotal.
		void UpdateCartSpeedClamps();

#pragma endregion
		
#pragma region Boost

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Boost")
		// Update timers relating to the boos
		void UpdateBoostStates(const float i_DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Boost")
		// Initiate or stop a boost
		void SetBoosting(bool i_ActivateBoost);

#pragma endregion

#pragma region Leaning

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Leaning")
		void CheckLeanStateForSparks();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Leaning")
		// Set the lean state and JumpLeanFactor based on controller input.
		void Lean(const float i_AxisVal);

#pragma endregion

#pragma region Gyro

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Gyro")
		// Converts gyro pump input into a more stable and readable format for use with main mechanics.
		void HandleGyroPump(const float i_AxisValue);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Gyro")
		// Averages out the gyro values to account for input noise.
		void AveragePumpValues();

#pragma endregion

#pragma region Getters

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Getters")
		// Returns the current lean state of the cart.
		// [LEFT, CENTER, RIGHT].
		E_CartLeanState GetCartLeanState() const { return _CartLeanState; }

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Getters")
		// Returns the current boost state of the cart.		
		bool GetCartBoostState() const { return _IsBoosting; }

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Getters")
		// Returns the current lean state of the cart with mesh rotation considered.
		// [LEFT, CENTER, RIGHT].
		E_CartLeanState GetCartRotatedLeanState() const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Getters")
		// Returns the current Speed State.
		// [SLOW, MEDIUM, FAST].
		E_CartSpeedState GetCartSpeedState() const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|PlayerHandcar|Getters")
		// Returns input axis value.
		float GetInputAxis() const;

#pragma endregion

#pragma endregion

#pragma region Getters and Setters

	void SetLastJumpTrigger(AJumpTrigger * i_LastJumpTrigger) { _LastJumpTrigger = i_LastJumpTrigger; }

#pragma endregion

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent * m_BottomCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Player Handcar")
		// Front right sparks particle system.
		UParticleSystemComponent* m_SparksFR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Player Handcar")
		// Front left sparks particle system.
		UParticleSystemComponent* m_SparksFL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Player Handcar")
		// Back right sparks particle system.
		UParticleSystemComponent* m_SparksBR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Player Handcar")
		// Back Left sparks particle system.
		UParticleSystemComponent* m_SparksBL;

#pragma endregion

#pragma region Movement Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings|Player Handcar")
		// Handcar friction
		float m_LocalHandcarFriction = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings|Player Handcar")
		// Min speed to be considered medium.
		float m_MediumSpeedThreshold = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings|Player Handcar")
		// Min speed to be considered medium.
		float m_FastSpeedThreshold = 85.0f;

#pragma endregion

#pragma region Pump Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pump Settings|Player Handcar")
		// Amount of time to move the cart for each pump.
		float m_BoostTimePerPump = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pump Settings|Player Handcar")
		// The change in angle when the handle rotates.
		float m_TargetHandleAngleDelta = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pump Settings|Player Handcar")
		// The change in angle when the handle rotates.
		float m_MaxHandleAngle = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pump Settings|Player Handcar")
		// Threshold at which a pump is registered.
		// Range of [0, 1].
		float m_PumpThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pump Settings|Player Handcar")
		// The acceleration applied while pumping the handcar.
		float m_PumpAcceleration = 5000.f;

#pragma endregion

#pragma region Sparks Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparks Settings|Player Handcar")
		// The threshold for activating the sparks when leaning.
		float m_SparksLeanThreshold = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sparks Settings|Player Handcar")
		// The threshold for activating the sparks when moving.
		float m_SparksSpeedThreshold = 10.f;

#pragma endregion

#pragma region Lean Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings|Player Handcar")
		// Threshold for triggering cart lean in game.
		// Range of [0, 1].
		float m_LeanThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings|Player Handcar")
		// TODO: Tooltip
		float m_MaxJumpLeanFactor = 200.f;

#pragma endregion

#pragma region Jump Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings|Player Handcar")
		// Indicator of if player is jumping.
		bool m_IsJumping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings|Player Handcar")
		// Indicator of if player is rotating.
		bool m_IsRotating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Settings|Player Handcar")
		// Rotating time limit.
		float m_MaxRotateDuration = 0;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Jump Settings|Player Handcar")
		// Sound to play when player lands on track
		USoundBase * m_TrackLandingSound;

#pragma endregion

#pragma region BoostSettings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Settings|Player Handcar")
		// How powerful the boost effect is
		float m_BoostStrength = 4700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Settings|Player Handcar")
		// How many seconds needed leaning into a turn to activate the speed boost
		float m_LeanTimeToActivateBoost = .5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Settings|Player Handcar")
		// How fast at a minimum the car must be moving to activate the speed boost
		float m_MinBoostActivateSpeed = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boost Settings|Player Handcar")
		// How many secondsto activate the speed boost for
		float m_BoostDuration= 5;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Boost Settings|Player Handcar")
		// Sound to play when player is boosting
		USoundBase * m_BoostSound;
	
#pragma endregion

#pragma endregion

private:

#pragma region Hit and Overlap

	virtual void NotifyHit(class UPrimitiveComponent * MyComp, AActor * Other, class UPrimitiveComponent * OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) override;

#pragma endregion

#pragma region Team Agent Interface

	virtual FGenericTeamId GetGenericTeamId() const override;

#pragma endregion

#pragma region Private Custom Helper Methods

	// If the cart is marked as stopped, will set it as movable.
	void MoveIfStopped();

	// Updates the movement to match with the current pumping values.
	void UpdateMovement(const float i_DeltaTime);

	// Sets pump timing values.
	void SetPumpValues();

	//increment the Slowdown timer and stop slowing if needed
	void UpdatePlayerSlowdown(const float i_DeltaTime);

#pragma endregion

#pragma region Private Variables

#pragma region Components

	UPROPERTY(VisibleAnywhere, Category = "Components")
		// AI Perception Stimuli Source Component
		UAIPerceptionStimuliSourceComponent * _AIPerceptionStimuliSource;

#pragma endregion

#pragma region Team Agent Variables

	FGenericTeamId _TeamId;

#pragma endregion

#pragma region Boost
	UPROPERTY(VisibleAnywhere, Category = "Boost Settings|Player Handcar")
		// How long has the cart been boosting for
		float _BoostTimer = 0;

	UPROPERTY(VisibleAnywhere, Category = "Boost Settings|Player Handcar")
		// How long has the cart been leaning into a turn for
		float _LeanTimer = 0;

	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetCartBoostState, Category = "Boost Settings|Player Handcar")
		// Is this handcar currently boosting
		bool _IsBoosting = false;
# pragma endregion 

#pragma region Jump Options

	/*bool        _IsJumping;
	float       _JumpRunTime;
	float       _JumpCurrentTime;
	float       _JumpPeak;
	FVector     _OriginalScale;
	FTransform  _JumpTransform;*/
	UPROPERTY(VisibleAnywhere, Category = "Jump Settings|Player Handcar")
	// Indicator of if the car have started rotating.
	bool _RotateBegan = false;
	UPROPERTY(VisibleAnywhere, Category = "Jump Settings|Player Handcar")
	// The time when the car start rotating.
	float _RotateStartTime = 0;

#pragma endregion

#pragma region Movement Settings

	UPROPERTY(VisibleAnywhere, Category = "Movement Settings|Player Handcar")
		//Is the cart currently under a slow effect
		bool _PlayerSlowdown = false;

	UPROPERTY(VisibleAnywhere, Category = "Movement Settings|Player Handcar")
		//Cumulative Strength of any current slow effect
		float m_SlowDownStrength = 0;

	UPROPERTY(VisibleAnywhere, Category = "Movement Settings|Player Handcar")
		//The time that the player should be slowed down
		float _SlowdownTimer = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Movement Settings|Player Handcar")
		//The total accumulated acceleration to be applied to the handcar at the end of each tick.
		float _LocalAccelerationTotal = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Movement Settings|Player Handcar")
		// The total accumulated friction to be applied to the handcar at the end of each tick.
		float _LocalFrictionTotal = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Speed Settings|Player Handcar")
		// The current Speed state of the cart.
		// [SLOW, MEDIUM, FAST]
		E_CartSpeedState _CartSpeedState = E_CartSpeedState::SLOW;
	

#pragma endregion

#pragma region Pump Settings

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		// Whether the right handle is down or not.
		// Used for pumping the cart forward when switched.
		bool _IsRightHandleDown;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		// The time between pumps.
		float _PumpTimer;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		// The time since the last pump.
		float _TimeSinceLastPump;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		// The average of the TimeSinceLastPump's.
		float _AveragePumpTime;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		//The position of the handle in the last frame.
		float _LastHandlePosition = .5f;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		//The last valid delta between pumps;
		float _LastValidDelta = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		//Is the current Direction of the pump growing away from .5
		bool _LastFramePumpDirection = false;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		//The magnitude of the current pump.
		float _TotalPumpDistance = 0;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		//The max angle to animate between for pumping
		float _MaxAngle = 70;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		// The current acceleration being applied for the pump.
		// Max value is m_PumpAcceleration.
		// Auto calculated, do not edit manually.
		float _CurrentPumpAcceleration = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Pump Settings|Player Handcar")
		// Input axis value
		float _InputAxis = 0.f;

#pragma endregion

#pragma region Lean Settings

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// The current lean state of the cart.
		// [LEFT, CENTER, RIGHT]
		E_CartLeanState _CartLeanState = E_CartLeanState::CENTER;	

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// The last state that was held.
		E_CartLeanState _LastLeanState;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// How many ticks should the player hold a direction for it to be counted.
		// This filters out sensor noise.
		int32 _NumberOfTicksToBeStable;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// How many ticks has this direction been held
		int32 _LeanTickCounter;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// An averaged lean value (accounts for accelerometer sensitivity and noise).
		float _SmoothedCartLean;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// An array for printing out values to the screen.
		TArray<FString> _DebugOutArray;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		// An array of the past lean values.
		// Used for calculating average lean.
		TArray<float> _PreviousLeanValues;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings|Player Handcar")
		float _JumpLeanFactor;

#pragma region Const Variables

	const float C_MaxLeanAmount = 10.f;

#pragma endregion

#pragma endregion

#pragma region Jump Settings

	UPROPERTY(VisibleAnywhere, Category = "Jump Settings|Player Handcar")
		// Indicator of if player is jumping.
		AJumpTrigger * _LastJumpTrigger;

#pragma endregion

#pragma region Physics Settings

	UPROPERTY(VisibleAnywhere, Category = "Physics Settings|Player Handcar")
		// The initial mass.
		float _SavedMass;

#pragma endregion

#pragma endregion

};

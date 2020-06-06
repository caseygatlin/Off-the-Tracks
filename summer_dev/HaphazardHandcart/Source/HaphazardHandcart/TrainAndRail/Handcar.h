#pragma once

#include "E_TrainAndRail.h"
#include "Structs/S_TrackConnection.h"
#include "TrainAndRail/Structs/S_TrackTimedIgnore.h"
#include "../Gamecode/E_CustomEnums.h"

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Handcar.generated.h"

class ATrainTrack;
class UBogie;

class UArrowComponent;
class UPrimitiveComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class USphereComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class HAPHAZARDHANDCART_API AHandcar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHandcar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostLoad() override;

#pragma region Editor Events

#if WITH_EDITOR
	virtual void PreEditChange(UProperty* PropertyThatWillChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Called after an actor has been moved in the editor
	virtual void PostEditMove(bool bFinished);
#endif // WITH_EDITOR

#pragma endregion

#pragma region Helper Methods

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Initalize")
		void InitalizeTrainHandcar();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Initalize")
		void AttachToTrack();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Initalize")
		void SetupBogies();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Initalize")
		void SetupCollision();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|HelperTicks")
		bool TickHandcarMovement(float i_DeltaSeconds, bool & o_UpdateAborted);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|HelperTicks")
		bool TickBogieMovement(UBogie * i_Bogie, float i_Speed, bool i_IgnoreEndOfTrack);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|UpdateVisuals")
		void UpdateHandcarMeshLocations();

	UFUNCTION(Category = "TrainAndRail|Handcar|TrackUpdate")
		void BogieChangeTracks(const FS_TrackConnection& i_TrackConnection, UBogie * i_Bogie, float i_Speed, float i_DistanceToEnd);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|TrackUpdate")
		void HandleEndOfTrack(bool i_HitByHelperBogie, const AHandcar * i_InstantiatingHandcar);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|TrackUpdate")
		void RefreshPositionOnTrack();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Settings")
		void ToggleHandcarDirection();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Settings")
		void StopHandcar();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Settings")
		void ApplyOrReleaseBrakes(bool i_IsBraking);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Settings")
		void SetHandcarState(E_HandcarState i_NewHandcarState, bool i_PassToChildren);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Settings")
		void WarpHandcarToStartingPoint(bool i_WarpChildren);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Settings")
		void ResetHandcar(bool i_ResetChildren);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|BogieSettings")
		void UpdateBogieDistances(float i_StartingTrackDistance, ATrainTrack * i_Track, int32 i_TrackSplineIndex, bool i_SkipAlignedBogies,
			TArray<UBogie*> i_Bogies, bool i_Invert);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|BogieSettings")
		void RevertDistances();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Collision")
		void TickCollision(float i_DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Collision")
		bool SensorPulse(const UBogie * i_StartBogie);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Collision")
		void PutSensorBogieOutFront();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Collision")
		void HandleActiveObstacle(float i_DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Collision")
		bool IsActorOnIgnoreList(const AActor * i_ActorToCheck) const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Collision")
		void AddActorToCollisionIgnoreList(AActor * i_ActorToIgnore, float i_IgnoreTime, bool i_NeverExpire);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|GameplayEffects")
		void LeanHandcar();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|GameplayEffects")
		void PerformRollCheck(float i_DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|GameplayEffects")
		void DerailHandcar(const FVector & i_ImpactLocation, const FVector & i_ImpactForce);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void UpdateStartingDistanceRelativeToParent();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void CheckDistanceToParent(float i_DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void DisconnectFromParent(bool i_AddToTimedIgnoreList);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void SetHandcarDirection(E_BogieDirectionOfTravel & i_DirOfTravel);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void ConnectToParent(USphereComponent * i_ParentHitch, USphereComponent * i_ChildHitch);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void SetParentHandcar(AHandcar * i_NewParrent, bool i_RefreshPostion);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void SetChildHandcar(AHandcar * i_NewChild);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Parents & Children")
		void UpdateSettingsFromParent();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Movement")
		void CalculateSpeed(float i_DeltaSeconds, bool i_IgnoreLerp);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Movement")
		bool IsDirectionOfTravelInverted(E_TrackSplineLocation & i_ConnectionLocation, float i_Speed) const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Movement")
		float GetWheelSpeed() const;
	
	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Movement")
		// Updates the internal acceleration to match the public variable m_Acceleration.
		void UpdateAcceleration();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Debugging")
		void PrintDebugString(FString & i_InfoString);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Smoothing")
		void SmoothOutBogieDistance(UBogie * i_Bogie, float i_TargetDistance, const FVector & i_PreviousLocation);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Smoothing")
		float GetRotationBasedSmoothErrorTolerance(const FRotator & i_SourceRotator) const;

#pragma endregion

#pragma region Getters and Setters

	TArray<UBogie*> GetBogies() const { return _Bogies; }

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Movement")
		// Returns the current speed of the Handcar.
		float GetCurrentSpeed() const { return _CurrentSpeed; }

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Movement")
		// Returns whether or not the Handcar is braking.
		bool GetIsBraking() const { return _IsBraking; }

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|Handcar|Leaning")
		// Returns the current target lean amount.
		float GetTargetLeanAmount() const { return _TargetLeanAmount; }

#pragma endregion

#pragma region Customer Helper Methods

	void UpdateTrainHandcar();

#pragma endregion

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent * m_RearHitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent * m_FrontHitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UArrowComponent * m_RearBogieArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UArrowComponent * m_FrontBogieArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBogie * m_SensorBogie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBogie * m_RearBogie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent * m_HandcarMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent * m_Handle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBogie * m_FrontBogie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent * m_HandcarRootComponent;

#pragma endregion

#pragma region Visual Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Options")
		// Distance rear bogie should be from the front bogie.
		float m_BogieDistance = -1024.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Options")
		// Offset that will be applied to the handcar mesh when positioning. 
		FVector m_HandcarOffset = FVector(0.0f, 0.0f, 16.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Options")
		// Offset that will be applied to the bogie mesh when fully leaning
		FVector m_HandcarLeanOffset = FVector(0.0f, 0.0f, 30.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Options")
		// Offset that will be applied to the handle when positioning. 
		FVector m_HandleOffset = FVector(-10.0f, 0.0f, 190.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Options")
		// Current speed of the Handcar will be multiplied by this value and used as the playback speed for the bogie wheel animations.
		float m_WheelAnimSpeedScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Options")
		// Indicator of if the car's mesh is inverted.
		bool m_InvertMesh = false;

#pragma endregion

#pragma region Track Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// Track this Handcar is attached to.
		ATrainTrack * m_AttachedTrack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// Index of the spline on the track this Handcar is attached to.
		int32 m_AttachedTrackSplineIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// Starting distance on the spline. This is auto set, do not edit
		// manually unless you have 'ManuallySetDistanceOnTrack' set to True
		float m_StartingDistanceOnTrack = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// If true then you must manually enter a value for 'StartingDistanceOnTrack'.
		// If False, the train Handcar will pick the closest point on the spline.
		bool m_ManuallySetDistanceOnTrack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// What this Handcar should do when it hits the end of the track.
		E_EndOfTrackOption m_EndOfTrackOption = E_EndOfTrackOption::INSTANTLY_REVERSE_DIRECTION;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// Number of distance samples to check when attaching this Handcar to the track.
		int32 m_AttachDistanceSampleCount = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Settings")
		// If true, the Handcar will snap to the selected track.
		bool m_SnapToTrack = true;

#pragma endregion

#pragma region Engine Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Friction used when calculating the final speed of this Handcar.
		float m_Friction = 0.988f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Gravity used when calculating the final speed of this Handcar.
		float m_Gravity = -9.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Max speed this Handcar can reach. 
		float m_MaxSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Mass of this Handcar used when calculating the final speed of this Handcar.
		float m_Mass = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Min speed this Handcar should be at.
		float m_MinSpeed = -5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Acceleration to use when calculating speed. This is only applied if the Handcar type is Engine.
		float m_Acceleration = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Deceleration to use when calculating speed. This is only used when braking. 
		float m_Deceleration = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Initial travel direction of this Handcar.
		E_BogieDirectionOfTravel m_StartingDirectionOfTravel = E_BogieDirectionOfTravel::Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// If true then the engine will always be at max speed.
		bool m_UseConstantSpeeds = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// If true then the game engine delta time will be ignored and the ConstantDeltaTime variable will be used
		bool m_UseConstantDeltaTime = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine Settings")
		// Constant delta time that will be applied to the update function
		float m_ConstantDeltaTime = 0.015f;

#pragma endregion

#pragma region Handcar Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handcar Settings")
		// Current state of the Handcar.
		E_HandcarState m_HandcarState = E_HandcarState::Moving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handcar Settings")
		// If true the rear bogie will be removed
		bool m_DisableRearBogie = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handcar Settings")
		// Type of Handcar, currently Engine and Handcar. 
		// Engine has acceleration, does not and needs to be hooked to an engine to move.
		E_HandcarType m_HandcarType = E_HandcarType::Engine;

#pragma endregion

#pragma region Travel Settings

	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// Total distance we have traveled along as we go track to track
		float m_DistanceTraveled = 0;

	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// Path the handcar has taken at each junction/jump
		FString m_PathTaken;

#pragma endregion

#pragma region Collision Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// If true the auto added collision bogies will knock items back and respond to impacts.
		bool m_ImpactCollisionEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// If true the sensor bogie will be disabled. This must be enabled for obstacles to be detected.
		bool m_DisableSensorBogie = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Distance the sensor bogie should be from the Handcar.
		float m_SensorBogieDistance = 1024.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Sensor bogie offset from the train track
		FVector m_SensorOffset = FVector(0.0f, 0.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Size of the box to use for box traces for objects at the sensor location on the tracks.
		FVector m_SensorBoxSize = FVector(20.0f, 140.0f, 40.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// If true draw the box trace for collisions
		bool m_DrawBoxTrace = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Anything below this size will be ignored by the sensor.The bounds radius of the obstacle is checked against this value.
		float m_MinObstacleSize = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// How the Handcar should respond to an obstacle on the tracks.
		E_TrackObstacleResponse m_ObstacleResponse = E_TrackObstacleResponse::BRAKE_STOP_IF_POSSIBLE_AND_WAIT_FOR_IT_TO_MOVE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Time between obstacle pulses, this is only used when the Handcar has an active obstacle.
		float m_TimeBetweenObstaclePulses = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Offset applied to the front bogie position.
		float m_FrontColBogieOffset = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Offset applied to the rear bogie position.
		float m_RearColBogieOffset = -350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// TODO: init array in blueprint version
		// Query types to use when doing any collision or sensor related traces.
		TArray<TEnumAsByte<EObjectTypeQuery>> m_CollisionQueryTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Strength of the plow when knocking items back and away from the train Handcar.
		float m_PlowImpactStrength = 12000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Amount of force that an impact must have in order to slow down this Handcar.
		float m_PlowImpactThreshold = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// If true this Handcar will never derail from an impact.
		bool m_NeverDerail = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// Amount of force applied to the Handcar when it derails. This value is multiplied by the current speed to create the final force.
		float m_DerailForce = 50000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Settings")
		// When the Handcar derails it will also derail its children.
		// The derail force that’s passed down to the child is multiplied by this value to create the final derail force for the child.
		float m_ChildDerailDampen = 0.75f;

#pragma endregion

#pragma region Train Handcars

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// Parent Handcar for this Handcar. 
		AHandcar * m_ParentHandcar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// Child Handcar for this Handcar. This is auto updated if the other Handcar sets a parent.
		AHandcar * m_ChildHandcar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// Distance this Handcar should start at from the parent Handcar.
		float m_DistanceFromParent = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// Max distance this Handcar can be from the parent before disconnect.
		float m_MaxDistanceFromParent = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// If true distance between parent and child will be checked against the MaxDistanceFromParent and if it is exceeded,
		// the child will disconnect from the parent.
		bool m_UseDistanceLimitChecks = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// TimeBetweenDistanceChecks
		float m_TimeBetweenDistanceChecks = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// Lerp value used when matching the parent speed. 0 = 100% Parent
		float m_ParentSpeedLerp = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Train Handcars")
		// If true, this Handcar will grab settings from the parent Handcar.
		bool m_InheritParentSettings = true;

#pragma endregion

#pragma region Lean Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// If true the lean effect will be used by this Handcar.
		bool m_UseLeanEffect = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lean Settings")
		bool m_IsLeaningWithTurn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// Max delta between yaw values that must be reached for the lean effect to kick in.
		float m_LeanMaxYawDelta = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// If the lean delta is above this value it will be ignored. 
		float m_LeanIgnoreDelta = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// Lerp value used when moving between the current lean and the target lean.
		float m_LeanLerpAmount = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// If true the lean effect will be applied to the bogies as well. If false, then it will just be applied to the body mesh.
		bool m_ApplyLeanToBogies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// Used to return the lean to the original location when the lean is at 0.
		float m_LeanReturnToRestMultiplier = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// Min precentage of the max speed that the Handcar must be at before the lean effect will kick in.
		float m_LeanMinSpeedPercentage = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lean Settings")
		// Lean value that will be used when leaning to mesh to the side.
		float m_LeanAmount = -10.0f;

#pragma endregion

#pragma region Roll Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll Settings")
		// If true, the roll check will be used.
		bool m_UseRollCheck = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll Settings")
		// Time to wait between roll checks.
		float m_TimeBetweenRollChecks = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll Settings")
		// Roll, Pitch and Yaw thresholds to use during the roll check.
		FRotator m_RollDeltaThresholdsRPY = FRotator(15.0f, 15.0f, 15.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll Settings")
		// Derail force to use if a roll threshold is broken. This value is multipled by the current speed to create the final value.
		float m_RollDerailForce = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll Settings")
		// Which type of threshold evaluation to use.
		E_RollCheckEval m_RollThresholdEval = E_RollCheckEval::Any;

#pragma endregion

#pragma region Debug Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Options")
		// If true any functions that need to print a message at run time will be allowed to do so.
		// E.g. Sensor bogie will print what’s currently blocking it from moving forward on the tracks.
		bool m_PrintDebugStrings = true;

#pragma endregion

#pragma region Movement Smoothing

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// Enable or disable movement smoothing for Handcar
		bool m_EnableMovementSmoothing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// If the distance between two points is less than this no smoothing will be applied, if it's greater another smooth pass will be performed.
		// Lower means smoother but more work for the engine.
		float m_SmoothDistanceErrorTolerance = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// If the delta between locations is greater than this value it will be ignore and no smoothing will be applied, this is to avoid snapping.
		float m_SmoothDistanceDeltaLimit = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// Number of smoothing attempts to make to a bogie. More attempts equals a smoother movement but more work for the engine.
		int32 m_SmoothAttemptsPerBogie = 14;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// Movement smoothing is more for visual elements and not really needed for the helper or sensor bogies.
		// But if required it can be turned on or off with this option.
		bool m_DoNotSmoothHelperOrSensorBogies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// If true the rotation of the track will be used to scale the error tolerance, this can be useful if you want to use the smooth movement option,
		// but also want allow Handcars to roll down a hill if detached from the engine.
		bool m_UseTrackRotationToScaleErrorTolerance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Smoothing")
		// This is the amount that the track rotation is scaled by to produce the final error tolerance value.
		// E.g If the rotator is 45, 45, 45 then the final value will be 135 (45 * 3) multiplied by this value.
		float m_TrackRotationScaleAmount = 1.0f;

#pragma endregion

#pragma endregion

protected:

#pragma region Lean Settings

    UPROPERTY(VisibleAnywhere, Category = "Lean Settings")
        E_CartLeanState _CurrentLeanState = E_CartLeanState::CENTER;

#pragma endregion

private:

#pragma region Private Helper Methods

	void AddTrackTriggersToIgnoreArray();
	void UpdateBogiesAndTrackers();
	void ValidateBogies();

#pragma endregion

#pragma region Macros and Inline

	float GetSensorBogieDistance() const;
	UBogie * GetFrontBogieBasedOnDirOfTravel() const;
	void AddCollisionBogie(FString i_DebugName, float i_DistanceFromHandcar);
	void CreateRandomDerailValues(bool i_UseRandomRotation, FVector & o_ImpactLocation, FVector & o_ImpactForce);
	float GetHitchToBogieXDistance(USphereComponent * i_Hitch, USkeletalMeshComponent * i_BogieMesh, AHandcar * i_ParrentHandcar);
	FTransform AddLocationOffsetToTransform(FTransform & i_SourceTransform, FVector & i_Offset);
	void BrakingClamps(float & o_Min, float & o_Max);
	float BrakingDeccel();
	FVector CalculateDerailForce(FVector & i_Normal);
	float TransformLocationLen(FTransform & i_TranformA, FTransform & i_TransformB);
	void UpdateBogiePreUpdatePositions();

#pragma endregion

#pragma region Private Variables

#pragma region VisualOptions

	UPROPERTY(VisibleAnywhere, Category = "Visual Options")
		// Used for correctly scaling the mesh. Auto generated, so not edit manually.
		FVector _AutoFilled_ActorScale = FVector::ZeroVector;

#pragma endregion

#pragma region Track Settings

	UPROPERTY(VisibleAnywhere, Category = "Track Settings")
		// Flag used internally to indicate of the Handcar came to a graceful stop.
		bool _GracefulStopForEOT = false;

	UPROPERTY(VisibleAnywhere, Category = "Track Settings")
		// Flag used internally to indicate if a sensor or helper bogie triggered the end of track 
		bool _SensorTriggeredEOT = false;

	UPROPERTY(VisibleAnywhere, Category = "Track Settings")
		// If true, the Handcar will start inverted on the attached track.
		bool _StartInverted = false;

#pragma endregion

#pragma region Engine Settings

	UPROPERTY(VisibleAnywhere, Category = "Engine Settings")
		// Current velocity of this Handcar. Set automatically, do not manually edit.
		float _Velocity = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Engine Settings")
		// Min speed this Handcar should be at.
		float _CurrentSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Engine Settings")
		// Internal flag to indicate if this Handcar is currently braking. Set automatically, do not manually edit.
		bool _IsBraking = false;

	UPROPERTY(VisibleAnywhere, Category = "Engine Settings")
		// Acceleration value used internally. 
		// Based on the public variable, this value is set as positive if moving forward or negative if moving backwards.
		// Set automatically, do not manually edit.
		float _InternalAcceleration = 500.0f;

#pragma endregion

#pragma region Collision Settings

	UPROPERTY(VisibleAnywhere, Category = "Collision Settings")
		// Internal flag used to indicate if this Handcar has an obstacle.
		bool _HasActiveObstacle = false;

	UPROPERTY(VisibleAnywhere, Category = "Collision Settings")
		// Internal countdown to next pulse. This is set automatically, do not edit manually.
		float _TimeToNextPule = 1024.0f;

	UPROPERTY(VisibleAnywhere, Category = "Collision Settings")
		// Bogies used for collision detection.
		// If you add any custom collision bogies to the Handcar make sure to add them to this array so they can be used for collision checks.
		TArray<UBogie*> _CollisionBogies;

	UPROPERTY(VisibleAnywhere, Category = "Collision Settings")
		// Internal array of items that are to be ignored during collision checks.
		TArray<FS_TrackTimedIgnore> _TimedIgnoreList;

#pragma endregion

#pragma region Train Handcars

	UPROPERTY(VisibleAnywhere, Category = "Train Handcars")
		// If true the child Handcar will be placed behind the parent.
		bool _ChildPlacedBehindHandcar = true;

	UPROPERTY(VisibleAnywhere, Category = "Train Handcars")
		// Internal countdown until next distance check. Automatically set, do not edit manually.
		float _TimeToNextDistanceCheck = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Train Handcars")
		// Internal reference to the parent hitch.
		USphereComponent * _ConnectedParentHitch;

	UPROPERTY(VisibleAnywhere, Category = "Train Handcars")
		// Internal reference to the child hitch.
		USphereComponent * _ConnectedChildHitch;

	UPROPERTY(VisibleAnywhere, Category = "Train Handcars")
		// Internal flag to indicate if this Handcar has been disconnected from the parent.
		bool _DisconnectedFromParent = false;

#pragma endregion

#pragma region Lean Settings

    UPROPERTY(VisibleAnywhere, Category = "Lean Settings")
        // Internal value used to track the current lean amount. Auto set, do not edit manually.
        float _CurrentLeanAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings")
		// Internal value used to track the current handcar lean position. Auto set, do not edit manually.
		FVector _CurrentHandcarLeanOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings")
		// Internal value used to track our current lean target. Auto set, do not edit manually.
		float _TargetLeanAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings")
		// Internal value used to track our curve lean target. Auto set, do not edit manually.
		float _CurveLeanAmount = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Lean Settings")
		// Internal value used to track our max lean amount. Auto set, do not edit manually.
		float _MaxLeanAmount = 50.0f;

#pragma endregion

#pragma region Roll Settings

	UPROPERTY(VisibleAnywhere, Category = "Roll Settings")
		// Internal countdown to next roll check. This auto updated, do not edit manually.
		float _TimeToNextRollCheck = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Roll Settings")
		// Last rolling sample used.
		FRotator _LastRollingSample = FRotator(0.0f, 0.0f, 0.0f);

#pragma endregion

#pragma region Movement Smoothing

	UPROPERTY(VisibleAnywhere, Category = "Movement Smoothing")
		// Holds the position in world space for the bogie before the current update so they can be used for comparison later
		TArray<FVector> _PreUpdateBogiePositions;

#pragma endregion

#pragma region Handcar Internal
	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// We store the transform from the last tick because it’s used in a few places.
		FTransform _FrontBogieTransformLastTick = FTransform::Identity;

	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// Array of bogies that are used when checking for trigger overlaps. 
		// Bogies not on this list won’t be checked. We do this to cut down on the number of checks we do each tick.
		// Add your custom boies here if you need to.
		TArray<UBogie*> _TriggerBogies;

	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// Transform from the current tick for the front bogie.
		// We store this because it’s used in so many places and it’s faster than each of those places requesting it from the spline each time.
		FTransform _FrontBogieTransform = FTransform::Identity;

	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// Array of bogies in this Handcar. Stored in this array because it’s faster than getting all components of a class each time the bogies are needed.
		// It’s also easier than constantly checking if one is disabled or not, we just process this list.
		TArray<UBogie*> _Bogies;

	UPROPERTY(VisibleAnywhere, Category = "Handcar Internal")
		// Transform from the current tick for the front bogie.
		// We store this because it’s used in so many places and it’s faster than each of those places requesting it from the spline each time.
		int32 _HandcarUpdateID = 0;

#pragma endregion

#pragma endregion
};


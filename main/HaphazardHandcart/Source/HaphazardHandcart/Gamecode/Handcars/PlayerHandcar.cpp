
#include "PlayerHandcar.h"

#include "Gamecode/DebugFunctionLibrary.h"
#include "Gamecode/Tracks/StartTrack.h"
#include "Gamecode/Triggers/JumpTrigger.h"
#include "HaphazardHandcartGameMode.h"

#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"


APlayerHandcar::APlayerHandcar()
{
	// the handle needs overlap events so it can pick up collectibles
	m_Handle->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksSystem(TEXT(
		"ParticleSystem'/Game/HandCarContent/Particles/P_Sparks_Cart.P_Sparks_Cart'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> BoostSound(TEXT(
		"SoundCue'/Game/HandCarContent/Audio/AudioSfx/Boost_Cue.Boost_Cue'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> LandingOnTrackSound(TEXT(
		"SoundCue'/Game/HandCarContent/Audio/AudioSfx/HandcarHittingTrack_Cue.HandcarHittingTrack_Cue'"));

	FRotator sparksOrientation = FRotator(0.f, 0.f, -24.f);
	FVector locationFR = FVector(96.7f, 116.4f, 24.4f);
	FVector locationFL = FVector(96.7f, -116.4f, 24.4f);
	FVector locationBR = FVector(-110.8f, 116.4f, 24.4f);
	FVector locationBL = FVector(-110.8f, -116.4f, 24.4f);

	m_SparksFR = CreateDefaultSubobject<UParticleSystemComponent>("Front Right Sparks");
	m_SparksFR->SetTemplate(SparksSystem.Object);
	m_SparksFR->SetRelativeLocation(locationFR);
	m_SparksFR->SetRelativeRotation(sparksOrientation);
	m_SparksFR->SetupAttachment(RootComponent);
	m_SparksFR->bAutoActivate = false;

	m_SparksFL = CreateDefaultSubobject<UParticleSystemComponent>("Front Left Sparks");
	m_SparksFL->SetTemplate(SparksSystem.Object);
	m_SparksFL->SetRelativeLocation(locationFL);
	m_SparksFL->SetRelativeRotation(sparksOrientation);
	m_SparksFL->SetupAttachment(RootComponent);
	m_SparksFL->bAutoActivate = false;

	m_SparksBR = CreateDefaultSubobject<UParticleSystemComponent>("Back Right Sparks");
	m_SparksBR->SetTemplate(SparksSystem.Object);
	m_SparksBR->SetRelativeLocation(locationBR);
	m_SparksBR->SetRelativeRotation(sparksOrientation);
	m_SparksBR->SetupAttachment(RootComponent);
	m_SparksBR->bAutoActivate = false;

	m_SparksBL = CreateDefaultSubobject<UParticleSystemComponent>("Back Left Sparks");
	m_SparksBL->SetTemplate(SparksSystem.Object);
	m_SparksBL->SetRelativeLocation(locationBL);
	m_SparksBL->SetRelativeRotation(sparksOrientation);
	m_SparksBL->SetupAttachment(RootComponent);
	m_SparksBL->bAutoActivate = false;

	m_BoostSound = Cast<USoundBase>(BoostSound.Object);

	m_TrackLandingSound = Cast<USoundBase>(LandingOnTrackSound.Object);

	m_BottomCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomCollsionBox"));
	m_BottomCollisionBox->SetupAttachment(m_HandcarMesh);
	m_BottomCollisionBox->SetWorldLocation(FVector(0.0f, 0.0f, 20.0f));
	m_BottomCollisionBox->SetBoxExtent(FVector(120.0f, 140.0f, 10.0f));

	_AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("HandcarStimuliSource"));
	_AIPerceptionStimuliSource->bAutoRegister = true;

	// so the playerhand car is on a different team then the enemies are
	_TeamId = FGenericTeamId(0);

	m_UseLeanEffect = true;
	AutoPossessPlayer = EAutoReceiveInput::Type::Player0;
}

void APlayerHandcar::BeginPlay()
{
	Super::BeginPlay();

	_AIPerceptionStimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	AStartTrack * startTrack = Cast<AStartTrack>(m_AttachedTrack);
	if (startTrack == nullptr)
	{
		FString errorText = "Being Play attached track is not a start track"; // Being Play attached track is not a start track
		UDebugFunctionLibrary::DebugPrintScreen(errorText);
	}
	_SavedMass = m_Mass;
}

void APlayerHandcar::Tick(float DeltaTime)
{
	if (!m_IsJumping)
	{
		Super::Tick(DeltaTime);

		_TimeSinceLastPump += DeltaTime;

		MoveIfStopped();
		UpdateMovement(DeltaTime);

		CheckLeanStateForSparks();

		UpdateBoostStates(DeltaTime);
		UpdatePlayerSlowdown(DeltaTime);
		UpdateParentAcceleration();
		UpdateParentFriction();
	}
	else
	{
		Super::Super::Tick(DeltaTime);

		if (m_IsRotating)
		{
			// Set rotating start time
			if (!_RotateBegan)
			{
				_RotateStartTime = GetWorld()->GetTimeSeconds();
				_RotateBegan = true;
			}

			// Check rotating duration
			if (GetWorld()->GetTimeSeconds() - _RotateStartTime > m_MaxRotateDuration)
			{
				m_IsRotating = false;
				m_HandcarRootComponent->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
				_RotateBegan = false;
			}
		}
	}
	UpdateCartSpeedClamps();
}

void APlayerHandcar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("NormalCtrl_Lean", this, &APlayerHandcar::Lean);

	bool bindKeyboardPump = true;
	for (const FInputAxisBinding& AxisBinding : PlayerInputComponent->AxisBindings)
	{
		if (AxisBinding.AxisName == FName("HandleLean"))
		{
			bindKeyboardPump = false;
			PlayerInputComponent->BindAxis("HandleLean", this, &APlayerHandcar::HandleGyroPump);
			break;
		}
	}

	if (bindKeyboardPump)
	{
		PlayerInputComponent->BindAxis("NormalCtrl_Pump", this, &APlayerHandcar::Pump);
	}

	// TODO: Bind and implement jump functions
}

#if WITH_EDITOR
void APlayerHandcar::PreEditChange(UProperty* PropertyThatWillChange)
{
	Super::PreEditChange(PropertyThatWillChange);

	if (m_LocalHandcarFriction < 0.f)
	{
		m_LocalHandcarFriction = 0.f;
	}
}

void APlayerHandcar::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (m_LocalHandcarFriction < 0.f)
	{
		m_LocalHandcarFriction = 0.f;
	}
}

void APlayerHandcar::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
}
#endif // WITH_EDITOR


void APlayerHandcar::UpdateDynamicMass()
{
	float frontHitchZ = m_FrontHitch->GetComponentLocation().Z;
	float rearHitchZ = m_RearHitch->GetComponentLocation().Z;
	float zDistance = frontHitchZ - rearHitchZ;
	float Tolerance = 0.01f;
	bool outsideTolerance = UKismetMathLibrary::Abs(zDistance) > Tolerance;

	if (m_InvertMesh)
	{
		zDistance = -zDistance;
	}

	if (outsideTolerance)
	{
		if (zDistance > 0.f)
		{
			m_Mass = 0.f;
		}
		else
		{
			m_Mass = _SavedMass;
		}
	}
}

void APlayerHandcar::UpdateHandleRotation(float i_TargetRotation)
{
	float pitch = m_Handle->GetRelativeRotation().Pitch;
	_LastValidDelta = i_TargetRotation - pitch;

	FRotator handleRotation = FRotator(i_TargetRotation, 0.f, 0.f);
	m_Handle->SetRelativeRotation(handleRotation);
}

void APlayerHandcar::CheckLeanStateForSparks()
{
	if (GetTargetLeanAmount() < m_SparksLeanThreshold * -1 &&
		GetCurrentSpeed() > m_SparksSpeedThreshold &&
		!m_IsJumping)
	{
		if (m_InvertMesh)
		{
			m_SparksFR->Activate();
			m_SparksBR->Activate();
			m_SparksFL->Deactivate();
			m_SparksBL->Deactivate();
		}
		else
		{
			m_SparksFL->Activate();
			m_SparksBL->Activate();
			m_SparksFR->Deactivate();
			m_SparksBR->Deactivate();
		}
	}
	else if (GetTargetLeanAmount() > m_SparksLeanThreshold &&
		GetCurrentSpeed() > m_SparksSpeedThreshold &&
		!m_IsJumping)
	{
		if (m_InvertMesh)
		{
			m_SparksFL->Activate();
			m_SparksBL->Activate();
			m_SparksFR->Deactivate();
			m_SparksBR->Deactivate();
		}
		else
		{
			m_SparksFR->Activate();
			m_SparksBR->Activate();
			m_SparksFL->Deactivate();
			m_SparksBL->Deactivate();
		}
	}
	else
	{
		m_SparksFR->Deactivate();
		m_SparksBR->Deactivate();
		m_SparksFL->Deactivate();
		m_SparksBL->Deactivate();
	}
}

void APlayerHandcar::HandleGyroPump(float i_AxisVal)
{
	float maxAngle = 70;
	AveragePumpValues();

	bool useNewCode = true;
	if (!useNewCode)
	{
		//PREVIOUS WORKING CODE
			//TODO: Remove once code below is finalized. 
		const float pumpLeftThreshold = 0.6f;
		const float pumprightThreshold = 0.4f;
		const float pumpLeftAxisVal = -1.f;
		const float pumpRightAxisVal = 1.f;

		if (i_AxisVal > pumpLeftThreshold)
		{
			Pump(pumpLeftAxisVal);
		}
		else if (i_AxisVal < pumprightThreshold)
		{
			Pump(pumpRightAxisVal);
		}
		///////////////////////////////////////////
	}
	else
	{
		float absoluteDistanceFromCenter = abs(.5 - i_AxisVal) * 2;
		float targetRotation = absoluteDistanceFromCenter * maxAngle;
		float signedHandleDelta = 0.0f;
		// Any value larger than this gets culled
		float deltaThreshold = .9f;

		if (i_AxisVal > .5f)
		{
			targetRotation *= -1;
		}

		signedHandleDelta = _LastHandlePosition - i_AxisVal;
		_LastHandlePosition = i_AxisVal;

		if (abs(signedHandleDelta) <= deltaThreshold)
		{
			UpdateHandleRotation(targetRotation);
		}
		CheckInputAndPumpIfNeeded();
	}
}

void APlayerHandcar::Pump(float i_AxisVal)
{
	_InputAxis = i_AxisVal;

	float targetRotation = i_AxisVal * m_MaxHandleAngle;

	UpdateHandleRotation(targetRotation);

	if (i_AxisVal <= m_PumpThreshold * -1.f && _IsRightHandleDown)
	{
		_IsRightHandleDown = false;
		SetPumpValues();
	}
	else if (i_AxisVal >= m_PumpThreshold && !_IsRightHandleDown)
	{
		_IsRightHandleDown = true;
		SetPumpValues();
	}
}

void APlayerHandcar::ApplyPump(const float i_PumpStrength)
{
	if (m_HandcarState == E_HandcarState::Stopped && GetIsBraking())
	{
		bool isBraking = false;
		ApplyOrReleaseBrakes(isBraking);
	}

	_PumpTimer = i_PumpStrength * m_BoostTimePerPump;
	_AveragePumpTime = (_AveragePumpTime + _TimeSinceLastPump)*.5;
	_TimeSinceLastPump = 0;
}

void APlayerHandcar::CheckInputAndPumpIfNeeded()
{
	bool a = _LastValidDelta <= 0 && !(_LastFramePumpDirection);
	bool b = _LastValidDelta > 0 && _LastFramePumpDirection;
	bool switchedDirection = a ? !b : b; //A XOR B

	//TODO: Direction switches aren't detected until .5 is passed

	_LastFramePumpDirection = _LastValidDelta <= 0;

	float pumpThreshold = 10;//How big a pump must be to trigger. 

	if (switchedDirection)
	{
		if (_TotalPumpDistance > pumpThreshold)
		{
			float strength = UKismetMathLibrary::FMin((_TotalPumpDistance / _MaxAngle), 1);
			ApplyPump(strength);
		}
		_TotalPumpDistance = 0;
	}
	else
	{
		_TotalPumpDistance += abs(_LastValidDelta);
	}
}

void APlayerHandcar::SlowdownPlayer(const float i_SlowdownTimeAmount, float i_Strength)
{
	_PlayerSlowdown = true;
	if (i_SlowdownTimeAmount > 0)
	{
		m_SlowDownStrength += i_Strength;
	}
	else
	{
		m_SlowDownStrength = i_Strength;
	}
	_SlowdownTimer = i_SlowdownTimeAmount;
}

void APlayerHandcar::UpdateParentAcceleration()
{
	m_Acceleration = _LocalAccelerationTotal;
	_LocalAccelerationTotal = 0.f;
	UpdateAcceleration();
}

void APlayerHandcar::AddAcceleration(float i_Acceleration)
{
	_LocalAccelerationTotal += i_Acceleration;

	if (_LocalAccelerationTotal < 0.f)
	{
		_LocalAccelerationTotal = 0.f;
	}
}

void APlayerHandcar::SubtractAcceleration(float i_Acceleration)
{
	_LocalAccelerationTotal -= i_Acceleration;

	if (_LocalAccelerationTotal < 0.f)
	{
		_LocalAccelerationTotal = 0.f;
	}
}

void APlayerHandcar::AddFriction(float i_Friction)
{
	_LocalFrictionTotal += i_Friction;

	if (_LocalFrictionTotal < 0.f)
	{
		_LocalFrictionTotal = 0.f;
	}
}

void APlayerHandcar::SubtractFriction(float i_Friction)
{
	_LocalFrictionTotal -= i_Friction;

	if (_LocalFrictionTotal < 0.f)
	{
		_LocalFrictionTotal = 0.f;
	}
}

void APlayerHandcar::UpdateParentFriction()
{
	_LocalFrictionTotal /= 10000.f;

	if (_LocalFrictionTotal < 0.f)
	{
		_LocalFrictionTotal = 0.f;
	}

	_LocalFrictionTotal += 1.f;
	m_Friction = _LocalFrictionTotal;
}

void APlayerHandcar::UpdateCartSpeedClamps()
{
	float currentSpeed = GetCurrentSpeed();

	if (currentSpeed > m_FastSpeedThreshold)
	{
		_CartSpeedState = E_CartSpeedState::FAST;
	}
	else if (currentSpeed > m_MediumSpeedThreshold)
	{
		_CartSpeedState = E_CartSpeedState::MEDIUM;
	}
	else
	{
		_CartSpeedState = E_CartSpeedState::SLOW;
	}

	if (m_PrintDebugStrings)
	{
		//FString text = _CartSpeedState == E_CartSpeedState::SLOW ? "Slow" : _CartSpeedState == E_CartSpeedState::MEDIUM ? "Medium" : "Fast";
		//UDebugFunctionLibrary::DebugPrintScreen(text);
	}
}

void APlayerHandcar::Lean(float i_AxisVal)
{
	_JumpLeanFactor = 0.f;
	_CartLeanState = E_CartLeanState::CENTER;

	if (i_AxisVal <= m_LeanThreshold * -1.f)
	{
		_JumpLeanFactor = m_MaxJumpLeanFactor * -1;
		_CartLeanState = E_CartLeanState::LEFT;
	}
	else if (i_AxisVal >= m_LeanThreshold)
	{
		_JumpLeanFactor = m_MaxJumpLeanFactor;
		_CartLeanState = E_CartLeanState::RIGHT;
	}

	_CurrentLeanState = _CartLeanState;
}

void APlayerHandcar::AveragePumpValues()
{
	// Add the current lean value up to a max of 20
	const int32 maxNumLeans = 3;
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	float currentCartLean = playerController->GetInputAxisValue("HandleLean");

	_PreviousLeanValues.Add(currentCartLean);

	if (_PreviousLeanValues.Num() > maxNumLeans)
	{
		_PreviousLeanValues.RemoveAt(0);

		// Average the previous values of the lean
		float sum = 0.f;
		for (int32 i = 0; i < _PreviousLeanValues.Num(); i++)
		{
			sum += _PreviousLeanValues[i];
		}

		_SmoothedCartLean = sum / static_cast<float>(_PreviousLeanValues.Num());
	}
}

E_CartLeanState APlayerHandcar::GetCartRotatedLeanState() const
{
	if (m_InvertMesh)
	{
		if (_CartLeanState == E_CartLeanState::LEFT)
		{
			return E_CartLeanState::RIGHT;
		}
		else if (_CartLeanState == E_CartLeanState::RIGHT)
		{
			return E_CartLeanState::LEFT;
		}
	}

	return _CartLeanState;
}

E_CartSpeedState APlayerHandcar::GetCartSpeedState() const
{
	return _CartSpeedState;
}

float APlayerHandcar::GetInputAxis() const
{
	return _InputAxis;
}

#pragma region Hit and Overlap

void APlayerHandcar::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	ATrainTrack * hitTrack = Cast<ATrainTrack>(Other);

	// collison with track
	if (hitTrack != nullptr && m_IsJumping)
	{
		for (int32 i = 0; i < _LastJumpTrigger->m_LandingTracks.Num(); i++)
		{
			if (hitTrack == _LastJumpTrigger->m_LandingTracks[i])
			{
				m_PathTaken.Append("J");
				m_PathTaken.AppendInt(i);

				(Cast<AHaphazardHandcartGameMode>(GetWorld()->GetAuthGameMode()))->SetLongestPossibleTrackLength();
				break;
			}
		}

		UGameplayStatics::PlaySound2D(this, m_TrackLandingSound);

		_LastJumpTrigger = nullptr;

		// reset physics state of handcar
		m_HandcarRootComponent->SetNotifyRigidBodyCollision(false);
		m_HandcarRootComponent->SetEnableGravity(false);
		m_HandcarRootComponent->SetSimulatePhysics(false);

		m_AttachedTrack = hitTrack;
		m_IsJumping = false;
		m_IsRotating = false;
		_RotateBegan = false;
		UpdateTrainHandcar();
	}
}

FGenericTeamId APlayerHandcar::GetGenericTeamId() const
{
	return _TeamId;
}

#pragma endregion

void APlayerHandcar::MoveIfStopped()
{
	if (m_HandcarState == E_HandcarState::Stopped && !GetIsBraking())
	{
		bool passToChildren = false;
		SetHandcarState(E_HandcarState::Moving, passToChildren);
		m_Mass = _SavedMass;
	}
}

void APlayerHandcar::UpdateMovement(float i_DeltaTime)
{
	if (m_HandcarState != E_HandcarState::Derailed)
	{
		AddFriction(m_LocalHandcarFriction);

		if (_PumpTimer > 0.f)
		{
			_PumpTimer -= i_DeltaTime;
			UpdateDynamicMass();

			float accelerationFalloffRate = m_PumpAcceleration / m_BoostTimePerPump;
			_CurrentPumpAcceleration = accelerationFalloffRate * _PumpTimer;

			if (!GetIsBraking())
			{
				AddAcceleration(_CurrentPumpAcceleration);
			}
		}

		if (!GetIsBraking())
		{
			bool passToChildren = false;
			SetHandcarState(E_HandcarState::Moving, passToChildren);
		}
	}
}

void APlayerHandcar::SetPumpValues()
{
	if (m_HandcarState != E_HandcarState::Derailed)
	{
		ApplyPump(1.0f);
	}
}

void APlayerHandcar::UpdatePlayerSlowdown(const float i_DeltaTime)
{
	if (_PlayerSlowdown)
	{
		_SlowdownTimer -= i_DeltaTime;
		_PlayerSlowdown = _SlowdownTimer > 0;
		AddFriction(m_SlowDownStrength);
	}
}

void APlayerHandcar::UpdateBoostStates(const float i_DeltaTime)
{
	if (m_IsLeaningWithTurn && GetCurrentSpeed() > m_MinBoostActivateSpeed)
	{
		_LeanTimer += i_DeltaTime;
		if (_LeanTimer >= m_LeanTimeToActivateBoost)
		{
			UGameplayStatics::PlaySound2D(this, m_BoostSound);
			SetBoosting(true);
		}
	}
	else
	{
		_LeanTimer = 0;
	}

	if (_IsBoosting)
	{
		AddAcceleration(m_BoostStrength);
		_BoostTimer += i_DeltaTime;
		if (_BoostTimer >= m_BoostDuration)
		{
			SetBoosting(false);
		}
	}
}
void APlayerHandcar::SetBoosting(bool i_ActivateBoost)
{
	if (i_ActivateBoost)
	{
		_IsBoosting = true;
		_BoostTimer = 0;
		_LeanTimer = 0;		
	}
	else
	{
		_IsBoosting = false;
		_BoostTimer = 0;		
	}
}

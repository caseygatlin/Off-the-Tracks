#include "CactusAIController.h"

#include "Cactus.h"
#include "GameCode/Handcars/PlayerHandcar.h"
#include "SpikeProjectile.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACactusAIController::ACactusAIController()
{
	// Initialize the blackboard component
	_BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardCompCactus"));

	_AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionCactus"));
	_AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ACactusAIController::OnTargetPerceptionUpdated);

	_SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfigCactus"));

	_AIPerceptionComp->ConfigureSense(*_SightConfig);

	// Assign to Team 1
	SetGenericTeamId(FGenericTeamId(1));
}

void ACactusAIController::BeginPlay()
{
	Super::Super::BeginPlay();
}

void ACactusAIController::OnPossess(APawn * InPawn)
{
	Super::Super::OnPossess(InPawn);

	// Get the possessed Pawn. If it's the AI Character we created
	// initialize it's blackboard and start it's corresponding behavior tree
	ACactus * cactus = Cast<ACactus>(InPawn);
	if (cactus != nullptr)
	{
		if (cactus->GetBehaviorTree()->BlackboardAsset)
		{
			_BlackboardComp->InitializeBlackboard(*(cactus->GetBehaviorTree()->BlackboardAsset));
			RunBehaviorTree(cactus->GetBehaviorTree());
		}

		_BlackboardComp->SetValueAsObject(_SelfActorBlackboardKey, cactus);

		_BlackboardComp->SetValueAsFloat(_AttackRadiusBlackboardKey, cactus->m_SightRadius);

		// Configure sight config based off of cactus settings
		_SightConfig->SightRadius = cactus->m_SightRadius;
		_SightConfig->LoseSightRadius = cactus->m_LoseSightRadius;
		_SightConfig->PeripheralVisionAngleDegrees = cactus->m_VisionAngle;

		_AIPerceptionComp->ConfigureSense(*_SightConfig);
	}
}

bool ACactusAIController::FireSpike(UAnimationAsset * i_LeftFireAnimation, UAnimationAsset * i_RightFireAnimation)
{
	ACactus * cactus = Cast<ACactus>(GetPawn());
	if (cactus != nullptr)
	{
		UAnimationAsset * fireAnimation = nullptr;

		// Spawn Spike
		FActorSpawnParameters spawnSpikeParams = FActorSpawnParameters();
		spawnSpikeParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnSpikeParams.Instigator = GetPawn();

		FVector spawnLocation = FVector::ZeroVector;
		switch (cactus->GetCactusFireState())
		{
		case E_CactusFireState::AIMING:
		{
			switch (cactus->GetCactusFireLane())
			{
			case E_CactusFireLane::LEFT:
			{
				fireAnimation = i_LeftFireAnimation;
				spawnLocation = cactus->m_LeftSpikeLaunchLocation->GetComponentLocation();
				break;
			}
			case E_CactusFireLane::RIGHT:
			{
				fireAnimation = i_RightFireAnimation;
				spawnLocation = cactus->m_RightSpikeLaunchLocation->GetComponentLocation();
				break;
			}
			default:
				break;
			}
		}
		// break out of AIMING case
		break;

		case E_CactusFireState::RANDOM:
		{
			int32 randNum = FMath::RandRange(0, 1);

			// Fire down left side
			if (randNum % 2 == 0)
			{
				fireAnimation = i_LeftFireAnimation;
				spawnLocation = cactus->m_LeftSpikeLaunchLocation->GetComponentLocation();
			}
			else
			{
				fireAnimation = i_RightFireAnimation;
				spawnLocation = cactus->m_RightSpikeLaunchLocation->GetComponentLocation();
			}
			break;
		}

		default:
			break;
		}

		USkeletalMeshComponent * skeletalMesh = GetPawn()->FindComponentByClass<USkeletalMeshComponent>();

		if (skeletalMesh != nullptr && fireAnimation != nullptr)
		{
			skeletalMesh->PlayAnimation(fireAnimation, false);
		}

		FTransform spawnTransform = FTransform();
		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetRotation(cactus->GetActorRotation().Quaternion());

		ASpikeProjectile * spike = GetWorld()->SpawnActor<ASpikeProjectile>(ASpikeProjectile::StaticClass(), spawnTransform, spawnSpikeParams);
		spike->SetLifeSpan(cactus->m_SpikeTimeToLive);

		spike->m_SlowdownTime = cactus->m_SpikeSlowdownTime;
		spike->m_SlowStrength = cactus->m_SpikeSlowStrength;
		spike->m_PrintDebugStrings = cactus->m_PrintDebugStrings;

		if (spike != nullptr)
		{
			spike->LaunchProjectile(cactus->m_SpikeLaunchSpeed);

			return true;
		}
	}

	return false;
}

void ACactusAIController::OnTargetPerceptionUpdated(AActor * UpdatedActor, FAIStimulus Stimulus)
{
	APlayerHandcar * handcar = Cast<APlayerHandcar>(UpdatedActor);
	if (handcar != nullptr)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			_BlackboardComp->SetValueAsObject(_PlayerBlackboardKey, UpdatedActor);
			_BlackboardComp->SetValueAsBool(_AlertedBlackboardKey, true);
			_BlackboardComp->SetValueAsBool(_CanFireBlackboardKey, true);
		}
		else
		{
			_BlackboardComp->SetValueAsObject(_PlayerBlackboardKey, nullptr);
		}
	}
}

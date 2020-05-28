#include "ScorpionAIController.h"

#include "Scorpion.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Engine/TargetPoint.h"
#include "Perception/AISenseConfig_Sight.h"

AScorpionAIController::AScorpionAIController()
{
	// Initialize the blackboard component
	_BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardCompScorpion"));

	_AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionScorpion"));
	_AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AScorpionAIController::OnTargetPerceptionUpdated);

	_SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfigScorpion"));

	_AIPerceptionComp->ConfigureSense(*_SightConfig);

	// Assign to Team 1
	SetGenericTeamId(FGenericTeamId(1));
}

void AScorpionAIController::BeginPlay()
{
	Super::Super::BeginPlay();
}

void AScorpionAIController::OnPossess(APawn * InPawn)
{
	Super::Super::OnPossess(InPawn);


	// Get the possessed Pawn. If it's the AI Character we created
	// initialize it's blackboard and start it's corresponding behavior tree
	AScorpion * scorpion = Cast<AScorpion>(InPawn);
	if (scorpion != nullptr)
	{
		if (scorpion->GetBehaviorTree()->BlackboardAsset)
		{
			_BlackboardComp->InitializeBlackboard(*(scorpion->GetBehaviorTree()->BlackboardAsset));
			RunBehaviorTree(scorpion->GetBehaviorTree());
		}

		_BlackboardComp->SetValueAsObject(_SelfActorBlackboardKey, scorpion);

		_BlackboardComp->SetValueAsFloat(_AttackRadiusBlackboardKey, scorpion->m_SightRadius);

		// Configure sight config based off of cactus settings
		_SightConfig->SightRadius = scorpion->m_SightRadius;
		_SightConfig->LoseSightRadius = scorpion->m_LoseSightRadius;
		_SightConfig->PeripheralVisionAngleDegrees = scorpion->m_VisionAngle;

		_AIPerceptionComp->ConfigureSense(*_SightConfig);
	}
}

void AScorpionAIController::EvaulateNextTargetPoint()
{
	AScorpion * scorpion = Cast<AScorpion>(GetPawn());
	if (_BlackboardComp != nullptr && scorpion->GetTargetPoints()->Count() > 0)
	{
		ATargetPoint * targetPoint;
		// remove next target point
		scorpion->GetTargetPoints()->Dequeue(targetPoint);

		_BlackboardComp->SetValueAsObject(_LocationBlackboardKey, targetPoint);
		// add target point to end of queue to continue circular link
		scorpion->GetTargetPoints()->Enqueue(targetPoint);
	}
}

bool AScorpionAIController::TailAttack()
{
	return false;
}

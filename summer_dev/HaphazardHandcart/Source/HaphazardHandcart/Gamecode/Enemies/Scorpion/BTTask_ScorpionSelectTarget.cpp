#include "BTTask_ScorpionSelectTarget.h"

#include "ScorpionAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"

UBTTask_ScorpionSelectTarget::UBTTask_ScorpionSelectTarget()
{
	NodeName = TEXT("Scorpion Select Target");
}

EBTNodeResult::Type UBTTask_ScorpionSelectTarget::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AScorpionAIController * scorpionAI = Cast<AScorpionAIController>(OwnerComp.GetAIOwner());

	if (scorpionAI != nullptr)
	{
		UBlackboardComponent * blackBoardComp = scorpionAI->GetBlackboardComp();

		ATargetPoint * targetPoint = Cast<ATargetPoint>(blackBoardComp->
			GetValueAsObject(scorpionAI->GetLocationBlackboardKey()));

		scorpionAI->EvaulateNextTargetPoint();

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

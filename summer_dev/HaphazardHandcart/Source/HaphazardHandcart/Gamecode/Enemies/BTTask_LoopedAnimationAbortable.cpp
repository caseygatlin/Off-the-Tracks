#include "BTTask_LoopedAnimationAbortable.h"

#include "EnemyAIController.h"
#include "Gamecode/Handcars/PlayerHandcar.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTTask_LoopedAnimationAbortable::UBTTask_LoopedAnimationAbortable()
{
	NodeName = TEXT("Looped Animation Abortable");

	bNotifyTick = true;

	_ObjectKey.AllowedTypes.Empty();
	_ObjectKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LoopedAnimationAbortable, _ObjectKey), UObject::StaticClass());
}

void UBTTask_LoopedAnimationAbortable::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	AEnemyAIController * const enemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	UObject * selectedBlackBoardObj = enemyAI->GetBlackboardComp()->GetValueAsObject(_ObjectKey.SelectedKeyName);

	// if the blackboard object for the playerhandcar is not set when it should be
	if (_IsValueSet)
	{
		if (selectedBlackBoardObj == nullptr)
		{
			CleanUp(OwnerComp);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	// if the blackboard object for the playerhandcar is not set when it shouldn't be
	else
	{
		if (selectedBlackBoardObj != nullptr)
		{
			CleanUp(OwnerComp);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}

void UBTTask_LoopedAnimationAbortable::CleanUp(UBehaviorTreeComponent & OwnerComp)
{
	if (_CachedSkelMesh != nullptr && _PreviousAnimationMode == EAnimationMode::AnimationBlueprint)
	{
		_CachedSkelMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
}

EBTNodeResult::Type UBTTask_LoopedAnimationAbortable::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AEnemyAIController * const enemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	UObject * selectedBlackBoardObj = enemyAI->GetBlackboardComp()->GetValueAsObject(_ObjectKey.SelectedKeyName);

	// if the blackboard object for the playerhandcar is not set when it should be
	if (_IsValueSet)
	{
		if (selectedBlackBoardObj == nullptr)
		{
			CleanUp(OwnerComp);
			return EBTNodeResult::Failed;
		}
	}
	// if the blackboard object for the playerhandcar is not set when it shouldn't be
	else
	{
		if (selectedBlackBoardObj != nullptr)
		{
			CleanUp(OwnerComp);
			return EBTNodeResult::Failed;
		}
	}

	if (_AnimationToPlay != nullptr && enemyAI != nullptr && enemyAI->GetPawn() != nullptr)
	{
		USkeletalMeshComponent * SkelMesh = enemyAI->GetPawn()->FindComponentByClass<USkeletalMeshComponent>();

		if (SkelMesh != nullptr)
		{
			_PreviousAnimationMode = SkelMesh->GetAnimationMode();
			_CachedSkelMesh = SkelMesh;

			SkelMesh->PlayAnimation(_AnimationToPlay, true);
			const float FinishDelay = _AnimationToPlay->GetMaxCurrentTime();

			if (FinishDelay > 0)
			{
				return EBTNodeResult::InProgress;
			}
		}
	}

	return EBTNodeResult::Failed;
}

#include "BTTask_WaitBBObject.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "EnemyAIController.h"

UBTTask_WaitBBObject::UBTTask_WaitBBObject()
{
	NodeName = TEXT("WaitBBCondition");

	_ObjectKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_WaitBBObject, _ObjectKey), UObject::StaticClass());
}

void UBTTask_WaitBBObject::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AEnemyAIController * const enemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	UObject * selectedBlackBoardObj = enemyAI->GetBlackboardComp()->GetValueAsObject(_ObjectKey.SelectedKeyName);

	// object should not be set
	if (!_ObjectSet && selectedBlackBoardObj == nullptr)
	{
		Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	}
	// object should be set
	else if(_ObjectSet && selectedBlackBoardObj != nullptr)
	{
		Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	}
	// no longer waiting
	else
	{
		WaitTime = 0.0f;

		// continue execution from this node
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
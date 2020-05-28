#include "BTService_SetBoolOnTimer.h"

#include "EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTService_SetBoolOnTimer::UBTService_SetBoolOnTimer()
{
	NodeName = TEXT("SetBoolOnTimer");

	BlackboardKey.AllowedTypes.Empty();
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SetBoolOnTimer, BlackboardKey));
}

void UBTService_SetBoolOnTimer::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyAIController * enemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	enemyAI->GetBlackboardComp()->SetValueAsBool(BlackboardKey.SelectedKeyName, true);
}

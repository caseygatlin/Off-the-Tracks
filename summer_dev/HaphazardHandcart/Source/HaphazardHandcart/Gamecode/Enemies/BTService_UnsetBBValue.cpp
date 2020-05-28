#include "BTService_UnsetBBValue.h"

#include "EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTService_UnsetBBValue::UBTService_UnsetBBValue()
{
	NodeName = TEXT("UnsetBBValue");

	BlackboardKey.AllowedTypes.Empty();
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UnsetBBValue, BlackboardKey), UObject::StaticClass());
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UnsetBBValue, BlackboardKey));
}

void UBTService_UnsetBBValue::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemyAIController * enemyAI = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	if (UBlackboardKeyType_Object::StaticClass() == BlackboardKey.SelectedKeyType)
	{
		enemyAI->GetBlackboardComp()->SetValueAsObject(BlackboardKey.SelectedKeyName, nullptr);
	}
	else if (UBlackboardKeyType_Bool::StaticClass() == BlackboardKey.SelectedKeyType)
	{
		enemyAI->GetBlackboardComp()->SetValueAsBool(BlackboardKey.SelectedKeyName, false);
	}
}

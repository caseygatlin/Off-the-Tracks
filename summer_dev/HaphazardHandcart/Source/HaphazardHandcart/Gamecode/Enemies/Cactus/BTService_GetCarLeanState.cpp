#include "BTService_GetCarLeanState.h"

#include "CactusAIController.h"
#include "Gamecode/Handcars/PlayerHandcar.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTService_GetCarLeanState::UBTService_GetCarLeanState()
{
	NodeName = TEXT("Get Handcar Lean State");

	HandcarKey.AllowedTypes.Empty();
	HandcarKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetCarLeanState, HandcarKey), APlayerHandcar::StaticClass());

	CarLeanStateKey.AllowedTypes.Empty();
	CarLeanStateKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_GetCarLeanState, HandcarKey), StaticEnum<E_CartLeanState>());
}

void UBTService_GetCarLeanState::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ACactusAIController * cactusAI = Cast<ACactusAIController>(OwnerComp.GetAIOwner());

	APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(cactusAI->GetBlackboardComp()->GetValueAsObject(HandcarKey.SelectedKeyName));
	if (playerHandcar != nullptr)
	{
		cactusAI->GetBlackboardComp()->SetValueAsEnum(CarLeanStateKey.SelectedKeyName, static_cast<uint8>(playerHandcar->GetCartLeanState()));
	}
}

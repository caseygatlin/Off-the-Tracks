#include "BTDecorator_RadiusCheck.h"

#include "Cactus.h"
#include "CactusAIController.h"
#include "Gamecode/Handcars/PlayerHandcar.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTDecorator_RadiusCheck::UBTDecorator_RadiusCheck()
{
	NodeName = TEXT("Radius check if in Attacking Range");

	BlackboardKey.AllowedTypes.Empty();
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_RadiusCheck, BlackboardKey), UObject::StaticClass());
}

// Decorator to check if object is within a certain radius
bool UBTDecorator_RadiusCheck::CalculateRawConditionValue(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	ACactus * cactus = Cast<ACactus>(OwnerComp.GetAIOwner()->GetPawn());
	if (cactus == nullptr)
	{
		return false;
	}

	APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BlackboardKey.SelectedKeyName));
	if (playerHandcar == nullptr)
	{
		return false;
	}

	// whether distance between cactus and handcar is less than specified sight radius
	float distanceBetween = FVector::Dist(cactus->GetActorLocation(), playerHandcar->GetActorLocation());
	return bResult && distanceBetween < cactus->m_SightRadius;
}

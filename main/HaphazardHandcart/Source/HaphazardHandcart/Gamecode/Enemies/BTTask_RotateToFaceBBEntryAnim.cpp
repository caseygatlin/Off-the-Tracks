#include "BTTask_RotateToFaceBBEntryAnim.h"

#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Rotator.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"

UBTTask_RotateToFaceBBEntryAnim::UBTTask_RotateToFaceBBEntryAnim()
{
	NodeName = "Rotate to face BB entry with anim";
	bNotifyTick = true;
}

namespace
{
	FORCEINLINE_DEBUGGABLE float CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB)
	{
		return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
			? 1.f
			: VectorA.CosineAngle2D(VectorB);
	}
}

EBTNodeResult::Type UBTTask_RotateToFaceBBEntryAnim::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	AAIController * AIController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn * Pawn = AIController->GetPawn();
	const FVector PawnLocation = Pawn->GetActorLocation();
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	float angleDifference = 0.0f;

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* ActorValue = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()));

		if (ActorValue != NULL)
		{
			angleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
				, (ActorValue->GetActorLocation() - PawnLocation));
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());

		if (FAISystem::IsValidLocation(KeyValue))
		{
			angleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector()
				, (KeyValue - PawnLocation));
		}
	}
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Rotator::StaticClass())
	{
		const FRotator KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Rotator>(BlackboardKey.GetSelectedKeyID());

		if (FAISystem::IsValidRotation(KeyValue))
		{
			angleDifference = CalculateAngleDifferenceDot(Pawn->GetActorForwardVector(), KeyValue.Vector());
		}
	}

	if (Result == EBTNodeResult::Type::InProgress)
	{
		USkeletalMeshComponent * skeletalMesh = Pawn->FindComponentByClass<USkeletalMeshComponent>();

		// turn right anim
		if (angleDifference > 0)
		{
			if (skeletalMesh != nullptr && _RightTurnAnimation != nullptr)
			{
				skeletalMesh->PlayAnimation(_RightTurnAnimation, false);
			}
		}
		// turn left anim
		else if (angleDifference < 0)
		{
			if (skeletalMesh != nullptr && _LeftTurnAnimation != nullptr)
			{
				skeletalMesh->PlayAnimation(_LeftTurnAnimation, false);
			}
		}
	}

	return Result;
}

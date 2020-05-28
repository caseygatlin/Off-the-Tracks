#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Components/SkeletalMeshComponent.h"
#include "BTTask_LoopedAnimationAbortable.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTTask_LoopedAnimationAbortable : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_LoopedAnimationAbortable();

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void CleanUp(UBehaviorTreeComponent& OwnerComp);

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Node")
		// Animation asset to play. Note that it needs to match the skeleton of pawn this BT is controlling */
		UAnimationAsset * _AnimationToPlay;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Blackboard key selector for object that needs a reference to prevent abort
		FBlackboardKeySelector _ObjectKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Whether the object should be set or not
		uint32 _IsValueSet : 1;

	UPROPERTY()
		USkeletalMeshComponent* _CachedSkelMesh;

	EAnimationMode::Type _PreviousAnimationMode;

};

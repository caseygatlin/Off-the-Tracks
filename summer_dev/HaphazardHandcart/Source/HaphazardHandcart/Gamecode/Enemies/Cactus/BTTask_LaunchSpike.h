#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LaunchSpike.generated.h"

class UAnimationAsset;

UCLASS()
class HAPHAZARDHANDCART_API UBTTask_LaunchSpike : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_LaunchSpike();

protected:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Player handcar key selector
		struct FBlackboardKeySelector m_PlayerHandcarKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Can fire key selector
		struct FBlackboardKeySelector m_CanFireKey;

	void CleanUp(UBehaviorTreeComponent& OwnerComp);

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Node")
		// Left fire animation
		UAnimationAsset * _LeftFireAnimation;

	UPROPERTY(EditAnywhere, Category = "Node")
		// Right fire animation
		UAnimationAsset * _RightFireAnimation;

	UPROPERTY()
		USkeletalMeshComponent* _CachedSkelMesh;

	EAnimationMode::Type _PreviousAnimationMode;
	
};

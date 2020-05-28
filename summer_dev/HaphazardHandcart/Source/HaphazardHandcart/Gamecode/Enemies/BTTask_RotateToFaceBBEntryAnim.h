#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/Tasks/BTTask_RotateToFaceBBEntry.h"
#include "BTTask_RotateToFaceBBEntryAnim.generated.h"

class UAnimationAsset;

UCLASS()
class HAPHAZARDHANDCART_API UBTTask_RotateToFaceBBEntryAnim : public UBTTask_RotateToFaceBBEntry
{
	GENERATED_BODY()

public:

	UBTTask_RotateToFaceBBEntryAnim();

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Node")
		// Left turn animation
		UAnimationAsset * _LeftTurnAnimation;

	UPROPERTY(EditAnywhere, Category = "Node")
		// Right turn animation
		UAnimationAsset * _RightTurnAnimation;

};

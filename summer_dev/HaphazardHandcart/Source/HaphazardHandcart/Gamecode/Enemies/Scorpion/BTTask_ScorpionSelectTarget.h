#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ScorpionSelectTarget.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTTask_ScorpionSelectTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTTask_ScorpionSelectTarget();

private:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

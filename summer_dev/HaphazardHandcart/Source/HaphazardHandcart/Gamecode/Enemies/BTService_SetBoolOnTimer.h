#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_SetBoolOnTimer.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTService_SetBoolOnTimer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTService_SetBoolOnTimer();

private:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

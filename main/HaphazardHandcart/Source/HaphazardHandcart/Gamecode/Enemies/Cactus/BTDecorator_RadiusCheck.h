#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_RadiusCheck.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTDecorator_RadiusCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:

	UBTDecorator_RadiusCheck();

protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) const override;

};

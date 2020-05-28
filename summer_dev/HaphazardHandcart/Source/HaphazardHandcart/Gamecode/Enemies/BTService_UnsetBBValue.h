#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UnsetBBValue.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTService_UnsetBBValue : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTService_UnsetBBValue();

private:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

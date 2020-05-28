#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_WaitBBObject.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTTask_WaitBBObject : public UBTTask_Wait
{
	GENERATED_BODY()

public:

	UBTTask_WaitBBObject();
	
protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Handcar key selector
		struct FBlackboardKeySelector _ObjectKey;

	UPROPERTY(EditAnywhere, Category = "Node")
		// Whether the object should be set or not while waiting
		bool _ObjectSet = false;

};

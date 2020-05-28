#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_MoveToBBObject.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTTask_MoveToBBObject : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:

	UBTTask_MoveToBBObject();

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY(EditAnywhere, Category = "Object")
		// Handcar key selector
		struct FBlackboardKeySelector _ObjectKey;

	UPROPERTY(EditAnywhere, Category = "Object")
		// Whether the object should be set or not while waiting
		bool _ObjectSet = false;
};

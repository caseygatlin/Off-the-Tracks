#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetCarLeanState.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API UBTService_GetCarLeanState : public UBTService
{
	GENERATED_BODY()

public:

	UBTService_GetCarLeanState();

protected:

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Handcar key selector
		struct FBlackboardKeySelector HandcarKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
		// Handcar Lean State key selector
		struct FBlackboardKeySelector CarLeanStateKey;

private:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};

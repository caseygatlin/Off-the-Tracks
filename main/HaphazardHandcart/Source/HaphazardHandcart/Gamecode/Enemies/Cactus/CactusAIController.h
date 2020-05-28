#pragma once

#include "CoreMinimal.h"
#include "Gamecode/Enemies/EnemyAIController.h"
#include "CactusAIController.generated.h"

class UAnimationAsset;

UCLASS()
class HAPHAZARDHANDCART_API ACactusAIController : public AEnemyAIController
{
	GENERATED_BODY()

public:

	// Constructor
	ACactusAIController();

	virtual void BeginPlay() override;

	// executes when the controller posses a pawn
	virtual void OnPossess(APawn * InPawn) override;

#pragma region Attack

	UFUNCTION(BlueprintCallable, Category = "Enemies|Cactus|Attack")
		bool FireSpike(UAnimationAsset * i_LeftFireAnimation, UAnimationAsset * i_RightFireAnimation);

#pragma endregion

protected:

protected:

	virtual void OnTargetPerceptionUpdated(AActor * UpdatedActor, FAIStimulus Stimulus) override;

private:

#pragma region Blackboard Keys

	UPROPERTY(VisibleAnywhere, Category = "AI|BlackboardKeys")
		// Blackboard key for alerted bool
		FName _AlertedBlackboardKey = "Alerted";

	UPROPERTY(VisibleAnywhere, Category = "AI|BlackboardKeys")
		// Blackboard key for can fire bool
		FName _CanFireBlackboardKey = "CanFire";

#pragma endregion

};

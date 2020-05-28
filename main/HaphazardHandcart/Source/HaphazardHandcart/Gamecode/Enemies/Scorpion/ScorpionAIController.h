#pragma once

#include "CoreMinimal.h"
#include "Gamecode/Enemies/EnemyAIController.h"
#include "ScorpionAIController.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API AScorpionAIController : public AEnemyAIController
{
	GENERATED_BODY()
	
public:

	// Constructor
	AScorpionAIController();

	virtual void BeginPlay() override;

	// executes when the controller posses a pawn
	virtual void OnPossess(APawn * InPawn) override;

	// Set the target point in the blackboard data
	void EvaulateNextTargetPoint();

#pragma region Attack

	UFUNCTION(BlueprintCallable, Category = "Enemies|Scorpion|Attack")
		bool TailAttack();

#pragma endregion

#pragma region Getters and Setters

	UFUNCTION(BlueprintCallable, Category = "Enemies|BlackboardKeys")
		FORCEINLINE FName GetLocationBlackboardKey() const { return _LocationBlackboardKey; }

#pragma endregion

private:

#pragma region Blackboard Keys

	UPROPERTY(VisibleAnywhere, Category = "AI|BlackboardKeys")
		// Blackboard key for the location to go to
		FName _LocationBlackboardKey = "LocationToGo";

#pragma endregion

};

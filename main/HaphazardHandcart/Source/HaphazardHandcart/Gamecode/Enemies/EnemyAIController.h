#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "EnemyAIController.generated.h"

class UAISenseConfig_Sight;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class HAPHAZARDHANDCART_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:

#pragma region Getters and Setters

	UFUNCTION(BlueprintCallable, Category = "Enemies|BlackboardKeys")
		FORCEINLINE FName GetPlayerBlackboardKey() const { return _PlayerBlackboardKey; }

	UFUNCTION(BlueprintCallable, Category = "Enemies|Behavior")
		FORCEINLINE class UBlackboardComponent * GetBlackboardComp() const { return _BlackboardComp; }

#pragma endregion

protected:

	UFUNCTION()
		virtual void OnTargetPerceptionUpdated(AActor * UpdatedActor, FAIStimulus Stimulus);

#pragma region Blackboard Keys

	UPROPERTY(VisibleAnywhere, Category = "AI|BlackboardKeys")
		// Blackboard key for the target handcar
		FName _SelfActorBlackboardKey = "SelfActor";

	UPROPERTY(VisibleAnywhere, Category = "AI|BlackboardKeys")
		// Blackboard key for the target handcar
		FName _PlayerBlackboardKey = "PlayerHandcar";

	UPROPERTY(VisibleAnywhere, Category = "AI|BlackboardKeys")
		// Blackboard key for the target handcar
		FName _AttackRadiusBlackboardKey = "AttackRadius";

#pragma endregion

#pragma region Behavior Components

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		// Blackboard component reference
		UBlackboardComponent * _BlackboardComp;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		// AI perception component reference
		UAIPerceptionComponent * _AIPerceptionComp;

#pragma endregion

#pragma region Sense Config

	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
		// The configuration for the sight perception
		UAISenseConfig_Sight * _SightConfig;

#pragma endregion

};

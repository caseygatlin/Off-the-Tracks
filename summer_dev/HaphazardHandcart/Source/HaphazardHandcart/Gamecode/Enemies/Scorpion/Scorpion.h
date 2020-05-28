#pragma once

#include "CoreMinimal.h"
#include "CircularQueue.h"
#include "GameFramework/Character.h"
#include "Scorpion.generated.h"

class ATargetPoint;
class UBehaviorTree;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class HAPHAZARDHANDCART_API AScorpion : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AScorpion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma region Public Variables

#pragma region AI Settings

#pragma region Perception Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Perception|Sight")
		// Radius in which the scorpion will attack the surronding player handcar
		float m_SightRadius = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Perception|Sight")
		// Radius in which the scorpion will attack the surronding player handcar
		float m_LoseSightRadius = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Perception|Sight")
		// Radius in which the scorpion will attack the surronding player handcar
		float m_VisionAngle = 360.0f;

#pragma endregion

#pragma region SlowDown

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Slowdown")
		// How long the player slows down for
		float m_SlowdownTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Slowdown")
		// How much this object slows down the player
		float m_SlowStrength = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings|Slowdown")
		// How much to shake the camera based on the handcar speed
		float m_CameraShakeStrength = 800.0f;

#pragma endregion

#pragma endregion

#pragma region Debug Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugOptions")
		// If true any functions that need to print a message at run time will be allowed to do so.
		bool m_PrintDebugStrings = false;

#pragma endregion

#pragma endregion

#pragma region Getters and Setters

	UBehaviorTree * GetBehaviorTree() const { return _BehaviorTree; }

	TCircularQueue<ATargetPoint *> * GetTargetPoints() { return _ScorpionTargetPointsQueue; }

#pragma endregion

private:

	// Initalize Circular Queue for target points
	void InitalizeTargetPointsQueue();

#pragma region AI Settings

	UPROPERTY(EditAnywhere, Category = "AI Settings|BehaviorTree")
		// Behavior tree for the scorpion
		UBehaviorTree * _BehaviorTree;

	UPROPERTY(EditAnyWhere, Category = "AI Settings")
		// add the elements to the set in the order we will visit them  
		TSet<ATargetPoint *> _ScorpionTargetPointsSet;

	TCircularQueue<ATargetPoint *> * _ScorpionTargetPointsQueue;

#pragma endregion

};

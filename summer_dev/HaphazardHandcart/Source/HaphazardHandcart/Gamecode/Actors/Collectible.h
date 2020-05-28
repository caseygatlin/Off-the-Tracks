#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Collectible.generated.h"

class USoundBase;
class UStaticMeshComponent;

UCLASS()
class HAPHAZARDHANDCART_API ACollectible : public AActor
{
	GENERATED_BODY()

public:
	ACollectible();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma region Components 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* m_CollectibleMesh;

#pragma endregion

#pragma region Public Variables

	UPROPERTY(EditAnywhere, Category = "Movement")
		// Amount it moves up and down
		int32 m_NumTimesMove = 60;

	UPROPERTY(EditAnywhere, Category = "Movement")
		// The movement rate (rotation and hover speed)
		float m_ZTransform = 35.0f;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		// Sound to play when picking up a collectible
		USoundBase * m_CollectiblePickupSound;

#pragma endregion

protected:
	virtual void BeginPlay() override;

private:

#pragma region Private Variables


	UPROPERTY(VisibleAnywhere, Category = "Movement")
		// Whether the collectible is going up or down
		bool _GoUp = false;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		// Internal tracker for amount of times it goes down before switching to up
		int32 _Down = 0;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		// Internal tracker for amount of times it goes up before switching to down
		int32 _Up = 0;

#pragma endregion
};

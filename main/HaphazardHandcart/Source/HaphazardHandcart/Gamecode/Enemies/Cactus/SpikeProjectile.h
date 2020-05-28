#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeProjectile.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;

UCLASS()
class HAPHAZARDHANDCART_API ASpikeProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpikeProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Enemies|Cactus|Spike")
		void LaunchProjectile(float i_Speed);

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent * m_SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent * m_SpikeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBoxComponent * m_SpikeCollision;

#pragma endregion

#pragma region SlowDown

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowdown")
		// How long the player slows down for
		float m_SlowdownTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowdown")
		// How much this object slows down the player
		float m_SlowStrength = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slowdown")
		// How much to shake the camera based on the handcar speed
		float m_CameraShakeStrength = 200.0f;

#pragma endregion

#pragma region Sound Effects

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sound")
		// Sound to play when player is hit
		USoundBase * m_SpikeHitSound;

#pragma endregion

#pragma region Particle Effect

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
		// Spike explosion particle
		UNiagaraSystem * m_SpikeExplosionParticle;

#pragma endregion

#pragma region Debug Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugOptions")
		// If true any functions that need to print a message at run time will be allowed to do so.
		bool m_PrintDebugStrings = false;

#pragma endregion

#pragma endregion

private:

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		// Projectile movement component reference
		UProjectileMovementComponent * _ProjectileMovement;

};

#include "SpikeProjectile.h"

#include "Gamecode/Actors/ObstacleCameraShake.h"
#include "Gamecode/DebugFunctionLibrary.h"
#include "Gamecode/Handcars/PlayerHandcar.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASpikeProjectile::ASpikeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SpikeModel(TEXT(
		"StaticMesh'/Game/HandCarContent/Geometry/Enemies/Cactus/Cactus_Projectile.Cactus_Projectile'"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpikeHitSystem(TEXT(
		"NiagaraSystem'/Game/HandCarContent/Particles/NP_SpikeHit_System.NP_SpikeHit_System'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> SpikeHitSound(TEXT(
		"SoundCue'/Game/HandCarContent/Audio/AudioSfx/SpikeHittingHandcar_Cue.SpikeHittingHandcar_Cue'"));

	// Init Static Mesh Component for Spike
	m_SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = m_SceneComponent;
	m_SceneComponent->SetMobility(EComponentMobility::Type::Movable);

	// Init Static Mesh Component for Spike
	m_SpikeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SpikeMesh"));
	m_SpikeMesh->SetupAttachment(m_SceneComponent);
	m_SpikeMesh->SetRelativeLocation(FVector::ZeroVector);
	m_SpikeMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	m_SpikeMesh->SetStaticMesh(SpikeModel.Object);
	m_SpikeMesh->AddLocalRotation(FRotator(0.0f, 90.0f, 0.0f));
	m_SpikeMesh->SetWorldScale3D(FVector(0.66f, 0.66f, 0.66f));
	m_SpikeMesh->SetEnableGravity(false);

	m_SpikeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SpikeCollision"));
	m_SpikeCollision->SetupAttachment(m_SpikeMesh);
	m_SpikeCollision->SetWorldLocation(FVector(1.0f, -38.0f, -16.0f));
	m_SpikeCollision->SetBoxExtent(FVector(26.0f, 36.0f, 6.0f));

	m_SpikeExplosionParticle = SpikeHitSystem.Object;

	m_SpikeHitSound = Cast<USoundBase>(SpikeHitSound.Object);

	_ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovementSpike"));
	_ProjectileMovement->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ASpikeProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (m_SpikeCollision != nullptr)
	{
		m_SpikeCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASpikeProjectile::BeginOverlap);
	}
	
}

// Called every frame
void ASpikeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpikeProjectile::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(OtherActor);
	if (playerHandcar != nullptr && Cast<UBoxComponent>(OtherComp) != nullptr)
	{
		if (m_PrintDebugStrings)
		{
			FString debugText = FString("Hit the player handcar");
			UDebugFunctionLibrary::DebugPrintScreen(debugText, 2.0f, FColor::Red);
		}

		UGameplayStatics::PlaySound2D(this, m_SpikeHitSound);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), m_SpikeExplosionParticle, GetActorLocation());
		playerHandcar->SlowdownPlayer(m_SlowdownTime, m_SlowStrength);\
			UGameplayStatics::PlayWorldCameraShake(GetWorld(), UObstacleCameraShake::StaticClass(), playerHandcar->GetActorLocation(),
				0.0, playerHandcar->GetCurrentSpeed() * m_CameraShakeStrength);

		Destroy();
	}
}

void ASpikeProjectile::LaunchProjectile(float i_Speed)
{
	_ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * i_Speed);
	_ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Tell Projectile to move
	_ProjectileMovement->Activate();
}


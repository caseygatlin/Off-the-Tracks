
#include "Collectible.h"

#include "Gamecode/Handcars/PlayerHandcar.h"
#include "HaphazardHandcartGameMode.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "UObject/ConstructorHelpers.h"

ACollectible::ACollectible()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CrystalModel(TEXT(
		"StaticMesh'/Game/HandCarContent/Geometry/Meshes/Crystal_Pickup.Crystal_Pickup'"));
	static ConstructorHelpers::FObjectFinder<USoundWave> CollectiblePickupSound(TEXT(
		"SoundWave'/Game/HandCarContent/Audio/CollectibleSoundEffect.CollectibleSoundEffect'"));

	m_CollectibleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollectibleMesh"));
	RootComponent = m_CollectibleMesh;
	m_CollectibleMesh->SetStaticMesh(CrystalModel.Object);
	m_CollectibleMesh->SetMobility(EComponentMobility::Movable);
	m_CollectibleMesh->SetEnableGravity(false);
	m_CollectibleMesh->bApplyImpulseOnDamage = false;
	m_CollectibleMesh->SetGenerateOverlapEvents(true);
	m_CollectibleMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	m_CollectibleMesh->SetRelativeRotation(FRotator(15, 0, 15));

	m_CollectiblePickupSound = Cast<USoundBase>(CollectiblePickupSound.Object);
}

void ACollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// collectible is moving up
	if (_GoUp) 
	{
		_Down = 0;
		_Up++;
		if (_Up < m_NumTimesMove) 
		{
			FVector newLocation = m_CollectibleMesh->GetComponentLocation();
			newLocation.Z += m_ZTransform * DeltaTime;
			FRotator newRotation = m_CollectibleMesh->GetRelativeRotation();
			newRotation.Yaw += m_ZTransform * DeltaTime;

			FTransform newTransform = FTransform();
			newTransform.SetLocation(newLocation);
			newTransform.SetRotation(newRotation.Quaternion());

			m_CollectibleMesh->SetRelativeTransform(newTransform);
		}
		else 
		{
			_GoUp = false;
		}
	}
	// collectible is moving down
	else 
	{
		_Up = 0;
		_Down++;
		if (_Down < m_NumTimesMove)
		{
			FVector newLocation = m_CollectibleMesh->GetComponentLocation();
			newLocation.Z -= m_ZTransform * DeltaTime;
			FRotator newRotation = m_CollectibleMesh->GetRelativeRotation();
			newRotation.Yaw += m_ZTransform * DeltaTime;

			FTransform newTransform = FTransform();
			newTransform.SetLocation(newLocation);
			newTransform.SetRotation(newRotation.Quaternion());

			m_CollectibleMesh->SetRelativeTransform(newTransform);
		}
		else
		{
			_GoUp = true;
		}
	}
}

void ACollectible::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerHandcar* playerHandcar = Cast<APlayerHandcar>(OtherActor);

	if (Cast<APlayerHandcar>(OtherActor) != nullptr && Cast<UStaticMeshComponent>(OtherComp) != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, m_CollectiblePickupSound);
		(Cast<AHaphazardHandcartGameMode>(GetWorld()->GetAuthGameMode()))->IncrementNumCollectibles();
		Destroy();
	}
}

void ACollectible::BeginPlay()
{
	Super::BeginPlay();
	if (m_CollectibleMesh != nullptr)
	{
		m_CollectibleMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACollectible::BeginOverlap);
	}

	APlayerHandcar* playerHandcar = Cast<APlayerHandcar>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	playerHandcar->AddActorToCollisionIgnoreList(this, 0, true);
}
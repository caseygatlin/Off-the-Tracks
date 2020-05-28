#include "Cactus.h"

#include "CactusAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACactus::ACactus()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CactusModel(TEXT(
		"SkeletalMesh'/Game/HandCarContent/Geometry/Enemies/Cactus/Cactus2.Cactus2'"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> CactusBehaviorTree(TEXT(
		"BehaviorTree'/Game/HandCarContent/Blueprints/Enemies/Cactus/BT_Cactus.BT_Cactus'"));

	m_CactusRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CactusRoot"));
	RootComponent = m_CactusRootComponent;
	m_CactusRootComponent->SetMobility(EComponentMobility::Type::Movable);

	m_CactusMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CactusMesh"));
	m_CactusMesh->SetupAttachment(RootComponent);
	m_CactusMesh->SetSkeletalMesh(CactusModel.Object);
	m_CactusMesh->SetMobility(EComponentMobility::Type::Movable);
	m_CactusMesh->SetRelativeLocation(FVector(0.0, 0.0, -1000.0f));
	m_CactusMesh->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));

	m_RightSpikeLaunchLocation = CreateDefaultSubobject<USceneComponent>(TEXT("RightSpikeLaunchLocation"));
	m_RightSpikeLaunchLocation->SetupAttachment(RootComponent);
	m_RightSpikeLaunchLocation->SetMobility(EComponentMobility::Type::Movable);
	m_RightSpikeLaunchLocation->SetRelativeLocation(FVector(350.0f, -400.0f, -780.0f));

	m_LeftSpikeLaunchLocation = CreateDefaultSubobject<USceneComponent>(TEXT("LeftSpikeLaunchLocation"));
	m_LeftSpikeLaunchLocation->SetupAttachment(RootComponent);
	m_LeftSpikeLaunchLocation->SetMobility(EComponentMobility::Type::Movable);
	m_LeftSpikeLaunchLocation->SetRelativeLocation(FVector(350.0f, 400.0f, -780.0f));

	_BehaviorTree = CactusBehaviorTree.Object;

	AIControllerClass = ACactusAIController::StaticClass();

	// To allow the cactus enemy to rotate towards the player handcar
	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ACactus::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACactus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACactus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
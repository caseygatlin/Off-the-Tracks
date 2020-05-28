#include "Scorpion.h"

#include "Gamecode/Actors/ObstacleCameraShake.h"
#include "Gamecode/DebugFunctionLibrary.h"
#include "Gamecode/Handcars/PlayerHandcar.h"
#include "ScorpionAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AScorpion::AScorpion()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ScorpionModel(TEXT(
		"SkeletalMesh'/Game/HandCarContent/Geometry/Enemies/Scorpion/Scorpion.Scorpion'"));
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> ScorpionBehaviorTree(TEXT(
		"BehaviorTree'/Game/HandCarContent/Blueprints/Enemies/Scorpion/BT_Scorpion.BT_Scorpion'"));

	// set behavior tree
	_BehaviorTree = ScorpionBehaviorTree.Object;

	AIControllerClass = AScorpionAIController::StaticClass();

	// Set mesh properies for scorpion
	GetMesh()->SetSkeletalMesh(ScorpionModel.Object);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -240.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));

	// set correct size for capsule
	GetCapsuleComponent()->SetCapsuleRadius(240.0f);
	GetCapsuleComponent()->SetCapsuleHalfHeight(240.0f);

	// to allow the scorpion to strafe
	bUseControllerRotationYaw = false;

	_ScorpionTargetPointsQueue = nullptr;
}

// Called when the game starts or when spawned
void AScorpion::BeginPlay()
{
	Super::BeginPlay();

	if (_ScorpionTargetPointsQueue == nullptr)
	{
		InitalizeTargetPointsQueue();
	}

	GetMesh()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AScorpion::BeginOverlap);
}

// Called every frame
void AScorpion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AScorpion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AScorpion::BeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(OtherActor);
	if (playerHandcar != nullptr)
	{
		if (m_PrintDebugStrings)
		{
			FString debugText = FString("Hit the player handcar");
			UDebugFunctionLibrary::DebugPrintScreen(debugText, 2.0f, FColor::Red);
		}

		playerHandcar->SlowdownPlayer(m_SlowdownTime, m_SlowStrength);
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), UObstacleCameraShake::StaticClass(), playerHandcar->GetActorLocation(),
			0.0, playerHandcar->GetCurrentSpeed() * m_CameraShakeStrength);
	}
}

void AScorpion::InitalizeTargetPointsQueue()
{
	int32 numTargets = _ScorpionTargetPointsSet.Num();
	_ScorpionTargetPointsQueue = new TCircularQueue<ATargetPoint *>(numTargets + 1);
	for (ATargetPoint * target : _ScorpionTargetPointsSet)
	{
		_ScorpionTargetPointsQueue->Enqueue(target);
	}
	_ScorpionTargetPointsSet.Empty();
}


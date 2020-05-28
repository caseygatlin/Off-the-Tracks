#include "LeanObstacleTrigger.h"

#include "Gamecode/Actors/ObstacleCameraShake.h"
#include "Gamecode/E_CustomEnums.h"
#include "Gamecode/Handcars/PlayerHandcar.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "PaperSpriteComponent.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"

ALeanObstacleTrigger::ALeanObstacleTrigger()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> HandcarHitSound(TEXT(
		"SoundCue'/Game/HandCarContent/Audio/AudioSfx/HandcarDamageSfx_Cue.HandcarDamageSfx_Cue'"));

	m_IgnoreHelperBogies = true;

	m_Obstacle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Obstacle"));
	m_Obstacle->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	m_Obstacle->SetMobility(EComponentMobility::Type::Movable);
	m_Obstacle->SetRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::None);
	FVector obstacleLocation = m_Obstacle->GetComponentLocation();

	// init editor sprites

	m_EditorLeft = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("LeftEditorSprite"));
	m_EditorLeft->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	m_EditorLeft->SetMobility(EComponentMobility::Type::Movable);
	m_EditorLeft->SetRelativeLocation(c_LeftSpriteLocation, false, nullptr, ETeleportType::None);
	m_EditorLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_EditorCenter = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CenterEditorSprite"));
	m_EditorCenter->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	m_EditorCenter->SetMobility(EComponentMobility::Type::Movable);
	m_EditorCenter->SetRelativeLocation(c_CenterSpriteLocation, false, nullptr, ETeleportType::None);
	m_EditorCenter->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_EditorRight = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RightEditorSprite"));
	m_EditorRight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	m_EditorRight->SetMobility(EComponentMobility::Type::Movable);
	m_EditorRight->SetRelativeLocation(c_RightSpriteLocation, false, nullptr, ETeleportType::None);
	m_EditorRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	m_HitSound = Cast<USoundBase>(HandcarHitSound.Object);
}

void ALeanObstacleTrigger::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	if (m_MarkedSprite != nullptr && m_BlankSprie != nullptr)
	{
		// set up left editor sprite
		if (m_Left)
		{
			m_EditorLeft->SetSprite(m_MarkedSprite);
		}
		else
		{
			m_EditorLeft->SetSprite(m_BlankSprie);
		}
		m_EditorLeft->SetVisibility(m_Visible);

		// set up center editor sprite
		if (m_Center)
		{
			m_EditorCenter->SetSprite(m_MarkedSprite);
		}
		else
		{
			m_EditorCenter->SetSprite(m_BlankSprie);
		}
		m_EditorCenter->SetVisibility(m_Visible);

		// set up right editor sprite
		if (m_Right)
		{
			m_EditorRight->SetSprite(m_MarkedSprite);
		}
		else
		{
			m_EditorRight->SetSprite(m_BlankSprie);
		}
		m_EditorRight->SetVisibility(m_Visible);
	}
}

void ALeanObstacleTrigger::CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar)
{
	Super::CustomTriggerEvent(i_Bogie, i_Handcar);

	APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(i_Handcar);

	if (playerHandcar != nullptr)
	{
		E_CartLeanState leanState = playerHandcar->GetCartRotatedLeanState();

		bool hitLeftObstacle = m_Left && leanState == E_CartLeanState::LEFT;
		bool hitCenterObstacle = m_Center && leanState == E_CartLeanState::CENTER;
		bool hitRightObstacle = m_Right && leanState == E_CartLeanState::RIGHT;

		if (hitLeftObstacle || hitCenterObstacle || hitRightObstacle)
		{
			UGameplayStatics::PlayWorldCameraShake(GetWorld(), UObstacleCameraShake::StaticClass(), playerHandcar->GetActorLocation(),
				0.0, playerHandcar->GetCurrentSpeed() * m_CameraShakeStrength);

			UGameplayStatics::PlaySound2D(this, m_HitSound);
			playerHandcar->SlowdownPlayer(m_SlowdownTime, m_SlowStrength);
		}
	}
}
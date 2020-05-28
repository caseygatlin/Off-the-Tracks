#include "BTTask_LaunchSpike.h"

#include "Cactus.h"
#include "CactusAIController.h"
#include "Gamecode/E_CustomEnums.h"
#include "Gamecode/Handcars/PlayerHandcar.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"

UBTTask_LaunchSpike::UBTTask_LaunchSpike()
{
	NodeName = TEXT("Launch Spike");

	m_PlayerHandcarKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LaunchSpike, m_PlayerHandcarKey), APlayerHandcar::StaticClass());

	m_CanFireKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LaunchSpike, m_CanFireKey));
}

void UBTTask_LaunchSpike::CleanUp(UBehaviorTreeComponent & OwnerComp)
{
	if (_CachedSkelMesh != nullptr && _PreviousAnimationMode == EAnimationMode::AnimationBlueprint)
	{
		_CachedSkelMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
}

EBTNodeResult::Type UBTTask_LaunchSpike::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACactusAIController * cactusAI = Cast<ACactusAIController>(OwnerComp.GetAIOwner());

	if (cactusAI != nullptr)
	{
		ACactus * cactus = Cast<ACactus>(cactusAI->GetPawn());

		if (cactus != nullptr)
		{
			APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(cactusAI->GetBlackboardComp()->GetValueAsObject(m_PlayerHandcarKey.SelectedKeyName));
			if (playerHandcar != nullptr)
			{
				E_CartLeanState carLeanState = playerHandcar->GetCartLeanState();

				switch (carLeanState)
				{
				case E_CartLeanState::LEFT:
				{
					cactus->SetCactusFireState(E_CactusFireState::AIMING);

					FVector forwardVectorCactus = cactusAI->GetPawn()->GetActorForwardVector();
					FVector forwardVectorHandcar = playerHandcar->GetActorForwardVector();

					if (FVector::DotProduct(forwardVectorCactus, forwardVectorHandcar) < 0)
					{
						cactus->SetCactusFireLane(E_CactusFireLane::LEFT);
					}
					else
					{
						cactus->SetCactusFireLane(E_CactusFireLane::RIGHT);
					}
					break;
				}
				case E_CartLeanState::RIGHT:
				{
					cactus->SetCactusFireState(E_CactusFireState::AIMING);
					
					FVector forwardVectorCactus = cactusAI->GetPawn()->GetActorForwardVector();
					FVector forwardVectorHandcar = playerHandcar->GetActorForwardVector();

					if (FVector::DotProduct(forwardVectorCactus, forwardVectorHandcar) < 0)
					{
						cactus->SetCactusFireLane(E_CactusFireLane::RIGHT);
					}
					else
					{
						cactus->SetCactusFireLane(E_CactusFireLane::LEFT);
					}

					break;
				}
				default:
				{
					cactus->SetCactusFireState(E_CactusFireState::RANDOM);
					break;
				}
				}
			}
			else
			{
				cactus->SetCactusFireState(E_CactusFireState::RANDOM);
			}
		}

		USkeletalMeshComponent * SkelMesh = cactusAI->GetPawn()->FindComponentByClass<USkeletalMeshComponent>();

		if (SkelMesh != nullptr)
		{
			_PreviousAnimationMode = SkelMesh->GetAnimationMode();
			_CachedSkelMesh = SkelMesh;
		}

		if (cactusAI->FireSpike(_LeftFireAnimation, _RightFireAnimation))
		{
			cactusAI->GetBlackboardComp()->SetValueAsBool(m_CanFireKey.SelectedKeyName, false);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

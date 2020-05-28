#include "EnemyAIController.h"

#include "Gamecode/Handcars/PlayerHandcar.h"

#include "BehaviorTree/BlackboardComponent.h"

void AEnemyAIController::OnTargetPerceptionUpdated(AActor * UpdatedActor, FAIStimulus Stimulus)
{
	APlayerHandcar * handcar = Cast<APlayerHandcar>(UpdatedActor);
	if (handcar != nullptr)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			_BlackboardComp->SetValueAsObject(_PlayerBlackboardKey, UpdatedActor);
		}
		else
		{
			_BlackboardComp->SetValueAsObject(_PlayerBlackboardKey, nullptr);
		}
	}
}
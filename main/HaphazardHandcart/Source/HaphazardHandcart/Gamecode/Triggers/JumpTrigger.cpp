#include "JumpTrigger.h"

#include "Gamecode/Handcars/PlayerHandcar.h"
#include "TrainAndRail/Bogie.h"
#include "TrainAndRail/TrainTrack.h"
#include "Gamecode/DebugFunctionLibrary.h"
#include "UObject/UObjectGlobals.h"

AJumpTrigger::AJumpTrigger()
{
	m_IgnoreHelperBogies = true;
	m_TriggerOnce = true;
}

void AJumpTrigger::CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar)
{
	Super::CustomTriggerEvent(i_Bogie, i_Handcar);

	APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(i_Handcar);

	if (playerHandcar != nullptr)
	{
		if (!playerHandcar->m_IsJumping)
		{
			playerHandcar->m_DistanceTraveled += playerHandcar->m_FrontBogie->GetTrack()->m_DefaultTrackSpline->GetSplineLength();

			// Detach from track
			playerHandcar->m_IsJumping = true;
			playerHandcar->m_AttachedTrack = nullptr;

			// Set player physics
			playerHandcar->m_HandcarRootComponent->SetSimulatePhysics(true);
			playerHandcar->m_HandcarRootComponent->SetEnableGravity(true);
			playerHandcar->m_HandcarRootComponent->SetNotifyRigidBodyCollision(true);

			// Set the reference for the jump trigger for the player handcar
			playerHandcar->SetLastJumpTrigger(this);

			// Get car speed state
			E_CartSpeedState speedState = playerHandcar->GetCartSpeedState();

			// Determine in air velocity
			float speed = m_MinSpeed;

			if (speedState == E_CartSpeedState::MEDIUM)
				speed = m_MidSpeed;
			else if (speedState == E_CartSpeedState::FAST)
				speed = m_MaxSpeed;

			FVector airVelocity = FVector(0, 0, m_Upforce) + (playerHandcar->GetActorForwardVector() * (m_AirSpeedFactor * speed));
			playerHandcar->m_HandcarRootComponent->SetPhysicsLinearVelocity(airVelocity);
		}
	}
}
#include "JumpRotateTrigger.h"

#include "Gamecode/Handcars/PlayerHandcar.h"
#include "TrainAndRail/Bogie.h"
#include "TrainAndRail/TrainTrack.h"

#include "UObject/UObjectGlobals.h"

void AJumpRotateTrigger::CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar)
{
	ATrackTrigger::CustomTriggerEvent(i_Bogie, i_Handcar);

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

			// Add rotation
			playerHandcar->m_HandcarRootComponent->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, m_RotateSpeed));

			// Set handcar's time limit of rotating
			if (m_RotateSpeed == 0)
			{
				playerHandcar->m_MaxRotateDuration = 0;
			}
			else
			{
				playerHandcar->m_MaxRotateDuration = m_RotateSpeed > 0 ? 180.0 / m_RotateSpeed : -180.0 / m_RotateSpeed;
			}

			playerHandcar->m_IsRotating = true;

			playerHandcar->m_InvertMesh = !playerHandcar->m_InvertMesh;
		}
	}
}
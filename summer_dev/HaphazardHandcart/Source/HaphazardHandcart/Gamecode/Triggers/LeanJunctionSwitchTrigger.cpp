
#include "LeanJunctionSwitchTrigger.h"

#include "Gamecode/Handcars/PlayerHandcar.h"
#include "HaphazardHandcartGameMode.h"
#include "TrainAndRail/TrainTrack.h"

#include "UObject/UObjectGlobals.h"

ALeanJunctionSwitchTrigger::ALeanJunctionSwitchTrigger()
{
	m_IgnoreHelperBogies = true;
}

void ALeanJunctionSwitchTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (IsValid(m_Track))
	{
		m_Track->m_End_UseActiveConnection = true;
	}
}

void ALeanJunctionSwitchTrigger::CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar)
{
	Super::CustomTriggerEvent(i_Bogie, i_Handcar);

	if (IsValid(m_Track))
	{
		// make sure to use end active connection
		m_Track->m_End_UseActiveConnection = true;

		APlayerHandcar * playerHandcar = Cast<APlayerHandcar>(i_Handcar);

		if (playerHandcar != nullptr)
		{
			E_CartLeanState cartLeanState = playerHandcar->GetCartRotatedLeanState();

			if (cartLeanState == E_CartLeanState::CENTER && m_CenterTrack == nullptr)
			{
				cartLeanState = E_CartLeanState::LEFT;
			}
			else if (cartLeanState == E_CartLeanState::RIGHT && m_RightTrack == nullptr)
			{
				cartLeanState = E_CartLeanState::CENTER;
			}
			else if (cartLeanState == E_CartLeanState::LEFT && m_LeftTrack == nullptr)
			{
				cartLeanState = E_CartLeanState::CENTER;
			}

			// set track based off of lean state
			switch (cartLeanState)
			{
			case E_CartLeanState::LEFT:
			{
				playerHandcar->m_PathTaken.Append("L");
				m_Track->m_End_ActiveConnection = m_LeftTrack;
				break;
			}
			case E_CartLeanState::CENTER:
			{
				playerHandcar->m_PathTaken.Append("C");
				m_Track->m_End_ActiveConnection = m_CenterTrack;
				break;
			}
			case E_CartLeanState::RIGHT:
			{
				playerHandcar->m_PathTaken.Append("R");
				m_Track->m_End_ActiveConnection = m_RightTrack;
				break;
			}
			default:
				break;
			}

			(Cast<AHaphazardHandcartGameMode>(GetWorld()->GetAuthGameMode()))->SetLongestPossibleTrackLength();
		}
	}
}

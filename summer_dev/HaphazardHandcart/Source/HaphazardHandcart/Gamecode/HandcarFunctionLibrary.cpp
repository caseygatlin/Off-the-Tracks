#include "HandcarFunctionLibrary.h"

#include "Gamecode/Triggers/JumpTrigger.h"
#include "Gamecode/Triggers/LeanJunctionSwitchTrigger.h"
#include "TrainAndRail/TrainTrack.h"

UHandcarFunctionLibrary::UHandcarFunctionLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UHandcarFunctionLibrary::GetAllPossibleTrackLengths(ATrainTrack * i_CurrentTrack, float i_CurrentTravelDistance,
	FString i_CurrentPath, TMap<FString, int32> & o_TrackLengths)
{
	while (i_CurrentTrack != nullptr)
	{
		// if end active connection is set we need to travel along this path
		if (i_CurrentTrack->m_End_UseActiveConnection && i_CurrentTrack->m_End_ActiveConnection != nullptr)
		{
			i_CurrentTravelDistance += i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
			i_CurrentTrack = i_CurrentTrack->m_End_ActiveConnection;
		}
		else
		{
			ALeanJunctionSwitchTrigger * junctionTrigger = nullptr;
			AJumpTrigger * jumpTrigger = nullptr;
			// check junction and jump triggers first
			if (TrackHasBranchingTrigger(i_CurrentTrack, junctionTrigger, jumpTrigger))
			{
				// branch left, center and right
				if (junctionTrigger != nullptr && junctionTrigger->m_Active)
				{
					// we have reached the end of the tracks
					if (junctionTrigger->m_LeftTrack == nullptr && junctionTrigger->m_CenterTrack == nullptr && junctionTrigger->m_RightTrack == nullptr)
					{
						i_CurrentTrack = nullptr;
						o_TrackLengths.Add(FString(), i_CurrentTravelDistance);
						continue;
					}
					if (junctionTrigger->m_LeftTrack != nullptr)
					{
						float currentTravelDistanceRight = i_CurrentTravelDistance + i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
						FString newPath = i_CurrentPath;
						newPath.Append("L");

						GetAllPossibleTrackLengths(junctionTrigger->m_LeftTrack, currentTravelDistanceRight, newPath, o_TrackLengths);
					}
					if (junctionTrigger->m_CenterTrack != nullptr)
					{
						float currentTravelDistanceCenter = i_CurrentTravelDistance + i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
						FString newPath = i_CurrentPath;
						newPath.Append("C");

						GetAllPossibleTrackLengths(junctionTrigger->m_CenterTrack, currentTravelDistanceCenter, newPath, o_TrackLengths);
					}
					if (junctionTrigger->m_RightTrack != nullptr)
					{
						float currentTravelDistanceRight = i_CurrentTravelDistance + i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
						FString newPath = i_CurrentPath;
						newPath.Append("R");

						GetAllPossibleTrackLengths(junctionTrigger->m_RightTrack, currentTravelDistanceRight, newPath, o_TrackLengths);
					}
					// stop current iteration knowing a recursive call was made
					i_CurrentTrack = nullptr;
				}
				// go through all possible jumps
				else if(jumpTrigger->m_Active)
				{
					// only one connecting source no need for recursive step
					if (jumpTrigger->m_LandingTracks.Num() == 1)
					{
						i_CurrentTravelDistance += i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
						i_CurrentTrack = jumpTrigger->m_LandingTracks[0];

						i_CurrentPath.Append("J");
						i_CurrentPath.AppendInt(0);
					}
					// recursivly go through all branching tracks
					else if (jumpTrigger->m_LandingTracks.Num() > 1)
					{
						for (int32 i = 0; i < jumpTrigger->m_LandingTracks.Num(); i++)
						{
							float currentTravelDistanceJump = i_CurrentTravelDistance + i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();

							FString newPath = i_CurrentPath;
							newPath.Append("J");
							newPath.AppendInt(i);

							GetAllPossibleTrackLengths(jumpTrigger->m_LandingTracks[i], currentTravelDistanceJump, newPath, o_TrackLengths);
						}
						i_CurrentTrack = nullptr;
					}
				}
				else
				{
					i_CurrentTravelDistance += i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
					i_CurrentTrack = nullptr;
					o_TrackLengths.Add(i_CurrentPath, i_CurrentTravelDistance);
				}
			}
			// go through normal track connections
			else
			{
				TArray<ATrainTrack*> connectedTracks = i_CurrentTrack->GetTrackConnections(E_TrackSplineLocation::End);

				// only one connecting source no need for recursive step
				if (connectedTracks.Num() == 1)
				{
					i_CurrentTravelDistance += i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
					i_CurrentTrack = connectedTracks[0];
				}
				// we have reached the end of the tracks
				else
				{
					i_CurrentTravelDistance += i_CurrentTrack->m_DefaultTrackSpline->GetSplineLength();
					i_CurrentTrack = nullptr;
					o_TrackLengths.Add(i_CurrentPath, i_CurrentTravelDistance);
				}
			}
		}
	}
}

bool const UHandcarFunctionLibrary::TrackHasBranchingTrigger(ATrainTrack * i_CurrentTrack, ALeanJunctionSwitchTrigger *& o_FoundJunctionTrigger,
	AJumpTrigger *& o_FoundJumpTrigger)
{
	TArray<ATrackTrigger*> currentTrackTriggers = i_CurrentTrack->GetTrackTriggers();
	for (int32 i = 0; i < currentTrackTriggers.Num(); i++)
	{
		// check for junction triggers first
		ALeanJunctionSwitchTrigger * possibleJunctionTrigger = Cast<ALeanJunctionSwitchTrigger>(currentTrackTriggers[i]);
		if (possibleJunctionTrigger != nullptr)
		{
			// junction triggers have higher priority than jump triggers so break
			o_FoundJunctionTrigger = possibleJunctionTrigger;
			break;
		}

		// check for possible jump trigger at some iteration
		AJumpTrigger * possibleJumpTrigger = Cast<AJumpTrigger>(currentTrackTriggers[i]);
		if (possibleJumpTrigger != nullptr)
		{
			o_FoundJumpTrigger = possibleJumpTrigger;
		}
	}
	return o_FoundJunctionTrigger != nullptr || o_FoundJumpTrigger != nullptr;
}

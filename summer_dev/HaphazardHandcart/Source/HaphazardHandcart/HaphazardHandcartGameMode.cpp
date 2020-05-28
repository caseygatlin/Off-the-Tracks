#include "HaphazardHandcartGameMode.h"

#include "Gamecode/HandcarFunctionLibrary.h"
#include "Gamecode/Handcars/PlayerHandcar.h"
#include "Gamecode/Tracks/StartTrack.h"

#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

AHaphazardHandcartGameMode::AHaphazardHandcartGameMode()
{

}

void AHaphazardHandcartGameMode::StartPlay()
{
	Super::StartPlay();

	UWorld * theWorld = GetWorld();
	if (theWorld != nullptr)
	{
		_PlayerHandcar = Cast<APlayerHandcar>(UGameplayStatics::GetPlayerPawn(theWorld, 0));

		TArray<AActor*> startTracks;
		UGameplayStatics::GetAllActorsOfClass(theWorld, AStartTrack::StaticClass(), startTracks);

		// TODO add check that there are 1 or more end tracks
		if (startTracks.Num() != 1)
		{
#ifdef UE_BUILD_DEBUG

			GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red, "There is not exactly one starting track");
#endif
		}
		else
		{
			UHandcarFunctionLibrary::GetAllPossibleTrackLengths(Cast<AStartTrack>(startTracks[0]), 0, FString(), _AllTrackLengths);
			SetLongestPossibleTrackLength();
		}
	}
}

void AHaphazardHandcartGameMode::IncreaseLevelTimer(float i_Amount)
{
	_LevelTimer += i_Amount;
}

void AHaphazardHandcartGameMode::DecreaseLevelTimer(float i_Amount)
{
	_LevelTimer -= i_Amount;
}

void AHaphazardHandcartGameMode::SetLongestPossibleTrackLength()
{
	// find the longest of all of them if there is no path
	if (_PlayerHandcar->m_PathTaken.IsEmpty())
	{
		int32 possibleLongest = 0;
		for (auto TrackLenIt = _AllTrackLengths.CreateIterator(); TrackLenIt; ++TrackLenIt)
		{
			if (TrackLenIt.Value() > possibleLongest)
			{
				possibleLongest = TrackLenIt.Value();
			}
		}
		_CurrentLongestTrackLength = possibleLongest;
		if (_CurrentLongestTrackLength == 0)
		{
			_CurrentLongestTrackLength = 0;
		}
	}
	// find the longest track length for any paths that could match the current one
	else
	{
		int32 possibleLongest = 0;
		for (auto TrackLenIt = _AllTrackLengths.CreateIterator(); TrackLenIt; ++TrackLenIt)
		{
			if (TrackLenIt.Key().StartsWith(_PlayerHandcar->m_PathTaken) && TrackLenIt.Value() > possibleLongest)
			{
				possibleLongest = TrackLenIt.Value();
			}
		}
		_CurrentLongestTrackLength = possibleLongest;
		if (_CurrentLongestTrackLength == 0)
		{
			_CurrentLongestTrackLength = 0;
		}
	}
}

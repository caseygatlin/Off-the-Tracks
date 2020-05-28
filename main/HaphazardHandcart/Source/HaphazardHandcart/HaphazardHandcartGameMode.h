#pragma once

#include "GameFramework/GameModeBase.h"
#include "Gamecode/Handcars/PlayerHandcar.h"
#include "HaphazardHandcartGameMode.generated.h"

UCLASS(minimalapi)
class AHaphazardHandcartGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHaphazardHandcartGameMode();

	virtual void StartPlay() override;

#pragma region Game Mode Helpers

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Player")
		APlayerHandcar * GetPlayerHandcar() const { return _PlayerHandcar; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Level Timer")
		float GetLevelTimer() const { return _LevelTimer; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Level Timer")
		void SetLevelTimer(float i_NewTime) { _LevelTimer = i_NewTime; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Collectibles")
		int32 GetNumCollectibles() const { return _NumCollectibles; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Collectibles")
		void IncrementNumCollectibles() { ++_NumCollectibles; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Track Lengths")
		float GetCurrentDistanceTraveled() const { return _PlayerHandcar->m_DistanceTraveled; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Track Lengths")
		float GetCurrentLongestTrackLength() const { return _CurrentLongestTrackLength; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Level Timer")
		void IncreaseLevelTimer(float i_Amount);

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Level Timer")
		void DecreaseLevelTimer(float i_Amount);

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Track Lengths")
		TMap<FString, int32> GetTrackLenghts() const { return _AllTrackLengths; }

	UFUNCTION(BlueprintCallable, Category = "Game Mode|Track Lengths")
		void SetLongestPossibleTrackLength();

#pragma endregion

private:

#pragma region GameMode Variables

	UPROPERTY(VisibleAnywhere, Category = "Player")
		APlayerHandcar * _PlayerHandcar;

	UPROPERTY(VisibleAnywhere, Category = "Level Timer")
		float _LevelTimer;

	UPROPERTY(VisibleAnywhere, Category = "Collectibles")
		// The number of picked up collectables
		uint32 _NumCollectibles = 0;

	UPROPERTY(VisibleAnywhere, Category = "Track Lengths")
		// All possible track lengths
		TMap<FString, int32> _AllTrackLengths;

	UPROPERTY(VisibleAnywhere, Category = "Track Lengths")
		// The current distance traveled by the handcar
		int32 _CurrentDistanceTraveled;

	UPROPERTY(VisibleAnywhere, Category = "Track Lengths")
		// The current longest possible track
		int32 _CurrentLongestTrackLength;

#pragma endregion

};




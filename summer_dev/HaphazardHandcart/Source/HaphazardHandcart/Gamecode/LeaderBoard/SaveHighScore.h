// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveHighScore.generated.h"

UCLASS()
class HAPHAZARDHANDCART_API USaveHighScore : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Save")
		// Save a high score to list.
		void SaveHighScore(int score);

	UFUNCTION(BlueprintCallable, Category = "Load")
		// Save a high score to list.
		TArray<int> GetHighScores() const;

	UPROPERTY(EditAnywhere, Category = "Save Settings")
		// Number of high scores to be saved.
		int m_Capacity = 10;

private:
	UPROPERTY(VisibleAnywhere, Category = "Save Settings")
		FString _SlotName = "HighScore";

	UPROPERTY(VisibleAnywhere, Category = "Save Settings")
		uint32 _UserIndex = 0;

	UPROPERTY(VisibleAnywhere, Category = "Save Settings")
		// Underlying high score container. 
		TArray<int> _HighScoreHolder;
};

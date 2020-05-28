#include "SaveHighScore.h"
#include "Kismet/GameplayStatics.h"

TArray<int> USaveHighScore::GetHighScores() const
{
	return _HighScoreHolder;
}

void USaveHighScore::SaveHighScore(int score)
{
	if (_HighScoreHolder.Num() < m_Capacity)
	{
		_HighScoreHolder.Add(score);
	}
	else if (score > _HighScoreHolder[m_Capacity - 1])
	{
		_HighScoreHolder[m_Capacity - 1] = score;
	}
	else
	{
		return;
	}

	for (int i = _HighScoreHolder.Num() - 1; i > 0; i--)
	{
		if (_HighScoreHolder[i] > _HighScoreHolder[i - 1])
		{
			int temp = _HighScoreHolder[i];
			_HighScoreHolder[i] = _HighScoreHolder[i - 1];
			_HighScoreHolder[i - 1] = temp;
		}
		else
			break;
	}

	UGameplayStatics::SaveGameToSlot(this, _SlotName, _UserIndex);
}
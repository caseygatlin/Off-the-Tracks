#include "ObstacleCameraShake.h"

UObstacleCameraShake::UObstacleCameraShake()
{
	OscillationDuration = 0.25f;

	RotOscillation.Pitch.Amplitude = 5.0f;
	RotOscillation.Pitch.Frequency = 50.0f;

	RotOscillation.Yaw.Amplitude = 5.0f;
	RotOscillation.Yaw.Frequency = 50.0f;

	LocOscillation.X.Amplitude = 50.0f;
	LocOscillation.X.Frequency = 1.0f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;

	LocOscillation.Z.Amplitude = 10.0f;
	LocOscillation.Z.Frequency = 50.0f;
}


#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

UENUM()
enum class E_BogieDirectionOfTravel : uint8
{
	Forward,
	Reverse
};

UENUM()
enum class E_HandcarState : uint8
{
	Moving,
	Stopped,
	Derailed
};

UENUM()
enum class E_HandcarType : uint8
{
	Engine,
	Handcar
};

UENUM()
enum class E_EndOfTrackOption : uint8
{
	BRAKE_AND_STOP_IF_POSSIBLE,
	BRAKE_AND_ALWAYS_STOP,
	BRAKE_AND_REVERSE_DIRECTION,
	ALWAYS_STOP,
	DERAIL,
	INSTANTLY_REVERSE_DIRECTION,
	WARP_TO_ORIGINAL_TRACK_AND_LOCATION
};

UENUM()
enum class E_RollCheckEval : uint8
{
	Any,
	All,
	Roll_Only,
	Pitch_Only,
	Yaw_Only
};

UENUM()
enum class E_TrackMeshSpacingAxis : uint8
{
	X,
	Y,
	Z
};

UENUM()
enum class E_TrackObstacleResponse : uint8
{
	IGNORE_ARE_CRASH_INTO_IT,
	BRAKE_STOP_IF_POSSIBLE_AND_WAIT_FOR_IT_TO_MOVE,
	BRAKE_STOP_IF_POSSIBLE_AND_REVERSE_DIRECTION,
	INSTANTLY_STOP_AND_WAIT_FOR_IT_TO_MOVE,
	INSTANTLY_STOP_AND_REVERSE_DIRECTION,
	DERAIL_NOW
};

UENUM()
enum class E_TrackSplineLocation : uint8
{
	Start,
	End
};
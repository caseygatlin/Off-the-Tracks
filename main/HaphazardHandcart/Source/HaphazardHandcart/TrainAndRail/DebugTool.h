
#pragma once

#include "TrainAndRail/Structs/S_TrackConnectionPoint.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebugTool.generated.h"

class AHandcar;
class ATrainTrack;

class UBillboardComponent;

UCLASS()
class HAPHAZARDHANDCART_API ADebugTool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADebugTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when an instance of this class is placed (in editor) or spawned.
	virtual void OnConstruction(const FTransform& Transform) override;

#pragma region Helper Methods

	void Conn_RebuildAllConnections();
	void Conn_PrintConnectionInfoForAllTracks();
	void PrintConnectionInfoForTrack(ATrainTrack * i_TrainTrack);
	void Conn_PrintConnInfoForSelectedTrack();
	void PrintConnectionPointInfo(FS_TrackConnectionPoint & i_ConnectionPoint, FString i_Name);
	void SelCar_PrintAllBogieNames();
	void Track_PrintTriggerRegions();
	void Track_DeleteAllTriggerRegions();
	void SelCar_ShowDebugHitch();
	void SelCar_DisplaySensorBogie();

#pragma endregion

#pragma region Custom Helper Methods

	void ValidateTracksAndTrains();

#pragma endregion

#pragma region Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UBillboardComponent * m_EditorIconBB;

#pragma endregion

#pragma region Connection Debug

	UPROPERTY(EditAnywhere, Category = "ConnectionDebug")
		bool m_RebuildAllConnections = false;

	UPROPERTY(EditAnywhere, Category = "ConnectionDebug")
		bool m_PrintConnInfoForAllTracks = false;

	UPROPERTY(EditAnywhere, Category = "ConnectionDebug")
		bool m_PrintConnInfoForSelectedTrack = false;

#pragma endregion

#pragma region Selected

	UPROPERTY(EditAnywhere, Category = "Selected")
		TArray<ATrainTrack*> m_SelectedTracks;

	UPROPERTY(EditAnywhere, Category = "Selected")
		TArray<AHandcar*> m_SelectedHandcars;

#pragma endregion

#pragma region Handcar

	UPROPERTY(EditAnywhere, Category = "Handcar")
		bool m_SelCar_PrintBogieNames = false;

	UPROPERTY(EditAnywhere, Category = "Handcar")
		bool m_SelCar_ShowDebugHitches = false;

	UPROPERTY(EditAnywhere, Category = "Handcar")
		bool m_SelCar_ShowSensorBogie = false;

#pragma endregion

#pragma region Tracks

	UPROPERTY(EditAnywhere, Category = "Tracks")
		bool m_PrintTriggerRegions = false;

	UPROPERTY(EditAnywhere, Category = "Tracks")
		bool m_DeleteAllTriggerRegions = false;

#pragma endregion

#pragma region Render Settings

	UPROPERTY(EditAnywhere, Category = "RenderSettings")
		float m_DebugTextTimeOnScreen = 0.004f;

	UPROPERTY(EditAnywhere, Category = "RenderSettings")
		float m_DebugShapeTimeOnScreen = 0.05f;

#pragma endregion

#pragma endregion

};

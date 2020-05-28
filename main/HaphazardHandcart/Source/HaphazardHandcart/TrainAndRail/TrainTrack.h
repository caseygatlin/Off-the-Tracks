#pragma once

#include "E_TrainAndRail.h"
#include "Structs/S_TrackSpline.h"
#include "Structs/S_TrackTriggerRegion.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/Material.h"
#include "TrainTrack.generated.h"

class AHandcar;
class ATrainCarriage;
class ATrackTrigger;
class UBogie;

class UArrowComponent;
class USceneComponent;
class USplineComponent;
class USplineMeshComponent;
class UStaticMeshComponent;

UCLASS()
class HAPHAZARDHANDCART_API ATrainTrack : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrainTrack();
	~ATrainTrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called when an instance of this class is placed (in editor) or spawned.
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostLoad() override;

#pragma region Editor Events

#if WITH_EDITOR
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditUndo() override;
	virtual void PostEditUndo() override;
	// Called after an actor has been moved in the editor
	virtual void PostEditMove(bool bFinished);
#endif // WITH_EDITOR

#pragma endregion

#pragma region Helper Methods

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Initalize")
		void InitalizeTrainTrack();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Initalize")
		void InitalizeTrackSplines();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Connection")
		void BuildTrackConnections(bool i_DoNotRefreshOtherTracks);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Arrows")
		void SetupArrows();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Arrows")
		void AddConnectionArrow(FRotator i_Rotation, FLinearColor i_Color, USplineComponent * i_Spline, int32 i_SplinePoint, FName i_arrowName, bool i_ShowArrow);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Spline")
		USplineComponent * GetSplineByIndex(int32 i_SplinePoint) const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Spline")
		FS_TrackSpline GetTrackSplineByIndex(int32 i_SplineIndex, bool & o_ValidTrackSpline) const;

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Spline")
		int32 GetSplineIndex(USplineComponent * i_Spline);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Connection")
		FS_TrackConnection PickConnectionToUse(const FS_TrackConnectionPoint & i_ConnectionPoint, bool i_ConnPointAtEOT, bool & o_ConnectionFound);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Trigger")
		bool RegisterTrackTrigger(ATrackTrigger * i_NewTrigger);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Trigger")
		void ValidateTriggers();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Trigger")
		void AddTriggerRegion(FS_TrackTriggerRegion & i_NewTriggerRegion);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Trigger")
		void ClearTriggerRegions(const ATrackTrigger * i_Trigger, bool i_WipeAllRegions);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Trigger")
		void CheckForRegionOverlap(UBogie * i_Bogie, AHandcar * i_Handcar);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Building")
		void BuildTrackMesh();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Connection")
		void RebuildTrackConnections();

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Arrows")
		void AddUpDirArrow(FVector & i_ArrowLocation, FVector & i_UpDirForArrow);

	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Building")
		void BuildSplineFromBakingSource();

#pragma endregion

#pragma region Custom Helper Methods

	void AddHandcarReference(AHandcar * i_Handcar);
	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Connections")
		TArray<ATrainTrack*> GetTrackConnections(E_TrackSplineLocation i_SplineLocation);
	void RemoveHandcarReference(AHandcar * i_Handcar);
	void UpdateTrack();
	void UpdateConnectionArrows();
	void UpdateConnectionArrow(USplineComponent * i_Spline, UArrowComponent * i_ConnectionArrow, bool i_ShowArrow);
	void UpdateConnections();
	void UpdateUpArrow(USplineComponent * i_Spline, UArrowComponent * i_UpArrow);

#pragma endregion

#pragma region Getters And Setters

	TArray<FS_TrackSpline> const GetTrackSplines() { return _TrackSplines; };
	UFUNCTION(BlueprintCallable, Category = "TrainAndRail|TrainTrack|Trigger")
		TArray<ATrackTrigger*> GetTrackTriggers() { return _Triggers; }
	TArray<FS_TrackTriggerRegion> const GetTriggerRegions() { return _TriggerRegions; }

#pragma endregion

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USplineComponent * m_DefaultTrackSpline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent * m_Scene;

#pragma endregion

#pragma region Build Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Mesh to use for the tracks
		UStaticMesh * m_TrackMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Turn on or off mesh building. Use this option to speed things up if you’re placing big tracks.
		bool m_DisableMeshBuilding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// If the length of the spline tangent are greater than MaxTangentLengththen it will be multiplied by this value to reduce it.
		float m_TangentScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// When adding spline mesh the length of the tangent will be compared to this value, 
		// if it’s greater than this value, the tangent will be scaled back.
		float m_MaxTangentLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Turn connection building on or off, turn this off to speed things up when building large tracks.
		bool m_DisableConnectionBuilding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// If turned on, arrows that the track connection points will always be visible.
		bool m_AlwaysShowConnectionArrows = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Set the up direction for the track spline. Default is 0, 0, 1
		FVector m_UpDirection = FVector(0.0f, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// If true then the UpDirection will be manually set at each spline point.
		bool m_SetUpDirForAllSplinePoints = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Offset applied to the connection arrows.
		FVector m_ArrowOffset = FVector(0.0f, 0.0f, 25.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// True by default, this will make sure that the track mesh is always the same length as the spline. 
		// If your track is 50 units in length and the track mesh is 20 then you'll get two normal size sections and one squashed section.
		bool m_ForceMeshToMatchSplineLength = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Select the axis (X, Y or Z) that you'd like to use when calculating the size of each section of track mesh.
		// E.g. If the mesh is 20 on X, then select x and a piece of mesh will be added every 20 units along the spline.
		E_TrackMeshSpacingAxis m_MeshSpacingAxis = E_TrackMeshSpacingAxis::X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		// Use this to increase the spacing between track mesh sections. 1.0 by default. 
		float m_MeshSpacingMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildOptions")
		bool m_ApplySplineRollToMesh = true;

#pragma endregion

#pragma region Connection Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConnectionOptions")
		// If the distance between two track connection points is greater than this value, then the connection won’t be made. 
		float m_MaxConnectionDistance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConnectionOptions")
		// Any track on this list will be ignored when building track connections.
		TArray<ATrainTrack *> m_TracksToIgnore;

#pragma endregion

#pragma region Active Connection Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveConnectionOptions")
		// Should the end connection point use a random connection
		bool m_End_UseRandomConnection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveConnectionOptions")
		// Should the end connection point use the active connection
		bool m_End_UseActiveConnection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveConnectionOptions")
		// Track the end point should use as the active connection. End_UseActiveConnection must be set to true for this to be used.
		ATrainTrack * m_End_ActiveConnection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveConnectionOptions")
		// Should the start connection point use a random connection
		bool m_Start_UseRandomConnection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveConnectionOptions")
		// Should the start connection point use the active connection
		bool m_Start_UseActiveConnection = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActiveConnectionOptions")
		// Track the start point should use as the active connection. Start_UseActiveConnection must be set to true for this to be used.
		ATrainTrack * m_Start_ActiveConnection;

#pragma endregion

#pragma region Debug Options

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugOptions")
		// Draws a debug arrow indicating the up direction for the track,
		// this will be a single arrow if the up direction is being set for all points,
		// otherwise an arrow will be added to every spline point.
		bool m_ShowUpDir = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DebugOptions")
		// Forces an arrow to be rendered at every spline point showing the up direction
		bool m_ShowUpDirForAllPoints = false;

#pragma endregion

#pragma region Track Baking

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackBaking")
		// Track to copy the spline points from. E.g. Convert a terrain track to a regular track.
		ATrainTrack * m_BakingSourceTrack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackBaking")
		// This will return this track to its original location after it's copying points from the source track.
		bool m_ReturnToOriginalLocation = false;

#pragma endregion

#pragma endregion

private:

#pragma region Private Methods

	// TODO set up components in constructor if its a prefab version
	void PrefabricatorReinit();
	void UpdateAttachedHandcars();

#pragma endregion

#pragma region Private Variables

#pragma region Components

	UArrowComponent * _StartConnectionArrow;
	UArrowComponent * _EndConnectionArrow;
	UArrowComponent * _UpArrow;

#pragma endregion

#pragma region Internal

	UPROPERTY(VisibleAnywhere, Category = "Internal")
		// Internal array of track splines. This is created and updated automatically, do not edit manually.
		TArray<FS_TrackSpline> _TrackSplines;

#pragma endregion

#pragma region Triggers
	UPROPERTY(VisibleAnywhere, Category = "Triggers")
		// Array of references to triggers that have a region on this track. This is updated automatically, do not edit manually.
		TArray<ATrackTrigger*> _Triggers;
	UPROPERTY(VisibleAnywhere, Category = "Triggers")
		// Array of trigger regions on this track, this updated automatically, do not edit manually.
		TArray<FS_TrackTriggerRegion> _TriggerRegions;

#pragma endregion

#pragma endregion

#pragma region Macros and Inline

	bool WithinConnectionDistance(FVector & i_PointA, FVector & i_PointB);

#pragma endregion

#pragma region Custom Private Variables

	TArray<USplineMeshComponent*> _TrackMeshes;
	TArray<AHandcar*> _AttachedHandcars;
	bool _ConnectionMade = false;
	// TODO possibly iterate on this to allow multiple splines on a track if we still want that feature
	int32 _NumSplinePoints = 2;

#pragma endregion

#pragma region Const Variables

	const FName c_StartArrowName = FName(TEXT("StartArrow"));
	const FRotator c_StartArrowRotation = FRotator::ZeroRotator;
	const FLinearColor c_StartArrowColor = FLinearColor(0.319862f, 1.0f, 0.0f, 1.0f);
	const FName c_EndArrowName = FName(TEXT("EndArrow"));
	const FRotator c_EndArrowRotation = FRotator(180.0f, 0.0f, 0.0f);
	const FLinearColor c_EndArrowColor = FLinearColor(1.0f, 0.167282f, 0.0f, 1.0f);
	const FName c_UpArrowName = FName(TEXT("UpDirArrow"));
	const FLinearColor c_UpArrowColor = FLinearColor(0.0f, 0.951893f, 1.0f, 1.0f);

#pragma endregion
};

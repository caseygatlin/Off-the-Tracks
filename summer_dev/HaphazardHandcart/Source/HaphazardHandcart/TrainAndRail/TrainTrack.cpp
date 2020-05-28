#include "TrainTrack.h"

#include "Bogie.h"
#include "Handcar.h"
#include "TrackTrigger.h"
#include "TrainAndRailFunctionLibrary.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"


// Sets default values
ATrainTrack::ATrainTrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initalize components
	m_Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = m_Scene;
	m_Scene->SetMobility(EComponentMobility::Type::Movable);
	m_Scene->SetVisibility(true);

	m_DefaultTrackSpline = CreateDefaultSubobject<USplineComponent>(TEXT("DefaultTrackSpline"));
	m_DefaultTrackSpline->SetupAttachment(RootComponent);
	m_Scene->SetMobility(EComponentMobility::Type::Movable);

	m_DefaultTrackSpline->bInputSplinePointsToConstructionScript = true;

	// Construction Script
	if (IsValid(m_BakingSourceTrack))
	{
		BuildSplineFromBakingSource();
		m_BakingSourceTrack = nullptr;
	}

	InitalizeTrainTrack();
}

ATrainTrack::~ATrainTrack()
{
	if (IsValid(this))
	{
		UpdateConnections();
	}
}

// Called when the game starts or when spawned
void ATrainTrack::BeginPlay()
{
	Super::BeginPlay();

	BuildTrackMesh();
	ValidateTriggers();
	UpdateConnections();
}

// Called every frame
void ATrainTrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrainTrack::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	// if we need to initalize the track data for prefabricator instance
	if (_TrackSplines[0].Spline == nullptr)
	{
		PrefabricatorReinit();
	}

	// using this for when we copy a track over
	TArray<USplineMeshComponent*> previousSplineMeshes;
	GetComponents<USplineMeshComponent>(previousSplineMeshes);

	for (int32 i = 0; i < previousSplineMeshes.Num(); i++)
	{
		if (!_TrackMeshes.Contains(previousSplineMeshes[i]))
		{
			previousSplineMeshes[i]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			previousSplineMeshes[i]->UnregisterComponent();
			previousSplineMeshes[i]->DestroyComponent();
		}
	}
	
	// only call constructor if we haven't added spline more than the 1 spline mesh
	if (_TrackMeshes.Num() <= 1)
	{
		UpdateTrack();
	}
}

void ATrainTrack::PostLoad()
{
	Super::PostLoad();

	_NumSplinePoints = m_DefaultTrackSpline->GetNumberOfSplinePoints();
}

#pragma region Editor Events

#if WITH_EDITOR

bool ATrainTrack::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);

	return false;
}

void ATrainTrack::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateTrack();
	UpdateAttachedHandcars();
	ValidateTriggers();
}

void ATrainTrack::PreEditUndo()
{
	Super::PreEditUndo();
}

void ATrainTrack::PostEditUndo()
{
	Super::PostEditUndo();
}

void ATrainTrack::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if (bFinished || _NumSplinePoints != m_DefaultTrackSpline->GetNumberOfSplinePoints())
	{
		_NumSplinePoints = m_DefaultTrackSpline->GetNumberOfSplinePoints();
		UpdateTrack();
		UpdateAttachedHandcars();
		ValidateTriggers();
	}
}

#endif // WITH_EDITOR

#pragma endregion

#pragma region HelperFunctions
void ATrainTrack::InitalizeTrainTrack()
{
	InitalizeTrackSplines();
	BuildTrackMesh();
	BuildTrackConnections(false);
	SetupArrows();
	ValidateTriggers();
}

void ATrainTrack::InitalizeTrackSplines()
{
	TArray<USplineComponent*> usableSplineComponents = UTrainAndRailFunctionLibrary::GetUsableSplinesFromTrack(this);
	for (int32 i = 0; i < usableSplineComponents.Num(); i++)
	{
		FS_TrackSpline trackSpline = FS_TrackSpline();
		trackSpline.Spline = usableSplineComponents[i];
		trackSpline.StartConnectionIndex = 0;
		trackSpline.EndConnectionIndex = 0;

		_TrackSplines.Add(trackSpline);
	}
}

void ATrainTrack::BuildTrackConnections(bool i_DoNotRefreshOtherTracks)
{
	if (!m_DisableConnectionBuilding)
	{
		UWorld * world = GetWorld();
		TArray<AActor*> tracksInWorld;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrainTrack::StaticClass(), tracksInWorld);

		TArray<ATrainTrack*> tracksToUpdate;

		TArray<FS_TrackConnection> startConnections;
		TArray<FS_TrackConnection> endConnections;
		// Loop through all the splines in our track
		for (int32 i = 0; i < _TrackSplines.Num(); i++)
		{
			startConnections.Empty();
			endConnections.Empty();

			// Store the start and end of our current spline
			FVector ourStart = UTrainAndRailFunctionLibrary::GetStartLocationOfSpline(_TrackSplines[i].Spline);
			FVector ourEnd = UTrainAndRailFunctionLibrary::GetEndLocationOfSpline(_TrackSplines[i].Spline);

			// Loop through the tracks in the world
			for (int32 j = 0; j < tracksInWorld.Num(); j++)
			{
				ATrainTrack * currentTestTrack = Cast<ATrainTrack>(tracksInWorld[j]);

				// Skip the track if it's on our ignore list
				if (!m_TracksToIgnore.Contains(currentTestTrack))
				{
					// Check if we are testing against ourself
					bool testingAgainstSelf = this == currentTestTrack;

					// Loop through all the splines on the track
					for (int32 k = 0; k < currentTestTrack->_TrackSplines.Num(); k++)
					{
						// Store the start and end of their spline
						bool connectionMade = false;
						FVector currentStart = UTrainAndRailFunctionLibrary::GetStartLocationOfSpline(currentTestTrack->_TrackSplines[k].Spline);
						FVector currentEnd = UTrainAndRailFunctionLibrary::GetEndLocationOfSpline(currentTestTrack->_TrackSplines[k].Spline);

						// End to start
						if (WithinConnectionDistance(ourEnd, currentStart))
						{
							FS_TrackConnection newTrackConnection = FS_TrackConnection();
							newTrackConnection.Track = currentTestTrack;
							newTrackConnection.ConnectionID = k;
							newTrackConnection.Location = E_TrackSplineLocation::Start;

							endConnections.Add(newTrackConnection);
							connectionMade = true;
						}
						// Start to start
						if (WithinConnectionDistance(ourStart, currentStart) && !testingAgainstSelf)
						{
							FS_TrackConnection newTrackConnection = FS_TrackConnection();
							newTrackConnection.Track = currentTestTrack;
							newTrackConnection.ConnectionID = k;
							newTrackConnection.Location = E_TrackSplineLocation::Start;

							startConnections.Add(newTrackConnection);
							connectionMade = true;
						}
						// End to end
						if (WithinConnectionDistance(ourEnd, currentEnd) && !testingAgainstSelf)
						{
							FS_TrackConnection newTrackConnection = FS_TrackConnection();
							newTrackConnection.Track = currentTestTrack;
							newTrackConnection.ConnectionID = k;
							newTrackConnection.Location = E_TrackSplineLocation::End;

							endConnections.Add(newTrackConnection);
							connectionMade = true;
						}
						// Start to end
						if (WithinConnectionDistance(ourStart, currentEnd))
						{
							FS_TrackConnection newTrackConnection = FS_TrackConnection();
							newTrackConnection.Track = currentTestTrack;
							newTrackConnection.ConnectionID = k;
							newTrackConnection.Location = E_TrackSplineLocation::End;

							startConnections.Add(newTrackConnection);
							connectionMade = true;
						}

						// Store the details of the track so we can update it after all connections are made
						if (connectionMade && !tracksToUpdate.Contains(currentTestTrack))
							tracksToUpdate.Add(currentTestTrack);
					}
				}
			}

			// Update the track spline with the connection info
			FS_TrackConnectionPoint startConnectionsPoint = FS_TrackConnectionPoint();
			startConnectionsPoint.Connections = startConnections;
			FS_TrackConnectionPoint endConnectionsPoint = FS_TrackConnectionPoint();
			endConnectionsPoint.Connections = endConnections;

			FS_TrackSpline updatedTrackSpline = FS_TrackSpline();
			updatedTrackSpline.Spline = _TrackSplines[i].Spline;
			updatedTrackSpline.StartConnectionPoint = startConnectionsPoint;
			updatedTrackSpline.StartConnectionIndex = _TrackSplines[i].StartConnectionIndex;
			updatedTrackSpline.EndConnectionPoint = endConnectionsPoint;
			updatedTrackSpline.EndConnectionIndex = _TrackSplines[i].EndConnectionIndex;

			_TrackSplines[i] = updatedTrackSpline;
		}

		// Loop through the tracks we made connections into and have them rebuild their connections
		if (!i_DoNotRefreshOtherTracks)
			for (ATrainTrack * updatedTrack : tracksToUpdate)
				updatedTrack->RebuildTrackConnections();
	}
}

void ATrainTrack::SetupArrows()
{
	TArray<UArrowComponent*> Arrows;
	GetComponents<UArrowComponent>(Arrows);

	// Remove any old marker arrows that might be present
	for (int32 i = 0; i < Arrows.Num(); i++)
	{
		if (Arrows[i]->ComponentHasTag(UTrainAndRailFunctionLibrary::GetInternalCompTag()))
		{
			Arrows[i]->DestroyComponent();
		}
	}

	// Loop through each track spline, if there are no connections,
	// or if the flag is set to always show the arrows, add an arrow
	for (int32 i = 0; i < _TrackSplines.Num(); i++)
	{
		// Added since we only want each track to deal with their own arrows
		if (m_DefaultTrackSpline == _TrackSplines[i].Spline)
		{
			FS_TrackSpline currentSpline = _TrackSplines[i];
			// Add start arrow (green color)
			AddConnectionArrow(c_StartArrowRotation, c_StartArrowColor, currentSpline.Spline, 0, c_StartArrowName,
				currentSpline.StartConnectionPoint.Connections.Num() == 0 || m_AlwaysShowConnectionArrows);
			// Add end arrow (orange color)
			int32 endSplinePoint = currentSpline.Spline->GetNumberOfSplinePoints() - 1;
			AddConnectionArrow(c_EndArrowRotation, c_EndArrowColor, currentSpline.Spline, endSplinePoint, c_EndArrowName,
				currentSpline.EndConnectionPoint.Connections.Num() == 0 || m_AlwaysShowConnectionArrows);

			// Add default up arrow
			float splineDistance = currentSpline.Spline->GetSplineLength() / 2.0f;
			FVector upArrowLocation = currentSpline.Spline->GetLocationAtDistanceAlongSpline(splineDistance, ESplineCoordinateSpace::World);
			AddUpDirArrow(upArrowLocation, m_UpDirection);
		}

		// Don't think we need this

		//if (m_ShowUpDir || m_ShowUpDirForAllPoints)
		//{
			// Add single arrow in the middle of the track to indicate up direction if it's being set for all points
			//if (m_SetUpDirForAllSplinePoints && !m_ShowUpDirForAllPoints)
			//{

		//}

		//// Add an up dir for each point on the spline
		//else
		////{
		//	int32 splinePointsToAddArrow = currentSpline.Spline->GetNumberOfSplinePoints() - 1;
		//	for (int32 j = 0; j < splinePointsToAddArrow; j++)
		//	{
		//		FVector locationAtSplinePoint = currentSpline.Spline->GetLocationAtSplinePoint(j, ESplineCoordinateSpace::World);
		//		FVector upVectorAtSplinePoint = currentSpline.Spline->GetUpVectorAtSplinePoint(j, ESplineCoordinateSpace::World);
		//		AddUpDirArrow(locationAtSplinePoint, upVectorAtSplinePoint);
		//	}
		//}
		//}
	}
}

void ATrainTrack::AddConnectionArrow(FRotator i_Rotation, FLinearColor i_Color, USplineComponent * i_Spline, int32 i_SplinePoint, FName i_arrowName, bool i_ShowArrow)
{
	// Check the spline point passed in is valid
	if (i_SplinePoint < i_Spline->GetNumberOfSplinePoints())
	{
		UArrowComponent * connectionArrow = NewObject<UArrowComponent>(this, UArrowComponent::StaticClass(), i_arrowName);

		connectionArrow->CreationMethod = EComponentCreationMethod::Native;
		connectionArrow->SetMobility(EComponentMobility::Movable);
		connectionArrow->AttachToComponent(i_Spline, FAttachmentTransformRules::KeepRelativeTransform);

		connectionArrow->ComponentTags.Add(UTrainAndRailFunctionLibrary::GetInternalCompTag());

		// Build the arrow transform
		FTransform transformAtSplinePoint = i_Spline->GetTransformAtSplinePoint(i_SplinePoint, ESplineCoordinateSpace::World);
		FTransform newWorldTransform = FTransform(UKismetMathLibrary::ComposeRotators(i_Rotation, transformAtSplinePoint.Rotator()),
			transformAtSplinePoint.GetLocation() + m_ArrowOffset);

		connectionArrow->SetWorldTransform(newWorldTransform, false, nullptr, ETeleportType::None);

		connectionArrow->SetArrowColor(i_Color);
		connectionArrow->SetVisibility(i_ShowArrow);

		if (i_arrowName == c_StartArrowName)
			_StartConnectionArrow = connectionArrow;
		else if (i_arrowName == c_EndArrowName)
			_EndConnectionArrow = connectionArrow;
	}
}

USplineComponent * ATrainTrack::GetSplineByIndex(int32 i_SplinePoint) const
{
	return _TrackSplines[i_SplinePoint].Spline;
}

FS_TrackSpline ATrainTrack::GetTrackSplineByIndex(int32 i_SplineIndex, bool & o_ValidTrackSpline) const
{
	o_ValidTrackSpline = i_SplineIndex < _TrackSplines.Num() && i_SplineIndex >= 0;

	if (o_ValidTrackSpline)
	{
		return _TrackSplines[i_SplineIndex];
	}
	else
	{
		return FS_TrackSpline();
	}
}

int32 ATrainTrack::GetSplineIndex(USplineComponent * i_Spline)
{
	// Get the spline index by searching for the spline reference
	for (int32 i = 0; i < _TrackSplines.Num(); i++)
	{
		if (_TrackSplines[i].Spline == i_Spline)
		{
			return i;
		}
	}

	return -1;
}

FS_TrackConnection ATrainTrack::PickConnectionToUse(const FS_TrackConnectionPoint & i_ConnectionPoint, bool i_ConnPointAtEOT, bool & o_ConnectionFound)
{
	bool useRandomConn = i_ConnPointAtEOT ? m_End_UseRandomConnection : m_Start_UseRandomConnection;
	bool useActiveConn = i_ConnPointAtEOT ? m_End_UseActiveConnection : m_Start_UseActiveConnection;
	ATrainTrack * activeConn = i_ConnPointAtEOT ? m_End_ActiveConnection : m_Start_ActiveConnection;

	// there are no connections to use
	if (i_ConnectionPoint.Connections.Num() == 0)
	{
		o_ConnectionFound = false;
		return FS_TrackConnection();
	}

	if (useRandomConn)
	{
		o_ConnectionFound = true;
		return i_ConnectionPoint.Connections[FMath::RandHelper(i_ConnectionPoint.Connections.Num())];
	}
	else
	{
		if (activeConn)
		{
			if (activeConn != nullptr)
			{
				// loop through the connections and find the one that matches
				for (int32 i = 0; i < i_ConnectionPoint.Connections.Num(); i++)
				{
					if (i_ConnectionPoint.Connections[i].Track == activeConn)
					{
						o_ConnectionFound = true;
						return i_ConnectionPoint.Connections[i];
					}
				}
				// if none are found default to first
				o_ConnectionFound = true;
				return i_ConnectionPoint.Connections[0];
			}
			// return first connection
			else
			{
				o_ConnectionFound = true;
				return i_ConnectionPoint.Connections[0];
			}
		}
		// return first connection
		else
		{
			o_ConnectionFound = true;
			return i_ConnectionPoint.Connections[0];
		}
	}
}

bool ATrainTrack::RegisterTrackTrigger(ATrackTrigger * i_NewTrigger)
{
	// Register the trigger if it's not already on the list
	if (!_Triggers.Contains(i_NewTrigger))
	{
		_Triggers.Add(i_NewTrigger);
	}

	return true;
}

void ATrainTrack::ValidateTriggers()
{
	TArray<int32> removableItems;
	// Check for any invalid triggers
	for (int32 i = 0; i < _Triggers.Num(); i++)
	{
		if (!IsValid(_Triggers[i]))
		{
			removableItems.Add(i);
		}
	}

	// Loop through the triggers to remove, clear the trigger regions first, then remove the trigger from the list
	for (int32 i = 0; i < removableItems.Num(); i++)
	{
		ClearTriggerRegions(_Triggers[removableItems[i] - i], false);
		_Triggers.RemoveAt(removableItems[i] - i);
	}

	for (int32 i = 0; i < _TriggerRegions.Num(); i++)
	{
		if (_TriggerRegions[i].Trigger == nullptr)
		{
			_TriggerRegions.RemoveAt(i);
			i--;
		}
	}

	if (removableItems.Num() > 0)
	{
#ifdef UE_BUILD_DEBUG
		FString errorText = "Track: ";											// Track:
		errorText.Append(UKismetSystemLibrary::GetDisplayName(this));           // [Track name]
		errorText.Append(FString(" removed "));                                 // removed
		errorText.Append(FString::FromInt(removableItems.Num()));				// [len of removable items]
		errorText.Append(FString(" stale track triggers "));					// stale track triggers

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
#endif
	}
}

void ATrainTrack::AddTriggerRegion(FS_TrackTriggerRegion & i_NewTriggerRegion)
{
	_TriggerRegions.Add(i_NewTriggerRegion);
}

void ATrainTrack::ClearTriggerRegions(const ATrackTrigger * i_Trigger, bool i_WipeAllRegions)
{
	// Remove regions for a single trigger, or all triggers
	if (i_WipeAllRegions)
	{
		_TriggerRegions.Empty();
	}
	else
	{
		TArray<int32> removeIndexes;
		for (int32 i = 0; i < _TriggerRegions.Num(); i++)
		{
			if (i_Trigger == _TriggerRegions[i].Trigger)
			{
				removeIndexes.Add(i);
			}
		}

		for (int32 i = 0; i < removeIndexes.Num(); i++)
		{
			_TriggerRegions.RemoveAt(removeIndexes[i] - i);
		}
	}
}

void ATrainTrack::CheckForRegionOverlap(UBogie * i_Bogie, AHandcar * i_Handcar)
{
	// Loop through the regions, if the bogie is within one and it's on the right track spline
	// (or the trigger is set to effect them all), then fire the trigger
	for (int32 i = 0; i < _TriggerRegions.Num(); i++)
	{
		if (i_Bogie->GetSplineDistance() >= _TriggerRegions[i].Region.StartDistance &&
			i_Bogie->GetSplineDistance() < _TriggerRegions[i].Region.EndDistance &&
			_TriggerRegions[i].Region.TrackSplineIndex == i_Bogie->GetSplineIndex())
		{
			// for any triggers that get removed at runtime
			if (_TriggerRegions[i].Trigger == nullptr)
			{
				_TriggerRegions.RemoveAt(i);
				i--;
			}
			else
			{
				_TriggerRegions[i].Trigger->FireTrackTrigger(i_Bogie, i_Handcar);
			}
		}
	}
}

void ATrainTrack::BuildTrackMesh()
{
	// If mesh building is disabled, or there's no valid mesh
	if (!(m_DisableMeshBuilding || m_TrackMesh == nullptr))
	{
		// Calculate the mesh spacing and the tangent clamp
		float boundsAxisValueUsed = 0.0f;
		if (m_MeshSpacingAxis == E_TrackMeshSpacingAxis::X)
			boundsAxisValueUsed = m_TrackMesh->GetBounds().BoxExtent.X;
		else if (m_MeshSpacingAxis == E_TrackMeshSpacingAxis::Y)
			boundsAxisValueUsed = m_TrackMesh->GetBounds().BoxExtent.Y;
		else if (m_MeshSpacingAxis == E_TrackMeshSpacingAxis::Z)
			boundsAxisValueUsed = m_TrackMesh->GetBounds().BoxExtent.Z;

		// Multiply By 2 because the bounds is a half size
		float meshSpacing = boundsAxisValueUsed * (m_MeshSpacingMultiplier * 2.0f);
		FVector2D tangentClamp = UKismetMathLibrary::MakeVector2D(-1.0f * meshSpacing, meshSpacing);

		// Loop through each spline on the track
		for (int32 i = 0; i < _TrackSplines.Num(); i++)
		{
			USplineComponent * currentSpline = _TrackSplines[i].Spline;
			// Set the up dir for the spline
			currentSpline->SetDefaultUpVector(m_UpDirection, ESplineCoordinateSpace::Local);

			// Generate the mesh count
			int32 possibleMeshCount = UKismetMathLibrary::FTrunc(currentSpline->GetSplineLength() / meshSpacing) + 1;
			int32 meshCount = UKismetMathLibrary::SelectInt(possibleMeshCount, possibleMeshCount - 1, m_ForceMeshToMatchSplineLength);

			// Loop through the number of meshes to add to the spline
			for (int32 j = 0; j < meshCount; j++)
			{
				float currentDistance = j * meshSpacing;
				bool nextDistanceSelector = meshCount == j && !m_ForceMeshToMatchSplineLength;
				float nextDistance = UKismetMathLibrary::SelectFloat(currentSpline->GetSplineLength(), currentDistance + meshSpacing, nextDistanceSelector);

				// Add the spline mesh component
				FName splineMeshName = MakeUniqueObjectName(this, USplineMeshComponent::StaticClass());
				USplineMeshComponent * splineMesh = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass(), splineMeshName);

				splineMesh->RegisterComponentWithWorld(GetWorld());

				splineMesh->CreationMethod = EComponentCreationMethod::Native;
				splineMesh->SetMobility(EComponentMobility::Movable);
				splineMesh->AttachToComponent(currentSpline, FAttachmentTransformRules::KeepRelativeTransform);
				splineMesh->SetGenerateOverlapEvents(false);

				splineMesh->SetRelativeTransform(currentSpline->GetRelativeTransform());

				// Set the start and end roll for this mesh
				float startRoll = UKismetMathLibrary::SelectFloat(
					UKismetMathLibrary::DegreesToRadians(currentSpline->GetRotationAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::Local).Roll),
					0.0f, m_ApplySplineRollToMesh);
				float endRoll = UKismetMathLibrary::SelectFloat(
					UKismetMathLibrary::DegreesToRadians(currentSpline->GetRotationAtDistanceAlongSpline(nextDistance, ESplineCoordinateSpace::Local).Roll),
					0.0f, m_ApplySplineRollToMesh);
				splineMesh->SetStartRoll(startRoll, false);
				splineMesh->SetEndRoll(endRoll, false);

				// Set the up dir for the spline mesh, use the one we get from the spline, or the supplied one
				FVector splineUpDir = UKismetMathLibrary::SelectVector(m_UpDirection,
					currentSpline->GetUpVectorAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::Local), m_SetUpDirForAllSplinePoints);
				splineMesh->SetSplineUpDir(splineUpDir, false);

				// Turn on collison, set the mesh and the material
				splineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				splineMesh->SetStaticMesh(m_TrackMesh);

				// Store the start and end tangent
				FVector startPosition = currentSpline->GetLocationAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::Local);
				FVector startTangent = currentSpline->GetTangentAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::Local);

				FVector endPosition = currentSpline->GetLocationAtDistanceAlongSpline(nextDistance, ESplineCoordinateSpace::Local);
				FVector endTangent = currentSpline->GetTangentAtDistanceAlongSpline(nextDistance, ESplineCoordinateSpace::Local);

				startTangent = UKismetMathLibrary::ClampVectorSize(
					UKismetMathLibrary::SelectVector(startTangent, startTangent * m_TangentScale, startTangent.Size() < m_MaxTangentLength),
					tangentClamp.X, tangentClamp.Y);

				endTangent = UKismetMathLibrary::ClampVectorSize(
					UKismetMathLibrary::SelectVector(endTangent, endTangent * m_TangentScale, endTangent.Size() < m_MaxTangentLength),
					tangentClamp.X, tangentClamp.Y);

				splineMesh->SetStartAndEnd(startPosition, startTangent, endPosition, endTangent, false);

				// Finally, update the mesh
				splineMesh->UpdateMesh();

				_TrackMeshes.Add(splineMesh);
			}
		}
	}
}

void ATrainTrack::RebuildTrackConnections()
{
	BuildTrackConnections(true);
	UpdateConnectionArrows();
}

void ATrainTrack::AddUpDirArrow(FVector & i_ArrowLocation, FVector & i_UpDirForArrow)
{
	_UpArrow = NewObject<UArrowComponent>(this, UArrowComponent::StaticClass(), FName(c_UpArrowName));

	_UpArrow->CreationMethod = EComponentCreationMethod::Native;
	_UpArrow->SetMobility(EComponentMobility::Movable);
	_UpArrow->AttachToComponent(m_DefaultTrackSpline, FAttachmentTransformRules::KeepRelativeTransform);

	_UpArrow->ComponentTags.Add(UTrainAndRailFunctionLibrary::GetInternalCompTag());

	FTransform newWorldTransform = FTransform(UKismetMathLibrary::Conv_VectorToRotator(i_UpDirForArrow * 90.0f), i_ArrowLocation + m_ArrowOffset);

	_UpArrow->SetWorldTransform(newWorldTransform, false, nullptr, ETeleportType::ResetPhysics);

	// Sets to tealish color
	_UpArrow->SetArrowColor(c_UpArrowColor);
	_UpArrow->SetVisibility(m_ShowUpDir);
}

void ATrainTrack::BuildSplineFromBakingSource()
{
	// Keep our original transform for restoring later
	FTransform restoreTransform = m_Scene->K2_GetComponentToWorld();

	// Match the source track transform
	FTransform bakingTrackTransform = m_BakingSourceTrack->m_Scene->K2_GetComponentToWorld();
	m_Scene->SetWorldTransform(bakingTrackTransform, false, nullptr, ETeleportType::ResetPhysics);

	// Copy spline points from source track
	m_DefaultTrackSpline->ClearSplinePoints(false);
	int32 sourceTrackSplinePointsCount = m_BakingSourceTrack->m_DefaultTrackSpline->GetNumberOfSplinePoints() - 1;
	for (int32 i = 0; i < sourceTrackSplinePointsCount; i++)
	{
		FVector newSplinePointLocation = m_BakingSourceTrack->m_DefaultTrackSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		m_DefaultTrackSpline->AddSplinePoint(newSplinePointLocation, ESplineCoordinateSpace::World, false);

		FVector newSplineTangent = m_BakingSourceTrack->m_DefaultTrackSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		m_DefaultTrackSpline->SetTangentAtSplinePoint(i, newSplineTangent, ESplineCoordinateSpace::World, false);
	}

	// Update the spline and restore the transform if it's needed
	m_DefaultTrackSpline->UpdateSpline();
	if (m_ReturnToOriginalLocation)
		m_Scene->SetWorldTransform(restoreTransform, false, nullptr, ETeleportType::ResetPhysics);
}
#pragma endregion

#pragma region CustomHelperFunctions

void ATrainTrack::AddHandcarReference(AHandcar * i_Handcar)
{
	_AttachedHandcars.Add(i_Handcar);
}

TArray<ATrainTrack*> ATrainTrack::GetTrackConnections(E_TrackSplineLocation i_SplineLocation)
{
	TArray<ATrainTrack*> connectedTracks;
	if (i_SplineLocation == E_TrackSplineLocation::Start)
	{
		TArray<FS_TrackConnection> trackConnections = _TrackSplines[0].StartConnectionPoint.Connections;
		for (int32 i = 0; i < trackConnections.Num(); i++)
		{
			connectedTracks.Add(trackConnections[i].Track);
		}
	}
	else
	{
		TArray<FS_TrackConnection> trackConnections = _TrackSplines[0].EndConnectionPoint.Connections;
		for (int32 i = 0; i < trackConnections.Num(); i++)
		{
			connectedTracks.Add(trackConnections[i].Track);
		}
	}
	return connectedTracks;
}

void ATrainTrack::RemoveHandcarReference(AHandcar * i_Handcar)
{
	for (int32 i = 0; i < _AttachedHandcars.Num(); i++)
	{
		if (_AttachedHandcars[i] == i_Handcar)
		{
			_AttachedHandcars.RemoveAt(i);
		}
	}
}

void ATrainTrack::UpdateTrack()
{
	TArray<FS_TrackConnectionPoint> previousStartConnectionPoints;
	TArray<FS_TrackConnectionPoint> previousEndConnectionPoints;

	for (int32 i = 0; i < _TrackSplines.Num(); i++)
	{
		previousStartConnectionPoints.Add(_TrackSplines[i].StartConnectionPoint);
		previousEndConnectionPoints.Add(_TrackSplines[i].EndConnectionPoint);
	}

	// ReBuild track connections
	BuildTrackConnections(false);

	// Update connection arrows
	UpdateConnectionArrows();

	for (int32 i = 0; i < previousStartConnectionPoints.Num(); i++)
	{
		for (int32 j = 0; j < previousStartConnectionPoints[i].Connections.Num(); j++)
		{
			ATrainTrack * previousConnectedTrack = previousStartConnectionPoints[i].Connections[j].Track;
			if (previousConnectedTrack != nullptr)
			{
				previousConnectedTrack->BuildTrackConnections(false);
				previousConnectedTrack->UpdateConnectionArrows();
			}
		}
	}

	for (int32 i = 0; i < previousEndConnectionPoints.Num(); i++)
	{
		for (int32 j = 0; j < previousEndConnectionPoints[i].Connections.Num(); j++)
		{
			ATrainTrack * previousConnectedTrack = previousEndConnectionPoints[i].Connections[j].Track;
			if (previousConnectedTrack != nullptr)
			{
				previousConnectedTrack->BuildTrackConnections(false);
				previousConnectedTrack->UpdateConnectionArrows();
			}
		}
	}

	if (m_ShowUpDir)
	{
		UpdateUpArrow(m_DefaultTrackSpline, _UpArrow);
	}
	else
	{
		_UpArrow->SetVisibility(m_ShowUpDir);
	}

	// Reinitialize the track mesh
	for (int32 i = 0; i < _TrackMeshes.Num(); i++)
	{
		_TrackMeshes[i]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		_TrackMeshes[i]->UnregisterComponent();
		_TrackMeshes[i]->DestroyComponent();
	}
	_TrackMeshes.Empty(m_DefaultTrackSpline->GetNumberOfSplinePoints());
	int32 numSplinePoints = m_DefaultTrackSpline->GetNumberOfSplinePoints();
	BuildTrackMesh();
}

void ATrainTrack::UpdateConnectionArrows()
{
	for (int32 i = 0; i < _TrackSplines.Num(); i++)
	{
		FS_TrackSpline currentSpline = _TrackSplines[i];
		if (m_DefaultTrackSpline == currentSpline.Spline)
		{
			bool nonConnectedTrack = currentSpline.StartConnectionPoint.Connections.Num() == 0 && currentSpline.EndConnectionPoint.Connections.Num() == 0;
			UpdateConnectionArrow(m_DefaultTrackSpline, _StartConnectionArrow, nonConnectedTrack || m_AlwaysShowConnectionArrows);
			UpdateConnectionArrow(m_DefaultTrackSpline, _EndConnectionArrow, nonConnectedTrack || m_AlwaysShowConnectionArrows);
		}
	}
}

void ATrainTrack::UpdateConnectionArrow(USplineComponent * i_Spline, UArrowComponent * i_ConnectionArrow, bool i_ShowArrow)
{
	FTransform transformAtSplinePoint;
	FTransform newWorldTransform;
	// Update start arrow
	if (c_StartArrowName == i_ConnectionArrow->GetFName())
	{
		// Build the arrow transform
		transformAtSplinePoint = i_Spline->GetTransformAtSplinePoint(0, ESplineCoordinateSpace::World);
		newWorldTransform = FTransform(UKismetMathLibrary::ComposeRotators(c_StartArrowRotation, transformAtSplinePoint.Rotator()),
			transformAtSplinePoint.GetLocation() + m_ArrowOffset);
	}
	// Update end arrow
	else if (c_EndArrowName == i_ConnectionArrow->GetFName())
	{
		// Build the arrow transform
		transformAtSplinePoint = i_Spline->GetTransformAtSplinePoint(i_Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		newWorldTransform = FTransform(UKismetMathLibrary::ComposeRotators(c_EndArrowRotation, transformAtSplinePoint.Rotator()),
			transformAtSplinePoint.GetLocation() + m_ArrowOffset);
	}

	i_ConnectionArrow->SetWorldTransform(newWorldTransform, false, nullptr, ETeleportType::None);
	i_ConnectionArrow->SetVisibility(i_ShowArrow);
}

void ATrainTrack::UpdateConnections()
{
	// only update connections if there is a world to get other tracks from
	if (GetWorld() != nullptr)
	{
		for (int32 i = 0; i < _TrackSplines.Num(); i++)
		{
			for (size_t j = 0; j < _TrackSplines[i].StartConnectionPoint.Connections.Num(); j++)
			{
				ATrainTrack * trackToUpdate = _TrackSplines[i].StartConnectionPoint.Connections[j].Track;
				if (IsValid(trackToUpdate) && !trackToUpdate->GetFName().IsEqual("None"))
				{
					trackToUpdate->BuildTrackConnections(true);
				}
				else
				{
					_TrackSplines[i].StartConnectionPoint.Connections.RemoveAt(j);
					j--;
				}
			}

			for (size_t j = 0; j < _TrackSplines[i].EndConnectionPoint.Connections.Num(); j++)
			{
				ATrainTrack * trackToUpdate = _TrackSplines[i].EndConnectionPoint.Connections[j].Track;
				if (IsValid(trackToUpdate) && !trackToUpdate->GetFName().IsEqual("None"))
				{
					trackToUpdate->BuildTrackConnections(true);
				}
				else
				{
					_TrackSplines[i].EndConnectionPoint.Connections.RemoveAt(j);
					j--;
				}
			}
		}
	}
}

void ATrainTrack::UpdateUpArrow(USplineComponent * i_Spline, UArrowComponent * i_UpArrow)
{
	// Set up arrow position in middle of track
	float splineDistance = i_Spline->GetSplineLength() / 2.0f;
	FVector upArrowLocation = i_Spline->GetLocationAtDistanceAlongSpline(splineDistance, ESplineCoordinateSpace::World);

	FTransform newWorldTransform = FTransform(UKismetMathLibrary::Conv_VectorToRotator(m_UpDirection * 90.0f), upArrowLocation + m_ArrowOffset);

	i_UpArrow->SetWorldTransform(newWorldTransform, false, nullptr, ETeleportType::ResetPhysics);

	i_UpArrow->SetWorldTransform(newWorldTransform, false, nullptr, ETeleportType::None);
	i_UpArrow->SetVisibility(m_ShowUpDir);
}

#pragma endregion

#pragma region Private Methods

void ATrainTrack::PrefabricatorReinit()
{
	_TrackSplines.RemoveAt(0);

	InitalizeTrackSplines();
	BuildTrackMesh();
	BuildTrackConnections(false);
}

void ATrainTrack::UpdateAttachedHandcars()
{
	// update handcars relying on this track
	for (int32 i = 0; i < _AttachedHandcars.Num(); i++)
	{
		if (IsValid(_AttachedHandcars[i]))
		{
			_AttachedHandcars[i]->UpdateTrainHandcar();
		}
		else
		{
			_AttachedHandcars.RemoveAt(i);
			i--;
		}
	}
}

#pragma endregion

#pragma region Macros and Inline
inline bool ATrainTrack::WithinConnectionDistance(FVector & i_PointA, FVector & i_PointB)
{
	float distanceBetween = UTrainAndRailFunctionLibrary::GetDistanceBetweenLocations(i_PointA, i_PointB);
	return UTrainAndRailFunctionLibrary::GetDistanceBetweenLocations(i_PointA, i_PointB) <= m_MaxConnectionDistance;
}
#pragma endregion
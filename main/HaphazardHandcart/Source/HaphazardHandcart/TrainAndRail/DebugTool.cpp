
#include "DebugTool.h"

#include "TrainAndRail/Bogie.h"
#include "TrainAndRail/Handcar.h"
#include "TrainAndRail/TrackTrigger.h"
#include "TrainAndRail/TrainTrack.h"

#include "Engine/Texture2D.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ADebugTool::ADebugTool()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_EditorIconBB = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIconBB"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> EditorIconTexture(TEXT(
		"Texture2D'/Game/TrainAndRail/Materials/EditorIcons/debug_tool_icon.debug_tool_icon'"));
	m_EditorIconBB->Sprite = EditorIconTexture.Object;
	m_EditorIconBB->ScreenSize = 0.0025f;
}

// Called when the game starts or when spawned
void ADebugTool::BeginPlay()
{
	Super::BeginPlay();

	ValidateTracksAndTrains();
}

// Called every frame
void ADebugTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SelCar_DisplaySensorBogie();
	SelCar_PrintAllBogieNames();
	SelCar_ShowDebugHitch();
}

void ADebugTool::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	Conn_RebuildAllConnections();
	Conn_PrintConnectionInfoForAllTracks();
	Conn_PrintConnInfoForSelectedTrack();
	Track_PrintTriggerRegions();
}

void ADebugTool::Conn_RebuildAllConnections()
{
#ifdef UE_BUILD_DEBUG
	// Force all tracks to rebuild their connections
	if (m_RebuildAllConnections)
	{

		FString debugText = "-- Forcing all tracks to rebuild connections --"; // -- Forcing all tracks to rebuild connections --

		UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);

		TArray<AActor*> tracksInWorld;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrainTrack::StaticClass(), tracksInWorld);

		for (int32 i = 0; i < tracksInWorld.Num(); i++)
		{
			ATrainTrack * track = Cast<ATrainTrack>(tracksInWorld[i]);
			track->RebuildTrackConnections();
		}

		debugText = "-- Forced Connection Rebuild Complete --"; // -- Forced Connection Rebuild Complete --

		UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);
#endif
	}
}

void ADebugTool::Conn_PrintConnectionInfoForAllTracks()
{
#ifdef UE_BUILD_DEBUG
	if (m_PrintConnInfoForAllTracks)
	{
		TArray<AActor*> tracksInWorld;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrainTrack::StaticClass(), tracksInWorld);

		for (int32 i = 0; i < tracksInWorld.Num(); i++)
		{
			ATrainTrack * track = Cast<ATrainTrack>(tracksInWorld[i]);
			PrintConnectionInfoForTrack(track);
		}
	}
#endif
}

void ADebugTool::PrintConnectionInfoForTrack(ATrainTrack * i_TrainTrack)
{
#ifdef UE_BUILD_DEBUG
	FString debugText = "--- Track: ";                                     // --- Track: 
	debugText.Append(UKismetSystemLibrary::GetDisplayName(i_TrainTrack));  // [track display name]: 
	debugText.Append(" has ");                                             // has
	debugText.AppendInt(i_TrainTrack->GetTrackSplines().Num());            // [num track splines]
	debugText.Append(" track splines ---");                                //  track splines ---

	UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);

	for (int32 i = 0; i < i_TrainTrack->GetTrackSplines().Num(); i++)
	{
		FS_TrackConnectionPoint startConnPoint = i_TrainTrack->GetTrackSplines()[i].StartConnectionPoint;
		PrintConnectionPointInfo(startConnPoint, "Starting Connection Point");

		FS_TrackConnectionPoint endConnPoint = i_TrainTrack->GetTrackSplines()[i].EndConnectionPoint;
		PrintConnectionPointInfo(endConnPoint, "Ending Connection Point");
	}
#endif
}

void ADebugTool::Conn_PrintConnInfoForSelectedTrack()
{
#ifdef UE_BUILD_DEBUG
	if (m_PrintConnInfoForSelectedTrack)
	{
		for (ATrainTrack * track : m_SelectedTracks)
		{
			PrintConnectionInfoForTrack(track);
		}
	}
#endif
}

void ADebugTool::PrintConnectionPointInfo(FS_TrackConnectionPoint & i_ConnectionPoint, FString i_Name)
{
#ifdef UE_BUILD_DEBUG
	FString debugText = i_Name;                               // [name]
	debugText.Append(" - ");                                  // -
	debugText.AppendInt(i_ConnectionPoint.Connections.Num()); // [connection point num connections]
	debugText.Append(" connections");                         //  connections

	UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);

	for (FS_TrackConnection connection : i_ConnectionPoint.Connections)
	{
		debugText = "    Conn: ";
		debugText.AppendInt(connection.ConnectionID);
		debugText.Append(" Location: ");
		UEnum * pEnum = FindObject<UEnum>(ANY_PACKAGE, *FString("E_TrackSplineLocation"));
		debugText.Append(pEnum->GetNameStringByIndex(static_cast<uint8>(connection.Location)));
		debugText.Append(" Track: ");
		debugText.Append(UKismetSystemLibrary::GetDisplayName(connection.Track));

		UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);
	}
#endif
}

void ADebugTool::SelCar_PrintAllBogieNames()
{
#ifdef UE_BUILD_DEBUG
	if (m_SelCar_PrintBogieNames)
	{
		for (AHandcar * handCar : m_SelectedHandcars)
		{
			if (IsValid(handCar))
			{
				for (UBogie * bogie : handCar->GetBogies())
				{
					UKismetSystemLibrary::DrawDebugString(GetWorld(), bogie->GetLocation(), bogie->GetDebugName(), nullptr, FLinearColor::Blue, 0.004f);
					UKismetSystemLibrary::DrawDebugSphere(GetWorld(), bogie->GetLocation(), 50.0f, 8, FLinearColor::Green, 0.05f, 1.0f);
				}
			}
		}
	}
#endif
}

void ADebugTool::Track_PrintTriggerRegions()
{
#ifdef UE_BUILD_DEBUG
	if (m_PrintTriggerRegions)
	{
		for (ATrainTrack * track : m_SelectedTracks)
		{
			FString debugText = "Track ";                                  // Track 
			debugText.Append(UKismetSystemLibrary::GetDisplayName(track)); // [track display name]
			debugText.Append(" has ");                                     // has
			debugText.AppendInt(track->GetTriggerRegions().Num());         // [track trigger regions]
			debugText.Append(" trigger regions ///");                      //  trigger regions ///

			UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);

			// Loop through each of the regions and add a col box as a rough marker for the region
			for (int32 i = 0; i < track->GetTriggerRegions().Num(); i++)
			{
				debugText = "     TR ";                                                                        //      TR 
				debugText.AppendInt(i);                                                                        // [trigger index]
				debugText.Append(" ");                                                                         // 
				debugText.Append(UKismetSystemLibrary::GetDisplayName(track->GetTriggerRegions()[i].Trigger)); // [track trigger display name]
				debugText.Append(" Start: ");                                                                  // Start: 
				debugText.Append(FString::SanitizeFloat(track->GetTriggerRegions()[i].Region.StartDistance));  // [track trigger start distance]
				debugText.Append(" End: ");                                                                    // End:
				debugText.Append(FString::SanitizeFloat(track->GetTriggerRegions()[i].Region.EndDistance));    // [track trigger end distance]

				bool validTrackSpline = false;
				USplineComponent * spline = track->GetTrackSplineByIndex(track->GetTriggerRegions()[i].Region.TrackSplineIndex, validTrackSpline).Spline;
				debugText.Append(" on spline: ");                                                              // on spline: 
				debugText.Append(UKismetSystemLibrary::GetDisplayName(spline));                                // [spline display name]

				UKismetSystemLibrary::PrintString(GetWorld(), debugText, true, true, FLinearColor::Blue, 2.0f);

				float startDist = track->GetTriggerRegions()[i].Region.StartDistance;
				float endDist = track->GetTriggerRegions()[i].Region.EndDistance;

				FVector startVector = spline->GetLocationAtDistanceAlongSpline(startDist, ESplineCoordinateSpace::World);
				FVector endVector = spline->GetLocationAtDistanceAlongSpline(endDist, ESplineCoordinateSpace::World);

				UBoxComponent * boxCollision = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass());
				boxCollision->SetBoxExtent(FVector((startDist + endDist / 2.0f), 0.0f, 0.0f), true);
				boxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				boxCollision->SetHiddenInGame(false, false);

				FVector vector = (startVector + endVector) / 2.0f;
				FRotator rotator = UKismetMathLibrary::FindLookAtRotation(startVector, endVector);
				FTransform boxTransform = FTransform(rotator, vector);
				boxCollision->SetWorldTransform(boxTransform, false, nullptr, ETeleportType::ResetPhysics);
			}
		}
	}
#endif
}

void ADebugTool::Track_DeleteAllTriggerRegions()
{
	if (m_DeleteAllTriggerRegions)
	{
		TArray<AActor*> tracksInWorld;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrainTrack::StaticClass(), tracksInWorld);

		for (AActor * actor : tracksInWorld)
		{
			ATrainTrack * track = Cast<ATrainTrack>(actor);
			track->ClearTriggerRegions(nullptr, true);
		}
	}
}

void ADebugTool::SelCar_ShowDebugHitch()
{
	if (m_SelCar_ShowDebugHitches)
	{
		for (int32 i = 0; i < m_SelectedHandcars.Num(); i++)
		{
			USphereComponent * frontHitch = m_SelectedHandcars[i]->m_FrontHitch;
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), frontHitch->GetComponentLocation(), frontHitch->GetScaledSphereRadius(), 12, FLinearColor::Green, 0.02f, 2.0f);
		
			USphereComponent * rearHitch = m_SelectedHandcars[i]->m_RearHitch;
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), rearHitch->GetComponentLocation(), rearHitch->GetScaledSphereRadius(), 12, FLinearColor::Red, 0.02f, 2.0f);
		}
	}
}

void ADebugTool::SelCar_DisplaySensorBogie()
{
#ifdef UE_BUILD_DEBUG
	if (m_SelCar_ShowSensorBogie)
	{
		for (AHandcar * handcar : m_SelectedHandcars)
		{
			if (!handcar->m_DisableSensorBogie)
			{
				UKismetSystemLibrary::DrawDebugString(GetWorld(), handcar->m_SensorBogie->GetLocation(), handcar->m_SensorBogie->GetDebugName(),
					nullptr, FLinearColor::Green, m_DebugTextTimeOnScreen);

				UKismetSystemLibrary::DrawDebugSphere(GetWorld(), handcar->m_SensorBogie->GetLocation(), 50.0f, 8, FLinearColor::Black, m_DebugShapeTimeOnScreen, 1.0f);

				FVector centerLocation = handcar->m_SensorBogie->GetLocation() + handcar->m_SensorBogie->GetRotation().RotateVector(handcar->m_SensorOffset);
				UKismetSystemLibrary::DrawDebugBox(GetWorld(), centerLocation, handcar->m_SensorBoxSize, FLinearColor::Blue,
					handcar->m_SensorBogie->GetRotation(), m_DebugShapeTimeOnScreen, 0.0f);
			}
		}
	}
#endif
}

void ADebugTool::ValidateTracksAndTrains()
{
	for (int32 i = 0; i < m_SelectedTracks.Num(); i++)
	{
		if (m_SelectedTracks[i] == nullptr)
		{
			m_SelectedTracks.RemoveAt(i);
		}
	}

	for (int32 i = 0; i < m_SelectedHandcars.Num(); i++)
	{
		if (m_SelectedHandcars[i] == nullptr)
		{
			m_SelectedHandcars.RemoveAt(i);
		}
	}
}


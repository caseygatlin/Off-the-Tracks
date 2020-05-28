#include "TrackTrigger.h"

#include "Bogie.h"
#include "Structs/S_TrackRegion.h"
#include "Structs/S_TrackTriggerRegion.h"
#include "TrainAndRailFunctionLibrary.h"
#include "TrainTrack.h"

#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EngineGlobals.h"
#include "Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATrackTrigger::ATrackTrigger()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    m_EditorIconBB = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIconBB"));
	RootComponent = m_EditorIconBB;
    m_RadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RadiusSphere"));
    m_RadiusSphere->SetupAttachment(m_EditorIconBB);
    m_RadiusSphere->SetSphereRadius(90.f);

    static ConstructorHelpers::FObjectFinder<UTexture2D> EditorIconTexture(TEXT(
        "Texture2D'/Game/TrainAndRail/Materials/EditorIcons/trigger_icon.trigger_icon'"));
    m_EditorIconBB->Sprite = EditorIconTexture.Object;
    m_EditorIconBB->ScreenSize = 0.0025f;

    InitializeTrackTrigger();
}

// Called when the game starts or when spawned
void ATrackTrigger::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ATrackTrigger::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TickEnableTimer(DeltaTime);
}

void ATrackTrigger::CustomTriggerEvent(UBogie * i_Bogie, AHandcar * i_Handcar)
{
}

#if WITH_EDITOR

void ATrackTrigger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	InitializeTrackTrigger();
}

void ATrackTrigger::PreEditUndo()
{
	Super::PreEditUndo();
}

void ATrackTrigger::PostEditUndo()
{
	Super::PostEditUndo();
}

void ATrackTrigger::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	InitializeTrackTrigger();
}

#endif // WITH_EDITOR

void ATrackTrigger::FireTrackTrigger(UBogie * i_Bogie, AHandcar * i_Handcar)
{
	if (TriggerFireCheck(i_Bogie, i_Handcar))
	{
		CustomTriggerEvent(i_Bogie, i_Handcar);
	}
}

void ATrackTrigger::SubscribeToFireEvent(UBogie* i_Bogie, AHandcar * i_Handcar)
{
	FName fireTrackTriggerName = FName(TEXT("FireTrackTrigger"));
	_FireTrackTriggerEvent.AddUFunction(this, fireTrackTriggerName, i_Bogie, i_Handcar);
}

void ATrackTrigger::InitializeTrackTrigger()
{
    AttachToTrackAndBuildRegions();
}

void ATrackTrigger::AttachToTrackAndBuildRegions()
{
    // Clear out this trigger from any track so we don't get duplicates.
    TArray<AActor*> outActors;

    UGameplayStatics::GetAllActorsOfClass(this, m_Track->StaticClass(), outActors);

    TArray<ATrainTrack*> tracks;
    
    for (int i = 0; i < outActors.Num(); i++)
    {
        tracks.Add(static_cast<ATrainTrack*>(outActors[i]));
        tracks[i]->ClearTriggerRegions(this, false);
    }

    // Check we have a track and that the track spline index is valid.
    if (m_Track)
    {
        bool isSplineIndexValid = UTrainAndRailFunctionLibrary::IsValidTrackSplineIndex(m_Track, m_TrackSplineIndex);
        bool areAllSplineIndexesValid = isSplineIndexValid || m_ApplyToAllTrackSplines;
        bool areThereSplines = m_Track->GetTrackSplines().Num() > 0;

        if (areAllSplineIndexesValid && areThereSplines)
        {
            bool couldRegisterTrigger = m_Track->RegisterTrackTrigger(this);

            if (couldRegisterTrigger)
            {
                // Generate the spline array to use
                TArray<FS_TrackSpline>  trackSplines;
                bool allTracksLoaded = false;

                if (m_ApplyToAllTrackSplines)
                {
                    trackSplines = m_Track->GetTrackSplines();
                    allTracksLoaded = true;
                }
                else
                {
                    // Make sure m_TrackSplineIndex is non-negative for GetTrackSplineByIndex
                    if (m_TrackSplineIndex < 0)
                    {
                        m_TrackSplineIndex = 0;
                    }

                    bool isValidTrackSpline = false;
                    trackSplines.Add(m_Track->GetTrackSplineByIndex(m_TrackSplineIndex, isValidTrackSpline));
                    allTracksLoaded = isValidTrackSpline;
                }

                if (allTracksLoaded && trackSplines.Num() > 0)
                {
                    // Snap to the closest point on the spline
                    USplineComponent* startSpline = trackSplines[0].Spline;
                    FVector editorIconPos = m_EditorIconBB->GetComponentLocation();

                    // Make sure m_DistanceSampleCount is non-negative for GetClosestDistanceOnSpline
                    if (m_DistanceSampleCount < 0)
                    {
                        m_DistanceSampleCount = 0;
                    }

                    bool isValidDistance = false;
                    _AutoSet_DistanceOnSpline = UTrainAndRailFunctionLibrary::GetClosestDistanceOnSpline(startSpline, editorIconPos, m_DistanceSampleCount, isValidDistance);

                    if (isValidDistance)
                    {
                        // Snap the trigger onto the spline
                        FVector snapLocation = startSpline->GetLocationAtDistanceAlongSpline(_AutoSet_DistanceOnSpline, ESplineCoordinateSpace::Type::World);

                        SetActorLocation(snapLocation, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);

                        m_RadiusSphere->SetSphereRadius(m_TriggerRadius);

                        for (int i = 0; i < trackSplines.Num(); i++)
                        {
                            isValidDistance = false;
                            float startDistanceOnSpline = UTrainAndRailFunctionLibrary::GetClosestDistanceOnSpline(trackSplines[i].Spline, editorIconPos,
								m_DistanceSampleCount, isValidDistance);

                            for (int j = 0; j < 2; j++)
                            {
                                // First we go forward on the spline
                                float searchDist = m_TriggerRadius;

                                // Then we go back on the spline
                                if (j == 1)
                                {
                                    searchDist *= -1;
                                }

                                TArray<FS_TrackPassThroughInfo> currentPassThroughInfo;
                                ATrainTrack* endTrack;
                                FS_TrackSpline endTrackSpline;
                                float endDistance;
                                bool endReached;
                                bool invertedDirOfTravel;
                                TArray<FS_TrackPassThroughInfo> travelInfo;

                                travelInfo = UTrainAndRailFunctionLibrary::FindLocationOnTracks(
                                    m_Track,
                                    trackSplines[i], 
                                    startDistanceOnSpline, 
                                    searchDist, 
                                    false, 
                                    currentPassThroughInfo,
                                    endTrack, 
                                    endTrackSpline, 
                                    endDistance,
                                    endReached, 
                                    invertedDirOfTravel
                                );


                                // Loop through the travel info to see what tracks and splines this
                                // trigger needs to cover, for each one create a trigger region
                                // for this trigger, if the distance is 0, skip creating a null
                                // region for it.
                                for (int k = 0; k < travelInfo.Num(); k++)
                                {
                                    float ti_startDistance = travelInfo[k].StartDistance;
                                    float ti_endDistance = travelInfo[k].EndDistance;
                                    ATrainTrack* ti_track = travelInfo[k].Track;
                                    USplineComponent* ti_trackSpline = travelInfo[k].TrackSpline.Spline;

                                    if (ti_endDistance - ti_startDistance != 0.f)
                                    {
                                        int ti_splineIndex = ti_track->GetSplineIndex(ti_trackSpline);

                                        FS_TrackRegion ti_trackRegion;
                                        ti_trackRegion.StartDistance = ti_startDistance;
                                        ti_trackRegion.EndDistance = ti_endDistance;
                                        ti_trackRegion.TrackSplineIndex = ti_splineIndex;

                                        FS_TrackTriggerRegion ti_triggerRegion;
                                        ti_triggerRegion.Region = ti_trackRegion;
                                        ti_triggerRegion.Trigger = this;

                                        ti_track->AddTriggerRegion(ti_triggerRegion);

                                        // Keep track of the tracks we have regions on
                                        if (!_TracksWithRegions.Contains(ti_track))
                                        {
                                            _TracksWithRegions.Add(ti_track);
                                        }
                                    }
                                }
                            }
                        }
                    }

#ifdef UE_BUILD_DEBUG
                    else
                    {
                        // If we failed to find a valid location on the spline, print the info and disable the trigger
                        FString errorText = "Failed to get a valid distance when creating trigger: ";   // Invalid distance
                        errorText.Append(UKismetSystemLibrary::GetDisplayName(this));                   // [Trigger name]
                        errorText.Append(FString(" on track: "));                                       // on track
                        errorText.Append(UKismetSystemLibrary::GetDisplayName(m_Track));                // [Track name]

                        DisableTrigger();
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
                    }
#endif
                }

#ifdef UE_BUILD_DEBUG
                else
                {
                    // If we failed to get the track spline, disable this trigger and print a warning
                    FString errorText = "Invalid spline index to use for track trigger: ";  // Invalid spline index
                    errorText.Append(UKismetSystemLibrary::GetDisplayName(this));           // [Trigger name]
                    errorText.Append(FString(" on track: "));                               // on track
                    errorText.Append(UKismetSystemLibrary::GetDisplayName(m_Track));        // [Track name]

                    DisableTrigger();
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
                }
#endif
            }

#ifdef UE_BUILD_DEBUG
            else
            {
                FString errorText = "Failed to attach trigger: ";                 // Failed to attach trigger:
                errorText.Append(UKismetSystemLibrary::GetDisplayName(this));     // Trigger name
                errorText.Append(FString(" to track: "));                         // To Track
                errorText.Append(UKismetSystemLibrary::GetDisplayName(m_Track));  // Track name

                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
            }
#endif
        }

#ifdef UE_BUILD_DEBUG
        else
        {
            FString errorText = UKismetSystemLibrary::GetDisplayName(this);                 // [Trigger name]
            errorText.Append(FString(" - Trigger can't attach to track, spline index: "));  // can't attach to track...
            errorText.AppendInt(m_TrackSplineIndex);                                        // [spline index]
            errorText.Append(FString(" is invalid, track only has "));                      // is invalid, track only has
            errorText.AppendInt(m_Track->GetTrackSplines().Num());                          // [number of splines]
            errorText.Append(FString(" track splines"));                                    // track splines

            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorText);
        }
#endif
    }
}

void ATrackTrigger::DisableTrigger()
{
    m_Active = false;
    ResetEnableTimer();
}

void ATrackTrigger::TickEnableTimer(const float& i_dt)
{
    // If this option is enabled and the trigger isn't active
    if (m_UseEnableTimer && !m_Active)
    {
        _TimeRemainingForReset -= i_dt;

        // If enough time has passed, enable the trigger
        if (_TimeRemainingForReset <= 0)
        {
            EnableAndResetTrigger();
        }
    }
}

void ATrackTrigger::EnableAndResetTrigger()
{
    m_Active = true;

	_TriggeredHandcars.Empty();

    //Implement if we need to do something on reset
    //PostTriggerResetEvent();
}

// Checks to see if this trigger can fire
bool ATrackTrigger::TriggerFireCheck(const UBogie* i_Bogie, AHandcar * i_Handcar)
{
    bool returnCanFire = false;

    if (m_Active)
    {
        bool isSensorOrHelperBogie = i_Bogie->IsSensorOrHelperBogie();

        if (!m_IgnoreHelperBogies || !isSensorOrHelperBogie)
        {
            returnCanFire = true;

            if (m_TriggerOnce)
            {
                DisableTrigger();
            }
        }
    }

    if (m_TriggerOncePerCarriage && returnCanFire)
    {
        if (_TriggeredHandcars.Contains(i_Handcar))
        {
            returnCanFire = false;
        }

        else
        {
			_TriggeredHandcars.Add(i_Handcar);
            returnCanFire = true;
        }
    }

    return returnCanFire;
}

void ATrackTrigger::ResetEnableTimer()
{
    if (m_UseEnableTimer)
    {
        _TimeRemainingForReset = m_ResetDelay;
    }
}
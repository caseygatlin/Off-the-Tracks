
#include "Handcar.h"
#include "TrainAndRail/Bogie.h"
#include "TrainAndRail/TrainAndRailFunctionLibrary.h"
#include "Gamecode/DebugFunctionLibrary.h"
#include "TrainAndRail/TrackTrigger.h"
#include "TrainAndRail/TrainTrack.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "EngineGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"

// converts variable name to const char *
#define NAME_OF(v) #v

// Sets default values
AHandcar::AHandcar()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initalize components
	m_FrontBogie = CreateDefaultSubobject<UBogie>(TEXT("FrontBogie"));
	m_FrontBogie->SetDebugName(FString("FrontBogie"));

	m_RearBogie = CreateDefaultSubobject<UBogie>(TEXT("RearBogie"));
	m_RearBogie->SetDebugName(FString("RearBogie"));

	m_SensorBogie = CreateDefaultSubobject<UBogie>(TEXT("SensorBogie"));
	m_SensorBogie->SetDebugName(FString("SensorBogie"));

	// Ititalize default meshes and materiales

	// Bogie model, material and anim class
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MetalMaterial(TEXT(
		"Material'/Game/HandCarContent/Materials/Old/Metal.Metal'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Metal2Material(TEXT(
		"Material'/Game/HandCarContent/Materials/Old/Metal2.Metal2'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> WoodMaterial(TEXT(
		"Material'/Game/HandCarContent/Materials/Old/Wood.Wood'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DarkGrayMaterial(TEXT(
		"MaterialInstanceConstant'/Game/HandCarContent/Materials/Instances/MI_Basic_DarkGray.MI_Basic_DarkGray'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HandcarModel(TEXT(
		"SkeletalMesh'/Game/HandCarContent/Geometry/Handcar/Handcar_Rigged.Handcar_Rigged'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HandcarModelBody(TEXT(
		"StaticMesh'/Game/HandCarContent/Geometry/Handcar/S_Handcar_Body.S_Handcar_Body'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HandleModel(TEXT(
		"StaticMesh'/Game/HandCarContent/Geometry/Handcar/S_Handcar_Handle.S_Handcar_Handle'"));
	//static ConstructorHelpers::FObjectFinder<UAnimInstance> BogieAnimClass(TEXT(
	//	"AnimBlueprint'/Game/TrainAndRail/Models/TrainCar/ANIMBP_TrainCarBogie.ANIMBP_TrainCarBogie'"));

	// Init StaticMeshComponent (needs collision set default for jump triggers)
	m_HandcarRootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultHandcarRoot"));
	RootComponent = m_HandcarRootComponent;
	m_HandcarRootComponent->SetMobility(EComponentMobility::Type::Movable);
	m_HandcarRootComponent->SetStaticMesh(HandcarModelBody.Object);
	m_HandcarRootComponent->SetVisibility(false);
	m_HandcarRootComponent->SetGenerateOverlapEvents(false);

	// Init FrontBogieMesh
	m_HandcarMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandcarMesh"));
	m_HandcarMesh->AttachToComponent(m_HandcarRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	m_HandcarMesh->SetMobility(EComponentMobility::Type::Movable);
	m_HandcarMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	m_HandcarMesh->SetSkeletalMesh(HandcarModel.Object);
	m_HandcarMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Init Handle
	m_Handle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Handle"));
	m_Handle->AttachToComponent(m_HandcarMesh, FAttachmentTransformRules::KeepRelativeTransform);
	m_Handle->SetMobility(EComponentMobility::Type::Movable);
	m_Handle->SetStaticMesh(HandleModel.Object);
	m_Handle->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	m_Handle->AddRelativeLocation(m_HandleOffset, false, nullptr, ETeleportType::None);

	// Init Hitches
	m_FrontHitch = CreateDefaultSubobject<USphereComponent>(TEXT("FrontHitch"));
	m_FrontHitch->AttachToComponent(m_HandcarMesh, FAttachmentTransformRules::KeepRelativeTransform);
	m_FrontHitch->SetMobility(EComponentMobility::Type::Movable);
	FVector frontHitchLocation = FVector(190.0f, 0.0f, 43.0f);
	m_FrontHitch->SetRelativeLocation(frontHitchLocation, false, nullptr, ETeleportType::None);
	m_FrontHitch->SetSphereRadius(32.0f);
	m_FrontHitch->ShapeColor = FColor::Green;
	// TODO: check if we need area class set for navigation or set in blueprint
	//m_FrontHitch->AreaClass = ;
	m_FrontHitch->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	m_RearHitch = CreateDefaultSubobject<USphereComponent>(TEXT("RearHitch"));
	m_RearHitch->AttachToComponent(m_HandcarMesh, FAttachmentTransformRules::KeepRelativeTransform);
	m_RearHitch->SetMobility(EComponentMobility::Type::Movable);
	FVector rearHitchLocation = FVector(-190.0f, 0.0f, 43.0f);
	m_RearHitch->SetRelativeLocation(rearHitchLocation, false, nullptr, ETeleportType::None);
	m_RearHitch->SetSphereRadius(32.0f);
	m_RearHitch->ShapeColor = FColor::Red;
	// TODO: check if we need area class set for navigation or set in blueprint
	//m_FrontHitch->AreaClass = ;
	m_RearHitch->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	// Init bogie meshes
	m_FrontBogieArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontBogieArrow"));
	m_FrontBogieArrow->AttachToComponent(m_HandcarMesh, FAttachmentTransformRules::KeepRelativeTransform);
	m_FrontBogieArrow->SetMobility(EComponentMobility::Type::Movable);
	FVector frontArrowTransform = FVector(0.0f, 0.0f, 220.0f);
	m_FrontBogieArrow->SetRelativeLocation(frontArrowTransform, false, nullptr, ETeleportType::None);
	m_FrontBogieArrow->SetArrowColor(FLinearColor::Green);

	// Add ourselves to timed ignore list
	AddActorToCollisionIgnoreList(this, 0.0f, true);

	// initalize things needed for train Handcar
	InitalizeTrainHandcar();

	// After everything is set up, call the refresh function to use the initialized settings.
	if (m_SnapToTrack && m_AttachedTrack != nullptr)
	{
		RefreshPositionOnTrack();
	}
	// If there's a child, update its position relative to this Handcar
	// This will also pass down any settings changes from this to the child
	else if (m_ChildHandcar != nullptr)
	{
		m_ChildHandcar->RefreshPositionOnTrack();
	}
}

// Called when the game starts or when spawned
void AHandcar::BeginPlay()
{
	Super::BeginPlay();

	m_PathTaken = FString();

	if (m_AttachedTrack != nullptr)
	{
		AttachToTrack();
		PutSensorBogieOutFront();

		ValidateBogies();
		if (_Bogies.Num() <= 3)
		{
			SetupCollision();
			SetupBogies();
		}

		AddTrackTriggersToIgnoreArray();
	}
}

// Called every frame
void AHandcar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	// If there's no parent, update normally
	if (m_ParentHandcar == nullptr && !m_HandcarRootComponent->IsSimulatingPhysics())
	{
		// Limit the delta time, if that option is enabled
		float pickedDeltaTime = UKismetMathLibrary::SelectFloat(m_ConstantDeltaTime, DeltaTime, m_UseConstantDeltaTime);
		bool isUpdateAborted = false;
		TickHandcarMovement(pickedDeltaTime, isUpdateAborted);
	}
}

// Called to bind functionality to input
void AHandcar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHandcar::PostLoad()
{
	Super::PostLoad();

	if (m_AttachedTrack != nullptr)
	{
		m_AttachedTrack->AddHandcarReference(this);
	}
}

#pragma region Editor Events

#if WITH_EDITOR

void AHandcar::PreEditChange(UProperty * PropertyThatWillChange)
{
	Super::PreEditChange(PropertyThatWillChange);

	if (PropertyThatWillChange != nullptr && PropertyThatWillChange->GetFName().IsEqual(FName(NAME_OF(m_AttachedTrack))))
	{
		if (m_AttachedTrack != nullptr)
		{
			m_AttachedTrack->RemoveHandcarReference(this);
		}
	}
}

void AHandcar::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != nullptr && PropertyChangedEvent.Property->GetFName().IsEqual(FName(NAME_OF(m_AttachedTrack))))
	{
		if (m_AttachedTrack != nullptr)
		{
			m_AttachedTrack->AddHandcarReference(this);
		}
	}
	if (m_AttachedTrack != nullptr)
	{
		UpdateTrainHandcar();
	}
}

void AHandcar::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	UpdateTrainHandcar();
}

#endif // WITH_EDITOR

#pragma endregion

#pragma region Helper Methods

void AHandcar::InitalizeTrainHandcar()
{
	UpdateSettingsFromParent();

	if (m_SnapToTrack && m_AttachedTrack != nullptr)
	{
		AttachToTrack();
		if (_Bogies.Num() <= 3)
		{
			SetupCollision();
			SetupBogies();
		}
		UpdateHandcarMeshLocations();

		// Set the internal acceleration based on direction of travel, forward = positive, backwards = negative
		if (E_BogieDirectionOfTravel::Forward == m_StartingDirectionOfTravel)
		{
			_InternalAcceleration = m_Acceleration;
		}
		else
		{
			_InternalAcceleration = -1.0f * m_Acceleration;
		}

		// Set our relation to the child Handcar if we have one
		if (m_ChildHandcar != nullptr)
		{
			m_ChildHandcar->SetParentHandcar(this, false);
			m_ChildHandcar->SetHandcarDirection(m_StartingDirectionOfTravel);
		}

		// Set our relation to the parent Handcar if we have one
		if (m_ParentHandcar != nullptr)
		{
			m_ParentHandcar->SetChildHandcar(this);
		}

		// If we are a Handcar and we don't have a parent, set the disconnected flag
		if (E_HandcarType::Handcar == m_HandcarType && m_ParentHandcar == nullptr)
		{
			_DisconnectedFromParent = true;
		}
	}
}

void AHandcar::AttachToTrack()
{
	// Check everything is valid before proceeding
	if (m_AttachedTrack != nullptr)
	{
		if (!UTrainAndRailFunctionLibrary::IsValidTrackSplineIndex(m_AttachedTrack, m_AttachedTrackSplineIndex))
		{
			m_AttachedTrackSplineIndex = 0;
#ifdef UE_BUILD_DEBUG
			FString errorText = "Attach track spline index is invalid: ";            // Attach track spline index is invalid:
			errorText.AppendInt(m_AttachedTrackSplineIndex);                         // [m_AttachedTrackSplineIndex]
			errorText.Append(FString(" when attaching Handcar: "));                 // when attaching Handcar: 
			errorText.Append(UKismetSystemLibrary::GetDisplayName(this));            // [this Handcar display name]
			errorText.Append(FString(" to track: "));                                // to track:
			errorText.Append(UKismetSystemLibrary::GetDisplayName(m_AttachedTrack)); // [attached track display name]

			PrintDebugString(errorText);
#endif

			if (!UTrainAndRailFunctionLibrary::HasTrackSplines(m_AttachedTrack))
			{
#ifdef UE_BUILD_DEBUG
				// Can't attach to track, track has no usable track splines. Track: 
				errorText = "Can't attach to track, track has no usable track splines. Track:  ";
				errorText.Append(UKismetSystemLibrary::GetDisplayName(m_AttachedTrack));  // [attached track display name]

				PrintDebugString(errorText);
#endif
				return;
			}
		}

		// Spline index = attached track spline index

		// Get the closest distance on the spline, if it can't be found or is invalid, default to 0
		if (!m_ManuallySetDistanceOnTrack)
		{
			USplineComponent * trackSpline = m_AttachedTrack->GetSplineByIndex(m_AttachedTrackSplineIndex);
			bool validDistance = false;
			FVector actorLocation = GetActorLocation();
			float distanceForTrack = UTrainAndRailFunctionLibrary::GetClosestDistanceOnSpline(trackSpline, actorLocation, m_AttachDistanceSampleCount,
				validDistance);

			if (validDistance)
			{
				m_StartingDistanceOnTrack = distanceForTrack;
			}
			else
			{
				m_StartingDistanceOnTrack = 0.0f;
			}
		}
	}
}

void AHandcar::SetupBogies()
{
	// Disable sensor if this is a Handcar
	if (m_HandcarType == E_HandcarType::Handcar)
	{
		m_DisableSensorBogie = true;
	}
	// Disable or setup the sensor bogie
	if (m_DisableSensorBogie)
	{
		m_SensorBogie->DestroyComponent();
	}
	else
	{
		m_SensorBogie->SetDistanceFromHandcar(GetSensorBogieDistance());
		m_SensorBogie->SetIgnoreEndOfTrack(false);
		m_SensorBogie->SetIsSensorOrHelperBogie(true);
	}

	// Setup the bogie array
	_Bogies.Empty();
	GetComponents<UBogie>(_Bogies);

	if (m_AttachedTrack != nullptr)
	{
		// Set the track for all bogies
		for (int32 i = 0; i < _Bogies.Num(); i++)
		{
			_Bogies[i]->ChangeTrack(m_AttachedTrack, m_AttachedTrackSplineIndex, m_StartingDistanceOnTrack);
		}
	}

	// Set the front and rear bogie distance from the Handcar
	m_FrontBogie->SetDistanceFromHandcar(0.0f);
	if (!m_DisableRearBogie)
	{
		m_RearBogie->SetDistanceFromHandcar(m_BogieDistance);
	}

	// Update the bogie positions on the track. Once that's done store the transforms since they are used by the mesh setup and position functions
	if (m_AttachedTrack != nullptr)
	{
		UpdateBogiesAndTrackers();
	}

	// Add the bogies to the trigger bogie array
	if (!_TriggerBogies.Contains(m_FrontBogie))
	{
		_TriggerBogies.Add(m_FrontBogie);
	}
	if (!m_DisableRearBogie)
	{
		_TriggerBogies.Add(m_RearBogie);
	}
	if (!m_DisableSensorBogie)
	{
		_TriggerBogies.Add(m_SensorBogie);
	}
}

void AHandcar::SetupCollision()
{
	for (int32 i = 0; i < _CollisionBogies.Num(); i++)
	{
		if (_CollisionBogies[i] == nullptr)
		{
			_CollisionBogies.RemoveAt(i);
			i--;
		}
		else
		{
			_CollisionBogies[i]->DestroyComponent();
		}
	}
	_CollisionBogies.Empty();
	AddCollisionBogie(FString("FrontColBogie"), m_FrontColBogieOffset);
	AddCollisionBogie(FString("RearColBogie"), m_RearColBogieOffset);
}

// Returns whether we sucessfully updated
bool AHandcar::TickHandcarMovement(float i_DeltaSeconds, bool & o_UpdateAborted)
{
	// If there is no track
	if (m_AttachedTrack == nullptr)
	{
		o_UpdateAborted = false;
		return false;
	}

	// If derailed no movement update to do
	if (m_HandcarState == E_HandcarState::Derailed)
	{
		o_UpdateAborted = false;
		return true;
	}

	// If the Handcar has an obstacle, handle that before anything else
	if (_HasActiveObstacle)
	{
		HandleActiveObstacle(i_DeltaSeconds);
	}

	// If the Handcar is stopped and it was a graveful stop, check if this is an end of track stop
	if (m_HandcarState == E_HandcarState::Stopped && _SensorTriggeredEOT)
	{
		_GracefulStopForEOT = true;
		HandleEndOfTrack(false, nullptr);
	}

	// If we aren't moving return
	if (m_HandcarState != E_HandcarState::Moving)
	{
		o_UpdateAborted = false;
		return true;
	}

	_HandcarUpdateID++;
	// Only can have 500 Handcars updateIds before we reset
	if (_HandcarUpdateID > 500)
	{
		_HandcarUpdateID = 0;
	}

	// Update smooth positions
	UpdateBogiePreUpdatePositions();
	// Calculate the speed for this Handcar
	CalculateSpeed(i_DeltaSeconds, false);

	// If this Handcar has a child and we are moving backwards we need to tick it first
	if (m_ChildHandcar != nullptr && !(_InternalAcceleration >= 0.0f))
	{
		bool childHandcarUpdateAborted = false;
		if (m_ChildHandcar->TickHandcarMovement(i_DeltaSeconds, childHandcarUpdateAborted))
		{
			if (childHandcarUpdateAborted)
			{
				o_UpdateAborted = true;
				return false;
			}
		}
		else
		{
			o_UpdateAborted = false;
			return false;
		}
	}

	// Tick each of the bogies with the current speed
	for (int32 i = 0; i < _Bogies.Num(); i++)
	{
		bool hitEndOfTrack = TickBogieMovement(_Bogies[i], _CurrentSpeed, false);
		// If we hit the end of the track, terminate the update
		if (hitEndOfTrack)
		{
			RevertDistances();
			o_UpdateAborted = true;
			return false;
		}
	}
	// Smooth out the distance on the spline if the option is enabled
	if (m_EnableMovementSmoothing)
	{
		for (int32 i = 0; i < _Bogies.Num(); i++)
		{
			FVector previousLocation = _PreUpdateBogiePositions[i];
			SmoothOutBogieDistance(_Bogies[i], _CurrentSpeed, previousLocation);
		}
	}

	// Tick any children we have, if moving forward
	if (m_ChildHandcar != nullptr && _InternalAcceleration >= 0.0f)
	{
		bool childUpdateAborted = false;
		if (m_ChildHandcar->TickHandcarMovement(i_DeltaSeconds, childUpdateAborted))
		{
			if (childUpdateAborted)
			{
				RevertDistances();
				o_UpdateAborted = true;
				return false;
			}
		}
		else
		{
			o_UpdateAborted = false;
			return false;
		}
	}

	// Tick collision, if anything happening during the collision update, exit the update early
	TickCollision(i_DeltaSeconds);
	if (m_HandcarState != E_HandcarState::Moving)
	{
		LeanHandcar();
		o_UpdateAborted = false;
		return true;
	}
	else
	{
		// Store the front bogie transform for easy access, then update the meshes
		_FrontBogieTransformLastTick = _FrontBogieTransform;
		_FrontBogieTransform = m_FrontBogie->GetTransform();

		if (!m_FrontBogie->GetTrack()->ActorHasTag("LandingTrack"))
		{
			m_DistanceTraveled += FVector::Dist(_FrontBogieTransformLastTick.GetLocation(), _FrontBogieTransform.GetLocation());
		}

		// Update the mesh positions and animations
		UpdateHandcarMeshLocations();

		// Check the distance to the parent
		CheckDistanceToParent(i_DeltaSeconds);

		// Check for any trigger region overlaps
		for (int32 i = 0; i < _TriggerBogies.Num(); i++)
		{
			if (_TriggerBogies[i] == nullptr)
			{
				_TriggerBogies.RemoveAt(i);
				i--;
			}
			else
			{
				_TriggerBogies[i]->GetTrack()->CheckForRegionOverlap(_TriggerBogies[i], this);
			}
		}

		// Apply the lean effect and perform the roll check
		LeanHandcar();
		PerformRollCheck(i_DeltaSeconds);

		// Fire the post move update event
		// NOTE: currently empty

		o_UpdateAborted = false;
		return true;
	}
}

// Returns whether we are at the end of the line
bool AHandcar::TickBogieMovement(UBogie * i_Bogie, float i_Speed, bool i_IgnoreEndOfTrack)
{
	// Setup some handy variables for later
	bool atSplineEnd = false;
	float distanceToEnd = i_Bogie->GetDistanceToEndOfTrack(i_Speed, atSplineEnd);

	// If we are about to hit the end of the track
	if (distanceToEnd <= UKismetMathLibrary::Abs(i_Speed))
	{
		// Get the connection point for the spline this bogie is on
		bool connectionPointAtEOT = false;
		FS_TrackConnectionPoint connectionPoint = i_Bogie->GetConnectionPoint(atSplineEnd, connectionPointAtEOT);

		// only add track distance when front bogie reaches the end of a landing track
		if (i_Bogie == m_FrontBogie && i_Bogie->GetTrack()->ActorHasTag("LandingTrack"))
		{
			m_DistanceTraveled += m_FrontBogie->GetTrack()->m_DefaultTrackSpline->GetSplineLength();
		}

		// Does this connection point have anything for us to connect into?
		if (connectionPoint.Connections.Num() == 0)
		{
			// If the spline is a closed, loop, keep on looping
			if (i_Bogie->GetSplineComponent()->IsClosedLoop())
			{
				i_Bogie->UpdateDistanceForEndOfClosedLoop(i_Speed);
				return false;
			}
			else
			{
				// End of track, if this isn't a closed spline loop and there's nothing for us to connect into
				if (i_IgnoreEndOfTrack)
				{
#ifdef UE_BUILD_DEBUG
					FString debugText = "Ignoring EOT";

					PrintDebugString(debugText);
#endif
					return true;
				}
				else
				{
					if (!i_Bogie->GetIgnoreEndOfTrack())
					{
						HandleEndOfTrack(i_Bogie->IsSensorOrHelperBogie(), nullptr);
					}

					// Keep ticking the helper bogies at the end of the track.
					// Their distance on track will continue to go up.
					// This way when they move away, the bogies won't be all bunched up and need alignment.
					if (i_Bogie->IsSensorOrHelperBogie())
					{
						i_Bogie->TickMovement(i_Speed, _HandcarUpdateID);
						return false;
					}

					return true;
				}
			}
		}
		else
		{
			// If there are multiple connections, pick the one to use, otherwise, go with the only one that's there
			FS_TrackConnection trackConnection;
			if (connectionPoint.Connections.Num() > 1)
			{
				bool connectionFound = false;
				trackConnection = i_Bogie->GetTrack()->PickConnectionToUse(connectionPoint, connectionPointAtEOT, connectionFound);
			}
			else
			{
				trackConnection = connectionPoint.Connections[0];
			}

			// Change tracks and set the inverted direction of travel flag
			BogieChangeTracks(trackConnection, i_Bogie, i_Speed, distanceToEnd);
			bool isInverted = IsDirectionOfTravelInverted(trackConnection.Location, i_Speed);
			i_Bogie->SetInvertDirOfTravel(isInverted);
			return false;
		}
	}
	else
	{
		i_Bogie->TickMovement(i_Speed, _HandcarUpdateID);
		return false;
	}
}

void AHandcar::UpdateHandcarMeshLocations()
{
	// Update the position and rotation, but retain the original scale
	FTransform newTransform = FTransform(_FrontBogieTransform);
	newTransform.SetScale3D(GetActorScale3D());
	SetActorTransform(newTransform, false, nullptr, ETeleportType::ResetPhysics);

	// Generate the final front bogie location with offsets and parent scale
	FVector frontBogieOffset = _FrontBogieTransform.Rotator().RotateVector(m_HandcarOffset + _CurrentHandcarLeanOffset);
	FTransform adjustedFrontBogieTrans = FTransform(AddLocationOffsetToTransform(_FrontBogieTransform,
		frontBogieOffset));

	// If jump rotate trigger has inverted moving direction
	if (m_InvertMesh)
	{
		//FRotator targetRotation = FRotator(-adjustedFrontBogieTrans.Rotator().Pitch,
		//									adjustedFrontBogieTrans.Rotator().Yaw,
		//									adjustedFrontBogieTrans.Rotator().Roll)
		//						  + FRotator(0, 180, 0);
		//
		//adjustedFrontBogieTrans = FTransform(targetRotation,
		//									 adjustedFrontBogieTrans.GetLocation(),
		//									 adjustedFrontBogieTrans.GetScale3D());
		adjustedFrontBogieTrans.ConcatenateRotation(FQuat(FRotator(0, 180, 0)));
	}

	adjustedFrontBogieTrans.SetScale3D(GetActorScale3D());

	m_HandcarMesh->SetWorldTransform(adjustedFrontBogieTrans, false, nullptr, ETeleportType::ResetPhysics);
}

void AHandcar::BogieChangeTracks(const FS_TrackConnection& i_TrackConnection, UBogie* i_Bogie, float i_Speed, float i_DistanceToEnd)
{
	// Calculate the distance on the track based on the connection point location
	float distanceOnNewTrack = 0.0f;
	if (i_TrackConnection.Location == E_TrackSplineLocation::Start)
	{
		distanceOnNewTrack = UKismetMathLibrary::Abs(i_Speed) - i_DistanceToEnd;
	}
	// END
	else
	{
		FS_TrackSpline endTrackSpline = i_TrackConnection.Track->GetTrackSplines()[i_TrackConnection.ConnectionID];
		distanceOnNewTrack = endTrackSpline.Spline->GetSplineLength() - (UKismetMathLibrary::Abs(i_Speed) - i_DistanceToEnd);
	}

	// Fire the exit track event
	// NOTE: event not currently implemented that just prints string
	// i_Bogie->GetTrack().HandcarExitedTrack();

	// Change tracks
	i_Bogie->ChangeTrack(i_TrackConnection.Track, i_TrackConnection.ConnectionID, distanceOnNewTrack);

	// Fire the enter track event
	// NOTE: event not currently implemented that just prints string
	// i_Bogie->GetTrack().HandcarEnteredTrack();
}

void AHandcar::HandleEndOfTrack(bool i_HitByHelperBogie, const AHandcar * i_InstantiatingHandcar)
{
	if (m_ParentHandcar != nullptr)
	{
		// If we have a parent, it will decide what to do
		m_ParentHandcar->HandleEndOfTrack(i_HitByHelperBogie, i_InstantiatingHandcar);
	}
	else
	{
		// Did a sensor or helper bogie hit the end of the track?
		if (i_HitByHelperBogie)
		{
			// If it was a sensor or helper bogie, handle as needed and set the flag that a sensor triggered it
			_SensorTriggeredEOT = true;
			if (m_EndOfTrackOption == E_EndOfTrackOption::BRAKE_AND_STOP_IF_POSSIBLE || m_EndOfTrackOption == E_EndOfTrackOption::BRAKE_AND_ALWAYS_STOP
				|| m_EndOfTrackOption == E_EndOfTrackOption::BRAKE_AND_REVERSE_DIRECTION)
			{
				ApplyOrReleaseBrakes(true);
				_GracefulStopForEOT = false;
			}
		}
		else
		{
			_SensorTriggeredEOT = false;
			switch (m_EndOfTrackOption)
			{
			case E_EndOfTrackOption::BRAKE_AND_STOP_IF_POSSIBLE:
			{
				if (!_GracefulStopForEOT)
				{
					FVector randomImpactLocation = FVector::ZeroVector;
					FVector randomImpactForce = FVector::ZeroVector;
					CreateRandomDerailValues(false, randomImpactLocation, randomImpactForce);
					DerailHandcar(randomImpactLocation, randomImpactForce);
				}
				break;
			}
			case E_EndOfTrackOption::BRAKE_AND_ALWAYS_STOP:
			{
				StopHandcar();
				break;
			}
			case E_EndOfTrackOption::BRAKE_AND_REVERSE_DIRECTION:
			{
				if (_GracefulStopForEOT)
				{
					ToggleHandcarDirection();
					ApplyOrReleaseBrakes(false);
					SetHandcarState(E_HandcarState::Moving, true);
				}
				else
				{
					FVector randomImpactLocation = FVector::ZeroVector;
					FVector randomImpactForce = FVector::ZeroVector;
					CreateRandomDerailValues(false, randomImpactLocation, randomImpactForce);
					DerailHandcar(randomImpactLocation, randomImpactForce);
				}
				break;
			}
			case E_EndOfTrackOption::ALWAYS_STOP:
			{
				StopHandcar();
				break;
			}
			case E_EndOfTrackOption::DERAIL:
			{
				FVector randomImpactLocation = FVector::ZeroVector;
				FVector randomImpactForce = FVector::ZeroVector;
				CreateRandomDerailValues(false, randomImpactLocation, randomImpactForce);
				DerailHandcar(randomImpactLocation, randomImpactForce);
				break;
			}
			case E_EndOfTrackOption::INSTANTLY_REVERSE_DIRECTION:
			{
				StopHandcar();
				ToggleHandcarDirection();
				ApplyOrReleaseBrakes(false);
				SetHandcarState(E_HandcarState::Moving, true);
				break;
			}
			case E_EndOfTrackOption::WARP_TO_ORIGINAL_TRACK_AND_LOCATION:
			{
				WarpHandcarToStartingPoint(false);
				break;
			}
			default:
				break;
			}
		}
	}
}

void AHandcar::RefreshPositionOnTrack()
{
	// Grab settings from out parent, if we have one
	if (m_ParentHandcar != nullptr)
	{
		UpdateSettingsFromParent();
	}

	// If we have a parent, correctly position around it
	UpdateStartingDistanceRelativeToParent();

	// Update the bogies and store the new transforms
	UpdateBogiesAndTrackers();

	// Update the Handcar mesh
	UpdateHandcarMeshLocations();

	if (m_ChildHandcar != nullptr)
	{
		m_ChildHandcar->RefreshPositionOnTrack();
	}
}

void AHandcar::ToggleHandcarDirection()
{
	// Clear the velocity and invert the acceleration
	_Velocity = 0.0f;
	_InternalAcceleration *= -1.0f;

	PutSensorBogieOutFront();

	if (m_ChildHandcar != nullptr)
	{
		m_ChildHandcar->ToggleHandcarDirection();
	}
}

void AHandcar::StopHandcar()
{
	SetHandcarState(E_HandcarState::Stopped, false);

	// Clear the speed and velocity then update the mesh and animations so they are in the correct place and stop moving
	_Velocity = 0.0f;
	_CurrentSpeed = 0.0f;
	UpdateHandcarMeshLocations();
	if (m_ChildHandcar != nullptr)
	{
		m_ChildHandcar->StopHandcar();
	}
}

void AHandcar::ApplyOrReleaseBrakes(bool i_IsBraking)
{
	_IsBraking = i_IsBraking;

	if (m_ChildHandcar != nullptr)
	{
		m_ChildHandcar->ApplyOrReleaseBrakes(i_IsBraking);
	}
}

void AHandcar::SetHandcarState(E_HandcarState i_NewHandcarState, bool i_PassToChildren)
{
	m_HandcarState = i_NewHandcarState;

	if (i_PassToChildren && m_ChildHandcar != nullptr)
	{
		m_ChildHandcar->SetHandcarState(i_NewHandcarState, i_PassToChildren);
	}
}

void AHandcar::WarpHandcarToStartingPoint(bool i_WarpChildren)
{
	// Set the bogies to the original track and starting distance
	UpdateBogiesAndTrackers();

	RefreshPositionOnTrack();

	if (m_ChildHandcar != nullptr && i_WarpChildren)
	{
		m_ChildHandcar->WarpHandcarToStartingPoint(true);
	}
}

void AHandcar::ResetHandcar(bool i_ResetChildren)
{
	_Velocity = 0.0f;
	_CurrentSpeed = 0.0f;

	// Reset EOT / Brakes
	_SensorTriggeredEOT = false;
	_IsBraking = false;

	// Reset the acceleration
	if (E_BogieDirectionOfTravel::Forward == m_StartingDirectionOfTravel)
	{
		_InternalAcceleration = m_Acceleration;
	}
	else
	{
		_InternalAcceleration = m_Acceleration * -1.0f;
	}

	// Reset from a derail
	m_HandcarMesh->SetSimulatePhysics(false);

	// Reset our child Handcar
	if (m_ChildHandcar != nullptr && i_ResetChildren)
	{
		m_ChildHandcar->ResetHandcar(true);
	}
}

void AHandcar::UpdateBogieDistances(float i_StartingTrackDistance, ATrainTrack * i_Track, int32 i_TrackSplineIndex, bool i_SkipAlignedBogies, TArray<UBogie*> i_Bogies, bool i_Invert)
{
	// Loop through all the bogies, using the starting point get the correct location on the track where the bogie should be and set it
	for (int32 i = 0; i < i_Bogies.Num(); i++)
	{
		if (i_Bogies[i] != nullptr)
		{
			if (!(i_SkipAlignedBogies && !i_Bogies[i]->GetNeedsAlignment()))
			{
				bool validTrackSpline = false;
				FS_TrackSpline startingTrackSpline = i_Track->GetTrackSplineByIndex(i_TrackSplineIndex, validTrackSpline);
				TArray<FS_TrackPassThroughInfo> currentPassThroughInfo;

				// init return variables
				ATrainTrack * endTrack = nullptr;
				FS_TrackSpline endTrackSpline = FS_TrackSpline();
				float endDistance = 0.0f;
				bool endReached = false;
				bool invertedDirOfTravel = false;
				TArray<FS_TrackPassThroughInfo> travelInfo;
				UTrainAndRailFunctionLibrary::FindLocationOnTracks(i_Track, startingTrackSpline, i_StartingTrackDistance, i_Bogies[i]->GetDistanceFromHandcar(), i_Invert, currentPassThroughInfo,
					endTrack, endTrackSpline, endDistance, endReached, invertedDirOfTravel);

				int32 endTrackSplineIndex = endTrack->GetSplineIndex(endTrackSpline.Spline);
				i_Bogies[i]->ChangeTrack(endTrack, endTrackSplineIndex, endDistance);
			}
		}
	}
}

// Validate this one seems possibly broken due to Handcar id
void AHandcar::RevertDistances()
{
	for (int32 i = 0; i < _Bogies.Num(); i++)
	{
		_Bogies[i]->RestorePreviousDistance(_HandcarUpdateID);
	}
}

void AHandcar::TickCollision(float i_DeltaSeconds)
{
	// local variables used in this method
	float updatedTimeout = 0.0f;
	TArray<int32> removeIndexes;

	// Tick the timed ignore list, decrease the timer for any items, and remove them if they've expired
	for (int32 i = 0; i < _TimedIgnoreList.Num(); i++)
	{
		if (!_TimedIgnoreList[i].NeverExpire)
		{
			updatedTimeout = _TimedIgnoreList[i].Timeout - i_DeltaSeconds;
			if (updatedTimeout <= 0.0f)
			{
				removeIndexes.Add(i);
			}
			else
			{
				_TimedIgnoreList[i].Timeout = updatedTimeout;
			}
		}
	}

	for (int32 i = 0; i < removeIndexes.Num(); i++)
	{
		_TimedIgnoreList.RemoveAt(removeIndexes[i] - i);
	}

	if (!m_DisableSensorBogie)
	{
		// variables for box trace
		FVector startAndEnd = m_SensorBogie->GetTransform().GetLocation() + m_SensorBogie->GetTransform().Rotator().RotateVector(m_SensorOffset);
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(m_ParentHandcar);
		actorsToIgnore.Add(m_ChildHandcar);
		actorsToIgnore.Add(m_SensorBogie->GetTrack());

		// out variables
		TArray<FHitResult> outHits;

		EDrawDebugTrace::Type drawBoxTrace;
		if (m_DrawBoxTrace)
		{
			drawBoxTrace = EDrawDebugTrace::ForOneFrame;
		}
		else
		{
			drawBoxTrace = EDrawDebugTrace::None;
		}

		// Box trace at the sensor bogie location on the tracks
		bool boxTraceSuccessful = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), startAndEnd, startAndEnd, m_SensorBoxSize, m_SensorBogie->GetTransform().Rotator(),
			m_CollisionQueryTypes, false, actorsToIgnore, drawBoxTrace, outHits, true);

		// Loop through the results, check the items isn't on the ignore list and that it's big enough for us to care about
		if (boxTraceSuccessful)
		{
			for (int32 i = 0; i < outHits.Num(); i++)
			{
				FVector componentOrigin = FVector::ZeroVector;
				FVector componentBoxExtent = FVector::ZeroVector;
				float componentSphereRadius = 0.0f;
				UKismetSystemLibrary::GetComponentBounds(outHits[i].GetComponent(), componentOrigin, componentBoxExtent, componentSphereRadius);

				bool importantCollision = componentSphereRadius >= m_MinObstacleSize && !IsActorOnIgnoreList(outHits[i].GetActor());
				if (importantCollision)
				{
					FString debugText = "Sensor hit: ";														 // Sensor hit: 
					debugText.Append(UKismetSystemLibrary::GetDisplayName(outHits[i].GetActor()));           // [outHits[i] actor hit]
					UDebugFunctionLibrary::DebugPrintScreen(debugText);

					switch (m_ObstacleResponse)
					{
					case E_TrackObstacleResponse::BRAKE_STOP_IF_POSSIBLE_AND_WAIT_FOR_IT_TO_MOVE:
					case E_TrackObstacleResponse::BRAKE_STOP_IF_POSSIBLE_AND_REVERSE_DIRECTION:
					{
						ApplyOrReleaseBrakes(true);
						_HasActiveObstacle = true;
						_TimeToNextPule = m_TimeBetweenObstaclePulses;
						break;
					}
					case E_TrackObstacleResponse::INSTANTLY_STOP_AND_WAIT_FOR_IT_TO_MOVE:
					{
						StopHandcar();
						_HasActiveObstacle = true;
						_TimeToNextPule = m_TimeBetweenObstaclePulses;
						break;
					}
					case E_TrackObstacleResponse::INSTANTLY_STOP_AND_REVERSE_DIRECTION:
					{
						ToggleHandcarDirection();
						break;
					}
					case E_TrackObstacleResponse::DERAIL_NOW:
					{
						FVector hitLocation = outHits[i].Location;
						FVector derailForce = CalculateDerailForce(outHits[i].Normal);
						DerailHandcar(hitLocation, derailForce);
						break;
					}
					default:
						break;
					}
				}
			}
		}
	}

	if (m_ImpactCollisionEnabled)
	{
		for (int32 i = 0; i < _CollisionBogies.Num(); i++)
		{
			// variables for box trace
			FVector startAndEnd = _CollisionBogies[i]->GetTransform().GetLocation() + _CollisionBogies[i]->GetTransform().Rotator().RotateVector(m_SensorOffset);
			TArray<AActor*> actorsToIgnore;
			actorsToIgnore.Add(_CollisionBogies[i]->GetTrack());
			actorsToIgnore.Add(m_ParentHandcar);
			actorsToIgnore.Add(m_ChildHandcar);

			// out variables
			TArray<FHitResult> outHits;

			EDrawDebugTrace::Type drawBoxTrace;
			if (m_DrawBoxTrace)
			{
				drawBoxTrace = EDrawDebugTrace::ForOneFrame;
			}
			else
			{
				drawBoxTrace = EDrawDebugTrace::None;
			}

			// Run a box trace at the bogie location
			bool boxTraceSuccessful = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), startAndEnd, startAndEnd, m_SensorBoxSize, _CollisionBogies[i]->GetTransform().Rotator(),
				m_CollisionQueryTypes, false, actorsToIgnore, drawBoxTrace, outHits, true);

			if (boxTraceSuccessful)
			{
				for (int32 j = 0; j < outHits.Num(); j++)
				{
					if (!IsActorOnIgnoreList(outHits[j].GetActor()))
					{
						if (outHits[j].GetComponent()->IsSimulatingPhysics())
						{
							// Calculate the impact force
							float impactForce = outHits[j].GetActor()->GetVelocity().Size() * outHits[j].GetComponent()->GetMass();

							// Try and knock the object away
							FVector impulseToKnockObject = m_HandcarMesh->GetForwardVector() * m_PlowImpactStrength * _CurrentSpeed;
							outHits[j].GetComponent()->AddImpulseAtLocation(impulseToKnockObject, outHits[j].Location);

							// Is it big enough to slow us down?
							if (impactForce >= m_PlowImpactThreshold)
							{
								_CurrentSpeed -= impactForce / m_PlowImpactThreshold;
							}
						}
						else
						{
							// Did we hit it hard enough to stop or derail us

							// Are we going fast enough to derail? (at least more than half speed)
							if (_CurrentSpeed >= m_MaxSpeed / 2.0f)
							{
								FVector hitLocation = outHits[j].Location;
								FVector derailForce = CalculateDerailForce(outHits[j].Normal);
								DerailHandcar(hitLocation, derailForce);

#ifdef UE_BUILD_DEBUG
								FString debugText = "Derailed";

								PrintDebugString(debugText);
#endif
							}
							else
							{
								StopHandcar();
#ifdef UE_BUILD_DEBUG
								FString debugText = "Stopped";

								PrintDebugString(debugText);
#endif
							}
						}
					}
				}
			}
		}
	}
}

// Returns whether the sensor hit something
bool AHandcar::SensorPulse(const UBogie * i_StartBogie)
{
	// Save the sensor bogie info and then switch the sensor to the front bogie location and track
	ATrainTrack * restoreTrack = m_SensorBogie->GetTrack();
	int32 restoreTrackSplineIndex = m_SensorBogie->GetSplineIndex();
	float restoreDistance = m_SensorBogie->GetSplineDistance();

	m_SensorBogie->ChangeTrack(i_StartBogie->GetTrack(), i_StartBogie->GetSplineIndex(), i_StartBogie->GetSplineDistance());

	// Setup some variables for later
	float pulseSpeed = m_MaxSpeed / 2.0f;
	float targetDistance = UKismetMathLibrary::Abs(m_SensorBogie->GetDistanceFromHandcar());

	// Keep moving the bogie out until we reach the target distance or hit something
	float distanceTraveled = 0.0f;
	bool hitSomething = false;
	while (distanceTraveled < targetDistance && !hitSomething)
	{
		// Move the bogie forward and update the distance
		TickBogieMovement(m_SensorBogie, pulseSpeed, true);
		distanceTraveled += pulseSpeed;

		// variables for box trace
		FVector startAndEnd = m_SensorBogie->GetTransform().GetLocation() + m_SensorOffset;
		TArray<AActor*> actorsToIgnore;
		actorsToIgnore.Add(m_ParentHandcar);
		actorsToIgnore.Add(m_ChildHandcar);

		// out variables
		TArray<FHitResult> outHits;

		// Box trace for obstacles
		bool boxTraceSuccessful = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), startAndEnd, startAndEnd, m_SensorBoxSize, m_SensorBogie->GetTransform().Rotator(),
			m_CollisionQueryTypes, false, actorsToIgnore, EDrawDebugTrace::None, outHits, true);

		if (boxTraceSuccessful)
		{
			for (int32 i = 0; i < outHits.Num(); i++)
			{
				// Check the hit to see if it's big enough and that it's not on the ignore list
				FVector origin = FVector::ZeroVector;
				FVector boxExtent = FVector::ZeroVector;
				float sphereRadius = 0.0f;
				UKismetSystemLibrary::GetComponentBounds(outHits[i].GetComponent(), origin, boxExtent, sphereRadius);

				hitSomething = !IsActorOnIgnoreList(outHits[i].GetActor()) && sphereRadius >= m_MinObstacleSize;
				if (hitSomething)
				{
#ifdef UE_BUILD_DEBUG
					FString debugText = "Train blocked by: ";                                      // Train blocked by:
					debugText.Append(UKismetSystemLibrary::GetDisplayName(outHits[i].GetActor())); // [outHits[i] Hit Actor display name]

					PrintDebugString(debugText);
#endif
					break;
				}
			}
		}
	}

	m_SensorBogie->ChangeTrack(restoreTrack, restoreTrackSplineIndex, restoreDistance);

	return hitSomething;
}

void AHandcar::PutSensorBogieOutFront()
{
	if (!m_DisableSensorBogie)
	{
		// Set the new distance for the sensor bogie
		m_SensorBogie->SetDistanceFromHandcar(GetSensorBogieDistance());

		// Update the sensor bogie location
		TArray<UBogie*> senseorBogies;
		senseorBogies.Add(m_SensorBogie);
		UpdateBogieDistances(m_FrontBogie->GetSplineDistance(), m_FrontBogie->GetTrack(), m_FrontBogie->GetSplineIndex(), false, senseorBogies, false);
	}
}

void AHandcar::HandleActiveObstacle(float i_DeltaSeconds)
{
	if (m_HandcarState == E_HandcarState::Moving || m_HandcarState == E_HandcarState::Stopped)
	{
		if (m_ObstacleResponse == E_TrackObstacleResponse::BRAKE_STOP_IF_POSSIBLE_AND_REVERSE_DIRECTION && m_HandcarState == E_HandcarState::Stopped)
		{
			ApplyOrReleaseBrakes(false);
			ToggleHandcarDirection();
			_HasActiveObstacle = false;
		}

		// Has enough time passed since last pulse?
		_TimeToNextPule -= i_DeltaSeconds;
		if (_TimeToNextPule <= 0.0f)
		{
			// Reset timer
			_TimeToNextPule = m_TimeBetweenObstaclePulses;

			// Firse the pulse and update the active obstacle indicator
			_HasActiveObstacle = SensorPulse(GetFrontBogieBasedOnDirOfTravel());

			if (!_HasActiveObstacle)
			{
				ApplyOrReleaseBrakes(false);
				SetHandcarState(E_HandcarState::Moving, true);
			}
		}
	}
}

bool AHandcar::IsActorOnIgnoreList(const AActor * i_ActorToCheck) const
{
	// Check if the passed in actor is on the ignore list
	for (int32 i = 0; i < _TimedIgnoreList.Num(); i++)
	{
		if (_TimedIgnoreList[i].ActorToIgnore == i_ActorToCheck)
		{
			return true;
		}
	}
	return false;
}

void AHandcar::AddActorToCollisionIgnoreList(AActor * i_ActorToIgnore, float i_IgnoreTime, bool i_NeverExpire)
{
	// Add an actor to the ignore list if it's not already on there
	if (i_IgnoreTime < 0.0f && !i_NeverExpire)
	{
#ifdef UE_BUILD_DEBUG
		FString errorText = "Actor: ";                                            // Actor:
		errorText.Append(UKismetSystemLibrary::GetDisplayName(i_ActorToIgnore));  // [attached track display name]
																				  // not added to ignore list, timeout must be greater than 0 if never expire is false
		errorText.Append(FString(" not added to ignore list, timeout must be greater than 0 if never expire is false"));
		PrintDebugString(errorText);
#endif
	}
	else
	{
		if (!IsActorOnIgnoreList(i_ActorToIgnore))
		{
			FS_TrackTimedIgnore newIgnoreItem = FS_TrackTimedIgnore();
			newIgnoreItem.ActorToIgnore = i_ActorToIgnore;
			newIgnoreItem.Timeout = i_IgnoreTime;
			newIgnoreItem.NeverExpire = i_NeverExpire;

			_TimedIgnoreList.Add(newIgnoreItem);
		}
	}
}

void AHandcar::LeanHandcar()
{
	if (m_UseLeanEffect)
	{
		// Use yaw delta to see if we have turned enough to lean, if so generate the lean amount
		float signedDeltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(_FrontBogieTransformLastTick.Rotator(), _FrontBogieTransform.Rotator()).Yaw;
		float deltaYaw = UKismetMathLibrary::Abs(signedDeltaYaw);
		_TargetLeanAmount = 0.f;
		float leanLerpSignMultiplier = 1.f;

		if (signedDeltaYaw > 0)
		{
			leanLerpSignMultiplier = -1.f;
		}


		// If going around a sharp turn
		if ((signedDeltaYaw <= -0.001f || signedDeltaYaw >= 0.001f) && _CurrentLeanState == E_CartLeanState::CENTER)
		{
			m_IsLeaningWithTurn = false;

			// Is the yaw delta large enough?
			// And make sure we are above the min speed
			bool isPastSpeedThresholdPos = _CurrentSpeed >= m_MaxSpeed * m_LeanMinSpeedPercentage;
			bool isPastSpeedThresholdNeg = _CurrentSpeed <= m_MaxSpeed * m_LeanMinSpeedPercentage * -1;
			bool isPastSpeedThreshold = isPastSpeedThresholdPos || isPastSpeedThresholdNeg;
			bool isPastLeanThreshold = deltaYaw > m_LeanMaxYawDelta;

			if (isPastLeanThreshold && isPastSpeedThreshold)
			{
				if (isPastSpeedThresholdNeg)
				{
					leanLerpSignMultiplier *= -1;
				}

				// To keep the movement smooth, we ignore the lean if it jumps too much
				float leanDelta = UKismetMathLibrary::Abs(_TargetLeanAmount - (m_LeanAmount * deltaYaw));
				if (!(leanDelta > m_LeanIgnoreDelta))
				{
					_TargetLeanAmount = m_LeanAmount * deltaYaw * leanLerpSignMultiplier;
				}
			}
			
			else
			{
				_TargetLeanAmount = 0.0f;
			}
		}
		else if (_CurrentLeanState != E_CartLeanState::CENTER)
		{
			m_IsLeaningWithTurn = false;

			if (m_InvertMesh)
			{
				if (signedDeltaYaw < 0 && _CurrentLeanState == E_CartLeanState::LEFT)
				{

					m_IsLeaningWithTurn = true;
				}
				else if (signedDeltaYaw > 0 && _CurrentLeanState == E_CartLeanState::RIGHT)
				{
					m_IsLeaningWithTurn = true;
				}

			}
			else
			{
				if (signedDeltaYaw > 0 && _CurrentLeanState == E_CartLeanState::LEFT)
				{
					m_IsLeaningWithTurn = true;
				}
				else if (signedDeltaYaw < 0 && _CurrentLeanState == E_CartLeanState::RIGHT)
				{
					m_IsLeaningWithTurn = true;
				}
			}

			
			

			// lean amount negative by default
			if (_CurrentLeanState == E_CartLeanState::LEFT)
			{
				_TargetLeanAmount = m_LeanAmount;
			}
			else if (_CurrentLeanState == E_CartLeanState::RIGHT)
			{
				_TargetLeanAmount = -m_LeanAmount;
			}
		}

		// If not going around a sharp turn
		else
		{
			m_IsLeaningWithTurn = false;
			// lean amount negative by default
			if (_CurrentLeanState == E_CartLeanState::LEFT)
			{
				_TargetLeanAmount = m_LeanAmount;
			}
			else if (_CurrentLeanState == E_CartLeanState::RIGHT)
			{
				_TargetLeanAmount = -m_LeanAmount;
			}
		}


		// If returning to rest (0 target lean) we use the multiplier on the lerp
		float lerpAlphaRotation = _TargetLeanAmount == 0 ? m_LeanLerpAmount * m_LeanReturnToRestMultiplier : m_LeanLerpAmount;

		// Lerp between our current lean and the target lean and apply it to the Handcar
		_CurrentLeanAmount = UKismetMathLibrary::Lerp(_CurrentLeanAmount, _TargetLeanAmount, lerpAlphaRotation);
		FRotator handcarMeshRotation = FRotator(0.0f, 0.0f, _CurrentLeanAmount);

		// Make sure we aren't simulating any physics if we are applying lean affect
		m_HandcarMesh->AddLocalRotation(handcarMeshRotation, false, nullptr, ETeleportType::None);

		// Lerp between our current 
		FVector handcarOffsetTargetAmount = _TargetLeanAmount == 0 ? FVector::ZeroVector : m_HandcarLeanOffset;
		_CurrentHandcarLeanOffset = UKismetMathLibrary::VLerp(_CurrentHandcarLeanOffset, handcarOffsetTargetAmount, lerpAlphaRotation);
	}
}

void AHandcar::PerformRollCheck(float i_DeltaSeconds)
{
	if (m_UseRollCheck)
	{
		// If enough time has passed
		_TimeToNextRollCheck -= i_DeltaSeconds;
		if (_TimeToNextRollCheck <= 0)
		{
			// Check the Handcar has broken a lean threshold
			FRotator deltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(_LastRollingSample, _FrontBogieTransform.Rotator());

			// ThresholdCheck
			bool rollThresholdReached = UKismetMathLibrary::Abs(deltaRotator.Roll) >= m_RollDeltaThresholdsRPY.Roll;     // x
			bool pitchThresholdReached = UKismetMathLibrary::Abs(deltaRotator.Pitch) >= m_RollDeltaThresholdsRPY.Pitch;  // y
			bool yawThresholdReached = UKismetMathLibrary::Abs(deltaRotator.Yaw) >= m_RollDeltaThresholdsRPY.Yaw;        // z

			// Use the eval enum to check the values
			bool thresholdBroken = false;
			switch (m_RollThresholdEval)
			{
			case E_RollCheckEval::Any:
				thresholdBroken = rollThresholdReached || pitchThresholdReached || yawThresholdReached;
				break;
			case E_RollCheckEval::All:
				thresholdBroken = rollThresholdReached && pitchThresholdReached && yawThresholdReached;
				break;
			case E_RollCheckEval::Roll_Only:
				thresholdBroken = rollThresholdReached;
				break;
			case E_RollCheckEval::Pitch_Only:
				thresholdBroken = pitchThresholdReached;
				break;
			case E_RollCheckEval::Yaw_Only:
				thresholdBroken = yawThresholdReached;
				break;
			default:
				break;
			}

			if (thresholdBroken)
			{
#ifdef UE_BUILD_DEBUG
				FString debugText = UKismetSystemLibrary::GetDisplayName(this); // [this train Handcar display name]
				debugText.Append("roll check deltas: ");                        // roll check deltas: 
				debugText.Append(deltaRotator.ToString());                      // [delta rotator]
				PrintDebugString(debugText);
#endif
				FVector impactLocation = _FrontBogieTransform.GetLocation() + m_HandcarOffset;

				// Add the forward vector, if traveling forward, reverse it if traveling backwards
				FVector forwardVector = _InternalAcceleration > 0.0f ? m_HandcarMesh->GetForwardVector() : m_HandcarMesh->GetForwardVector() * -1.0f;

				// Rotate the right vector to generate the impact impulse
				FVector rightVector = _LastRollingSample.RotateVector(m_HandcarMesh->GetRightVector());

				FVector impaceForce = (rightVector + forwardVector) * _CurrentSpeed * m_RollDerailForce;

				DerailHandcar(impactLocation, impaceForce);
			}
			else
			{
				_TimeToNextRollCheck = m_TimeBetweenRollChecks;
				_LastRollingSample = _FrontBogieTransform.Rotator();
			}
		}
	}
}

void AHandcar::DerailHandcar(const FVector & i_ImpactLocation, const FVector & i_ImpactForce)
{
	if (!m_NeverDerail)
	{
		m_HandcarState = E_HandcarState::Derailed;

		// Clear the speed and velocity then update the mesh and animations so they are in the correct place and stop moving
		_Velocity = 0.0f;
		_CurrentSpeed = 0.0f;
		UpdateHandcarMeshLocations();

		// Turn on physics simulation for the meshes
		m_HandcarMesh->SetSimulatePhysics(true);

		m_HandcarMesh->AddImpulseAtLocation(i_ImpactForce, i_ImpactLocation);

		if (m_ChildHandcar != nullptr)
		{
			FVector childDerailForce = i_ImpactForce * m_ChildDerailDampen;
			m_ChildHandcar->DerailHandcar(i_ImpactLocation, childDerailForce);
		}
	}

	// NOTE: There is a whole bunch of blueprint nodes that aren't hooked up in the original file
}

// Validate later if we use parents
void AHandcar::UpdateStartingDistanceRelativeToParent()
{
	// Make sure this Handcar has a parent and this parent has a track before we continue
	if (m_ParentHandcar != nullptr && m_ParentHandcar->m_AttachedTrack != nullptr)
	{
		// variables used to find location on tracks
		USphereComponent * parentHitch = nullptr;
		float parentHitchDistance = 0.0f;
		UBogie * parentBogie = nullptr;
		float childHitchDistance = 0.0f;
		USphereComponent * childHitch = nullptr;
		float targetDistance = 0.0f;

		if (m_ParentHandcar->_ChildPlacedBehindHandcar)
		{
			if (m_ParentHandcar->m_DisableRearBogie)
			{
				// Parent rear hitch to front bogie
				parentHitch = m_ParentHandcar->m_RearHitch;
				parentHitchDistance = GetHitchToBogieXDistance(m_ParentHandcar->m_RearHitch, m_ParentHandcar->m_HandcarMesh, m_ParentHandcar);

				// Pick the bogie to use for the starting point
				parentBogie = m_ParentHandcar->m_FrontBogie;
			}
			else
			{
				// Parent rear hitch to rear bogie
				parentHitch = m_ParentHandcar->m_RearHitch;
				// TODO update this if we use parent/children
				//parentHitchDistance = GetHitchToBogieXDistance(m_ParentHandcar->m_RearHitch, m_ParentHandcar->m_RearBogieMesh, m_ParentHandcar);

				// Pick the bogie to use for the starting point
				parentBogie = m_ParentHandcar->m_RearBogie;
			}
			childHitchDistance = GetHitchToBogieXDistance(m_FrontHitch, m_HandcarMesh, this);
			childHitch = m_FrontHitch;

			// Calculate the target distance from the parent
			targetDistance = parentHitchDistance + childHitchDistance + m_DistanceFromParent * -1.0f;
		}
		else
		{
			parentHitch = m_ParentHandcar->m_FrontHitch;
			parentHitchDistance = GetHitchToBogieXDistance(m_ParentHandcar->m_FrontHitch, m_ParentHandcar->m_HandcarMesh, m_ParentHandcar);

			// Distance from our front hitch to front bogie
			if (m_DisableRearBogie)
			{
				// Distance from our front hitch to front bogie
				childHitchDistance = GetHitchToBogieXDistance(m_FrontHitch, m_HandcarMesh, this);

				// Calculate the target distance from the parent
				childHitch = m_FrontHitch;
				targetDistance = parentHitchDistance + childHitchDistance + m_DistanceFromParent;
			}
			// Distance from our rear bogie to rear hitch
			else
			{
				// Distance from our rear bogie to rear hitch
				// TODO update this if we use parent/children
				//childHitchDistance = GetHitchToBogieXDistance(m_RearHitch, m_RearBogieMesh, this);

				//Calculate the target distance from the parent
				childHitch = m_RearHitch;
				targetDistance = parentHitchDistance + childHitchDistance + m_DistanceFromParent + UKismetMathLibrary::Abs(m_BogieDistance);
			}

			parentBogie = m_ParentHandcar->m_FrontBogie;
		}

		// input variables for find location
		ATrainTrack * startingTrack = parentBogie->GetTrack();

		bool startTrackValidSpline = false;
		FS_TrackSpline startingTrackSpline = startingTrack->GetTrackSplineByIndex(parentBogie->GetSplineIndex(), startTrackValidSpline);
		float startingDistance = parentBogie->GetSplineDistance();
		TArray<FS_TrackPassThroughInfo> currentPassThroughInfo;

		// init return variables for find location
		ATrainTrack * endTrack = nullptr;
		FS_TrackSpline endTrackSpline = FS_TrackSpline();
		float endDistance = 0.0f;
		bool endReached = false;
		bool invertedDirOfTravel = false;
		TArray<FS_TrackPassThroughInfo> travelInfo;

		// Use the calculated distance and the starting point to find the location relative to the parent Handcar on the tracks
		UTrainAndRailFunctionLibrary::FindLocationOnTracks(startingTrack, startingTrackSpline, startingDistance, targetDistance, false, currentPassThroughInfo,
			endTrack, endTrackSpline, endDistance, endReached, invertedDirOfTravel);

		if (endReached)
		{
			// Use the location found to update this Handcar's starting location on the track
			int32 endTrackSplineIndex = endTrack->GetSplineIndex(endTrackSpline.Spline);
			if (endTrackSplineIndex != -1)
			{
				m_StartingDistanceOnTrack = endDistance;
				m_AttachedTrack = endTrack;
				m_AttachedTrackSplineIndex = endTrackSplineIndex;
				ConnectToParent(parentHitch, childHitch);
			}
			else
			{
#ifdef UE_BUILD_DEBUG
				// Print an error message about the invalid spline

				// Unable to reach correct target destination for placement of child Handcar: 
				FString errorText = "Unable to reach correct target destination for placement of child Handcar: ";
				errorText.Append(UKismetSystemLibrary::GetDisplayName(this));             // [this Handcar display name]
				errorText.Append(FString(" relative to parent: "));                       // relative to parent:
				errorText.Append(UKismetSystemLibrary::GetDisplayName(m_ParentHandcar)); // [parent Handcar display name]
				errorText.Append(FString(" Invalid spline index: "));                     // Invalid spline index: 
				errorText.AppendInt(endTrackSplineIndex);                                 // [end track spline index]
				errorText.Append(FString(" requested of track: "));                       // requested of track:
				errorText.Append(UKismetSystemLibrary::GetDisplayName(endTrack));          // [end track display name]

				PrintDebugString(errorText);
#endif
			}
		}
		else
		{
#ifdef UE_BUILD_DEBUG
			// Print an error message if we couldn't reach the target destination

			// Unable to reach correct target destination for placement of child Handcar: 
			FString errorText = "Unable to reach correct target destination for placement of child Handcar: ";
			errorText.Append(UKismetSystemLibrary::GetDisplayName(this));             // [this Handcar display name]
			errorText.Append(FString(" relative to parent: "));                       // relative to parent:
			errorText.Append(UKismetSystemLibrary::GetDisplayName(m_ParentHandcar)); // [parent Handcar display name]
			errorText.Append(FString(" (tracks might be too small)"));                // (tracks might be too small)

			PrintDebugString(errorText);
#endif
		}
	}
}

// Validate if we use parents
void AHandcar::CheckDistanceToParent(float i_DeltaSeconds)
{
	if (m_ParentHandcar != nullptr && m_UseDistanceLimitChecks)
	{
		// Has enough time passed between checks
		_TimeToNextDistanceCheck -= i_DeltaSeconds;
		if (_TimeToNextDistanceCheck <= 0.0f)
		{
			_TimeToNextDistanceCheck = m_TimeBetweenDistanceChecks;

			// Check the hitches are valid
			if (_ConnectedParentHitch != nullptr && _ConnectedChildHitch != nullptr)
			{
				// Check the distance between the parent hitch and our hitch
				FVector parentHitchLocation = _ConnectedParentHitch->GetComponentLocation();
				FVector childHitchLocation = _ConnectedChildHitch->GetComponentLocation();
				float distanceBetweenHitches = UTrainAndRailFunctionLibrary::GetDistanceBetweenLocations(parentHitchLocation, childHitchLocation);
				if (distanceBetweenHitches <= 0.0f || distanceBetweenHitches >= m_MaxDistanceFromParent)
				{
#ifdef UE_BUILD_DEBUG
					// Can't check distance to parent from child, one or more hitches are invalid. ParentHitch: 
					FString debugText = "Distance from parent outside limits, distance to parent: ";
					debugText.Append(FString::SanitizeFloat(distanceBetweenHitches));  // [distanceBetweenHitches]
					PrintDebugString(debugText);
#endif

					DisconnectFromParent(true);
				}
			}
			else
			{
#ifdef UE_BUILD_DEBUG
				// Can't check distance to parent from child, one or more hitches are invalid. ParentHitch: 
				FString debugText = "Can't check distance to parent from child, one or more hitches are invalid. ParentHitch: ";
				debugText.Append(UKismetSystemLibrary::GetDisplayName(_ConnectedParentHitch));  // [connected parent hitch display name]
				debugText.Append(FString(" ChildHitch: "));                                                    // ChildHitch: 
				debugText.Append(UKismetSystemLibrary::GetDisplayName(_ConnectedChildHitch));   // [connected child hitch display name]
				PrintDebugString(debugText);
#endif
			}
		}
	}
}

// Validate if we use parents
void AHandcar::DisconnectFromParent(bool i_AddToTimedIgnoreList)
{
	if (m_ParentHandcar != nullptr)
	{
		// Add the parent to the ignore list for a few seconds
		if (i_AddToTimedIgnoreList)
		{
			AddActorToCollisionIgnoreList(m_ParentHandcar, 5.0f, false);
		}

		// Disconnect ourselves from the parent and update the Handcar variables
		m_ParentHandcar->m_ChildHandcar = nullptr;
		m_ParentHandcar = nullptr;
		_ConnectedParentHitch = nullptr;
		_ConnectedChildHitch = nullptr;
		_DisconnectedFromParent = true;
	}
}

// Validate if using parents/children
void AHandcar::SetHandcarDirection(E_BogieDirectionOfTravel & i_DirOfTravel)
{
	m_StartingDirectionOfTravel = i_DirOfTravel;

	// Change the track for all bogies
	for (int32 i = 0; i < _Bogies.Num(); i++)
	{
		_Bogies[i]->ChangeTrack(_Bogies[i]->GetTrack(), _Bogies[i]->GetSplineIndex(), _Bogies[i]->GetSplineDistance());
		PutSensorBogieOutFront();

		if (m_ChildHandcar != nullptr)
		{
			m_ChildHandcar->SetHandcarDirection(i_DirOfTravel);
		}
	}
}

// Validate if we use parents
void AHandcar::ConnectToParent(USphereComponent * i_ParentHitch, USphereComponent * i_ChildHitch)
{
	_ConnectedParentHitch = i_ParentHitch;
	_ConnectedChildHitch = i_ChildHitch;
	_DisconnectedFromParent = false;
}

// Validate if we use parents
void AHandcar::SetParentHandcar(AHandcar * i_NewParrent, bool i_RefreshPostion)
{
	m_ParentHandcar = i_NewParrent;
	UpdateSettingsFromParent();

	if (i_RefreshPostion)
	{
		RefreshPositionOnTrack();
	}
}

// Validate if we use children
void AHandcar::SetChildHandcar(AHandcar * i_NewChild)
{
	m_ChildHandcar = i_NewChild;
}

// Validate if we use parents
void AHandcar::UpdateSettingsFromParent()
{
	if (m_ParentHandcar != nullptr && m_InheritParentSettings)
	{
		// Track settings
		m_AttachedTrack = m_ParentHandcar->m_AttachedTrack;
		m_AttachedTrackSplineIndex = m_ParentHandcar->m_AttachedTrackSplineIndex;
		m_EndOfTrackOption = m_ParentHandcar->m_EndOfTrackOption;
		m_AttachDistanceSampleCount = m_ParentHandcar->m_AttachDistanceSampleCount;
		m_SnapToTrack = m_ParentHandcar->m_SnapToTrack;

		// Train Handcars settings
		m_TimeBetweenDistanceChecks = m_ParentHandcar->m_TimeBetweenDistanceChecks;
		m_ParentSpeedLerp = m_ParentHandcar->m_ParentSpeedLerp;
		m_MaxDistanceFromParent = m_ParentHandcar->m_MaxDistanceFromParent;
		m_DistanceFromParent = m_ParentHandcar->m_DistanceFromParent;
		m_UseDistanceLimitChecks = m_ParentHandcar->m_UseDistanceLimitChecks;
		_ChildPlacedBehindHandcar = m_ParentHandcar->_ChildPlacedBehindHandcar;
		m_StartingDirectionOfTravel = m_ParentHandcar->m_StartingDirectionOfTravel;
		_InternalAcceleration = m_ParentHandcar->_InternalAcceleration;

		// Roll settings
		m_UseRollCheck = m_ParentHandcar->m_UseRollCheck;
		m_RollDerailForce = m_ParentHandcar->m_RollDerailForce;
		m_TimeBetweenRollChecks = m_ParentHandcar->m_TimeBetweenRollChecks;
		m_RollDeltaThresholdsRPY = m_ParentHandcar->m_RollDeltaThresholdsRPY;

		// Lean settings
		m_UseLeanEffect = m_ParentHandcar->m_UseLeanEffect;
		m_LeanMaxYawDelta = m_ParentHandcar->m_LeanMaxYawDelta;
		m_LeanIgnoreDelta = m_ParentHandcar->m_LeanIgnoreDelta;
		m_LeanLerpAmount = m_ParentHandcar->m_LeanLerpAmount;
		m_ApplyLeanToBogies = m_ParentHandcar->m_ApplyLeanToBogies;
		m_LeanReturnToRestMultiplier = m_ParentHandcar->m_LeanReturnToRestMultiplier;
		m_LeanMinSpeedPercentage = m_ParentHandcar->m_LeanMinSpeedPercentage;
		m_LeanAmount = m_ParentHandcar->m_LeanAmount;

		// Collision Settings
		m_ObstacleResponse = m_ParentHandcar->m_ObstacleResponse;
		m_NeverDerail = m_ParentHandcar->m_NeverDerail;
		m_DerailForce = m_ParentHandcar->m_DerailForce;
		m_TimeBetweenObstaclePulses = m_ParentHandcar->m_TimeBetweenObstaclePulses;
		m_CollisionQueryTypes = m_ParentHandcar->m_CollisionQueryTypes;
		m_PlowImpactStrength = m_ParentHandcar->m_PlowImpactStrength;
		m_PlowImpactThreshold = m_ParentHandcar->m_PlowImpactThreshold;
		m_DerailForce = m_ParentHandcar->m_DerailForce;

		// Movement smooth
		m_EnableMovementSmoothing = m_ParentHandcar->m_EnableMovementSmoothing;
		m_SmoothDistanceErrorTolerance = m_ParentHandcar->m_SmoothDistanceErrorTolerance;
		m_SmoothDistanceDeltaLimit = m_ParentHandcar->m_SmoothDistanceDeltaLimit;
		m_SmoothAttemptsPerBogie = m_ParentHandcar->m_SmoothAttemptsPerBogie;
		m_DoNotSmoothHelperOrSensorBogies = m_ParentHandcar->m_DoNotSmoothHelperOrSensorBogies;
		m_UseTrackRotationToScaleErrorTolerance = m_ParentHandcar->m_UseTrackRotationToScaleErrorTolerance;
		m_TrackRotationScaleAmount = m_ParentHandcar->m_TrackRotationScaleAmount;

		// Sensor / Collision
		m_ImpactCollisionEnabled = m_ParentHandcar->m_ImpactCollisionEnabled;
	}
}

void AHandcar::CalculateSpeed(float i_DeltaSeconds, bool i_IgnoreLerp)
{
	// If we have a parent, lerp to their speed and velocity
	if (m_ParentHandcar != nullptr)
	{
		float alpha = i_IgnoreLerp ? 0.0f : m_ParentSpeedLerp;

		_CurrentSpeed = UKismetMathLibrary::Lerp(m_ParentHandcar->_CurrentSpeed, _CurrentSpeed, alpha);
		_Velocity = UKismetMathLibrary::Lerp(m_ParentHandcar->_Velocity, _Velocity, alpha);
	}
	else
	{
		// Don't use const speeds if we have a parent
		if (m_UseConstantSpeeds)
		{
			// Constant speed calculations

			// if acceleration is possitive we are going max speed positive, otherwise negative
			_CurrentSpeed = _InternalAcceleration > 0.0f ? m_MaxSpeed : m_MaxSpeed * -1.0f;

			if (_IsBraking)
			{
				StopHandcar();
			}
		}
		else
		{
			float frontBogiePitch = _FrontBogieTransform.Rotator().Pitch;

			// If this is a Handcar or we are braking set the acceleration as 0
			float localAccel = m_HandcarType == E_HandcarType::Engine && !_IsBraking ? _InternalAcceleration : 0.0f;

			float rearBogieVelocity = 0.0f;
			float rearBogiePitch = 0.0f;

			// If we have a rear bogie, grab the pitch
			if (!m_DisableRearBogie)
			{
				rearBogieVelocity = _Velocity;
				rearBogiePitch = m_RearBogie->GetTransform().Rotator().Pitch;
			}

			// Acceleration update to the velocity
			_Velocity += localAccel * i_DeltaSeconds;

			// Calculate the parallel force
			float frontBogieParallelForce = m_Mass * m_Gravity * UKismetMathLibrary::DegSin(frontBogiePitch);

			// Update the velocity with the force
			_Velocity += frontBogieParallelForce;

			float brakingMinClamp = 0.0f;
			float brakingMaxClamp = 0.0f;
			BrakingClamps(brakingMinClamp, brakingMaxClamp);

			// Appply deceleration if we are braking
			if (_IsBraking)
			{
				_Velocity = UKismetMathLibrary::Clamp(_Velocity - (BrakingDeccel() * i_DeltaSeconds), brakingMinClamp, brakingMaxClamp);
			}

			if (!m_DisableRearBogie)
			{
				// Acceleration update to the velocity
				rearBogieVelocity += localAccel * i_DeltaSeconds;

				// Calculate the parallel force
				float rearBogieParallelForce = m_Mass * m_Gravity * UKismetMathLibrary::DegSin(rearBogiePitch);

				// Update the velocity with the force
				rearBogieVelocity += rearBogieParallelForce;

				// Appply deceleration if we are braking
				if (_IsBraking)
				{
					rearBogieVelocity = UKismetMathLibrary::Clamp(rearBogieVelocity - (BrakingDeccel() * i_DeltaSeconds), brakingMinClamp, brakingMaxClamp);
				}

				// Average out the velocity between the two bogies
				_Velocity = (_Velocity + rearBogieVelocity) / 2.0f;
			}

			// Add friction
			_Velocity /= m_Friction;

			// Generate the final speed and clamp the value
			_CurrentSpeed = UKismetMathLibrary::FClamp(_Velocity * i_DeltaSeconds, m_MinSpeed, m_MaxSpeed);

			// If the Handcar is braking and we've hit 0, set it as stopped
			if (_IsBraking && _CurrentSpeed == 0.0f)
			{
				StopHandcar();
			}
		}
	}
}

bool AHandcar::IsDirectionOfTravelInverted(E_TrackSplineLocation & i_ConnectionLocation, float i_Speed) const
{
	// Use the direction of travel and the location of the connection point to see if the direction of travel is inverted
	// If negative, then we are going in reverse

	// In reverse
	if (i_Speed < 0.0f)
	{
		return i_ConnectionLocation == E_TrackSplineLocation::Start;
	}
	// Going forward
	else
	{
		return i_ConnectionLocation == E_TrackSplineLocation::End;
	}
}

float AHandcar::GetWheelSpeed() const
{
	// If the Handcar is braking or it's not moving, send back 0
	return m_HandcarState != E_HandcarState::Moving || _IsBraking ? 0.0f : _CurrentSpeed * m_WheelAnimSpeedScale;
}

void AHandcar::UpdateAcceleration()
{
	_InternalAcceleration = m_Acceleration;
}

void AHandcar::PrintDebugString(FString & i_InfoString)
{
#ifdef UE_BUILD_DEBUG
	if (m_PrintDebugStrings)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, i_InfoString);
	}
#endif
}

void AHandcar::SmoothOutBogieDistance(UBogie * i_Bogie, float i_TargetDistance, const FVector & i_PreviousLocation)
{
	// Exit early if this is a helper bogie and we aren't updating them
	if (!(i_Bogie->IsSensorOrHelperBogie() && m_DoNotSmoothHelperOrSensorBogies))
	{
		int32 loopCount = 0;

		bool notCloseEnough = true;
		while (notCloseEnough)
		{
			loopCount++;

			// Calc the distance traveled in world space
			float vecLen = (i_Bogie->GetLocation() - i_PreviousLocation).Size();

			// If it's 0, there's nothing for us to adjust
			if (vecLen == 0.0)
			{
				notCloseEnough = false;
			}
			else
			{
				// Get the error tolerance value
				FRotator sourceRotator = i_Bogie->GetRotation();
				float errorTolerance = m_UseTrackRotationToScaleErrorTolerance ? GetRotationBasedSmoothErrorTolerance(sourceRotator) : m_SmoothDistanceErrorTolerance;

				// Are we close enough? If not, smooth it
				bool vecLenEqualsTarget = UKismetMathLibrary::NearlyEqual_FloatFloat(UKismetMathLibrary::Abs(i_TargetDistance), UKismetMathLibrary::Abs(vecLen), errorTolerance);

				if (vecLenEqualsTarget)
				{
					notCloseEnough = false;
				}
				else
				{
					// Check the size of the delta
					float notAbsDelta = UKismetMathLibrary::Abs(i_TargetDistance) - vecLen;
					float delta = _CurrentSpeed > 0.0f ? notAbsDelta : notAbsDelta * -1.0f;
					if (UKismetMathLibrary::Abs(delta) >= m_SmoothDistanceDeltaLimit)
					{
						// Let the user know if we ignored this
#ifdef UE_BUILD_DEBUG
						FString debugText = "Ignoring smooth delta of size: ";                   // Ignoring smooth delta of size: 
						debugText.Append(FString::SanitizeFloat(delta));                         // [delta]
						debugText.Append(FString(" on bogie: "));                                // on bogie: 
						debugText.Append(UKismetSystemLibrary::GetDisplayName(this));            // [this Handcar display name]
						debugText.Append(i_Bogie->GetDebugName());                               // [boige display name]

						PrintDebugString(debugText);
#endif
						notCloseEnough = false;
					}
					else
					{
						// Adjust the bogie position on the spline based on the delta
						bool hitEOT = TickBogieMovement(i_Bogie, delta, false);
						if (hitEOT)
						{
							notCloseEnough = false;
						}
						else
						{
							// Stop if we have been smoothing this boogie too long
							if (loopCount >= m_SmoothAttemptsPerBogie)
							{
#ifdef UE_BUILD_DEBUG
								FString debugText = "Forced smooth loop to stop for: ";                   // Forced smooth loop to stop for: 
								debugText.Append(i_Bogie->GetDebugName());                                // [boige display name]

								PrintDebugString(debugText);
#endif
								notCloseEnough = false;
							}
						}
					}
				}
			}
		}
	}
}

float AHandcar::GetRotationBasedSmoothErrorTolerance(const FRotator & i_SourceRotator) const
{
	float smoothErrorTolerance = (i_SourceRotator.Roll + i_SourceRotator.Pitch + i_SourceRotator.Yaw) * m_TrackRotationScaleAmount * m_SmoothDistanceErrorTolerance;

	// Make sure we don't go below 0
	if (smoothErrorTolerance <= 0.0f)
	{
		smoothErrorTolerance = 0.01f;
	}

	return smoothErrorTolerance;
}

#pragma endregion

#pragma region Custom Helper Methods

void AHandcar::UpdateTrainHandcar()
{
	UpdateSettingsFromParent();

	if (m_SnapToTrack && m_AttachedTrack != nullptr)
	{
		AttachToTrack();
		if (_Bogies.Num() <= 3)
		{
			SetupCollision();
			SetupBogies();
		}

		UpdateBogiesAndTrackers();

		UpdateHandcarMeshLocations();

		// Set the internal acceleration based on direction of travel, forward = positive, backwards = negative
		if (E_BogieDirectionOfTravel::Forward == m_StartingDirectionOfTravel)
		{
			_InternalAcceleration = m_Acceleration;
		}
		else
		{
			_InternalAcceleration = -1.0f * m_Acceleration;
		}

		// Set our relation to the child Handcar if we have one
		if (m_ChildHandcar != nullptr)
		{
			m_ChildHandcar->SetParentHandcar(this, false);
			m_ChildHandcar->SetHandcarDirection(m_StartingDirectionOfTravel);
		}

		// Set our relation to the parent Handcar if we have one
		if (m_ParentHandcar != nullptr)
		{
			m_ParentHandcar->SetChildHandcar(this);
		}

		// If we are a Handcar and we don't have a parent, set the disconnected flag
		if (E_HandcarType::Handcar == m_HandcarType && m_ParentHandcar == nullptr)
		{
			_DisconnectedFromParent = true;
		}
	}
}

#pragma endregion

#pragma region Private Helper Methods

void AHandcar::AddTrackTriggersToIgnoreArray()
{
	TArray<AActor*> triggersInWorld;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrackTrigger::StaticClass(), triggersInWorld);

	for (AActor * ignoreActor : triggersInWorld)
	{
		AddActorToCollisionIgnoreList(ignoreActor, 0.0f, true);
	}
}

void AHandcar::UpdateBogiesAndTrackers()
{
	UpdateBogieDistances(m_StartingDistanceOnTrack, m_AttachedTrack, m_AttachedTrackSplineIndex, false, _Bogies, _StartInverted);
	_FrontBogieTransform = m_FrontBogie->GetTransform();
	_FrontBogieTransformLastTick = _FrontBogieTransform;
	_LastRollingSample = _FrontBogieTransform.Rotator();
}

void AHandcar::ValidateBogies()
{
	for (int32 i = 0; i < _Bogies.Num(); i++)
	{
		if (_Bogies[i] == nullptr)
		{
			_Bogies.RemoveAt(i);
			i--;
		}
	}
}

#pragma endregion

#pragma region Macros and Inline

inline float AHandcar::GetSensorBogieDistance() const
{
	// Use the direction of travel to place the sensor our front or back of the front bogie
	bool frontOrBack = _InternalAcceleration > 0.0f && !m_FrontBogie->GetInvertDirOfTravel();

	float backBogieDistance = m_DisableRearBogie ? m_SensorBogieDistance * -1.0f : (m_SensorBogieDistance * -1.0f) + m_BogieDistance;

	// returns front or back bogie distance
	return frontOrBack ? m_SensorBogieDistance : backBogieDistance;
}

inline UBogie * AHandcar::GetFrontBogieBasedOnDirOfTravel() const
{
	// Get the bogie that's at the front of the Handcar based on direction of travel.
	// If going forward, it's the front bogie, if backwards, it's the rear bogie
	if (_InternalAcceleration <= 0.0 && !m_DisableRearBogie)
	{
		return m_RearBogie;
	}
	return m_FrontBogie;
}

inline void AHandcar::AddCollisionBogie(FString i_DebugName, float i_DistanceFromHandcar)
{
	// Create the bogie, track and other settings are applied automatically to all bogies
	FName bogieName = MakeUniqueObjectName(this, UBogie::StaticClass());
	UBogie * newBogie = NewObject<UBogie>(this, bogieName);

	newBogie->SetDebugName(i_DebugName);
	newBogie->SetDistanceFromHandcar(i_DistanceFromHandcar);
	newBogie->SetIgnoreEndOfTrack(true);
	newBogie->SetIsSensorOrHelperBogie(true);

	// Any bogie added to this array will have a collision test performed
	_CollisionBogies.Add(newBogie);
}

inline void AHandcar::CreateRandomDerailValues(bool i_UseRandomRotation, FVector & o_ImpactLocation, FVector & o_ImpactForce)
{
	o_ImpactLocation = _FrontBogieTransform.GetLocation();

	FRotator forceRotator = i_UseRandomRotation ? UKismetMathLibrary::RandomRotator() : FRotator::ZeroRotator;
	FVector forceDirection = forceRotator.RotateVector(m_HandcarMesh->GetForwardVector());
	FVector impactForce = forceDirection * m_DerailForce * _CurrentSpeed;
}

inline float AHandcar::GetHitchToBogieXDistance(USphereComponent * i_Hitch, USkeletalMeshComponent * i_BogieMesh, AHandcar * i_ParrentHandcar)
{
	// The hitches are parented to the Handcar mesh, but the bogie meshes are parented to the Handcar.
	// So we must convert the transforms before we use them
	return UKismetMathLibrary::Abs(i_BogieMesh->GetRelativeLocation().X -
		UKismetMathLibrary::MakeRelativeTransform(i_Hitch->GetComponentTransform(), i_ParrentHandcar->RootComponent->GetComponentToWorld()).GetLocation().X);
}

inline FTransform AHandcar::AddLocationOffsetToTransform(FTransform & i_SourceTransform, FVector & i_Offset)
{
	return FTransform(i_SourceTransform.Rotator(), i_SourceTransform.GetLocation() + i_Offset, i_SourceTransform.GetScale3D());
}

inline void AHandcar::BrakingClamps(float & o_Min, float & o_Max)
{
	// Set velocity clamps for braking based on dir of travel
	o_Min = _CurrentSpeed > 0.0f ? 0.0 : _Velocity;
	o_Max = _CurrentSpeed > 0.0f ? _Velocity : 0.0;
}

inline float AHandcar::BrakingDeccel()
{
	return _CurrentSpeed > 0.0f ? m_Deceleration : m_Deceleration * -1.0f;
}

inline FVector AHandcar::CalculateDerailForce(FVector & i_Normal)
{
	FRotator rotator = FRotator(180.0f, 0.0f, 0.0f);
	return rotator.RotateVector(i_Normal) * m_DerailForce * _CurrentSpeed;
}

inline float AHandcar::TransformLocationLen(FTransform & i_TranformA, FTransform & i_TransformB)
{
	return (i_TranformA.GetLocation() - i_TransformB.GetLocation()).Size();
}

inline void AHandcar::UpdateBogiePreUpdatePositions()
{
	if (m_EnableMovementSmoothing)
	{
		_PreUpdateBogiePositions.Empty();
		for (int32 i = 0; i < _Bogies.Num(); i++)
		{
			_PreUpdateBogiePositions.Insert(_Bogies[i]->GetLocation(), i);
		}
	}
}

#pragma endregion

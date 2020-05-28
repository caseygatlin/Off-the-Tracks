// Fill out your copyright notice in the Description page of Project Settings.


#include "DestructibleWall.h"
#include "Gamecode/Handcars/PlayerHandcar.h"
#include "DestructibleComponent.h"
#include "DestructibleMesh.h"

#include "UObject/ConstructorHelpers.h"



// Sets default values
ADestructibleWall::ADestructibleWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//static ConstructorHelpers::FObjectFinder<UDestructibleMesh> WallMesh(TEXT(
	//	"DestructibleMesh'/Game/HandCarContent/Geometry/Meshes/SM_Destructible_DM.SM_Destructible_DM'"));

	//m_DestructibleWall = CreateDefaultSubobject<UDestructibleComponent>("Destructible Wall");
	//m_DestructibleWall->SetDestructibleMesh(WallMesh.Object);

}

// Called when the game starts or when spawned
void ADestructibleWall::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(m_PlayerHandcar))
	{
		bool neverExpire = true;
		m_PlayerHandcar->AddActorToCollisionIgnoreList(this, 0.f, neverExpire);
	}
}

// Called every frame
void ADestructibleWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


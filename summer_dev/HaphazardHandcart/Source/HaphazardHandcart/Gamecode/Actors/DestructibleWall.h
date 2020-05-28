// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DestructibleWall.generated.h"

class UDestructibleComponent;
class APlayerHandcar;

UCLASS()
class HAPHAZARDHANDCART_API ADestructibleWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructibleWall();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Public Variables

#pragma region Components

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UDestructibleComponent* m_DestructibleWall;

#pragma endregion

#pragma region Handcar

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		APlayerHandcar* m_PlayerHandcar;

#pragma endregion

#pragma endregion




};

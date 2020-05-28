// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFileWriteBlueprints.generated.h"

/**
 * 
 */
UCLASS()
class HAPHAZARDHANDCART_API UDebugFileWriteBlueprints : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Save String Array", CompactNodeTitle = "Save String Array", Keywords = "Save String Array"), Category = Game)
		static bool SaveStringArray(TArray<FString> values, FString FileName);

};

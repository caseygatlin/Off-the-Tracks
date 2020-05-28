// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class HAPHAZARDHANDCART_API UDebugFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintCallable, Category = "Handcar|Debug Library")
        static void DebugPrintLog(FString i_StringToPrint);

    UFUNCTION(BlueprintCallable, Category = "Handcar|Debug Library")
        static void DebugPrintScreen(FString i_StringToPrint, float i_TimeToDisplay = 5.f, FColor i_TextColor = FColor::Blue, int32 i_Key = -1);

};

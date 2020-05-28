// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugFileWriteBlueprints.h"
#include "Misc/FileHelper.h"

bool UDebugFileWriteBlueprints::SaveStringArray(TArray<FString> values, FString FileName)
{
	UE_LOG(LogClass, Log, TEXT("%s"), *FileName);
	return FFileHelper::SaveStringArrayToFile(values,*FileName);

}

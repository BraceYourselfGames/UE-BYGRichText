// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "BYGStyleTagData.generated.h"

USTRUCT( BlueprintType )
struct FBYGStyleTagData
{
	GENERATED_BODY()

public:
	FString ID;
	FString Content;
	TMap<FString, FString> Pairs;
};


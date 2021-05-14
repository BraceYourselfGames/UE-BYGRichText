// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "BYGStyleDisplayType.generated.h"

UENUM()
enum class EBYGStyleDisplayType : uint8
{
	// Display alongside other elements on the same line.
	Inline UMETA( DisplayName = "Inline" ),
	// Display on its own line.
	Block UMETA( DisplayName = "Block" ),
};


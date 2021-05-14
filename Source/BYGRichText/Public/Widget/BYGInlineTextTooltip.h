// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BYGRichTextMarkupProcessing.h"
#include "Core/BYGStyleTagData.h"
#include "BYGInlineTextTooltip.generated.h"

UINTERFACE( MinimalAPI )
class UBYGInlineTextTooltip : public UInterface
{
    GENERATED_BODY()
};


class BYGRICHTEXT_API IBYGInlineTextTooltip
{
    GENERATED_BODY()

public:
	virtual void InitializeTooltip( const FBYGStyleTagData& Data ) = 0;

};

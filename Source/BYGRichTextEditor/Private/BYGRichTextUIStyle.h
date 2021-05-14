// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"
#include "Styling/SlateStyle.h"

class FBYGRichTextUIStyle
{
public:
	static void Initialize();

	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

	static FName GetStyleSetName();

	static const FSlateBrush* GetBrush( FName PropertyName, const ANSICHAR* Specifier = NULL );

private:
	static TSharedPtr< class FSlateStyleSet > StyleSet;
};

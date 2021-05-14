// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

// Multi-purpose decorator
// Outputs text, widgets, hyperlinks depending on what is required

#include "CoreMinimal.h"
#include "SlateGlobals.h"
#include "Framework/Text/ISlateRun.h"
#include "Framework/Text/SlateHyperlinkRun.h"
#include "Framework/Text/ITextDecorator.h"
#include "Framework/Text/SlateWidgetRun.h"

class ISlateStyle;
class UBYGRichTextBlock;


class BYGRICHTEXT_API FBYGInlineTextFormatDecorator : public ITextDecorator
{
public:

	static TSharedRef< FBYGInlineTextFormatDecorator > Create( FString InRunName, const UBYGRichTextBlock* InOwner );
	virtual ~FBYGInlineTextFormatDecorator() {}

	virtual bool Supports( const FTextRunParseResults& RunParseResult, const FString& Text ) const override;
	virtual TSharedRef< ISlateRun > Create( const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style ) override;

private:

	FBYGInlineTextFormatDecorator( FString InRunName, const UBYGRichTextBlock* InOwner );

	FString RunName;

	const class UBYGRichTextBlock* RichTextBlockOwner = nullptr;
};

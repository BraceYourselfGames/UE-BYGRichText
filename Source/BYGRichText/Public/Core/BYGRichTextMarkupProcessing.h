// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "Runtime/Slate/Public/Framework/Text/IRichTextMarkupParser.h"
#include "Runtime/Slate/Public/Framework/Text/IRichTextMarkupWriter.h"
#include "Runtime/Slate/Public/Framework/Text/RichTextMarkupProcessing.h"

#include "BYGStyleTagData.h"

#include "Core/BYGRichTextMarkupProcessing.h"

class UWidget;
class UBYGRichTextPropertyBase;

struct FBYGTextBlockInfo
{
	FBYGTextBlockInfo();
	// For testing
	FBYGTextBlockInfo( const FString& InRawText, const TArray<FName>& InStyles, const TMap<FString, FString>& InPayload )
		: RawText( InRawText )
		, StylesApplied( InStyles )
		, Payload( InPayload )
	{ }
	FString RawText;
	TArray<FName> StylesApplied;
	TMap<FString, FString> Payload;
	TMap<FName, const UBYGRichTextPropertyBase*> BlockPropertiesMap;
	void OverwriteProperties( const FName& StyleName, const TArray<UBYGRichTextPropertyBase*>& NewBlockProperties );
	int32 InlineStyleStackCount = 0;
};


// There are two parts to our parser
// 1) Extracting the block-level formatting info: margins, alignment
// 2) Extracting the inline formatting info: font etc.
class BYGRICHTEXT_API FBYGRichTextMarkupParser : public IRichTextMarkupParser //FDefaultRichTextMarkupParser
{
public:
	static TSharedRef<FBYGRichTextMarkupParser> Create( class UBYGRichTextBlock* InTextBlockOwner, const FString& XMLElementName );

	virtual void Process( TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output ) override;

	TArray<FBYGTextBlockInfo> SplitIntoBlocks( const FString& Input );

protected:
	FBYGRichTextMarkupParser( class UBYGRichTextBlock* TextBlockOwner, const FString& InXMLElementName );

	FString ConvertInputToInlineXML( const FString& Input );

	class UBYGRichTextBlock* TextBlockOwner = nullptr;
	FString XMLElementName = "";
};


class BYGRICHTEXT_API FBYGRichTextMarkupWriter : public IRichTextMarkupWriter
{
public:
	static TSharedRef<FBYGRichTextMarkupWriter> Create();

	virtual void Write(const TArray<FRichTextLine>& InLines, FString& Output) override;

protected:
	//TSharedRef<class FDefaultRichTextMarkupWriter> DefaultWriter;

private:
	FBYGRichTextMarkupWriter() {}

	//static void EscapeText(FString& TextToEscape);
};


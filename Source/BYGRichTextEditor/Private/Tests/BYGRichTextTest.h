// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Widget/BYGRichTextBlock.h"
#include <Framework/Text/ITextDecorator.h>
#include "Settings/BYGRichTextStylesheet.h"
#include <Tests/AutomationEditorCommon.h>
#include <FunctionalTestBase.h>
#include <UObject/GCObject.h>


class FBYGRichTextParseTestBase : public FFunctionalTestBase, public FGCObject
{

public:
	FBYGRichTextParseTestBase( const FString& InName, const bool bInComplexTask );

	virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const override;
	virtual bool RunTest( const FString& Parameters ) override;

protected:

	struct FTestInstanceRegex
	{
		const FString Input;
		const FString ExpectedPattern;
	};
	TMap<FString, FTestInstanceRegex> TestData;
	//class UBYGRichTextStylesheet* DefaultStylesheet = nullptr;

	//~ Begin FGCObject interface
	void AddReferencedObjects( FReferenceCollector& Collector )
	{
		//Collector.AddReferencedObject( DefaultStylesheet );
	}
	//~ End FGCObject interface
};


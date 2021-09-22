// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"

#include "Settings/BYGRichTextStylesheet.h"

#include "BYGRichTextRuntimeSettings.generated.h"

class UBYGRichTextStylesheet;

UCLASS( config = Engine, defaultconfig )
class BYGRICHTEXT_API UBYGRichTextRuntimeSettings : public UObject
{
	GENERATED_BODY()

public:
	UBYGRichTextRuntimeSettings( const FObjectInitializer& ObjectInitializer )
		: Super( ObjectInitializer )
		, TagOpenCharacter( "[" )
		, TagCloseCharacter( "]" )
		, ParagraphSeparator( "\r\n\r\n" )
	{
	}

	// Stylesheet selected by default in new BYG Rich Text Block widgets
	UPROPERTY( config, EditAnywhere, Category = Settings, meta = ( MetaClass = "BYGRichTextStylesheet" ) )
		FSoftClassPath DefaultStylesheet;

	UPROPERTY( config, EditAnywhere, Category = Settings )
		FString TagOpenCharacter;
	UPROPERTY( config, EditAnywhere, Category = Settings )
		FString TagCloseCharacter;

	UPROPERTY( config, EditAnywhere, Category = Settings )
		FString ParagraphSeparator;

#if WITH_EDITOR
	EDataValidationResult IsDataValid( TArray<FText>& ValidationErrors ) override
	{
		EDataValidationResult Result = Super::IsDataValid( ValidationErrors );

		if ( TagOpenCharacter.Len() <= 0 )
		{
			ValidationErrors.Add( FText::FromString( "Tag close character cannot be blank" ) );
		}
		if ( TagCloseCharacter.Len() <= 0 )
		{
			ValidationErrors.Add( FText::FromString( "Tag close character cannot be blank" ) );
		}
		if ( ValidationErrors.Num() > 0 )
		{
			return EDataValidationResult::Invalid;
		}

		return Result;
	}
#endif
};

// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "BYGRichTextProperty.h"
#include "BYGStyleDisplayType.h"
#include "BYGRIchTextStyle.generated.h"

class UBYGRichTextPropertyBase;

DECLARE_DELEGATE( FBYGOnStylePropertyChangedSignature );

// A style contains one or more properties that define how text with this
// style should be displayed 
UCLASS( EditInlineNew, meta = ( DisplayName = "BYG Rich Text Style" ) )
class BYGRICHTEXT_API UBYGRichTextStyle : public UObject
{
	GENERATED_BODY()

public:
	void InitDefaultProperties();

	FName GetID() const { return ID; }
	void SetID( const FName& InID )
	{
		if ( IsValidID( InID ) ) ID = InID;
	}

	void AddProperty( UBYGRichTextPropertyBase* Property );

	UPROPERTY( EditAnywhere, Instanced, meta = ( DisplayOrder = 50 ) )
		TArray<UBYGRichTextPropertyBase*> Properties;

	bool HasShortcut() const
	{
		return !Shortcut.IsEmpty();
	}
	int32 GetShortcutLen() const
	{
		return Shortcut.Len();
	}

	EBYGStyleDisplayType GetDisplayType() const
	{
		return DisplayType;
	}
	void SetDisplayType( EBYGStyleDisplayType InDisplayType )
	{
		DisplayType = InDisplayType;
	}

	FString GetShortcut() const { return Shortcut; }
	void SetShortcut( const FString& InShortcut )
	{
		Shortcut = ValidateShortcut( InShortcut );
	}

	static bool IsValidID( const FName& InID );
	static bool IsValidShortcut( const FString& InShortcut );

	FBYGOnStylePropertyChangedSignature OnStylePropertyChangedDelegate;

protected:
	static FString ValidateShortcut( const FString& InShortcut );

	void SortProperties();

	// The unique identifier of the style
	UPROPERTY( EditAnywhere )
		FName ID;

	// A short string used in place of the full identifier for both inline and
	// block styles.
	// For example a shortcut "*" on an inline style allows us to write
	// "*cat*" to mark up the word "cat".
	UPROPERTY( EditAnywhere )
		FString Shortcut;

	// Block styles create newlines after them
	// Certain text properties only work with block or inline
	UPROPERTY( EditAnywhere )
		EBYGStyleDisplayType DisplayType = EBYGStyleDisplayType::Inline;

	void PostInitProperties() override;

	void PostLoad() override;

	friend class FBYGRichTextStyleCustomization;
};


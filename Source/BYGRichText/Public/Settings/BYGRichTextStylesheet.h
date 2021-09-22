// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BYGRichTextProperty.h"
#include "BYGStyleDisplayType.h"
#include "Framework/Text/ITextLayoutMarshaller.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include <Engine/DataAsset.h>
#include "BYGRIchTextStylesheet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FBYGOnStylesheetPropertiesChangedSignature );

class UBYGRichTextStyle;

/**
 * Stylesheet allows for definition of multiple styles
 */
UCLASS( Blueprintable, meta = ( DisplayName = "BYG Rich Text Stylesheet" ) )
class BYGRICHTEXT_API UBYGRichTextStylesheet : public UDataAsset
{
	GENERATED_BODY()

public:
	UBYGRichTextStylesheet( const FObjectInitializer& ObjectInitializer );

	UBYGRichTextStyle* FindStyle( TCHAR const* Input, int32 CurrentIndex, TOptional<EBYGStyleDisplayType> DisplayType = TOptional<EBYGStyleDisplayType>() ) const;
	UBYGRichTextStyle* FindStyle( const FName& ID ) const;

	const UBYGRichTextPropertyBase* FindProperty( int32 InlineID ) const;

	TSharedPtr<SWidget> RebuildWidget( const FText& InText, TSharedRef<FRichTextLayoutMarshaller> Marshaller );

	void AddStyle( UBYGRichTextStyle* InStyle );
	void RemoveStyle( const FName& StyleID );
	bool GetHasStyle( const FName& StyleID ) const;

	UPROPERTY( EditAnywhere, Instanced )
		TArray<UBYGRichTextStyle*> Styles;

	TArray<const UBYGRichTextPropertyBase*> GetDefaultProperties() const { return DefaultProperties; }

	FName GetDefaultStyleName() const { return DefaultStyleName; }
	void SetDefaultStyleName( const FName& NewDefaultName ) { DefaultStyleName = NewDefaultName; }

	virtual void BeginDestroy() override;
	virtual void PostLoad() override;
	virtual void PostCDOContruct() override;
	virtual void PostEditImport() override;

#if WITH_EDITOR
	virtual bool Modify( bool bAlwaysMarkDirty = true ) override;
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent ) override;
	virtual void PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent ) override;

	// Custom validation for Blueprint error logs
	void Validate( FCompilerResultsLog& ResultsLog ) const;
#endif

	// So we can have const stuff and still register for changes
	mutable FBYGOnStylesheetPropertiesChangedSignature OnStylesheetPropertiesChangedDelegate;


protected:
	UPROPERTY( EditDefaultsOnly )
		FName DefaultStyleName;

public: // testing hack
	void RebuildLookup();
protected:
	UPROPERTY( Instanced )
		TArray<const UBYGRichTextPropertyBase*> DefaultProperties;

	// Properties should be owned by the styles, not through this lookup
	//UPROPERTY( Transient )
	TMap<int32, TWeakObjectPtr<const UBYGRichTextPropertyBase>> PropertyLookup;

	// Hacky way of allowing customization from the editor
	friend class FBYGRichTextStyleCustomization;
};



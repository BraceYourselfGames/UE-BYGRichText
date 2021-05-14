// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "Core/BYGRichTextMarkupProcessing.h"
#include "BYGRichTextBlock.generated.h"

UENUM()
enum class EBYGRichTextOverflowBehavior : uint8
{
	Wrap,
	Clip,
};

class SRichTextBlock;
class URichTextBlockDecorator;
class ITextDecorator;
class IRichTextMarkupParser;
class IRichTextMarkupWriter;
class UBYGRichTextStylesheet;

/**
 * 
 */
UCLASS( meta = ( DisplayName = "BYG Rich Text Block" ) )
class BYGRICHTEXT_API UBYGRichTextBlock : public UWidget
{
	GENERATED_BODY()

public:
	UBYGRichTextBlock( const FObjectInitializer& ObjectInitializer );

	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources( bool bReleaseChildren ) override;
	// End of UVisual interface

	void SetText( const FText& InText );
	inline FText GetText() { return Text; }


	// TODO should store unmodifiable rich text stylesheet instance in the module?
	const UBYGRichTextStylesheet* GetRichTextStylesheet() const;
	void SetRichTextStylesheet( const UBYGRichTextStylesheet* InRichTextStylesheet );
	void SetRichTextStylesheetClass( TSubclassOf<UBYGRichTextStylesheet> InRichTextStylesheetClass );


#if WITH_EDITOR
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface

	bool bIsSlatePreview = false; 
#endif


#if WITH_EDITOR
	void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

	UFUNCTION()
		void OnRichTextStylesheetChanged();

	void RebuildContents();

	virtual void CreateDecorators( TArray< TSharedRef<ITextDecorator> >& OutDecorators );
	virtual TSharedPtr<IRichTextMarkupParser> CreateMarkupParser();
	virtual TSharedPtr<IRichTextMarkupWriter> CreateMarkupWriter();

	// The text to display.
	UPROPERTY( EditAnywhere, Category = "Rich Text", meta = ( MultiLine = "true", DisplayOrder = 0 ) )
		FText Text;

	// Defines the visual style of the text. Must be set.
	UPROPERTY( EditAnywhere, Category = "Rich Text", meta = ( DisplayOrder = 20 ) )
		TSubclassOf<UBYGRichTextStylesheet> RichTextStylesheetClass;
	UPROPERTY( Transient )
		const UBYGRichTextStylesheet* RichTextStylesheet;
	bool bHasExternallyDefinedStylesheet = false;


	TArray<FBYGTextBlockInfo> BlockInfos;

	TSharedPtr<SVerticalBox> MyVerticalBox;
	TArray<TSharedPtr<SRichTextBlock> >MyRichTextBlocks;
};

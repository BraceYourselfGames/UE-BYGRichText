// Copyright Brace Yourself Games. All Rights Reserved.

#include "Widget/BYGRichTextBlock.h"

#include "BYGRichTextRuntimeSettings.h"
#include "Components/RichTextBlockDecorator.h"
#include "Core/BYGInlineTextFormatDecorator.h"
#include "Core/BYGRichTextMarkupProcessing.h"
#include "Framework/Text/RichTextLayoutMarshaller.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/SRichTextBlock.h"
#include <Modules/ModuleManager.h>
#include "BYGRichTextModule.h"

#define LOCTEXT_NAMESPACE "BYGRichText"

UBYGRichTextBlock::UBYGRichTextBlock( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	// Don't make BP variable by default, it's messy and annoying
	bIsVariable = false;
}

void UBYGRichTextBlock::ReleaseSlateResources( bool bReleaseChildren )
{
	Super::ReleaseSlateResources( bReleaseChildren );

	MyVerticalBox.Reset();
	for ( TSharedPtr<SRichTextBlock>& TextBlock : MyRichTextBlocks )
	{
		TextBlock.Reset();
	}
}

TSharedRef<SWidget> UBYGRichTextBlock::RebuildWidget()
{
	SAssignNew( MyVerticalBox, SVerticalBox );

	RebuildContents();

	return MyVerticalBox.ToSharedRef();
}

void UBYGRichTextBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	RebuildContents();
}

void UBYGRichTextBlock::RebuildContents()
{
	FBYGRichTextModule& RichTextModule = FModuleManager::GetModuleChecked<FBYGRichTextModule>( TEXT( "BYGRichText" ) );

	TArray< TSharedRef< class ITextDecorator > > CreatedDecorators;
	CreateDecorators( CreatedDecorators );

	if ( RichTextStylesheetClass )
	{
		RichTextStylesheet = RichTextStylesheetClass->GetDefaultObject<UBYGRichTextStylesheet>();
		if ( RichTextStylesheet )
		{
			RichTextStylesheet->OnStylesheetPropertiesChangedDelegate.AddUniqueDynamic( this, &UBYGRichTextBlock::OnRichTextStylesheetChanged );
		}
	}

	TSharedPtr<FBYGRichTextMarkupParser> Parser = FBYGRichTextMarkupParser::Create( this, "s" );
	TSharedRef<FRichTextLayoutMarshaller> Marshaller = FRichTextLayoutMarshaller::Create( Parser, CreateMarkupWriter(), CreatedDecorators, RichTextModule.SlateStyleSet.Get() );

	BlockInfos = Parser->SplitIntoBlocks( Text.ToString() );

	MyRichTextBlocks.Empty();
	// TODO: Need to Reset each one here?

	if ( !ensure( MyVerticalBox ) )
	{
		UE_LOG( LogTemp, Error, TEXT( "Veritcal box is null!" ) );
		return;
	}
	MyVerticalBox->ClearChildren();

	for ( const FBYGTextBlockInfo& BlockInfo : BlockInfos )
	{
		TSharedPtr<SRichTextBlock> TextBlock =
			SNew( SRichTextBlock )
			//.TextStyle( &DefaultTextStyle )
			.Marshaller( Marshaller );

		TSharedRef<SRichTextBlock> TextBlockRef = TextBlock.ToSharedRef();

		// Fill with the properties for this block, based on formatting info
		TMap<FName, const UBYGRichTextPropertyBase*> BlockPropertiesMap = BlockInfo.BlockPropertiesMap;

		// Add any properties that should be applied, if they have not already got defaults
		const UBYGRichTextStylesheet* LocStylesheet = RichTextStylesheet;
		if ( !RichTextStylesheet )
		{
			LocStylesheet = RichTextModule.GetFallbackStylesheet();
		}
		for ( const UBYGRichTextPropertyBase* Prop : LocStylesheet->GetDefaultProperties() )
		{
			if ( !BlockPropertiesMap.Contains( Prop->GetTypeID() )
				&& Prop->GetShouldApplyToDefault() )
			{
				BlockPropertiesMap.Add( Prop->GetTypeID(), Prop );
			}
		}

		// Apply block-level formatting like margin, line-height percentage
		for ( const auto& Pair : BlockPropertiesMap )
		{
			Pair.Value->ApplyToTextBlock( TextBlockRef );
		}

		TSharedRef<SWidget> FinalWidget = TextBlock.ToSharedRef();
		for ( const auto& Pair : BlockPropertiesMap )
		{
			FinalWidget = Pair.Value->WrapBlock( FinalWidget, this, BlockInfo.Payload );
		}

		TextBlock->SetText( FText::FromString( BlockInfo.RawText ) );

		MyRichTextBlocks.Add( TextBlock );

		MyVerticalBox->AddSlot()
			.AutoHeight()
			[
				FinalWidget
			];
	}
}


void UBYGRichTextBlock::SetText( const FText& InText )
{
	Text = InText;

	RebuildWidget();
}

void UBYGRichTextBlock::CreateDecorators( TArray< TSharedRef< class ITextDecorator > >& OutDecorators )
{
	OutDecorators.Add( FBYGInlineTextFormatDecorator::Create( "s", this ) );
}

TSharedPtr<IRichTextMarkupParser> UBYGRichTextBlock::CreateMarkupParser()
{
	return FBYGRichTextMarkupParser::Create( this, "s" );
}

TSharedPtr<IRichTextMarkupWriter> UBYGRichTextBlock::CreateMarkupWriter()
{
	return FBYGRichTextMarkupWriter::Create();
}

void UBYGRichTextBlock::SetRichTextStylesheet( const UBYGRichTextStylesheet* InRichTextStylesheet )
{
	ensure( InRichTextStylesheet );

	RichTextStylesheet = InRichTextStylesheet;

	if ( RichTextStylesheet )
	{
		bHasExternallyDefinedStylesheet = true;
		RichTextStylesheet->OnStylesheetPropertiesChangedDelegate.AddUniqueDynamic( this, &UBYGRichTextBlock::OnRichTextStylesheetChanged );
	}
}

void UBYGRichTextBlock::SetRichTextStylesheetClass( TSubclassOf<UBYGRichTextStylesheet> InRichTextStylesheetClass )
{
	ensure( InRichTextStylesheetClass );

	TSubclassOf<UBYGRichTextStylesheet> StylesheetClass = InRichTextStylesheetClass;
	const UBYGRichTextRuntimeSettings* Settings = GetDefault<UBYGRichTextRuntimeSettings>();
	if ( Settings )
	{
		Settings->DefaultStylesheet.TryLoad();
		SetRichTextStylesheetClass( Settings->DefaultStylesheet.ResolveClass() );
	}

	if ( !StylesheetClass )
	{
		FBYGRichTextModule& RichTextModule = FModuleManager::GetModuleChecked<FBYGRichTextModule>( TEXT( "BYGRichText" ) );
		RichTextStylesheet = RichTextModule.GetFallbackStylesheet();
	}
	else if ( !RichTextStylesheet && StylesheetClass )
	{
		SetRichTextStylesheet( StylesheetClass->GetDefaultObject<UBYGRichTextStylesheet>() );
	}
}

void UBYGRichTextBlock::OnRichTextStylesheetChanged()
{
	RebuildWidget();
}

#if WITH_EDITOR
const FText UBYGRichTextBlock::GetPaletteCategory()
{
	return LOCTEXT( "BYG", "Brace Yourself Games" );
}
#endif

const UBYGRichTextStylesheet* UBYGRichTextBlock::GetRichTextStylesheet() const
{
	if ( !RichTextStylesheet )
	{
		FBYGRichTextModule& RichTextModule = FModuleManager::GetModuleChecked<FBYGRichTextModule>( TEXT( "BYGRichText" ) );
		return RichTextModule.GetFallbackStylesheet();
	}

	return RichTextStylesheet;
}

#if WITH_EDITOR
void UBYGRichTextBlock::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	if ( PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED( UBYGRichTextBlock, RichTextStylesheetClass ) )
	{
		if ( RichTextStylesheetClass )
		{
			RichTextStylesheet = RichTextStylesheetClass->GetDefaultObject<UBYGRichTextStylesheet>();
			if ( RichTextStylesheet )
			{
				RichTextStylesheet->OnStylesheetPropertiesChangedDelegate.AddUniqueDynamic( this, &UBYGRichTextBlock::OnRichTextStylesheetChanged );
			}
		}
		else
		{
			FBYGRichTextModule& RichTextModule = FModuleManager::GetModuleChecked<FBYGRichTextModule>( TEXT( "BYGRichText" ) );
			RichTextStylesheet = RichTextModule.GetFallbackStylesheet();
		}
	}

	for ( TSharedPtr<SRichTextBlock>& TextBlock : MyRichTextBlocks )
	{
		TextBlock->Refresh();
	}
}
#endif

#undef LOCTEXT_NAMESPACE


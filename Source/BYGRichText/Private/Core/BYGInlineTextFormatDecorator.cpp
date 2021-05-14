// Copyright Brace Yourself Games. All Rights Reserved.

#include "Core/BYGInlineTextFormatDecorator.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"
#include "Framework/Text/SlateTextRun.h"
#include "Widget/BYGRichTextBlock.h"
#include "BYGStyleStack.h"
#include <Framework/Text/SlateImageRun.h>
#include <Fonts/FontMeasure.h>

TSharedRef< FBYGInlineTextFormatDecorator > FBYGInlineTextFormatDecorator::Create( FString InRunName, const UBYGRichTextBlock* InOwner )
{
	return MakeShareable( new FBYGInlineTextFormatDecorator( InRunName, InOwner ) );
}

FBYGInlineTextFormatDecorator::FBYGInlineTextFormatDecorator( FString InRunName, const UBYGRichTextBlock* InOwner )
	: RunName( InRunName )
	, RichTextBlockOwner( InOwner )
{

}

bool FBYGInlineTextFormatDecorator::Supports( const FTextRunParseResults& RunParseResult, const FString& Text ) const
{
	return ( RunParseResult.Name == RunName );
}

TSharedRef<ISlateRun> FBYGInlineTextFormatDecorator::Create( const TSharedRef<class FTextLayout>& TextLayout, const FTextRunParseResults& RunParseResult, const FString& OriginalText, const TSharedRef< FString >& InOutModelText, const ISlateStyle* Style )
{
	FRunInfo RunInfo( RunParseResult.Name );
	for ( const TPair<FString, FTextRange>& Pair : RunParseResult.MetaData )
	{
		RunInfo.MetaData.Add( Pair.Key, OriginalText.Mid( Pair.Value.BeginIndex, Pair.Value.EndIndex - Pair.Value.BeginIndex ) );
	}

	// Resolve IDs
	// Detect if any of the properties for this require us to create an inline widget
	bool bAnyWidgetRequiresBlockWrap = false;
	TArray<const UBYGRichTextPropertyBase*> Props;
	const FString* const IDsString = RunInfo.MetaData.Find( TEXT( "ids" ) );
	if ( ensure( IDsString ) )
	{
		TArray<FString> IDs;
		IDsString->ParseIntoArray( IDs, TEXT( " " ) );
		const UBYGRichTextStylesheet* Stylesheet = RichTextBlockOwner->GetRichTextStylesheet();
		for ( const FString& ID : IDs )
		{
			const int32 InlineID = FCString::Atoi( *ID );
			const UBYGRichTextPropertyBase* Prop = Stylesheet->FindProperty( InlineID );
			if ( ensure( Prop ) )
			{
				bAnyWidgetRequiresBlockWrap = bAnyWidgetRequiresBlockWrap || Prop->RequiresInlineTextBlock();
				Props.Add( Prop );
			}
			else
			{
				UE_LOG( LogTemp, Error, TEXT( "Failed to find property with inline ID %d / %s" ), InlineID, *ID );
			}
		}
	}

	FTextBlockStyle TextBlockStyle;
	for ( const UBYGRichTextPropertyBase* Prop : Props )
	{
		if ( Prop )
		{
			Prop->ApplyToTextStyle( TextBlockStyle );
		}
	}

	if ( bAnyWidgetRequiresBlockWrap )
	{
		TSharedPtr<SRichTextBlock> TextBlock = 
			SNew( SRichTextBlock )
			.Text( FText::FromString( OriginalText.Mid( RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex ) ) )
			.TextStyle( &TextBlockStyle );

		TSharedRef<SWidget> TextBlockRef = TextBlock.ToSharedRef();
		for ( const UBYGRichTextPropertyBase* Prop : Props )
		{
			if ( Prop->RequiresInlineTextBlock() )
			{
				UE_LOG( LogTemp, Warning, TEXT( "Wrapping once with '%s'" ), *Prop->GetName() );
				TextBlockRef = Prop->WrapBlock( TextBlockRef, nullptr, RunInfo.MetaData );
			}
		}

		//*InOutModelText += TEXT( '\u200B' ); // Zero-Width Breaking Space
		//ModelRange.EndIndex = InOutModelText->Len();

		// Calculate the baseline of the text within the owning rich text
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
		const int16 WidgetBaseline = FontMeasure->GetBaseline( TextBlockStyle.Font ) - FMath::Min( 0.0f, TextBlockStyle.ShadowOffset.Y ); // + RichTextSettings->DefaultReplacementBaselineOffset;

		FTextRange ModelRange;
		ModelRange.BeginIndex = InOutModelText->Len();
		*InOutModelText += TEXT( '\u00A0' ); // Zero-Width Breaking Space
		ModelRange.EndIndex = InOutModelText->Len();

		//return FSlateWidgetRun::Create( TextLayout, RunInfo, InOutModelText, CreateWidgetDelegate.Execute( RunInfo, Style ), ModelRange );

		return FSlateWidgetRun::Create( TextLayout, RunInfo, InOutModelText, FSlateWidgetRun::FWidgetRunInfo( TextBlockRef, WidgetBaseline ), ModelRange );
	}
	else
	{
		FTextRange ModelRange;
		ModelRange.BeginIndex = InOutModelText->Len();
		*InOutModelText += OriginalText.Mid( RunParseResult.ContentRange.BeginIndex, RunParseResult.ContentRange.EndIndex - RunParseResult.ContentRange.BeginIndex );
		ModelRange.EndIndex = InOutModelText->Len();


		return FSlateTextRun::Create( RunInfo, InOutModelText, TextBlockStyle, ModelRange );

	}
}


#if 0
	if ( RunInfo.MetaData.Contains( "run" ) && RunInfo.MetaData[ "run" ] == "img" )
	{
		const FString BrushNameString = OriginalText.Mid(BrushNameRange->BeginIndex, BrushNameRange->EndIndex - BrushNameRange->BeginIndex);
		if ( OverrideStyle != NULL )
		{
			Style = OverrideStyle;
		}

		FName BrushName( *BrushNameString );
		if ( Style->HasWidgetStyle< FInlineTextImageStyle >( BrushName ) )
		{
			const FInlineTextImageStyle& ImageStyle = Style->GetWidgetStyle< FInlineTextImageStyle >( BrushName );
			return FSlateImageRun::Create( RunInfo, InOutModelText, &ImageStyle.Image, ImageStyle.Baseline, ModelRange );
		}

		const UBYGRichTextStylesheet* Stylesheet = RichTextBlockOwner->GetRichTextStylesheet();
		Stylesheet

		return FSlateImageRun::Create( RunInfo, InOutModelText, Style->GetBrush( BrushName ), 0, ModelRange );

		return FSlateImageRun::Create( RunInfo, InOutModelText, TextMetadataToTextBlockStyle( RichTextBlockOwner, RunInfo.MetaData ), ModelRange );
	}
	if ( BrushNameRange != NULL )
	{
		const FString BrushNameString = OriginalText.Mid( BrushNameRange->BeginIndex, BrushNameRange->EndIndex - BrushNameRange->BeginIndex );
		if ( OverrideStyle != NULL )
		{
			Style = OverrideStyle;
		}

		if ( Style != nullptr )
		{
			FName BrushName( *BrushNameString );
			if ( Style->HasWidgetStyle< FInlineTextImageStyle >( BrushName ) )
			{
				const FInlineTextImageStyle& ImageStyle = Style->GetWidgetStyle< FInlineTextImageStyle >( BrushName );
				FString* HeightText = RunInfo.MetaData.Find( "Height" );
				if ( HeightText )
				{
					//int32 Height = FCString::Atoi(**HeightText);
					//int32 Width = (ImageStyle.Image.ImageSize.Y / Height) * ImageStyle.Image.ImageSize.X;
					//ImageStyle.Image.ImageSize = FVector2D(Height, Width);
				}
				return FSlateImageRun::Create( RunInfo, InOutModelText, &ImageStyle.Image, ImageStyle.Baseline, ModelRange );
			}

			return FSlateImageRun::Create( RunInfo, InOutModelText, Style->GetBrush( BrushName ), 0, ModelRange );
		}
		else
		{
			UE_LOG( LogTemp, Error, TEXT( "Text style was null again" ) );
		}
	}

	const FInlineTextImageStyle& ImageStyle = FInlineTextImageStyle::GetDefault();
	return FSlateImageRun::Create( RunInfo, InOutModelText, &ImageStyle.Image, ImageStyle.Baseline, ModelRange );
#endif

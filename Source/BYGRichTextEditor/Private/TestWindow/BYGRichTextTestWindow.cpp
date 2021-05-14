// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextTestWindow.h"

#include "BYGRichTextUIStyle.h"
#include "Widget/BYGRichTextBlock.h"
#include "Settings/BYGRichTextStylesheet.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Engine/Font.h"
#include "Interfaces/IPluginManager.h"

FString FillTestStylesheetText( UBYGRichTextStylesheet* Stylesheet, FString Payload = "" )
{
	FString SpacedPayload = Payload.IsEmpty() ? "" : " " + Payload;
	FString str;
	for ( const UBYGRichTextStyle* Style : Stylesheet->Styles )
	{
		if ( Style->GetDisplayType() == EBYGStyleDisplayType::Block )
		{
			str.Append( FString::Printf(
				TEXT( "[%s%s]Block %s Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.[/]\n" ),
				*Style->GetID().ToString(),
				*SpacedPayload,
				*Style->GetID().ToString()
			) );
		}
		else
		{
			str.Append( FString::Printf(
				TEXT( "Inline %s \"[%s%s]Lorem ipsum dolor sit amet, consectetur adipiscing elit,[/]\" sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n" ),
				*Style->GetID().ToString(),
				*Style->GetID().ToString(),
				*SpacedPayload
			) );
		}
	}
	return str;
}


SBYGRichTextTestWindow::~SBYGRichTextTestWindow()
{
}

void AddDummyProperties( const FString& Prefix, UBYGRichTextStylesheet* Stylesheet, UBYGRichTextPropertyBase* Prop, FString Payload = "" )
{
	{
		UBYGRichTextPropertyBase* InlineProp = DuplicateObject<UBYGRichTextPropertyBase>( Prop, Stylesheet );
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( Stylesheet );
		Style->SetID( FName( FString::Printf( TEXT( "%s_Inline" ), *Prefix ) ) );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->Properties.Empty();
		Style->Properties.Add( InlineProp );
		Stylesheet->AddStyle( Style );
	}
	{
		UBYGRichTextPropertyBase* BlockProp = DuplicateObject<UBYGRichTextPropertyBase>( Prop, Stylesheet );
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( Stylesheet );
		Style->SetID( FName( FString::Printf( TEXT( "%s_Block" ), *Prefix ) ) );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->Properties.Empty();
		Style->Properties.Add( BlockProp );
		Stylesheet->AddStyle( Style );
	}
}

void FillTestStylesheet( UBYGRichTextStylesheet* Stylesheet )
{
	Stylesheet->Styles.Empty();

	{
		auto* Color = NewObject<UBYGRichTextColorProperty>( Stylesheet );
		Color->SetColor( FLinearColor::Red );
		AddDummyProperties( "Red", Stylesheet, Color );
	}

	{
		auto* Typeface = NewObject<UBYGRichTextTypefaceProperty>( Stylesheet );
		// TODO this is a gross hack
		const UObject* Blah = ( const UObject* ) GEngine->GetTinyFont();
		Typeface->SetFont( Blah );
		AddDummyProperties( "SmallFont", Stylesheet, Typeface );
	}

	{
		// TODO Nest Italics within Bold
		auto* Italic = NewObject<UBYGRichTextTypeVariantProperty>( Stylesheet );
		Italic->SetName( "italic" );
		AddDummyProperties( "Italic", Stylesheet, Italic );
	}

	{
		auto* Bold = NewObject<UBYGRichTextTypeVariantProperty>( Stylesheet );
		Bold->SetName( "bold" );
		AddDummyProperties( "Bold", Stylesheet, Bold );
	}

	{
		auto* Large = NewObject<UBYGRichTextSizeProperty>( Stylesheet );
		Large->SetSize( 24 );
		AddDummyProperties( "Large", Stylesheet, Large );
	}

	{
		auto* Small = NewObject<UBYGRichTextSizeProperty>( Stylesheet );
		Small->SetSize( 6 );
		AddDummyProperties( "Small", Stylesheet, Small );
	}

	{
		auto* Case = NewObject<UBYGRichTextCaseProperty>( Stylesheet );
		Case->SetCase( ETextTransformPolicy::ToUpper );
		AddDummyProperties( "Upper", Stylesheet, Case );
	}

	{
		auto* Shadow = NewObject<UBYGRichTextShadowProperty>( Stylesheet );
		Shadow->SetColor( FLinearColor::Blue );
		Shadow->SetOffset( FIntPoint( 2, 2 ) );
		AddDummyProperties( "Shadow", Stylesheet, Shadow );
	}

	{
		auto* Margin = NewObject<UBYGRichTextMarginProperty>( Stylesheet );
		Margin->SetMargin( FMargin( 10, 30 ) );
		AddDummyProperties( "Margin", Stylesheet, Margin );
	}

	{
		auto* BackgroundColor = NewObject<UBYGRichTextBackgroundBrushProperty>( Stylesheet );
		auto BackgroundBrushBrush = FSlateImageBrush(
			"",
			FVector2D( 16.0f, 16.0f ),
			FSlateColor( FLinearColor::Blue )
		);
		BackgroundColor->SetBrush( BackgroundBrushBrush );
		AddDummyProperties( "ColorBG", Stylesheet, BackgroundColor );
	}

	{
		auto* InlineBrush = NewObject<UBYGRichTextInlineBrushProperty>( Stylesheet );
		InlineBrush->SetBrushDirectory( "/Brushes" );
		AddDummyProperties( "InlineBrush", Stylesheet, InlineBrush, "test=\"something\"" );
	}

	// Failing
	{
		auto* Justification = NewObject<UBYGRichTextJustificationProperty>( Stylesheet );
		Justification->SetJustification( ETextJustify::Right );
		AddDummyProperties( "Justify", Stylesheet, Justification );
	}

	// Line height doesn't seem to be affected
	{
		auto* LineHeight = NewObject<UBYGRichTextLineHeightPercentProperty>( Stylesheet );
		LineHeight->SetLineHeight( 1.8f );
		AddDummyProperties( "LineHeight", Stylesheet, LineHeight );
	}

	// Doesn't seem to line wrap
	{
		auto* LineWrap = NewObject<UBYGRichTextLineWrapProperty>( Stylesheet );
		LineWrap->SetAutoWrap( false );
		AddDummyProperties( "LineWrap", Stylesheet, LineWrap );
	}

	{
		auto* BackgroundBrush = NewObject<UBYGRichTextBackgroundBrushProperty>( Stylesheet );
		auto TestBrush = FSlateImageBrush(
			IPluginManager::Get().FindPlugin( TEXT( "BYGRichText" ) )->GetContentDir() / "Icons/BYGRichTextBlock_64x.png",
			FVector2D( 32.0f, 32.0f )
		);
		BackgroundBrush->SetBrush( TestBrush );
		AddDummyProperties( "BrushBG", Stylesheet, BackgroundBrush );
	}

#if 0
// Disabled until I implement a custom Text Marshaller
	auto* Strike = NewObject<UBYGRichTextStrikeProperty>( Stylesheet );
	auto StrikeBrush = FSlateImageBrush(
		FPaths::EngineContentDir() / TEXT( "Slate / Checkerboard.png" ),
		FVector2D( 16.0f, 16.0f ),
		FSlateColor( FLinearColor::Green )
	);
	Strike->SetBrush( StrikeBrush );
	AddDummyProperties( Stylesheet, Strike );

	auto* Highlight = NewObject<UBYGRichTextHighlightProperty>( Stylesheet );
	auto HighlightBrush = FSlateImageBrush(
		FPaths::EngineContentDir() / TEXT( "Slate / Checkerboard.png" ),
		FVector2D( 16.0f, 16.0f ),
		FSlateColor( FLinearColor::Red )
	);
	Highlight->SetBrush( HighlightBrush );
	AddDummyProperties( Stylesheet, Highlight );

	auto* Underline = NewObject<UBYGRichTextUnderlineProperty>( Stylesheet );
	auto UnderlineBrush = FSlateImageBrush(
		FPaths::EngineContentDir() / TEXT( "Slate / Checkerboard.png" ),
		FVector2D( 16.0f, 16.0f ),
		FSlateColor( FLinearColor::Blue )
	);
	Underline->SetBrush( UnderlineBrush );
	AddDummyProperties( Stylesheet, Underline );
#endif
}

void SBYGRichTextTestWindow::Construct( const FArguments& InArgs )
{
	// Yuck, this can't be right, can it?
	RichTextBlock = NewObject<UBYGRichTextBlock>();
	Stylesheet = NewObject<UBYGRichTextStylesheet>();

	FillTestStylesheet( Stylesheet );
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( Stylesheet );
		Style->SetID( FName("default") );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		// Set up defaults
		{
			UBYGRichTextColorProperty* pTextColor = NewObject<UBYGRichTextColorProperty>( Style );
			Style->Properties.Add( pTextColor );

			UBYGRichTextSizeProperty* pTextSize = NewObject<UBYGRichTextSizeProperty>( Style );
			Style->Properties.Add( pTextSize );

			auto* Typeface = NewObject<UBYGRichTextTypefaceProperty>( Stylesheet );
			// TODO this is a gross hack
			const UObject* Blah = ( const UObject* ) GEngine->GetMediumFont();
			Typeface->SetFont( Blah );
			Style->Properties.Add(Typeface);
		}
		Stylesheet->AddStyle( Style );
		Stylesheet->SetDefaultStyleName( "default" );
	}

	const FString Text = FillTestStylesheetText( Stylesheet );
	RichTextBlock->SetRichTextStylesheet( Stylesheet );
	RichTextBlock->SetText( FText::FromString( Text ) );
	RichTextBlock->Modify( true );
	
	ChildSlot
	[
		SNew( SBorder )
		.Padding( 3 )
		.BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
		[
			SNew( SScrollBox )
			.Orientation( Orient_Vertical )
			+ SScrollBox::Slot()
			[
				RichTextBlock->TakeWidget()
			]
		]
	];
}

void SBYGRichTextTestWindow::AddReferencedObjects( FReferenceCollector& Collector )
{
	Collector.AddReferencedObject( RichTextBlock );
	Collector.AddReferencedObject( Stylesheet );
}

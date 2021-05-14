// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextUIStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "EditorStyleSet.h"
#include "Interfaces/IPluginManager.h"
#include "SlateOptMacros.h"



#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)


TSharedPtr< FSlateStyleSet > FBYGRichTextUIStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FBYGRichTextUIStyle::Get() { return StyleSet; }

FName FBYGRichTextUIStyle::GetStyleSetName()
{
	static FName BYGRichTextStyleName( TEXT( "BYGRichTextUIStyle" ) );
	return BYGRichTextStyleName;
}

void FBYGRichTextUIStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8( 8.0f, 8.0f );
	const FVector2D Icon16x16( 16.0f, 16.0f );
	const FVector2D Icon20x20( 20.0f, 20.0f );
	const FVector2D Icon40x40( 40.0f, 40.0f );
	const FVector2D Icon64x64( 64.0f, 64.0f );
	const FVector2D Spacer32x4( 32, 1 );

	// Only register once
	if ( StyleSet.IsValid() )
	{
		return;
	}

	StyleSet = MakeShareable( new FSlateStyleSet( GetStyleSetName() ) );

	StyleSet->SetContentRoot( IPluginManager::Get().FindPlugin( TEXT( "BYGRichText" ) )->GetContentDir() );
	StyleSet->SetCoreContentRoot( FPaths::EngineContentDir() / TEXT( "Slate" ) );

	// BYGRichTextBlock
	StyleSet->Set( "BYGRichText.TabIcon", new IMAGE_BRUSH( "Icons/BYGRichTextBlock_16x", Icon16x16 ) );

	StyleSet->Set( "ClassIcon.BYGRichTextBlock", new IMAGE_BRUSH( "Icons/BYGRichTextBlock_16x", Icon16x16 ) );
	StyleSet->Set( "ClassThumbnail.BYGRichTextBlock", new IMAGE_BRUSH( "Icons/BYGRichTextBlock_64x", Icon64x64 ) );

	StyleSet->Set( "ClassIcon.BYGRichTextStylesheet", new IMAGE_BRUSH( "Icons/BYGRichTextStylesheet_16x", Icon16x16 ) );
	StyleSet->Set( "ClassThumbnail.BYGRichTextStylesheet", new IMAGE_BRUSH( "Icons/BYGRichTextStylesheet_64x", Icon64x64 ) );

	StyleSet->Set( "ClassIcon.BYGRichTextPropertyBase", new IMAGE_BRUSH( "Icons/BYGRichTextPropertyBase_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextBackgroundBrushProperty", new IMAGE_BRUSH( "Icons/BYGRichTextBackgroundBrushPRoperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextCaseProperty", new IMAGE_BRUSH( "Icons/BYGRichTextCaseProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextColorProperty", new IMAGE_BRUSH( "Icons/BYGRichTextColorProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextHighlightProperty", new IMAGE_BRUSH( "Icons/BYGRichTextHighlightProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextJustificationProperty", new IMAGE_BRUSH( "Icons/BYGRichTextJustificationProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextLineHeightPercentProperty", new IMAGE_BRUSH( "Icons/BYGRichTextLineHeightPercentProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextLineWrapProperty", new IMAGE_BRUSH( "Icons/BYGRichTextLineWrapProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextMarginProperty", new IMAGE_BRUSH( "Icons/BYGRichTextMarginProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextShadowProperty", new IMAGE_BRUSH( "Icons/BYGRichTextShadowProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextSizeProperty", new IMAGE_BRUSH( "Icons/BYGRichTextSizeProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextStrikeProperty", new IMAGE_BRUSH( "Icons/BYGRichTextStrikeProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextTooltipProperty", new IMAGE_BRUSH( "Icons/BYGRichTextTooltipProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextTypeVariantProperty", new IMAGE_BRUSH( "Icons/BYGRichTextTypeVariantProperty_16x", Icon16x16 ) );
	StyleSet->Set( "ClassIcon.BYGRichTextTypefaceProperty", new IMAGE_BRUSH( "Icons/BYGRichTextTypefaceProperty_16x", Icon16x16 ) );

	StyleSet->Set( "BYGStyleDisplayType_Block", new IMAGE_BRUSH( "Icons/BYGStyleDisplayType_Block_20x", Icon20x20 ) );
	StyleSet->Set( "BYGStyleDisplayType_Inline", new IMAGE_BRUSH( "Icons/BYGStyleDisplayType_Inline_20x", Icon20x20 ) );
	StyleSet->Set( "BYGTextTransform_Lower", new IMAGE_BRUSH( "Icons/BYGTextTransform_Lower_20x", Icon20x20 ) );
	StyleSet->Set( "BYGTextTransform_Upper", new IMAGE_BRUSH( "Icons/BYGTextTransform_Upper_20x", Icon20x20 ) );

	StyleSet->Set( "PropertySpacer", new IMAGE_BRUSH( "PropertySpacer", Spacer32x4, FLinearColor( 1, 1, 1, 0.2f ) ) );

	const FSlateFontInfo NormalFont = DEFAULT_FONT( "Regular", FCoreStyle::RegularTextSize );


	const FTextBlockStyle NormalText = FEditorStyle::GetWidgetStyle<FTextBlockStyle>( "NormalText" );
	#if 0
	NormalText = FTextBlockStyle()
		.SetFont( DEFAULT_FONT( "Regular", FCoreStyle::RegularTextSize ) )
		.SetColorAndOpacity( FSlateColor::UseForeground() )
		.SetShadowOffset( FVector2D::ZeroVector )
		.SetShadowColorAndOpacity( FLinearColor::Black )
		.SetHighlightColor( FLinearColor( 0.02f, 0.3f, 0.0f ) )
		.SetHighlightShape( BOX_BRUSH( "Common/TextBlockHighlightShape", FMargin( 3.f / 8.f ) ) );
		#endif

	StyleSet->Set( "RichTextStyle.Title", FTextBlockStyle( NormalText )
		.SetFont( DEFAULT_FONT( "Bold", FCoreStyle::RegularTextSize ) )
		.SetFontSize( 10 )
		.SetColorAndOpacity( FLinearColor( 1.0f, 1.0f, 1.0f ) )
		.SetHighlightColor( FLinearColor( 1.0f, 1.0f, 1.0f ) )
		.SetShadowOffset( FVector2D( 1, 1 ) )
		.SetShadowColorAndOpacity( FLinearColor( 0, 0, 0, 0.9f ) ) );

	StyleSet->Set( "RichTextProperty.Title", FTextBlockStyle( NormalText )
		.SetFont( DEFAULT_FONT( "Bold", FCoreStyle::RegularTextSize ) )
		.SetFontSize( 10 )
		.SetColorAndOpacity( FLinearColor( 1.0f, 1.0f, 1.0f ) )
		.SetHighlightColor( FLinearColor( 1.0f, 1.0f, 1.0f ) )
		.SetShadowOffset( FVector2D( 1, 1 ) )
		.SetShadowColorAndOpacity( FLinearColor( 0, 0, 0, 0.9f ) ) );


	FSlateStyleRegistry::RegisterSlateStyle( *StyleSet.Get() );
};

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef DEFAULT_FONT

void FBYGRichTextUIStyle::Shutdown()
{
	if ( StyleSet.IsValid() )
	{
		FSlateStyleRegistry::UnRegisterSlateStyle( *StyleSet.Get() );
		ensure( StyleSet.IsUnique() );
		StyleSet.Reset();
	}
}

const FSlateBrush* FBYGRichTextUIStyle::GetBrush( FName PropertyName, const ANSICHAR* Specifier )
{
	return StyleSet->GetBrush( PropertyName, Specifier );
}

// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextModule.h"
#include "Brushes/SlateImageBrush.h"
#include "Brushes/SlateNoResource.h"
#include "Styling/SlateStyle.h"
#include "Settings/BYGRichTextStylesheet.h"
#include "Settings/BYGRichTextStyle.h"
#include "Settings/BYGRichTextProperty.h"

#define LOCTEXT_NAMESPACE "BYGRichTextModule"

void FBYGRichTextModule::StartupModule()
{
	NullIcon = FSlateNoResource();

	SlateStyleSet = MakeShareable( new FSlateStyleSet( TEXT( "BYGRichTextStyle" ) ) );

	FCoreDelegates::OnPostEngineInit.AddRaw( this, &FBYGRichTextModule::OnPostEngineInit );
}


void FBYGRichTextModule::ShutdownModule()
{
	InlineIconsCache.Empty();
	IconTextures.Empty();

	SlateStyleSet.Reset();

	FallbackStylesheet = nullptr;
}

const FSlateBrush* FBYGRichTextModule::GetIconBrush( const FString& Path, const FVector2D& MaxSize )
{
	// Can have multiple instances of the same texture/brush, but rendered at different sizes
	// We need to create a brush asset for every time that the brush is used for a particular size, and maintain it

	const FString CacheKey = FString::Printf( TEXT( "%s-%dx%d" ), *Path, MaxSize.X, MaxSize.Y );
	if ( !InlineIconsCache.Contains( CacheKey ) )
	{
		FSoftObjectPath ObjectPath = FSoftObjectPath( Path );
		// TODO support loading brushes?
		UTexture2D* IconTexture = Cast<UTexture2D>( ObjectPath.TryLoad() );
		if ( IconTexture != nullptr )
		{
			IconTextures.AddUnique( IconTexture );
			InlineIconsCache.Add( CacheKey, FSlateImageBrush( IconTexture, MaxSize ) );
		}
		else
		{
			UE_LOG( LogTemp, Warning, TEXT( "Could not load texture at path %s" ), *Path );
		}
	}
	// Could still miss the cache
	if ( InlineIconsCache.Contains( CacheKey ) )
	{
		return &InlineIconsCache[ CacheKey ];
	}

	return &NullIcon;
}

void FBYGRichTextModule::OnPostEngineInit()
{
	FallbackStylesheet = NewObject<UBYGRichTextStylesheet>( ( UObject* )GetTransientPackage(), FName( "FallbackStylesheet" ) );
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( FallbackStylesheet );
		Style->SetID( "error" );
		{
			UBYGRichTextTypefaceProperty* Typeface = NewObject<UBYGRichTextTypefaceProperty>( Style );
			// TODO this is a gross hack
			const UObject* FontObject = ( const UObject* ) GEngine->GetSmallFont();
			Typeface->SetFont( FontObject );
			Style->Properties.Add( Typeface );
		}
		{
			UBYGRichTextSizeProperty* TextSize = NewObject<UBYGRichTextSizeProperty>( Style );
			TextSize->SetSize( 16 );
			Style->Properties.Add( TextSize );
		}
		{
			// Magenta for delicious errors
			UBYGRichTextColorProperty* Color = NewObject<UBYGRichTextColorProperty>( Style );
			Color->SetColor( FLinearColor( 1.0f, 0.0f, 1.0f, 1.0f ) );
			Style->Properties.Add( Color );
		}
		FallbackStylesheet->AddStyle( Style );
		FallbackStylesheet->SetDefaultStyleName( "error" );
	}
}

void FBYGRichTextModule::AddReferencedObjects( FReferenceCollector& Collector )
{
	Collector.AddReferencedObject( FallbackStylesheet );
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE( FBYGRichTextModule, BYGRichText )



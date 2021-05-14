// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextStylesheetTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#include "Widget/BYGRichTextBlock.h"
#include <Framework/Text/ITextDecorator.h>
#include "Settings/BYGRichTextStylesheet.h"
#include <Tests/AutomationEditorCommon.h>
#include <FunctionalTestBase.h>

static const int StylesheetTestFlags = (
	EAutomationTestFlags::EditorContext
	| EAutomationTestFlags::CommandletContext
	| EAutomationTestFlags::ClientContext
	| EAutomationTestFlags::ProductFilter );


FBYGRichTextStylesheetTestBase::FBYGRichTextStylesheetTestBase( const FString& InName, const bool bInComplexTask )
	: FFunctionalTestBase( InName, bInComplexTask )
{
}

void FBYGRichTextStylesheetTestBase::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	for ( const auto& Pair : TestData )
	{
		OutBeautifiedNames.Add( Pair.Key );
		OutTestCommands.Add( Pair.Key );
	}
}

bool FBYGRichTextStylesheetTestBase::RunTest( const FString& Parameters )
{
#if 0
	const auto& TestDatum = TestData[ Parameters ];

	// Better to create this here every time than store centrally and have issues where the
	// default properties cannot be found
	UBYGRichTextStylesheet* DefaultStylesheet = NewObject<UBYGRichTextStylesheet>();
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "default" );
		DefaultStylesheet->AddStyle( Style );
		DefaultStylesheet->SetDefaultStyleName( "default" );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "strong" );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->SetShortcut( "*" );
		UBYGRichTextCaseProperty* Case = NewObject<UBYGRichTextCaseProperty>();
		Case->SetCase( ETextTransformPolicy::ToUpper );
		Style->Properties.Add( Case );
		DefaultStylesheet->AddStyle( Style );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "h1" );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "#" );
		DefaultStylesheet->AddStyle( Style );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "h2" );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "##" );
		DefaultStylesheet->AddStyle( Style );
	}

	// Ensure that all IDs are unique
	TArray<int32> SeenIDs;
	for ( UBYGRichTextStyle* Style : DefaultStylesheet->Styles )
	{
		for ( UBYGRichTextPropertyBase* Property : Style->Properties )
		{
			const int32 NewID = Property->InlineID;
			TestFalse( "Should not contain", SeenIDs.Contains( NewID ) );
			SeenIDs.Add( NewID);
		}
	}

	//TestEqual( TestDatum.Input + " block count", TextBlocks.Num(), TestDatum.ExpectedTextBlockInfo.Num() );
	#endif

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST( FBYGRichTextStylesheetIDs, "BYG.RichText.StylesheetIDs", StylesheetTestFlags )
bool FBYGRichTextStylesheetIDs::RunTest( const FString& Parameters )
{
	UBYGRichTextStylesheet* DefaultStylesheet = NewObject<UBYGRichTextStylesheet>();
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "default" );
		{
			UBYGRichTextColorProperty* Color = NewObject<UBYGRichTextColorProperty>();
			Color->SetColor( FLinearColor::Green );
			Style->Properties.Add( Color );
		}
		{
			UBYGRichTextJustificationProperty* Justification = NewObject<UBYGRichTextJustificationProperty>();
			Justification->SetJustification( ETextJustify::Center );
			Style->Properties.Add( Justification );
		}
		DefaultStylesheet->AddStyle( Style );
		DefaultStylesheet->SetDefaultStyleName( "default" );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "strong" );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->SetShortcut( "*" );
		{
			UBYGRichTextCaseProperty* Case = NewObject<UBYGRichTextCaseProperty>();
			Case->SetCase( ETextTransformPolicy::ToUpper );
			Style->Properties.Add( Case );
		}
		DefaultStylesheet->AddStyle( Style );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "h1" );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "#" );
		{
			UBYGRichTextCaseProperty* Case = NewObject<UBYGRichTextCaseProperty>();
			Case->SetCase( ETextTransformPolicy::ToLower );
			Style->Properties.Add( Case );
		}
		DefaultStylesheet->AddStyle( Style );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "h2" );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "##" );
		{
			UBYGRichTextCaseProperty* Case = NewObject<UBYGRichTextCaseProperty>();
			Case->SetCase( ETextTransformPolicy::ToLower );
			Style->Properties.Add( Case );
		}
		DefaultStylesheet->AddStyle( Style );
	}

	TestTrue( "Should have some styles", DefaultStylesheet->Styles.Num() > 0 );
	// Ensure that all IDs are unique
	TArray<int32> SeenIDs;
	for ( UBYGRichTextStyle* Style : DefaultStylesheet->Styles )
	{
		TestTrue( "Each style should have some properties", Style->Properties.Num() > 0 );
		for ( UBYGRichTextPropertyBase* Property : Style->Properties )
		{
			const int32 NewID = Property->InlineID;
			TestFalse( "Should not contain", SeenIDs.Contains( NewID ) );
			SeenIDs.Add( NewID );
		}
	}
	TestTrue( "Should have expected number of unique IDs", SeenIDs.Num() == 4 );

	return true;
}

IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST( FBYGRichTextStylesheetTest, FBYGRichTextStylesheetTestBase, "BYG.RichText.StylesheetIDs", StylesheetTestFlags )
void FBYGRichTextStylesheetTest::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	FBYGRichTextStylesheetTestBase::GetTests( OutBeautifiedNames, OutTestCommands );
}

bool FBYGRichTextStylesheetTest::RunTest( const FString& Parameters )
{
	return FBYGRichTextStylesheetTestBase::RunTest( Parameters );
}


#if 0
void UBYGRichTextStylesheet::FillWithMarkdown()
{
	Styles.Empty();

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID( "default" );
		Styles.Add( Style );
	}

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID( "h1" );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "#" );
		Style->Properties.Empty();

		UBYGRichTextSizeProperty* Size = NewObject<UBYGRichTextSizeProperty>( this );
		Size->SetSize( 32 );
		Style->Properties.Add( Size );

		UBYGRichTextTypeVariantProperty* Variant = NewObject<UBYGRichTextTypeVariantProperty>( this );
		Variant->SetName( "bold" );
		Style->Properties.Add( Variant );

		Styles.Add( Style );
	}

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID("h2");
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "##" );
		Style->Properties.Empty();

		UBYGRichTextSizeProperty* Size = NewObject<UBYGRichTextSizeProperty>( this );
		Size->SetSize( 18 );
		Style->Properties.Add( Size );

		UBYGRichTextTypeVariantProperty* Variant = NewObject<UBYGRichTextTypeVariantProperty>(this);
		Variant->SetName( "bold" );
		Style->Properties.Add( Variant );

		Styles.Add( Style );
	}

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID("h3");
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( "###" );
		Style->Properties.Empty();

		UBYGRichTextSizeProperty* Size = NewObject<UBYGRichTextSizeProperty>( this );
		Size->SetSize( 14 );
		Style->Properties.Add( Size );

		UBYGRichTextTypeVariantProperty* Variant = NewObject<UBYGRichTextTypeVariantProperty>( this );
		Variant->SetName( "bold" );
		Style->Properties.Add( Variant );

		Styles.Add( Style );
	}

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID( "strong" );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->SetShortcut( "*" );
		Style->Properties.Empty();

		UBYGRichTextTypeVariantProperty* Variant = NewObject<UBYGRichTextTypeVariantProperty>( this );
		Variant->SetName( "bold" );
		Style->Properties.Add( Variant );

		Styles.Add( Style );
	}

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID( "em" );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->SetShortcut( "_" );
		Style->Properties.Empty();

		UBYGRichTextTypeVariantProperty* Variant = NewObject<UBYGRichTextTypeVariantProperty>( this );
		Variant->SetName( "italic" );
		Style->Properties.Add( Variant );

		Styles.Add( Style );
	}

	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( this );
		Style->SetID( "quote" );
		Style->SetDisplayType( EBYGStyleDisplayType::Block );
		Style->SetShortcut( ">" );
		Style->Properties.Empty();

		UBYGRichTextMarginProperty* Margin = NewObject<UBYGRichTextMarginProperty>( this );
		Margin->SetMargin( FMargin( 10 ) );
		Style->Properties.Add( Margin );

		Styles.Add( Style );
	}
}
#endif

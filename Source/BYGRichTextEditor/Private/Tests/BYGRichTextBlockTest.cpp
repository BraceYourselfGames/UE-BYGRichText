// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextBlockTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#include "Widget/BYGRichTextBlock.h"
#include <Framework/Text/ITextDecorator.h>
#include "Settings/BYGRichTextStylesheet.h"
#include <Tests/AutomationEditorCommon.h>
#include <FunctionalTestBase.h>

static const int BlockTestFlags = (
	EAutomationTestFlags::EditorContext
	| EAutomationTestFlags::CommandletContext
	| EAutomationTestFlags::ClientContext
	| EAutomationTestFlags::ProductFilter );


FBYGRichTextBlockTestBase::FBYGRichTextBlockTestBase( const FString& InName, const bool bInComplexTask )
	: FFunctionalTestBase( InName, bInComplexTask )
{
	TestData = {
		{ "No formatting", {
			"No formatting",
			"Hello World",
			{ FBYGTextBlockInfo( "Hello World", { "default" }, {{}} ) }
		} },
		{ "Single line break", {
			"Single line break",
			"Hello World\nNew Line",
			{ 
				FBYGTextBlockInfo( "Hello World\nNew line", { "default" }, {{}} )
			}
		} },
		{ "Double line break", {
			"Double line break",
			"Hello World\r\n\r\nNew Line",
			{ 
				FBYGTextBlockInfo( "Hello World", { "default" }, {{}} ),
				FBYGTextBlockInfo( "New Line", { "default" }, {{}} )
			}
		} },
		{ "Many line breaks should be collapsed into a single paragraph break", {
			"Many line breaks should be collapsed into a single paragraph break",
			"Hello World\r\n\r\n\r\n\r\n\r\nNew Line",
			{ 
				FBYGTextBlockInfo( "Hello World", { "default" }, {{}} ),
				FBYGTextBlockInfo( "New Line", { "default" }, {{}} )
			}
		} },
		{ "Block styles on same line", {
			"Block styles on same line",
			"[h1]First[/][h1]Second[/]",
			{ 
				FBYGTextBlockInfo( "[h1]First[/]", { "default", "h1" }, {{}} ),
				FBYGTextBlockInfo( "[h1]Second[/]", { "default", "h1" }, {{}} )
			}
		} },
		{ "Block with shortcut", {
			"Block with shortcut",
			"This is some text\r\n#First header",
			{ 
				FBYGTextBlockInfo( "This is some text", { "default" }, {{}} ),
				FBYGTextBlockInfo( "#First header", { "default", "h1" }, {{}} )
			}
		} },
		{ "Block with shortcut with spaces", {
			"Block with shortcut with spaces",
			"This is some text\r\n# First header",
			{ 
				FBYGTextBlockInfo( "This is some text", { "default" }, {{}} ),
				FBYGTextBlockInfo( "# First header", { "default", "h1" }, {{}} )
			}
		} },
		{ "Block with shortcut not at the start of a line", {
			"Block with shortcut not at the start of a line",
			"This is some text # shouldn't be a header",
			{ 
				FBYGTextBlockInfo( "This is some text # shouldn't be a header", { "default" }, {} )
			}
		} },
		{ "Block with shortcut at the start of a line with whitespace", {
			"Block with shortcut at the start of a line with whitespace",
			"This is some text\r\n # Should Be A Header",
			{ 
				FBYGTextBlockInfo( "This is some text", { "default" }, {} ),
				FBYGTextBlockInfo( "Should Be A Header", { "default", "h1" }, {} )
			}
		} },
		{ "Differentiate between IDs that are substrings of the other", {
			"Differentiate between IDs that are substrings of the other",
			"This is some text\r\n## Should be h2\r\n# Should be h1",
			{ 
				FBYGTextBlockInfo( "This is some text", { "default" }, {} ),
				FBYGTextBlockInfo( "## Should be h2", { "default", "h2" }, {} ),
				FBYGTextBlockInfo( "# Should be h1", { "default", "h1" }, {} )
			}
		} },
		{ "Non-block style with payload", {
			"Non-block style with payload",
			"[default hello:world]Hello World[/]",
			{ 
				FBYGTextBlockInfo( "[default hello:world]Hello World[/]", { "default" }, {} )
			}
		} },
		{ "Block style with payload", {
			"Block style with payload",
			"[h1 hello:world]Hello World[/]",
			{ 
				FBYGTextBlockInfo( "[h1 hello:world]Hello World[/]", { "default", "h1" }, {{ "hello", "world" }} )
			}
		} },
		{ "Non-block style with payload on one line", {
			"Non-block style with payload on one line",
			"[default hello:world]Hello[/] world",
			{ 
				FBYGTextBlockInfo( "[default hello:world]Hello[/] world", { "default" }, {} )
			}
		} },
		{ "Block style with non-block style inside", {
			"Block style with non-block style inside",
			"[h1]This is [strong]strong[/][/]",
			{ 
				FBYGTextBlockInfo( "[h1]This is [strong]strong[/][/]", { "default", "h1" }, {} )
			}
		} },
		{ "Block style with block style inside", {
			"Block style with block style inside",
			"[h1]This is [h2]weird[/] now[/]",
			{ 
				FBYGTextBlockInfo( "[h1]This is", { "default", "h1" }, {} ),
				FBYGTextBlockInfo( "[h2]weird[/]", { "default", "h2" }, {} ),
				FBYGTextBlockInfo( "now[/]", { "default" /*, "h1" */ }, {} ) // Should this be default and h1 styles?
			}
		} },
		{ "Block style with one newline inside", {
			"Block style with one newline inside",
			"[h1]This is \r\nsplit over lines[/]\r\nAnd then another",
			{ 
				FBYGTextBlockInfo( "[h1]This is \r\nsplit over lines[/]", { "default", "h1" }, {} ),
				FBYGTextBlockInfo( "And then another", { "default"}, {} )
			}
		} },
		{ "Block style with two newlines inside", {
			"Block style with two newlines inside",
			"[h1]This is \r\n\r\nsplit over lines[/]\r\nAnd then another",
			{ 
				FBYGTextBlockInfo( "[h1]This is", { "default", "h1" }, {} ),
				FBYGTextBlockInfo( "split over lines[/]", { "default" }, {} ),
				FBYGTextBlockInfo( "And then another", { "default"}, {} )
			}
		} },
		{ "Block style with payload on one line", {
			"Block style with payload on one line",
			"[h1 hello:world]Hello[/] world",
			{ 
				FBYGTextBlockInfo( "[h1 hello:world]Hello[/]", { "default", "h1" }, {{ "hello", "world" }} ),
				FBYGTextBlockInfo( "world", { "default" }, {} )
			}
		} },
		{ "Non-block style with payload on two lines", {
			"Non-block style with payload on two lines",
			"[default hello:world]Hello[/]\r\n\r\nworld",
			{ 
				FBYGTextBlockInfo( "[default hello:world]Hello[/]", { "default" }, {} ),
				FBYGTextBlockInfo( "world", { "default" }, {} )
			}
		} },
		{ "Escaped block style at the start of a line", {
			"Escaped block style at the start of a line",
			"\\[h1\\]This should just be default[/]",
			{ 
				FBYGTextBlockInfo( "\\[h1\\]This should just be default[/]", { "default" }, {} ),
			}
		} },
		{ "Escaped block style shortcut at the start of a line", {
			"Escaped block style shortcut at the start of a line",
			"\\#This should just be default",
			{ 
				FBYGTextBlockInfo( "\\#This should just be default", { "default" }, {} ),
			}
		} },
		{ "Escaped block style between lines a line", {
			"Escaped block style between lines a line",
			"This should just be default\r\n\r\n\\[h1\\]this too[/]",
			{ 
				FBYGTextBlockInfo( "This should just be default", { "default" }, {} ),
				FBYGTextBlockInfo( "\\[h1\\]this too[/]", { "default" }, {} ),
			}
		} },
	};
}

void FBYGRichTextBlockTestBase::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	for ( const auto& Pair : TestData )
	{
		OutBeautifiedNames.Add( Pair.Key );
		OutTestCommands.Add( Pair.Key );
	}
}

bool FBYGRichTextBlockTestBase::RunTest( const FString& Parameters )
{
	const auto& TestDatum = TestData[ Parameters ];

	// Better to create this here every time than store centrally and have issues where the
	// default properties cannot be found
	UBYGRichTextStylesheet* DefaultStylesheet = NewObject<UBYGRichTextStylesheet>();
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "default" );
		DefaultStylesheet->AddStyle( Style );
		DefaultStylesheet->SetDefaultStyleName("default");
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "strong" );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->SetShortcut( "*" );
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

	UBYGRichTextBlock* Block = NewObject<UBYGRichTextBlock>();
	Block->SetRichTextStylesheet( DefaultStylesheet );

	TSharedRef<FBYGRichTextMarkupParser> Parser = FBYGRichTextMarkupParser::Create( Block, "s" );
	TArray<FBYGTextBlockInfo> TextBlocks = Parser.Get().SplitIntoBlocks( TestDatum.Input );
	FString None = "None";

	TestEqual( TestDatum.Input + " block count", TextBlocks.Num(), TestDatum.ExpectedTextBlockInfo.Num() );
	for ( int32 i = 0; i < FMath::Max<int32>( TextBlocks.Num(), TestDatum.ExpectedTextBlockInfo.Num() ); ++i )
	{
		const FString Output = TextBlocks.IsValidIndex( i ) ? TextBlocks[ i ].RawText : None;
		const FString Expected = TestDatum.ExpectedTextBlockInfo.IsValidIndex( i ) ? TestDatum.ExpectedTextBlockInfo[ i ].RawText : None;
		const int32 OutputCount = TextBlocks.IsValidIndex( i ) ? TextBlocks[ i ].StylesApplied.Num() : INDEX_NONE;
		const int32 ExpectedCount = TestDatum.ExpectedTextBlockInfo.IsValidIndex( i ) ? TestDatum.ExpectedTextBlockInfo[ i ].StylesApplied.Num() : INDEX_NONE;

		TestEqual( FString::Printf( TEXT( "%s, block #%d, raw text" ), *TestDatum.Input, i ), Output, Expected );
		TestEqual( FString::Printf( TEXT( "%s, block #%d, style count" ), *TestDatum.Input, i ), OutputCount, ExpectedCount );

		for ( int32 j = 0; j < FMath::Max<int32>( OutputCount, ExpectedCount ); ++j )
		{
			FString OutputStyle = TextBlocks.IsValidIndex( i ) && TextBlocks[ i ].StylesApplied.IsValidIndex( j ) ? TextBlocks[ i ].StylesApplied[ j ].ToString() : None;
			FString ExpectedStyle = TestDatum.ExpectedTextBlockInfo.IsValidIndex( i ) && TestDatum.ExpectedTextBlockInfo[ i ].StylesApplied.IsValidIndex( j ) ? TestDatum.ExpectedTextBlockInfo[ i ].StylesApplied[ j ].ToString() : None;

			TestEqual( FString::Printf( TEXT( "%s, block #%d, style #%d" ), *TestDatum.Input, i, j ),
				OutputStyle,
				ExpectedStyle );
		}
	}

	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST( FBYGRichTextBlockTest, FBYGRichTextBlockTestBase, "BYG.RichText.Block", BlockTestFlags )
void FBYGRichTextBlockTest::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	FBYGRichTextBlockTestBase::GetTests( OutBeautifiedNames, OutTestCommands );
}

bool FBYGRichTextBlockTest::RunTest( const FString& Parameters )
{
	return FBYGRichTextBlockTestBase::RunTest( Parameters );
}


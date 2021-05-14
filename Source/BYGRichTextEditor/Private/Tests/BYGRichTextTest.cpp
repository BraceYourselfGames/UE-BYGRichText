// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGRichTextTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#include "Widget/BYGRichTextBlock.h"
#include <Framework/Text/ITextDecorator.h>
#include "Settings/BYGRichTextStylesheet.h"
#include <Tests/AutomationEditorCommon.h>
#include <FunctionalTestBase.h>

static const int TestFlags = (
	EAutomationTestFlags::EditorContext
	| EAutomationTestFlags::CommandletContext
	| EAutomationTestFlags::ClientContext
	| EAutomationTestFlags::ProductFilter );


FBYGRichTextParseTestBase::FBYGRichTextParseTestBase( const FString& InName, const bool bInComplexTask )
	: FFunctionalTestBase( InName, bInComplexTask )
{
	TestData = {
		{ "No formatting", {
			"Hello World",
			"<s ids=\"0 1 2 3 4 5\">Hello World</>",
		} },
		{ "Inline shortcut tag", {
			"Hello *World*",
			"<s ids=\"0 1 2 3 4 5\">Hello </><s ids=\"[0-9 ]+\">World</>",
		} },
		{ "Inline tags", {
			"Hello [strong]World[/]",
			"<s ids=\"[0-9 ]+\">Hello </><s ids=\"[0-9 ]+\">World</>",
		} },
		{ "Escape characters", {
			"Hello \\[style\\]World",
			"<s ids=\"[0-9 ]+\">Hello \\[style\\]World</>",
		} },
		{ "End tab without starting", {
			"Hello[/] World",
			"<s ids=\"[0-9 ]+\">Hello World</>",
		} },
		{ "Tag with spaces", {
			"[ strong ]Hello[/] World",
			"<s ids=\"[0-9 ]+\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with start space", {
			"[ strong]Hello[/] World",
			"<s ids=\"[0-9 ]+\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "End tag with end space", {
			"[strong]Hello[/ ] World",
			"<s ids=\"[0-9 ]+\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "End tag with more spaces", {
			"[strong]Hello[ / ] World",
			"<s ids=\"[0-9 ]+\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with mismatching case", {
			"[Strong]Hello[/] World",
			"<s ids=\"[0-9 ]+\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with payload", {
			"[strong img:cool]Hello[/] World",
			"<s ids=\"[0-9 ]+\" img=\"cool\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with payload and quotes", {
			"[strong mykey:\"great stuff\"]Hello[/] World",
			"<s ids=\"[0-9 ]+\" mykey=\"great stuff\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with payload and spaces at the end", {
			"[strong key:val ]Hello[/] World",
			"<s ids=\"[0-9 ]+\" key=\"val\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with payload with only key", {
			"[strong key]Hello[/] World",
			"<s ids=\"[0-9 ]+\" key=\"\">Hello</><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Mismatching order between start/end shortcuts", {
			"Start *bold then _emph, end bold*, end emph_",
			"<s ids=\"[0-9 ]+\">Start </><s ids=\"[0-9 ]+\">bold then </><s ids=\"[0-9 ]+\">emph, end bold</><s ids=\"[0-9 ]+\">, end emph</>",
		} },
		{ "Tag with no content", {
			"[strong][/] World",
			"<s ids=\"[0-9 ]+\"></><s ids=\"[0-9 ]+\"> World</>",
		} },
		{ "Tag with payload and no content", {
			"[strong key:val][/] World",
			"<s ids=\"[0-9 ]+\" key=\"val\"></><s ids=\"[0-9 ]+\"> World</>",
		} },
	};
}

void FBYGRichTextParseTestBase::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	for ( const auto& Pair : TestData )
	{
		OutBeautifiedNames.Add( Pair.Key );
		OutTestCommands.Add( Pair.Key );
	}
}

bool FBYGRichTextParseTestBase::RunTest( const FString& Parameters )
{
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
		DefaultStylesheet->AddStyle( Style );
	}
	{
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
		Style->SetID( "em" );
		Style->SetDisplayType( EBYGStyleDisplayType::Inline );
		Style->SetShortcut( "_" );
		DefaultStylesheet->AddStyle( Style );
	}

	UBYGRichTextBlock* Block = NewObject<UBYGRichTextBlock>();
	Block->SetRichTextStylesheet( DefaultStylesheet );

	TSharedRef<FBYGRichTextMarkupParser> Parser = FBYGRichTextMarkupParser::Create( Block, "s" );
	TArray<FTextLineParseResults> Results;
	FString Out;
	Parser.Get().Process( Results, TestDatum.Input, Out );

	FRegexPattern Pattern( TestDatum.ExpectedPattern );
	FRegexMatcher Matcher( Pattern, Out );

	TestTrue( FString::Printf( TEXT( "Input '%s', Output '%s' matches regex '%s'" ), *TestDatum.Input, *Out, *TestDatum.ExpectedPattern ), Matcher.FindNext() );

	return true;
}


IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST( FBYGRichTextParseTest, FBYGRichTextParseTestBase, "BYG.RichText.Parse", TestFlags )
void FBYGRichTextParseTest::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	FBYGRichTextParseTestBase::GetTests( OutBeautifiedNames, OutTestCommands );
}

bool FBYGRichTextParseTest::RunTest( const FString& Parameters )
{
	return FBYGRichTextParseTestBase::RunTest( Parameters );
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST( FRichTextDefaultsTest, "BYG.RichText.Defaults", TestFlags )
bool FRichTextDefaultsTest::RunTest( const FString& Parameters )
{
	UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>();
	TestEqual( "Must default to Inline", Style->GetDisplayType(), EBYGStyleDisplayType::Inline );
	TestEqual( "Must default to no shortcut", Style->GetShortcut(), "" );

	TestEqual( "Must default to no shortcut", UBYGRichTextStyle::IsValidShortcut( "#" ), true );
	TestEqual( "Must default to no shortcut", UBYGRichTextStyle::IsValidShortcut( "[" ), false );
	TestEqual( "Must default to no shortcut", UBYGRichTextStyle::IsValidShortcut( "]" ), false );

	return true;
}


IMPLEMENT_COMPLEX_AUTOMATION_TEST( FBYGRichTextStyleNameTest, "BYG.RichText.Style.IDs", TestFlags )
void FBYGRichTextStyleNameTest::GetTests( TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands ) const
{
	TMap<FString, FString> TestData = {
		{ "Fail on spaces", " blah" },
		{ "Fail on non-alphanumeric", "bbl.ah" },
	};

	for ( const auto& Pair : TestData )
	{
		OutBeautifiedNames.Add( Pair.Key );
		OutTestCommands.Add( Pair.Value );
	}
}

bool FBYGRichTextStyleNameTest::RunTest( const FString& Parameters )
{
	TestFalse( *Parameters, UBYGRichTextStyle::IsValidID( FName( Parameters ) ) );

	return true;
}

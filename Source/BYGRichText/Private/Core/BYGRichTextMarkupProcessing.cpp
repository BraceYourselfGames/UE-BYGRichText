// Copyright Brace Yourself Games. All Rights Reserved.

#include "Core/BYGRichTextMarkupProcessing.h"

#include "Engine/Font.h"
#include "Framework/Text/RichTextMarkupProcessing.h"
#include "Internationalization/TextTransformer.h"

#include "Widget/BYGRichTextBlock.h"
#include "Settings/BYGRichTextStylesheet.h"
#include "Settings/BYGRichTextProperty.h"
#include "BYGRichTextRuntimeSettings.h"
#include "BYGStyleStack.h"
#include "BYGRichTextModule.h"


static const FString CloseTag = "/";

struct FBYGIDPayload
{
	FBYGIDPayload( const FString& StartBlockContents )
		: Len( StartBlockContents.Len() )
	{
		// contents of the start block can be [id key:val key:val], id cannot have spaces
		// Skip any leading spaces
		const int32 FirstSpaceIndex = StartBlockContents.Find( " " );
		ID = StartBlockContents;
		if ( FirstSpaceIndex != INDEX_NONE )
		{
			ID = StartBlockContents.Left( FirstSpaceIndex );
			const FString Remainder = StartBlockContents.Right( StartBlockContents.Len() - FirstSpaceIndex - 1 );

			TArray<FString> PayloadParts;
			Remainder.ParseIntoArray( PayloadParts, TEXT( " " ) );
			for ( const FString& Part : PayloadParts )
			{
				TArray<FString> KeyValues;
				Part.ParseIntoArray( KeyValues, TEXT( ":" ) );
				if (KeyValues.Num() == 2 )
					Payload.Add( KeyValues[ 0 ], KeyValues[ 1 ] );
				else if (KeyValues.Num() == 1 )
					Payload.Add( KeyValues[ 0 ], "" );
			}
		}
	}
	FString ID;
	TMap<FString, FString> Payload;
	const int32 Len;
};

FBYGTextBlockInfo::FBYGTextBlockInfo()
{
	RawText = "";

	// Set up block propeties with defaults for padding, alignment etc.
	#if 0
	for ( TObjectIterator<UClass> It; It; ++It )
	{
		if ( It->IsChildOf( UBYGRichTextPropertyBase::StaticClass() ) && !It->HasAnyClassFlags( CLASS_Abstract ) )
		{
			BlockProperties.Add( It->GetDefaultObject<UBYGRichTextPropertyBase>() );
		}
	}
	#endif
}

void FBYGTextBlockInfo::OverwriteProperties( const FName& StyleName, const TArray<UBYGRichTextPropertyBase*>& NewBlockProperties )
{
	if ( StylesApplied.Contains( StyleName ) )
		return;

	StylesApplied.Add( StyleName );

	for ( const UBYGRichTextPropertyBase* Prop : NewBlockProperties )
	{
		BlockPropertiesMap.Add( Prop->GetTypeID(), Prop );
	}
}


TSharedRef<FBYGRichTextMarkupParser> FBYGRichTextMarkupParser::Create( UBYGRichTextBlock* InTextBlockOwner, const FString& InXMLElementName )
{
	return MakeShareable( new FBYGRichTextMarkupParser( InTextBlockOwner, InXMLElementName ) );
}

FBYGRichTextMarkupParser::FBYGRichTextMarkupParser( UBYGRichTextBlock* InTextBlockOwner, const FString& InXMLElementName )
	: TextBlockOwner( InTextBlockOwner )
	, XMLElementName( InXMLElementName )
{
}

void FBYGRichTextMarkupParser::Process( TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output )
{
	TSharedRef<class FDefaultRichTextMarkupParser> DefaultParser = FDefaultRichTextMarkupParser::Create();
	DefaultParser->Process( Results, ConvertInputToInlineXML( Input ), Output );
}


void EmitStyledText( FString& Dst, FString& Content, const TArray<const UBYGRichTextPropertyBase*>& Properties, const FString& XMLElementName, const TMap<FString, FString>& Payload  )
{
	TArray<FString> PropIDs;
	for ( const UBYGRichTextPropertyBase* Prop : Properties )
	{
		Prop->TransformString( Content );
		PropIDs.Add( Prop->GetInlineID() );
	}
	ensure( PropIDs.Num() > 0 );

	FString PayloadString = "";
	{

		TArray<FString> PayloadParts;
		for ( const auto& Pair : Payload )
		{
			PayloadParts.Add( FString::Printf( TEXT( "%s=\"%s\"" ), *Pair.Key, *Pair.Value ) );
		}
		if ( PayloadParts.Num() > 0 )
		{
			PayloadString = " " + FString::Join( PayloadParts, TEXT( " " ) );
		}
	}

	Dst += FString::Printf( TEXT( "<%s ids=\"%s\"%s>%s</>" ),
		*XMLElementName,
		*FString::Join( PropIDs, TEXT( " " ) ),
		*PayloadString,
		*Content );
}


void AppendCharacters( FString& CurrentString, TCHAR c, int Count = 1 )
{
	for ( int i = 0; i < Count; ++i )
	{
		CurrentString += c;
	}
}



// Output the current state of the system to the regular <span> format that Unreal expects
void FlushToken( FString& Dst, FString& CurrentToken, const FBYGStyleStack& StyleStack, const FString& XMLElementName, const TMap<FString, FString>& Payload )
{
	if ( CurrentToken.Len() == 0 ) // || StyleStack.Num() == 0)
	{
		//return;
	}

	TArray<const UBYGRichTextPropertyBase*> Properties = StyleStack.GetHeadProperties();

	//CurrentToken.TrimStartAndEndInline();

	EmitStyledText( Dst, CurrentToken, Properties, XMLElementName, Payload );

	CurrentToken = FString();
}
void TrimNewlineStartInline( FString& Str )
{
	int32 Pos = 0;
	while ( Pos < Str.Len() && FChar::IsLinebreak( Str[ Pos ] ) )
	{
		Pos++;
	}
	Str.RemoveAt( 0, Pos );
}

void TrimNewlineEndInline( FString& Str )
{
	int32 End = Str.Len();
	while ( End > 0 && FChar::IsLinebreak( Str[ End - 1 ] ) )
	{
		End--;
	}
	Str.RemoveAt( End, Str.Len() - End );
}

void FlushTokenRaw( TArray<FBYGTextBlockInfo>& TextBlockInfos, FBYGTextBlockInfo& CurrentTextBlockInfo )
{
	//TrimNewlineEndInline( CurrentTextBlockInfo.RawText );
	//TrimNewlineStartInline( CurrentTextBlockInfo.RawText );
	CurrentTextBlockInfo.RawText.TrimStartAndEndInline();
	if ( CurrentTextBlockInfo.RawText.Len() > 0 )
	{
		TextBlockInfos.Add( CurrentTextBlockInfo );
	}
	CurrentTextBlockInfo = FBYGTextBlockInfo();
}


#if 0
bool EmitReplacement( FString& Result, FString& CurrentToken,
	TCHAR const* InputText, int& i,
	const FBYGStyleStack& StyleStack,
	const UBYGRichTextSettings* TextSettings )
{
	FString ReplacementKey = "";
	int OriginalI = i;
	int PayloadStartI = INDEX_NONE;
	while ( InputText[ i ] != 0 )
	{
		char c = InputText[ i ];
		if ( c == ':' )
		{
			ReplacementKey = FString( i - OriginalI, &InputText[ OriginalI ] );
			PayloadStartI = i + 1;
		}
		else if ( c == ']' )
		{
			FString OutputStr = "";
			if ( PayloadStartI == INDEX_NONE )
			{
				ReplacementKey = FString( i - OriginalI, &InputText[ OriginalI ] );
			}


			// Current format:
			// [replacement_id:payload]
			// TODO: Might have to change format
			TArray<FString> ExtraAttributes;
			ExtraAttributes.Add( FString::Printf( TEXT( "id=\"%s\"" ), *ReplacementKey ) );
			FString Payload;
			if ( PayloadStartI != INDEX_NONE )
			{
				Payload = FString( i - PayloadStartI, &InputText[ PayloadStartI ] );
				//Result += FString::Printf( TEXT( "<widget id=\"%s\" p=\"%s\"></>" ), *ReplacementKey, *Payload );
				ExtraAttributes.Add( FString::Printf( TEXT( "payload=\"%s\"" ), *Payload ) );
			}

			FBYGRichTextStyleOld CurrentStyle = StyleStack.GetHead();
			if ( TextSettings->HasTextReplacement( FName( *ReplacementKey ) ) )
			{
				FBYGRichTextReplacement Replacement = TextSettings->GetTextReplacement( FName( *ReplacementKey ), Payload );
				if ( Replacement.bOverrideColor )
				{
					CurrentStyle.bUseColor = true;
					CurrentStyle.Color = Replacement.TextColor;
				}
			}

			if ( CurrentStyle.CaseOverride == EBYGCaseOverride::ForceLowercase )
			{
				OutputStr = OutputStr.ToLower();
			}
			else if ( CurrentStyle.CaseOverride == EBYGCaseOverride::ForceUppercase )
			{
				OutputStr = OutputStr.ToUpper();
			}

			// Now emit that content
			EmitStyledText( Result, OutputStr, CurrentStyle, "widget", ExtraAttributes );

			return true;

			// Failed to find key
			Result += FString::Printf( TEXT( "(Unk:%s) %s" ), *ReplacementKey, *OutputStr );

			return false;
		}
		++i;
	}
	return false;
}
#endif

#if 0
bool EmitLink( FString& Result, FString& CurrentToken,
	TCHAR const* InputText, int& i,
	const FBYGStyleStack& StyleStack,
	const UBYGRichTextSettings* TextSettings )
{
	FString Url = "";
	int UrlStartI = i;
	int LinkTextStartI = INDEX_NONE;
	while ( InputText[ i ] != 0 )
	{
		// Hit end of URL part
		if ( InputText[ i ] == '}' )
		{
			FString OutputStr = "";

			// Stuff within the { } is the link, stuff until the {/} is the content
			Url = FString( i - UrlStartI, &InputText[ UrlStartI ] );
			LinkTextStartI = i + 1;

			++i;

			while ( InputText[ i ] != 0 )
			{
				// search until we find the end of the link link
				if ( InputText[ i ] == '{' && InputText[ i + 1 ] == '/' && InputText[ i + 2 ] == '}' )
				{
					FString LinkText = FString( i - LinkTextStartI, &InputText[ LinkTextStartI ] );
					FBYGRichTextStyleOld CurrentStyle = StyleStack.GetHead();
					if ( CurrentStyle.CaseOverride == EBYGCaseOverride::ForceLowercase )
					{
						LinkText = LinkText.ToLower();
					}
					else if ( CurrentStyle.CaseOverride == EBYGCaseOverride::ForceUppercase )
					{
						LinkText = LinkText.ToUpper();
					}

					FString Content = FString::Printf( TEXT( "<a id=\"link\" href=\"%s\">%s</>" ), *Url, *LinkText );
					Result += Content;

					// Now emit that content
					EmitStyledText( Result, OutputStr, CurrentStyle );

					// Skip over stuff
					i += 2;

					return true;
				}
				++i;
			}

			return false;
		}
		++i;
	}
	return false;
}
#endif

bool MatchForward( TCHAR const* InputText, const int32 StartIndex, const FString& Str )
{
	const int32 InputLength = FCString::Strlen( InputText );
	const int32 MatchLength = FCString::Strlen( *Str );
	// Match string is longer than space we have left
	if ( StartIndex + MatchLength >= InputLength )
		return false;

	for ( int32 i = 0; i < MatchLength; ++i )
	{
		const TCHAR a = InputText[ StartIndex + i ];
		const TCHAR b = Str[ i ];
		if ( a != b )
			return false;
	}
	return true;

}

TArray<FBYGTextBlockInfo> FBYGRichTextMarkupParser::SplitIntoBlocks( const FString& Input )
{
	const UBYGRichTextRuntimeSettings* Settings = GetDefault<UBYGRichTextRuntimeSettings>();
	ensureMsgf( Settings, TEXT( "Could not load default BYGRichTextRuntimeSettings" ) );
	const bool bSplitParagraphs = Settings && !Settings->ParagraphSeparator.IsEmpty();
	const FString ParagraphSeparator = Settings ? Settings->ParagraphSeparator : "\r\n\r\n";

	const UBYGRichTextStylesheet* RichTextStylesheet = TextBlockOwner->GetRichTextStylesheet();

	TArray<FBYGTextBlockInfo> BlockInfos;

	#if 0
	if ( !RichTextStylesheet )
	{
		// Use a fallback one for now so we at least show Something
		FBYGRichTextModule& RichTextModule = FModuleManager::GetModuleChecked<FBYGRichTextModule>( TEXT( "BYGRichText" ) );
		RichTextStylesheet = RichTextModule.GetFallbackStylesheet();
	}
	#endif

	FBYGTextBlockInfo CurrentBlockInfo;

	const UBYGRichTextStyle* DefaultStyle = RichTextStylesheet->FindStyle( RichTextStylesheet->GetDefaultStyleName() );
	if ( DefaultStyle )
	{
		CurrentBlockInfo.OverwriteProperties( DefaultStyle->GetID(), DefaultStyle->Properties );
	}
	else
	{
		UE_LOG( LogTemp, Error, TEXT( "Failed to find default style '%s' in Stylesheet." ), *RichTextStylesheet->GetDefaultStyleName().ToString() );
	}


	TCHAR const* InputText = *Input;

	// iterate over all characters
	bool bEscapeCharacter = false;
	for ( int i = 0; InputText[ i ] != 0; ++i )
	{
		const TCHAR c = InputText[ i ];

		// Paragraph separator, e.g. two newlines
		if ( bSplitParagraphs && MatchForward( InputText, i, ParagraphSeparator) )
		{
			FlushTokenRaw( BlockInfos, CurrentBlockInfo );
			if ( DefaultStyle )
				CurrentBlockInfo.OverwriteProperties( DefaultStyle->GetID(), DefaultStyle->Properties );
			i += ParagraphSeparator.Len() - 1;
		}
		else if ( c == Settings->TagOpenCharacter[ 0 ] )
		{
			// Look forward until we find an end tag
			size_t j = i;
			int32 IDEndIndex = INDEX_NONE;
			while ( j < Input.Len() )
			{
				if ( InputText[ j ] == Settings->TagCloseCharacter[ 0 ] )
				{
					IDEndIndex = j;
					break;
				}

				++j;
			}
			if ( IDEndIndex != INDEX_NONE )
			{
				// contents of the start block can be [id key:val key:val], id cannot have spaces
				const FString TagInternals = Input.Mid( i + 1, IDEndIndex - i - 1 ).TrimStartAndEnd();
				if ( TagInternals == CloseTag )
				{
					while ( i <= IDEndIndex )
					{
						AppendCharacters( CurrentBlockInfo.RawText, InputText[ i ] );
						++i;
					}
					i -= 1;
					if ( CurrentBlockInfo.InlineStyleStackCount > 0 )
					{
						CurrentBlockInfo.InlineStyleStackCount -= 1;
						//AppendCharacters( CurrentBlockInfo.RawText, c );
					}
					else
					{
						FlushTokenRaw( BlockInfos, CurrentBlockInfo );
						if ( DefaultStyle )
							CurrentBlockInfo.OverwriteProperties( DefaultStyle->GetID(), DefaultStyle->Properties );
					}
				}
				else
				{
					const FBYGIDPayload IDPayload( TagInternals );
					const UBYGRichTextStyle* NewStyle = RichTextStylesheet->FindStyle( FName( IDPayload.ID ) );
					if ( NewStyle )
					{
						if ( NewStyle->GetDisplayType() == EBYGStyleDisplayType::Block )
						{
							FlushTokenRaw( BlockInfos, CurrentBlockInfo );
							if ( DefaultStyle )
								CurrentBlockInfo.OverwriteProperties( DefaultStyle->GetID(), DefaultStyle->Properties );
							CurrentBlockInfo.OverwriteProperties( NewStyle->GetID(), NewStyle->Properties );
							CurrentBlockInfo.Payload = IDPayload.Payload;
						}
						else
						{
							CurrentBlockInfo.InlineStyleStackCount += 1;
						}
					}
					else
					{
						UE_LOG( LogTemp, Warning, TEXT( "Style '%s' not found" ), *IDPayload.ID );
					}

					AppendCharacters( CurrentBlockInfo.RawText, c );
				}
			}
		}
		else
		{
			// if we're the start of a new line, see if we have a block short identifier
			if ( i == 0 || InputText[ i - 1 ] == '\n' )
			{
				int32 j = i;
				while ( j < InputText[ j ] && FChar::IsWhitespace( InputText[ j ] ) )
				{
					j++;
				}
				const UBYGRichTextStyle* NewStyle = RichTextStylesheet->FindStyle( InputText, j, EBYGStyleDisplayType::Block );
				if ( NewStyle )
				{
					FlushTokenRaw( BlockInfos, CurrentBlockInfo );
					if ( DefaultStyle )
						CurrentBlockInfo.OverwriteProperties( DefaultStyle->GetID(), DefaultStyle->Properties );
					CurrentBlockInfo.OverwriteProperties( NewStyle->GetID(), NewStyle->Properties );
					i = j;
				}
			}

			AppendCharacters( CurrentBlockInfo.RawText, c );
		}
	}

	FlushTokenRaw( BlockInfos, CurrentBlockInfo );

	return BlockInfos;
}

FString FBYGRichTextMarkupParser::ConvertInputToInlineXML( const FString& Input )
{
	if ( !TextBlockOwner )
	{
		return Input;
	}

	const UBYGRichTextStylesheet* RichTextStylesheet = TextBlockOwner->GetRichTextStylesheet();
	#if 0
	if ( !RichTextStylesheet )
	{
		// Use a fallback one for now so we at least show Something
		FBYGRichTextModule& RichTextModule = FModuleManager::GetModuleChecked<FBYGRichTextModule>( TEXT( "BYGRichText" ) );
		RichTextStylesheet = RichTextModule.GetFallbackStylesheet();
	}
	#endif

	if ( !ensure( RichTextStylesheet->GetDefaultProperties().Num() > 0 ) )
	{
		UE_LOG( LogTemp, Warning, TEXT( "No default properties!" ) );
	}

	// Create the style stack with the defaults from the config
	const UBYGRichTextRuntimeSettings* Settings = GetMutableDefault<UBYGRichTextRuntimeSettings>();
	UBYGRichTextStyle* DefaultStyle = RichTextStylesheet->FindStyle( RichTextStylesheet->GetDefaultStyleName() );

	FBYGStyleStack StyleStack;
	for ( const UBYGRichTextPropertyBase* Prop : RichTextStylesheet->GetDefaultProperties() )
	{
		StyleStack.SetRootProperty( Prop, false );
	}
	if ( DefaultStyle )
	{
		// Default style overwrites any auto-filled properties, and cannot be popped
		for ( const UBYGRichTextPropertyBase* Prop : DefaultStyle->Properties )
		{
			StyleStack.SetRootProperty( Prop, true );
		}
	}

	FString Result;

	TCHAR const* InputText = *Input;

	FString CurrentToken;
	TMap<FString, FString> CurrentPayload;

	// Iterate over all characters
	bool bEscapeCharacter = false;
	for ( int i = 0; InputText[ i ] != 0; ++i )
	{
		const TCHAR c = InputText[ i ];

		bool bNewEscapeCharacter = ( c == '\\' );

		// Found a paragraph break
		// Convert "\ " into non-breaking space
		// Ignore carriage return (UNIX/WINDOWS differences) : no different handling if previous character was an escape character
		if ( c == '\r' )
		{

		}
		// Next line : no different handling if previous character was an escape character
		else if ( c == '\n' )
		{
			// XXX : pseudo HTML does not support \n inside markups
			FlushToken( Result, CurrentToken, StyleStack, XMLElementName, CurrentPayload );
			CurrentPayload.Empty();
			Result += c;
		}
		// If we have an explicit backslash escape character, just output it
		else if ( bEscapeCharacter )
		{
			AppendCharacters( CurrentToken, c );
		}
		// We have an escape character, just do nothing
		else if ( bNewEscapeCharacter )
		{

		}
		// Start of a new style name
		else if ( c == Settings->TagOpenCharacter[ 0 ] )
		{
			// Look forward until we find an end tag
			size_t j = i;
			int32 IDEndIndex = INDEX_NONE;
			while ( j < Input.Len() )
			{
				if ( InputText[ j ] == Settings->TagCloseCharacter[ 0 ] )
				{
					IDEndIndex = j;
					break;
				}

				++j;
			}
			if ( IDEndIndex != INDEX_NONE )
			{
				// contents of the start block can be [id key:val key:val], id cannot have spaces
				const FString TagInternalsUnstripped = Input.Mid( i + 1, IDEndIndex - i - 1 );
				const FString TagInternals = TagInternalsUnstripped.TrimStartAndEnd();

				if ( TagInternals == CloseTag )
				{
					if ( StyleStack.CanPopStyle() )
					{
						FlushToken( Result, CurrentToken, StyleStack, XMLElementName, CurrentPayload );
						CurrentPayload.Empty();
						StyleStack.PopStyle();
					}
				}
				else
				{
					FlushToken( Result, CurrentToken, StyleStack, XMLElementName, CurrentPayload );
					CurrentPayload.Empty();

					// compose the identifier
					const FBYGIDPayload IDPayload( TagInternals );
					CurrentPayload = IDPayload.Payload;
					UBYGRichTextStyle* NewStyle = RichTextStylesheet->FindStyle( FName( IDPayload.ID ) );
					if ( NewStyle )
					{
						StyleStack.PushStyle( NewStyle );
					}
					else
					{
						UE_LOG( LogTemp, Warning, TEXT( "Style '%s' not found" ), *IDPayload.ID );
					}
					// Skip over the [id] stuff
				}
				i = IDEndIndex;
			}
		}
		// close previously started markup 
		// Regular text output
		else
		{

			// Block styles cannot come in the middle of a line 
			const bool bIsStartOfLine = i == 0 || InputText[ i - 1 ] == '\n';

			// Try to pop the current style if it matches
			if ( !StyleStack.IsEmpty() 
				&& StyleStack.GetHeadStyle()->HasShortcut()
				&& ( StyleStack.GetHeadStyle()->GetDisplayType() == EBYGStyleDisplayType::Inline
					|| ( StyleStack.GetHeadStyle()->GetDisplayType() == EBYGStyleDisplayType::Block && bIsStartOfLine ) )
				&& MatchForward( InputText, i, StyleStack.GetHeadStyle()->GetShortcut() ) )
			{
				FlushToken( Result, CurrentToken, StyleStack, XMLElementName, CurrentPayload );
				CurrentPayload.Empty();
				StyleStack.PopStyle();
			}
			else
			{
				// When searching for possible styles, optionally set that it *must* be inline (we don't allow block mid-line)
				TOptional<EBYGStyleDisplayType> DisplayType;
				if ( i > 0 && InputText[ i - 1 ] != '\n' )
					DisplayType = EBYGStyleDisplayType::Inline;
				UBYGRichTextStyle* NewStyle = RichTextStylesheet->FindStyle( InputText, i, DisplayType );
				if ( NewStyle )
				{
					FlushToken( Result, CurrentToken, StyleStack, XMLElementName, CurrentPayload );
					CurrentPayload.Empty();

					// Skip over the shortcut stuff
					i += NewStyle->GetShortcutLen() - 1;

					StyleStack.PushStyle( NewStyle );
				}
				else
				{

					// Write the current character into the CurrentToken buffer
					AppendCharacters( CurrentToken, c );
				}
			}
		}

		bEscapeCharacter = !bEscapeCharacter && bNewEscapeCharacter;
	}

	FlushToken( Result, CurrentToken, StyleStack, XMLElementName, CurrentPayload );
	CurrentPayload.Empty();

	UE_LOG( LogTemp, Warning, TEXT( "Result:\n%s" ), *Result );

	return Result;
}



TSharedRef<FBYGRichTextMarkupWriter> FBYGRichTextMarkupWriter::Create()
{
	return MakeShareable( new FBYGRichTextMarkupWriter() );
}

void FBYGRichTextMarkupWriter::Write( const TArray<FRichTextLine>& InLines, FString& Output )
{
	// Writer takes the FRichTextLine/FRichTextRun and converts it into XML
	TSharedRef<FDefaultRichTextMarkupWriter> DefaultWriter = FDefaultRichTextMarkupWriter::Create();
	return DefaultWriter->Write( InLines, Output );
}

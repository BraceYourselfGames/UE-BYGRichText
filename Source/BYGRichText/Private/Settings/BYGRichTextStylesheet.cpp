// Copyright Brace Yourself Games. All Rights Reserved.

#include "Settings/BYGRichTextStylesheet.h"
#include "Settings/BYGRichTextStyle.h"

#include <Widgets/SBoxPanel.h>
#include <Widgets/Text/SRichTextBlock.h>
#if WITH_EDITOR
#include <Kismet2/CompilerResultsLog.h>
#endif

#define LOCTEXT_NAMESPACE "BYGRichTextModule"

UBYGRichTextStylesheet::UBYGRichTextStylesheet( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	DefaultProperties.Empty();
	for ( TObjectIterator<UClass> It; It; ++It )
	{
		if ( It->IsChildOf( UBYGRichTextPropertyBase::StaticClass() ) && !It->HasAnyClassFlags( CLASS_Abstract ) )
		{
			const UBYGRichTextPropertyBase* DefaultObj = It->GetDefaultObject<UBYGRichTextPropertyBase>();
			if ( DefaultObj->GetShouldApplyToDefault() )
			{
				//UBYGRichTextPropertyBase* base = NewObject<UBYGRichTextPropertyBase>( this, *It, DefaultObj->GetTypeID() );
				//UBYGRichTextPropertyBase* base = ObjectInitializer.CreateDefaultSubobject<UBYGRichTextPropertyBase>( this, DefaultObj->GetTypeID() );
				UBYGRichTextPropertyBase* base = Cast<UBYGRichTextPropertyBase>( ObjectInitializer.CreateDefaultSubobject( this, DefaultObj->GetTypeID(), UBYGRichTextPropertyBase::StaticClass(), *It, true, false ) );
				DefaultProperties.Add( base );
			}
		}
	}
	ensureMsgf( DefaultProperties.Num() > 0, TEXT( "We should have found properties to instantiate" ) );

	RebuildLookup();
}

UBYGRichTextStyle* UBYGRichTextStylesheet::FindStyle( TCHAR const* Input, int32 CurrentIndex, TOptional<EBYGStyleDisplayType> DisplayType ) const
{
	TArray<UBYGRichTextStyle*> Candidates;
	for ( UBYGRichTextStyle* Style : Styles )
	{
		if ( !Style ) continue; // TODO: remove after we use custom editor?
		if ( Style->HasShortcut()
			&& ( !DisplayType.IsSet()
				|| DisplayType.GetValue() == Style->GetDisplayType() ) )
		{
			Candidates.Add( Style );
		}
	}

	int32 CurrentOffset = 0;
	while ( Candidates.Num() >= 1 && Input[ CurrentIndex ] != 0 )
	{
		for ( int32 i = Candidates.Num() - 1; i >= 0; --i )
		{
			if ( Candidates[i]->HasShortcut() )
			{
				if ( Candidates[ i ]->GetShortcut().IsValidIndex( CurrentOffset ) )
				{
					if ( Candidates[ i ]->GetShortcut()[ CurrentOffset ] != Input[ CurrentIndex ] )
					{
						Candidates.RemoveAt( i );
						continue;
					}
				}
			}
		}

		CurrentOffset += 1;
		CurrentIndex += 1;
	}

	if ( Candidates.Num() > 1 )
	{
		Candidates.Sort( []( const UBYGRichTextStyle& A, const UBYGRichTextStyle& B )
			{
				return A.GetShortcutLen() > B.GetShortcutLen();
			} );
	}
	if ( Candidates.Num() >= 1 )
	{
		return Candidates[ 0 ];
	}

	return nullptr;
}

UBYGRichTextStyle* UBYGRichTextStylesheet::FindStyle( const FName& ID ) const
{
	// TODO lookup?
	for ( UBYGRichTextStyle* Style : Styles )
	{
		if ( !Style ) continue;
		if ( Style->GetID() == ID )
			return Style;
	}

	return nullptr;
}

bool UBYGRichTextStylesheet::GetHasStyle( const FName& ID ) const
{
	for ( UBYGRichTextStyle* Style : Styles )
	{
		if ( !Style ) continue;
		if ( Style->GetID() == ID )
			return true;
	}
	return false;
}

TSharedPtr<SWidget> UBYGRichTextStylesheet::RebuildWidget( const FText& Text, TSharedRef<FRichTextLayoutMarshaller> Marshaller )
{
	TArray<FString> Paragraphs;
	FString TextStr = Text.ToString();
	int32 PrevSplit = 0;
	for ( int32 i = 0; i < TextStr.Len(); ++i )
	{
		// Looking for double-newlines
		if ( TextStr.Mid( i, 4 ) == "\r\n\r\n" )
		{
			Paragraphs.Add( TextStr.Mid( PrevSplit, i - PrevSplit ) );
		}
	}
	Paragraphs.Add( TextStr.Mid( PrevSplit, TextStr.Len() - PrevSplit ) );

	TSharedPtr<SVerticalBox> MyVerticalBox = SNew( SVerticalBox );

	for ( int32 i = 0; i < Paragraphs.Num(); ++i )
	{
		TSharedPtr<SRichTextBlock> Wha = 
			SNew( SRichTextBlock )
			//.TextStyle( &DefaultTextStyle )
			.Marshaller( Marshaller );

		Wha->SetText( FText::FromString( Paragraphs[ i ] ) );

		MyVerticalBox->AddSlot()
			.Padding( 2.0f ) // TODO get padding for this
			.AutoHeight()
			[
				Wha.ToSharedRef()
			];
	}

	return MyVerticalBox;
}

void UBYGRichTextStylesheet::PostLoad()
{
	Super::PostLoad();

	RebuildLookup();
}

#if WITH_EDITOR
void UBYGRichTextStylesheet::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );
	RebuildLookup();

	OnStylesheetPropertiesChangedDelegate.Broadcast();
}

void UBYGRichTextStylesheet::PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent )
{
	Super::PostEditChangeChainProperty( PropertyChangedEvent );
	RebuildLookup();

	OnStylesheetPropertiesChangedDelegate.Broadcast();
}

void UBYGRichTextStylesheet::Validate( FCompilerResultsLog& ResultsLog ) const
{
	if ( !FindStyle( DefaultStyleName ) )
	{
		FFormatNamedArguments Args;
		Args.Add( TEXT( "DefaultStyleName" ), FText::FromName( DefaultStyleName ) );
		Args.Add( TEXT( "StylesheetName" ), FText::FromName( GetFName() ) );

		TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create( EMessageSeverity::Error );
		Message->AddToken( FTextToken::Create( FText::Format( LOCTEXT( "DefaultStyleNotFound", "Could not find default style '{DefaultStyleName}' in the list of styles in Stylesheet '{StylesheetName}'" ), Args ) ) );
		ResultsLog.AddTokenizedMessage( Message );
	}

	TMap<FName, int32> DuplicateStyleIDs;
	TMap<FString, int32> DuplicateStyleShortcuts;
	for ( const UBYGRichTextStyle* Style : Styles )
	{
		if ( !Style )
			continue;
		DuplicateStyleIDs.FindOrAdd( Style->GetID() ) += 1;
		if ( Style->HasShortcut() )
			DuplicateStyleShortcuts.FindOrAdd( Style->GetShortcut() ) += 1;

		if ( !Style->IsValidID( Style->GetID() ) )
		{
			FFormatNamedArguments Args;
			Args.Add( TEXT( "StyleName" ), FText::FromName( Style->GetID() ) );
			TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create( EMessageSeverity::Warning );
			Message->AddToken( FTextToken::Create( FText::Format( LOCTEXT( "StyleInvalidID", "Style '{StyleName}' has an invalid ID" ), Args ) ) );
			ResultsLog.AddTokenizedMessage( Message );
		}
		if ( Style->HasShortcut() && !Style->IsValidShortcut( Style->GetShortcut() ) )
		{
			FFormatNamedArguments Args;
			Args.Add( TEXT( "StyleName" ), FText::FromName( Style->GetID() ) );
			TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create( EMessageSeverity::Warning );
			Message->AddToken( FTextToken::Create( FText::Format( LOCTEXT( "StyleInvalidID", "Style '{StyleName}' has an invalid shortcut" ), Args ) ) );
			ResultsLog.AddTokenizedMessage( Message );
		}
		if ( Style->Properties.Num() == 0 )
		{
			FFormatNamedArguments Args;
			Args.Add( TEXT( "StyleName" ), FText::FromName( Style->GetID() ) );
			Args.Add( TEXT( "StylesheetName" ), FText::FromName( GetFName() ) );
			TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create( EMessageSeverity::Warning );
			Message->AddToken( FTextToken::Create( FText::Format( LOCTEXT( "StyleNoProperties", "Style '{StyleName}' has no properties" ), Args ) ) );
			ResultsLog.AddTokenizedMessage( Message );
		}
	}

	for ( const auto& Pair : DuplicateStyleIDs )
	{
		if ( Pair.Value > 1 )
		{
			FFormatNamedArguments Args;
			Args.Add( TEXT( "StyleName" ), FText::FromName( Pair.Key ) );
			Args.Add( TEXT( "StylesheetName" ), FText::FromName( GetFName() ) );
			TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create( EMessageSeverity::Warning );
			Message->AddToken( FTextToken::Create( FText::Format( LOCTEXT( "DuplicateStyleIDs", "There are multiple styles with the same ID '{StyleName}'" ), Args ) ) );
			ResultsLog.AddTokenizedMessage( Message );
		}
	}

	for ( const auto& Pair : DuplicateStyleShortcuts )
	{
		if ( Pair.Value > 1 )
		{
			// TODO show the name of the things with duplicate shortcuts
			FFormatNamedArguments Args;
			Args.Add( TEXT( "Shortcut" ), FText::FromString( *Pair.Key ) );
			Args.Add( TEXT( "StylesheetName" ), FText::FromName( GetFName() ) );
			TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create( EMessageSeverity::Warning );
			Message->AddToken( FTextToken::Create( FText::Format( LOCTEXT( "DuplicateStyleIDs", "There are multiple styles with the same Shortcut '{Shortcut}'" ), Args ) ) );
			ResultsLog.AddTokenizedMessage( Message );
		}
	}
}
#endif

const UBYGRichTextPropertyBase* UBYGRichTextStylesheet::FindProperty( int32 InlineID ) const
{
	for ( int32 i = 0; i < PropertyLookup.Num(); ++i )
	{
		ensure( PropertyLookup[ i ] != nullptr );
	}

	const TWeakObjectPtr<const UBYGRichTextPropertyBase>* Prop = PropertyLookup.Find( InlineID );
	if ( ensure( Prop != nullptr ) )
		return Prop->Get();
	return nullptr;
}

void UBYGRichTextStylesheet::RebuildLookup()
{
	for ( int32 i = 0; i < PropertyLookup.Num(); ++i )
	{
		ensure( PropertyLookup[ i ] != nullptr );
	}

	ensure( DefaultProperties.Num() > 0 );

	PropertyLookup.Empty();
	int32 i = 0;

	for ( const UBYGRichTextPropertyBase* Prop : DefaultProperties )
	{
		if ( !Prop )
			continue;
		PropertyLookup.Add( i, Prop );
		Prop->SetInlineID( i );
		i++;
	}

	for ( UBYGRichTextStyle* Style : Styles )
	{
		// When adding a new prop in editor (w/o custom editor), it can be null until we pick the class
		if ( !Style )
			continue;
		for ( UBYGRichTextPropertyBase* Prop : Style->Properties )
		{
			if ( !Prop )
				continue;
			PropertyLookup.Add( i, Prop );
			Prop->SetInlineID( i );
			i++;

#if WITH_EDITOR
			Prop->OnPropertyPropertyChangedDelegate.BindLambda( [&]()
			{
				OnStylesheetPropertiesChangedDelegate.Broadcast();
			} );
#endif
		}
	}

}


void UBYGRichTextStylesheet::AddStyle( UBYGRichTextStyle* InStyle )
{
	// Validate new style
	ensure( UBYGRichTextStyle::IsValidID( InStyle->GetID() ) );
	for ( const UBYGRichTextStyle* ExistingStyle : Styles )
	{
		if ( ExistingStyle->GetID() == InStyle->GetID() )
		{
			UE_LOG( LogTemp, Warning, TEXT( "Duplicate ID %s" ), *ExistingStyle->GetID().ToString() );
		}
	}
	Styles.Add( InStyle );

	RebuildLookup();
}

void UBYGRichTextStylesheet::RemoveStyle( const FName& StyleID )
{
	UBYGRichTextStyle* FoundStyle = nullptr;
	for ( UBYGRichTextStyle* ExistingStyle : Styles )
	{
		if ( ExistingStyle->GetID() == StyleID )
		{
			FoundStyle = ExistingStyle;
		}
	}
	if ( FoundStyle )
	{
		Styles.Remove( FoundStyle );
		RebuildLookup();
	}
}

#undef LOCTEXT_NAMESPACE




void UBYGRichTextStylesheet::PostCDOContruct()
{
	Super::PostCDOContruct();
	UE_LOG( LogTemp, Warning, TEXT( "Stylesheet PostCDOConstruct" ) );

	// We need to set up default properties but if we do this, the BP save fails
	//RebuildLookup();
}

void UBYGRichTextStylesheet::BeginDestroy()
{
	Super::BeginDestroy();
	UE_LOG( LogTemp, Warning, TEXT( "Stylesheet beginning destruction" ) );
}

#if WITH_EDITOR
bool UBYGRichTextStylesheet::Modify( bool bAlwaysMarkDirty /*= true */ )
{
	bool bFound = Super::Modify( bAlwaysMarkDirty );
	UE_LOG( LogTemp, Warning, TEXT( "Stylesheet modify" ) );
	return bFound;
}
#endif

void UBYGRichTextStylesheet::PostEditImport()
{
	Super::PostEditImport();
	UE_LOG( LogTemp, Warning, TEXT( "Stylesheet post edit import" ) );
}

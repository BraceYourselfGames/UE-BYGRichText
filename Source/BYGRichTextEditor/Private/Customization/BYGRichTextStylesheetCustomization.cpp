// Copyright Brace Yourself Games. All Rights Reserved.

#include "Customization/BYGRichTextStylesheetCustomization.h"

#include "Settings/BYGRichTextStylesheet.h"
#include "PropertyEditing.h"

#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include <PropertyEditor/Public/IDetailCustomNodeBuilder.h>
#include "BYGRichTextUIStyle.h"

#define LOCTEXT_NAMESPACE "BYGRichTextEditorModule"

DECLARE_DELEGATE_OneParam( FBYGOnDeleteActivatedSignature, int32 Index );

class FBYGStyleDetails : public IDetailCustomNodeBuilder, public TSharedFromThis<FBYGStyleDetails>, public FGCObject
{
public:
	FBYGStyleDetails( UBYGRichTextStylesheet* InOwningStylesheet, UBYGRichTextStyle* InOwningStyle, int32 InIndex );

	//~ Begin IDetailCustomNodeBuilder interface
	virtual void SetOnRebuildChildren( FSimpleDelegate InOnRegenerateChildren ) override { OnRegenerateChildren = InOnRegenerateChildren; }
	virtual void GenerateHeaderRowContent( FDetailWidgetRow& NodeRow ) override;
	virtual void GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder ) override;
	virtual void Tick( float DeltaTime ) override {}
	virtual bool RequiresTick() const override { return false; }
	virtual bool InitiallyCollapsed() const override { return false; }
	virtual FName GetName() const override;
	//~ End IDetailCustomNodeBuilder interface

	//~ Begin FGCObject interface
	void AddReferencedObjects( FReferenceCollector& Collector )
	{
		Collector.AddReferencedObject( RichTextBlock );
		Collector.AddReferencedObject( Stylesheet );
		Collector.AddReferencedObject( LocalTextStyleInstance );
	}
	//~ End FGCObject interface

	FBYGOnDeleteActivatedSignature OnDeleteActivatedDelegate;

protected:
	void OnPropertyValueChanged();

	class UBYGRichTextBlock* RichTextBlock = nullptr;
	class UBYGRichTextStylesheet* Stylesheet = nullptr;

	TWeakObjectPtr<UBYGRichTextStylesheet> OwnerStylesheet;
	TWeakObjectPtr<UBYGRichTextStyle> OwnerTextStyle;
	UBYGRichTextStyle* LocalTextStyleInstance = nullptr;
	int32 Index;
	FSimpleDelegate OnRegenerateChildren;
};

void FBYGRichTextStylesheetCustomization::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized( Objects );
	if ( Objects.Num() != 1 )
		return;
	TextStylesheet = Cast<UBYGRichTextStylesheet>( Objects[ 0 ].Get() );

	MyDetailBuilder = &DetailBuilder;

	// Set up global props for the stylesheet itself
	auto& StylesheetCat = DetailBuilder.EditCategory( TEXT( "BYGRichTextStylesheet" ), LOCTEXT( "Stylesheet_Stylesheet", "Stylesheet" ) );

	// Hide styles default appearance, render it ourselves
	TSharedPtr<IPropertyHandle> StylesProperty = DetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextStylesheet, Styles ) );
	StylesProperty->MarkHiddenByCustomization();

	auto& StylesCat = DetailBuilder.EditCategory( TEXT( "Styles" ), LOCTEXT( "Stylesheet_Styles", "Styles" ) );

	// Create style button
	StylesCat.AddCustomRow( LOCTEXT( "CreateStyleRowID", "Create Style" ) )
		.WholeRowContent()
		[
			SNew( SButton )
			.HAlign( HAlign_Fill )
			.OnClicked( this, &FBYGRichTextStylesheetCustomization::OnNewStyleButtonClicked )
			[
				SNew( SHorizontalBox )
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew( STextBlock )
					.TextStyle( FEditorStyle::Get(), "ContentBrowser.TopBar.Font" )
					.Font( FEditorStyle::Get().GetFontStyle( "FontAwesome.11" ) )
					.Text( FText::FromString( FString( TEXT( "\xf067" ) ) ) /*fa-plus*/ )
					.Margin( FMargin( 0, 0, 2, 0 ) )
				]
				+ SHorizontalBox::Slot()
				.FillWidth( 1 )
				[
					SNew( STextBlock )
					.Text( LOCTEXT( "CreateStyleButtonLabel", "Create Style" ) )
				]
			]
		];

	for ( int32 i = 0; i < TextStylesheet->Styles.Num(); ++i )
	{
		// Sometimes badly loaded styles can be null
		if ( !TextStylesheet->Styles[ i ] )
			continue;

		if ( i > 0 )
		{
			FDetailWidgetRow& Row = StylesCat.AddCustomRow( LOCTEXT( "Foo", "Text" ) );
			Row.WholeRowContent()
			.VAlign( VAlign_Center )
			[
				SNew( SImage )
				.Image( FBYGRichTextUIStyle::GetBrush( FName( TEXT( "PropertySpacer" ) ) ) )
			];
		}

		TSharedRef<FBYGStyleDetails> StyleBuilder = MakeShared<FBYGStyleDetails>( TextStylesheet.Get(), TextStylesheet->Styles[ i ], i );
		StylesCat.AddCustomBuilder( StyleBuilder );
		StyleBuilder->OnDeleteActivatedDelegate.BindSP( this, &FBYGRichTextStylesheetCustomization::OnDeleteStyle );
	}

}

void FBYGRichTextStylesheetCustomization::OnPropertyValueChanged()
{
	if ( MyDetailBuilder )
	{
		MyDetailBuilder->ForceRefreshDetails();
	}
}

void FBYGRichTextStylesheetCustomization::OnDeleteStyle( int32 Index )
{
	if ( !TextStylesheet.IsValid() )
		return;

	if ( !TextStylesheet->Styles.IsValidIndex( Index ) )
		return;

	TextStylesheet->Styles.RemoveAt( Index );

	if ( MyDetailBuilder )
	{
		MyDetailBuilder->ForceRefreshDetails();
	}
}

FReply FBYGRichTextStylesheetCustomization::OnNewStyleButtonClicked()
{
	if ( TextStylesheet.IsValid() )
	{

		UPackage* Pkg = TextStylesheet->GetPackage();
		//UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( TextStylesheet.Get(), UBYGRichTextStyle::StaticClass(), NAME_None, RF_Standalone | RF_Transactional | RF_Public );
		// TODO: Not really sure what is the correct package for this style or the properties below
		UBYGRichTextStyle* Style = NewObject<UBYGRichTextStyle>( TextStylesheet.Get() );

		FName FoundStyleID = NAME_None;
		for ( int32 i = 1; i < 100; ++i )
		{
			FName StyleID = FName( FString::Printf( TEXT( "style%d" ), i ) );
			if ( !TextStylesheet->GetHasStyle( StyleID ) )
			{
				FoundStyleID = StyleID;
				break;
			}
		}
		if ( FoundStyleID != NAME_None )
		{
			Style->SetID( FoundStyleID );
		}

		// Pick a sensible ID
		// Add some sensible defaults, maybe
		{
			// TODO: Not really sure what is the correct package for this style or the properties below
			Style->AddProperty( NewObject<UBYGRichTextSizeProperty>( Style, UBYGRichTextSizeProperty::StaticClass() ) );
			Style->AddProperty( NewObject<UBYGRichTextColorProperty>( Style, UBYGRichTextColorProperty::StaticClass() ) );
		}

		TextStylesheet->Styles.Add( Style );
		TextStylesheet->Modify();
	}

	// Refresh?
	if ( MyDetailBuilder )
	{
		MyDetailBuilder->ForceRefreshDetails();
	}

	return FReply::Handled();
}


FBYGStyleDetails::FBYGStyleDetails( UBYGRichTextStylesheet* InOwningStylesheet, UBYGRichTextStyle* InOwningStyle, int32 InIndex )
	: OwnerStylesheet( InOwningStylesheet )
	, OwnerTextStyle( InOwningStyle )
	, Index( InIndex )
{
}

FName FBYGStyleDetails::GetName() const
{
	static const FName Name( "BYGStyleDetails" );
	return Name;
}

void FBYGStyleDetails::GenerateHeaderRowContent( FDetailWidgetRow& NodeRow )
{
}

void FBYGStyleDetails::OnPropertyValueChanged()
{
	if ( OwnerTextStyle.IsValid() )
	{
		RichTextBlock->SetText( FText::FromName( OwnerTextStyle->GetID() == NAME_None ? "none" : OwnerTextStyle->GetID() ) );
	}
}

void FBYGStyleDetails::GenerateChildContent( IDetailChildrenBuilder& ChildrenBuilder )
{
	check( OwnerTextStyle.IsValid() );

	TArray<UObject*> ChildObjects{ OwnerTextStyle.Get() };

	FAddPropertyParams Params = FAddPropertyParams()
		.UniqueId( OwnerTextStyle->GetFName() )
		.AllowChildren( true )
		.CreateCategoryNodes( false );
	IDetailPropertyRow* Row = ChildrenBuilder.AddExternalObjectProperty( ChildObjects, NAME_None, Params );

	if ( !Row )
		return;

	Row->DisplayName( FText::FromName( OwnerTextStyle->GetID() == NAME_None ? "none" : OwnerTextStyle->GetID() ) );

	FDetailWidgetRow& CustomWidget = Row->CustomWidget( true );

	TSharedPtr<SWidget> DefaultNameWidget;
	TSharedPtr<SWidget> DefaultValueWidget;
	Row->GetDefaultWidgets( DefaultNameWidget, DefaultValueWidget, CustomWidget );
	//Row->OverrideResetToDefault( FResetToDefaultOverride::Hide() );
	TSharedPtr<SWidget> ValueWidget = DefaultValueWidget;

	Row->GetPropertyHandle()->SetOnPropertyValueChanged( FSimpleDelegate::CreateSP( this, &FBYGStyleDetails::OnPropertyValueChanged ) );
	Row->GetPropertyHandle()->SetOnChildPropertyValueChanged( FSimpleDelegate::CreateSP( this, &FBYGStyleDetails::OnPropertyValueChanged ) );

	RichTextBlock = NewObject<UBYGRichTextBlock>();
#if WITH_EDITOR
	RichTextBlock->bIsSlatePreview = true;
#endif
	const FString SafeID = OwnerTextStyle->GetID() == NAME_None ? "none" : OwnerTextStyle->GetID().ToString();
	const FString Text = "[" + SafeID + "]" + SafeID + "[/]";
	RichTextBlock->SetRichTextStylesheet( OwnerStylesheet.Get() );
	OwnerStylesheet->RebuildLookup();
	RichTextBlock->SetText( FText::FromString( Text ) );
	//Stylesheet = NewObject<UBYGRichTextStylesheet>();
	//Stylesheet->SetDefaultStyleName( OwnerTextStyle->GetID() );
	// TODO: Problem here is that the OwnerTextStyle
	//LocalTextStyleInstance = DuplicateObject<UBYGRichTextStyle>( OwnerTextStyle.Get(), OwnerStylesheet.Get() );
	//Stylesheet->AddStyle( LocalTextStyleInstance );
	RichTextBlock->Modify( true );

	CustomWidget
		.NameContent()
		[
			SNew( SBox )
			.Padding( FMargin( 0.0f, 2.0f ) )
			[
				RichTextBlock->TakeWidget()
			]
		]
		.ValueContent()
		[
			SNew( SOverlay )
			+ SOverlay::Slot()
			.HAlign( HAlign_Left )
			.VAlign( VAlign_Fill )
			[
				SNew( SButton )
				//.OnClicked( this, &FBYGRichTextStyleCustomization::OnRemoveStyleButtonClicked )
				.OnClicked_Lambda( [this]() {
					OnDeleteActivatedDelegate.ExecuteIfBound( Index );
					return FReply::Handled();
				} )
				.ToolTipText( LOCTEXT( "RemoveStyleTooltip", "Remove Style" ) )
				.ButtonStyle( FEditorStyle::Get(), "FlatButton" )
				[
					SNew( SHorizontalBox )
					+ SHorizontalBox::Slot()
					.HAlign( HAlign_Center )
					.VAlign( VAlign_Center )
					.AutoWidth()
					[
						SNew( STextBlock )
						.TextStyle( FEditorStyle::Get(), "ContentBrowser.TopBar.Font" )
						.Font( FEditorStyle::Get().GetFontStyle( "FontAwesome.11" ) )
						.Text( FText::FromString( FString( TEXT( "\xf00d" ) ) ) /*fa-plus*/ )
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE

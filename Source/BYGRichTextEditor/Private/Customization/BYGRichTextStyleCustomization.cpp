// Copyright Brace Yourself Games. All Rights Reserved.

#include "Customization/BYGRichTextStyleCustomization.h"
#include "PropertyEditing.h"

#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "BYGRichTextUIStyle.h"
#include "BYGTextTransformPolicyCustomization.h"
#include "Widgets/Layout/SSplitter.h"

#include "Settings/BYGRichTextStylesheet.h"
#include "Settings/BYGRichTextStyle.h"

#define LOCTEXT_NAMESPACE "BYGRichTextEditorModule"

void FBYGRichTextStyleCustomization::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	// Set up the object being edited
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized( Objects );
	if ( Objects.Num() != 1 )
	{
		return;
	}
	TextStyle = Cast<UBYGRichTextStyle>( Objects[ 0 ].Get() );

	MyDetailBuilder = &DetailBuilder;

	IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory( TEXT( "Root" ) );
	Cat.SetSortOrder( 0 );

	// Manually add the properties we want, in the order we want.
	IDetailPropertyRow& IDRow = Cat.AddProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextStyle, ID ), UBYGRichTextStyle::StaticClass() );
	TSharedPtr<SWidget> IDNameWidget;
	TSharedPtr<SWidget> IDValueWidget;
	FDetailWidgetRow IDDetailWidgetRow;
	IDRow.GetDefaultWidgets(IDNameWidget, IDValueWidget, IDDetailWidgetRow );
	Cat.AddProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextStyle, Shortcut ), UBYGRichTextStyle::StaticClass() );
	Cat.AddProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextStyle, DisplayType ), UBYGRichTextStyle::StaticClass() );

	// Hide the default "Properties" list, so we can show it in a custom way
	TSharedPtr<IPropertyHandle> PropertiesProperty = DetailBuilder.GetProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextStyle, Properties ) );
	PropertiesProperty->MarkHiddenByCustomization();

	IDetailCategoryBuilder& PropertiesCat = DetailBuilder.EditCategory( TEXT( "Properties" ) );
	PropertiesCat.SetSortOrder( 1 );
	PropertiesCat.AddCustomRow( LOCTEXT("Blah2","Blah") )
	.NameContent()
	[
		SNew( SOverlay )
		+ SOverlay::Slot()
		.HAlign( HAlign_Left )
		.VAlign( VAlign_Center )
		[
			SAssignNew( AddPropertyMenuAnchor, SMenuAnchor )
			.Placement(MenuPlacement_BelowAnchor)
			.OnGetMenuContent( this, &FBYGRichTextStyleCustomization::CreatePropertiesMenu, true )
			[
				SNew( SButton )
				.OnClicked( this, &FBYGRichTextStyleCustomization::OnAddPropertyButtonActivated )
				.IsEnabled( this, &FBYGRichTextStyleCustomization::GetCanAddProperty )
				.ToolTipText( LOCTEXT( "AddPropertyTooltip", "Choose a new property to add to this style." ) )
				[
					SNew( SHorizontalBox )
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew( STextBlock )
						.TextStyle( FEditorStyle::Get(), "ContentBrowser.TopBar.Font" )
						.Font( FEditorStyle::Get().GetFontStyle( "FontAwesome.11" ) )
						.Text( FText::FromString( FString( TEXT( "\xf067" ) ) ) /*fa-plus*/ )
						.Margin( FMargin( 0, 0, 4, 0 ) )
					]
					+ SHorizontalBox::Slot()
					.FillWidth( 1 )
					[
						SNew( STextBlock )
						.Text( LOCTEXT( "AddProperty", "Add Property..." ) )
					]
				]
			]
		]
	];

	// Show Properties in a custom way
	#if 0
	PropertiesCat.AddCustomRow( LOCTEXT( "PropertiesRow", "PropertiesRow" ) )
		.NameContent()
		[
			SNew( STextBlock )
			.Text( LOCTEXT( "Properties", "Properties" ) )
		]
		.ValueContent()
		[
			SAssignNew( AddPropertyMenuAnchor, SMenuAnchor )
			.Placement(MenuPlacement_BelowAnchor)
			.OnGetMenuContent( this, &FBYGRichTextStyleCustomization::CreatePropertiesMenu, true )
			[
				SNew( SButton )
				.OnClicked( this, &FBYGRichTextStyleCustomization::OnAddPropertyButtonActivated )
				.IsEnabled( this, &FBYGRichTextStyleCustomization::GetCanAddProperty )
				.ToolTipText( LOCTEXT( "AddPropertyTooltip", "Choose a new property to add to this style." ) )
				[
					SNew( SHorizontalBox )
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew( STextBlock )
						.TextStyle( FEditorStyle::Get(), "ContentBrowser.TopBar.Font" )
						.Font( FEditorStyle::Get().GetFontStyle( "FontAwesome.11" ) )
						.Text( FText::FromString( FString( TEXT( "\xf067" ) ) ) /*fa-plus*/ )
						.Margin( FMargin( 0, 0, 4, 0 ) )
					]
					+ SHorizontalBox::Slot()
					.FillWidth( 1 )
					[
						SNew( STextBlock )
						.Text( LOCTEXT( "AddProperty", "Add Property..." ) )
					]
				]
			]
		];
	#endif
	

	for ( int32 i = 0; i < TextStyle->Properties.Num(); ++i )
	{
		if ( !TextStyle->Properties[ i ] )
			continue;

		if ( i > 0 )
		{
			FDetailWidgetRow& Row = PropertiesCat.AddCustomRow( LOCTEXT( "Foo", "Text" ) );
			Row.WholeRowContent()
			.VAlign( VAlign_Center )
			[
				SNew( SImage )
				.Image( FBYGRichTextUIStyle::GetBrush( FName( TEXT( "PropertySpacer" ) ) ) )
			];
		}

		FDetailWidgetRow& Row = PropertiesCat.AddCustomRow( LOCTEXT( "Thing", "Something" ) );
		Row.NameContent()
		[
			SNew( SHorizontalBox )
			+ SHorizontalBox::Slot()
			.Padding( FMargin( -16, 0, 0, 0 ) )
			.HAlign( HAlign_Center )
			.VAlign( VAlign_Center )
			.AutoWidth()
			[
				SNew( SImage )
				.Image( FBYGRichTextUIStyle::GetBrush( FName( FString::Printf( TEXT( "ClassIcon.%s" ), *TextStyle->Properties[ i ]->GetClass()->GetName() ) ) ) )
			]
			+ SHorizontalBox::Slot()
			.FillWidth( 1 )
			[
				SNew( STextBlock )
				.Text( TextStyle->Properties[ i ]->GetClass()->GetDisplayNameText() )
				.TextStyle( FBYGRichTextUIStyle::Get(), "RichTextStyle.Title" )
			]
		]
		.ValueContent()
		[
			SNew( SOverlay)
			+ SOverlay::Slot()
			.HAlign( HAlign_Left )
			.VAlign( VAlign_Center )
			[
				SNew( SButton )
				.OnClicked_Lambda( [this, i]()
				{
					this->RemoveProperty( i );
					return FReply::Handled();
				} )
				.ToolTipText( LOCTEXT( "RemovePropertyTooltip", "Remove Property" ) )
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
		TArray<UObject*> ExternalObjects{ TextStyle->Properties[ i ] };

		//PropertiesCat.AddProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextPropertyBase, InlineID ), UBYGRichTextPropertyBase::StaticClass() );
		//PropertiesCat.AddProperty( GET_MEMBER_NAME_CHECKED( UBYGRichTextPropertyBase, CachedInlineID ), UBYGRichTextPropertyBase::StaticClass() );

		// Show warning
		if ( !TextStyle->Properties[ i ]->GetSupportsDisplayType( TextStyle->GetDisplayType() ) )
		{
			const UEnum* StyleEnumObject = StaticEnum<EBYGStyleDisplayType>();
			static FLinearColor WarningColor( FColor( 0xffbbbb44 ) );
			PropertiesCat.AddCustomRow( LOCTEXT( "SupportWarning", "SupportWarning" ) )
			.WholeRowContent()
			[
				SNew(SHorizontalBox)
				//.Visibility(this, &SSequencerTimePanel::GetWarningVisibility)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(0.f, 0.f, 7.f, 0.f))
				.AutoWidth()
				[
					SNew(STextBlock)
					.TextStyle(FEditorStyle::Get(), "TextBlock.ShadowedTextWarning")
   					.ColorAndOpacity(WarningColor)
					.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
					.Text(FText::FromString(FString(TEXT("\xf071"))) /*fa-exclamation-triangle*/)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.TextStyle(FEditorStyle::Get(), "TextBlock.ShadowedTextWarning")
					.ColorAndOpacity(WarningColor)
					.Text( FText::FormatNamed(
						LOCTEXT( "NotSupportedFormat", "This property does not support '{DisplayType}'  display type. It will not be used." ),
						"DisplayType", StyleEnumObject->GetDisplayNameTextByValue( ( int64 )TextStyle->GetDisplayType() )
					) )
				]
			];
		}

		// TODO: if the style's supported types do not match this style's
		// block/inline setting, then show a warning?

		for ( const FProperty* TestProperty : TFieldRange<FProperty>( TextStyle->Properties[ i ]->GetClass() ) )
		{
			if ( TestProperty->HasAnyPropertyFlags( CPF_Edit ) )
			{
				const bool bAdvancedDisplay = TestProperty->HasAnyPropertyFlags( CPF_AdvancedDisplay );
				const EPropertyLocation::Type PropertyLocation = bAdvancedDisplay ? EPropertyLocation::Advanced : EPropertyLocation::Common;

				IDetailPropertyRow* NewRow = PropertiesCat.AddExternalObjectProperty( ExternalObjects, TestProperty->GetFName(), PropertyLocation );
				if ( ensure( NewRow ) )
				{
					//NewRow->GetPropertyHandle()->SetOnPropertyValueChanged( FSimpleDelegate::CreateSP( this, &FBYGRichTextStyleCustomization::OnPropertyValueChanged ) );
					//NewRow->GetPropertyHandle()->SetOnChildPropertyValueChanged( FSimpleDelegate::CreateSP( this, &FBYGRichTextStyleCustomization::OnChildPropertyValueChanged ) );
				}
			}
		}
	}

	DetailBuilder.RegisterInstancedCustomPropertyTypeLayout( TEXT( "ETextTransformPolicy" ), FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FBYGTextTransformPolicyCustomization::MakeInstance ) );
	DetailBuilder.RegisterInstancedCustomPropertyTypeLayout( TEXT( "EBYGStyleDisplayType" ), FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FBYGStyleDisplayTypeCustomization::MakeInstance ) );
	// We have to duplicate the one in UMG because it's not exported
	DetailBuilder.RegisterInstancedCustomPropertyTypeLayout( TEXT( "ETextJustify" ), FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FBYGTextJustifyCustomization::MakeInstance ) );
}

TSharedRef<SWidget> FBYGRichTextStyleCustomization::CreatePropertiesMenu( bool bSomething ) const
{
	FMenuBuilder MenuBuilder( true, NULL );

	TArray<UClass*> UnusedTextProperties = GetUnusedProperties();

	for ( int32 i = 0; i < UnusedTextProperties.Num(); ++i )
	{
		// TODO add horizontal space between block and inline styles? Add
		// text after them?
		UClass* ClassPtr = UnusedTextProperties[ i ];
		const FText DisplayName = ClassPtr->GetDisplayNameText();
		//UBYGRichTextStyle* Style = Cast<UBYGRichTextStyle>( ClassPtr->GetDefaultObject<UBYGRichTextStyle>() );
		class FName IconName = FName( FString::Printf( TEXT( "ClassIcon.%s" ), *ClassPtr->GetName() ) );
		MenuBuilder.AddMenuEntry( DisplayName, DisplayName, FSlateIcon( FBYGRichTextUIStyle::GetStyleSetName(), IconName ),
			FUIAction(
				FExecuteAction::CreateRaw( const_cast< FBYGRichTextStyleCustomization* >( this ), &FBYGRichTextStyleCustomization::AddProperty, i )
			),
			NAME_None, EUserInterfaceActionType::Button );
	}

	return MenuBuilder.MakeWidget();
}

FReply FBYGRichTextStyleCustomization::OnAddPropertyButtonActivated()
{
	AddPropertyMenuAnchor->SetIsOpen( !AddPropertyMenuAnchor->IsOpen() );
	return FReply::Handled();
}

void FBYGRichTextStyleCustomization::AddProperty( int32 Idx )
{
	// Instantiate and add the correct property to the list
	TArray<UClass*> UnusedTextProperties = GetUnusedProperties();
	if ( !UnusedTextProperties.IsValidIndex( Idx ) )
		return;

	if ( !TextStyle.IsValid() )
		return;

	TextStyle->Properties.Add( NewObject<UBYGRichTextPropertyBase>( TextStyle.Get(), UnusedTextProperties[ Idx ] ) );
	TextStyle->Properties.Sort( []( const UBYGRichTextPropertyBase& A, const UBYGRichTextPropertyBase& B )
	{
		const int32 OrderA = A.GetClass()->GetIntMetaData( "DisplayOrder" );
		const int32 OrderB = B.GetClass()->GetIntMetaData( "DisplayOrder" );
		return OrderA < OrderB;
	} );
	TextStyle->Modify();
	if ( MyDetailBuilder )
	{
		MyDetailBuilder->ForceRefreshDetails();
	}
}

void FBYGRichTextStyleCustomization::RemoveProperty( int32 Idx )
{
	if ( !TextStyle.IsValid() )
		return;

	if ( !TextStyle->Properties.IsValidIndex( Idx ) )
		return;

	TextStyle->Properties.RemoveAt( Idx );
	TextStyle->Modify();
	if ( MyDetailBuilder )
	{
		MyDetailBuilder->ForceRefreshDetails();
	}
}

bool FBYGRichTextStyleCustomization::GetCanAddProperty() const
{
	return GetUnusedProperties().Num() > 0;
}

TArray<UClass*> FBYGRichTextStyleCustomization::GetUnusedProperties() const
{
	TArray<UClass*> UnusedTextProperties;
	if ( TextStyle.IsValid() )
	{
		for ( TObjectIterator<UClass> It; It; ++It )
		{
			if ( It->IsChildOf( UBYGRichTextPropertyBase::StaticClass() ) && !It->HasAnyClassFlags( CLASS_Abstract ) )
			{
				bool bFound = false;
				for ( UBYGRichTextPropertyBase* pProp : TextStyle->Properties )
				{
					if ( pProp->IsA( *It ) )
					{
						bFound = true;
						break;
					}
				}
				if ( !bFound )
				{
					UnusedTextProperties.Add( *It );
				}
			}
		}
	}
	UnusedTextProperties.Sort( []( const UClass& A, const UClass& B )
	{
		const int32 OrderA = A.GetIntMetaData( "DisplayOrder" );
		const int32 OrderB = B.GetIntMetaData( "DisplayOrder" );
		return OrderA < OrderB;
	} );
	return UnusedTextProperties;
}

#if 0
void FBYGRichTextStyleCustomization::OnPropertyValueChanged()
{
	if ( TextStyle.IsValid() )
	{
		// How is this not being updated?
	}
}

void FBYGRichTextStyleCustomization::OnChildPropertyValueChanged()
{
	if ( TextStyle.IsValid() )
	{
		// How is this not being updated?
	}
}
#endif

#undef LOCTEXT_NAMESPACE


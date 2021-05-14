// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGStyleDisplayTypeCustomization.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "DetailWidgetRow.h"
#include "BYGRichTextUIStyle.h"


#define LOCTEXT_NAMESPACE "BYG"

void FBYGStyleDisplayTypeCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils )
{
	const FMargin OuterPadding( 2 );
	const FMargin ContentPadding( 2 );

	HeaderRow
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew( SHorizontalBox )

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding( OuterPadding )
		[
			SNew(SCheckBox)
			.Style( FEditorStyle::Get(), "ToggleButtonCheckbox" )
			.ToolTipText( LOCTEXT( "EBYGStyleDisplayType::Block", "Block display" ) )
			.Padding( ContentPadding )
			.OnCheckStateChanged( this, &FBYGStyleDisplayTypeCustomization::HandleCheckStateChanged, PropertyHandle, EBYGStyleDisplayType::Block )
			.IsChecked( this, &FBYGStyleDisplayTypeCustomization::GetCheckState, PropertyHandle, EBYGStyleDisplayType::Block )
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding( ContentPadding )
				[
					SNew( SImage )
					.Image( FBYGRichTextUIStyle::GetBrush( "BYGStyleDisplayType_Block" ) )
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(OuterPadding)
		[
			SNew( SCheckBox )
			.Style( FEditorStyle::Get(), "ToggleButtonCheckbox" )
			.ToolTipText( LOCTEXT( "EBYGStyleDisplayType::Inline", "Inline display" ) )
			.Padding(ContentPadding)
			.OnCheckStateChanged( this, &FBYGStyleDisplayTypeCustomization::HandleCheckStateChanged, PropertyHandle, EBYGStyleDisplayType::Inline )
			.IsChecked( this, &FBYGStyleDisplayTypeCustomization::GetCheckState, PropertyHandle, EBYGStyleDisplayType::Inline )
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding( ContentPadding )
				[
					SNew(SImage)
					.Image( FBYGRichTextUIStyle::GetBrush( "BYGStyleDisplayType_Inline" ) )
				]
			]
		]

	];
}

void FBYGStyleDisplayTypeCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FBYGStyleDisplayTypeCustomization::HandleCheckStateChanged(ECheckBoxState InCheckboxState, TSharedRef<IPropertyHandle> PropertyHandle, EBYGStyleDisplayType InStyleDisplayType)
{
	PropertyHandle->SetValue( ( uint8 )InStyleDisplayType );
}

ECheckBoxState FBYGStyleDisplayTypeCustomization::GetCheckState( TSharedRef<IPropertyHandle> PropertyHandle, EBYGStyleDisplayType InStyleDisplayType ) const
{
	uint8 Value;
	if ( PropertyHandle->GetValue( Value ) == FPropertyAccess::Result::Success )
	{
		return ( EBYGStyleDisplayType )Value == InStyleDisplayType ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	return ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

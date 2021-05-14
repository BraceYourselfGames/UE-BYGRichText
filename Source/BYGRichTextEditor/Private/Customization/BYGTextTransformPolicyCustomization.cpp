// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGTextTransformPolicyCustomization.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "DetailWidgetRow.h"
#include <Framework/Text/TextLayout.h>
#include "BYGRichTextUIStyle.h"


#define LOCTEXT_NAMESPACE "BYG"

void FBYGTextTransformPolicyCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils )
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
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding( OuterPadding )
		[
			SNew( SCheckBox )
			.Style( FEditorStyle::Get(), "ToggleButtonCheckbox" )
			.ToolTipText( LOCTEXT( "ETextTransformPolicy::Upper", "Uppercase" ) )
			.Padding( ContentPadding )
			.OnCheckStateChanged( this, &FBYGTextTransformPolicyCustomization::HandleCheckStateChanged, PropertyHandle, ETextTransformPolicy::ToUpper )
			.IsChecked( this, &FBYGTextTransformPolicyCustomization::GetCheckState, PropertyHandle, ETextTransformPolicy::ToUpper )
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew( SImage )
					.Image( FBYGRichTextUIStyle::GetBrush( "BYGTextTransform_Upper" ) )
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding( OuterPadding )
		[
			SNew( SCheckBox )
			.Style( FEditorStyle::Get(), "ToggleButtonCheckbox" )
			.ToolTipText( LOCTEXT( "ETextTransformPolicy::Lower", "Lowercase" ) )
			.Padding( ContentPadding )
			.OnCheckStateChanged( this, &FBYGTextTransformPolicyCustomization::HandleCheckStateChanged, PropertyHandle, ETextTransformPolicy::ToLower )
			.IsChecked( this, &FBYGTextTransformPolicyCustomization::GetCheckState, PropertyHandle, ETextTransformPolicy::ToLower )
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew( SImage )
					.Image( FBYGRichTextUIStyle::GetBrush( "BYGTextTransform_Lower" ) )
				]
			]
		]

	];
}

void FBYGTextTransformPolicyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FBYGTextTransformPolicyCustomization::HandleCheckStateChanged( ECheckBoxState InCheckboxState, TSharedRef<IPropertyHandle> PropertyHandle, ETextTransformPolicy InPolicy )
{
	PropertyHandle->SetValue( ( uint8 )InPolicy );
}

ECheckBoxState FBYGTextTransformPolicyCustomization::GetCheckState( TSharedRef<IPropertyHandle> PropertyHandle, ETextTransformPolicy InPolicy ) const
{
	uint8 Value;
	if ( PropertyHandle->GetValue( Value ) == FPropertyAccess::Result::Success )
	{
		return ( ETextTransformPolicy )Value == InPolicy ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	return ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

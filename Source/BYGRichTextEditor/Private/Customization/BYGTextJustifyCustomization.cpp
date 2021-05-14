// Copyright Brace Yourself Games. All Rights Reserved.

#include "BYGTextJustifyCustomization.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SCheckBox.h"
#include "DetailWidgetRow.h"


#define LOCTEXT_NAMESPACE "UMG"

void FBYGTextJustifyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FMargin OuterPadding(2);
	const FMargin ContentPadding(2);

	
	HeaderRow
	.IsEnabled(TAttribute<bool>(PropertyHandle, &IPropertyHandle::IsEditable))
	.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(OuterPadding)
		[
			SNew( SCheckBox )
			.Style(FEditorStyle::Get(), "ToggleButtonCheckbox")
			.ToolTipText(LOCTEXT("AlignTextLeft", "Align Text Left"))
			.Padding(ContentPadding)
			.OnCheckStateChanged(this, &FBYGTextJustifyCustomization::HandleCheckStateChanged, PropertyHandle, ETextJustify::Left)
			.IsChecked(this, &FBYGTextJustifyCustomization::GetCheckState, PropertyHandle, ETextJustify::Left)
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("HorizontalAlignment_Left"))
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(OuterPadding)
		[
			SNew(SCheckBox)
			.Style(FEditorStyle::Get(), "ToggleButtonCheckbox")
			.ToolTipText(LOCTEXT("AlignTextCenter", "Align Text Center"))
			.Padding(ContentPadding)
			.OnCheckStateChanged(this, &FBYGTextJustifyCustomization::HandleCheckStateChanged, PropertyHandle, ETextJustify::Center)
			.IsChecked(this, &FBYGTextJustifyCustomization::GetCheckState, PropertyHandle, ETextJustify::Center)
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("HorizontalAlignment_Center"))
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(OuterPadding)
		[
			SNew(SCheckBox)
			.Style(FEditorStyle::Get(), "ToggleButtonCheckbox")
			.ToolTipText(LOCTEXT("AlignTextRight", "Align Text Right"))
			.Padding(ContentPadding)
			.OnCheckStateChanged(this, &FBYGTextJustifyCustomization::HandleCheckStateChanged, PropertyHandle, ETextJustify::Right)
			.IsChecked(this, &FBYGTextJustifyCustomization::GetCheckState, PropertyHandle, ETextJustify::Right)
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("HorizontalAlignment_Right"))
			]
		]
	];
}

void FBYGTextJustifyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

void FBYGTextJustifyCustomization::HandleCheckStateChanged(ECheckBoxState InCheckboxState, TSharedRef<IPropertyHandle> PropertyHandle, ETextJustify::Type ToAlignment)
{
	PropertyHandle->SetValue((uint8)ToAlignment);
}

ECheckBoxState FBYGTextJustifyCustomization::GetCheckState(TSharedRef<IPropertyHandle> PropertyHandle, ETextJustify::Type ForAlignment) const
{
	uint8 Value;
	if ( PropertyHandle->GetValue(Value) == FPropertyAccess::Result::Success)
	{
		return Value == ForAlignment ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}

	return ECheckBoxState::Unchecked;
}

#undef LOCTEXT_NAMESPACE

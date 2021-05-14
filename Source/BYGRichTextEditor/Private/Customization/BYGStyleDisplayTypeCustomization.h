// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"
#include "Settings/BYGStyleDisplayType.h"

class FBYGStyleDisplayTypeCustomization : public IPropertyTypeCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<class IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable( new FBYGStyleDisplayTypeCustomization() );
	}

	FBYGStyleDisplayTypeCustomization()
	{
	}

	virtual void CustomizeHeader( TSharedRef<IPropertyHandle> PropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils ) override;

	virtual void CustomizeChildren( TSharedRef<IPropertyHandle> PropertyHandle, class IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils ) override;

	void HandleCheckStateChanged( ECheckBoxState InCheckboxState, TSharedRef<IPropertyHandle> PropertyHandle, EBYGStyleDisplayType InDisplayType );

	ECheckBoxState GetCheckState( TSharedRef<IPropertyHandle> PropertyHandle, EBYGStyleDisplayType InDisplayType ) const;
};

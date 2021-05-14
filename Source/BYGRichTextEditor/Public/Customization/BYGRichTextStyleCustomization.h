// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "IDetailCustomization.h"

class IPropertyHandle;
class FBYGRichTextStyleCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShared<FBYGRichTextStyleCustomization>();
	}

	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

protected:
	TSharedRef<SWidget> CreatePropertiesMenu( bool bSomething ) const;
	bool GetCanAddProperty() const;
	FReply OnAddPropertyButtonActivated();
	void AddProperty( int32 InHistoryIdx );
	void RemoveProperty( int32 InHistoryIdx );

	//void OnPropertyValueChanged();
	//void OnChildPropertyValueChanged();

	TSharedPtr<IPropertyHandle> PropertyHandle;

	TSharedPtr<class IDetailsView> PropertyView;

	TSharedPtr<class SMenuAnchor> AddPropertyMenuAnchor;

	TWeakObjectPtr<UBYGRichTextStyle> TextStyle;

	TArray<UClass*> GetUnusedProperties() const;

	IDetailLayoutBuilder* MyDetailBuilder;

};



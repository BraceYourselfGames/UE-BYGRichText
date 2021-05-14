// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "IDetailCustomization.h"

// Stylesheet has many styles
//   Style has many properties

class FBYGRichTextStylesheetCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShared<FBYGRichTextStylesheetCustomization>();
	}

	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

protected:
	void OnDeleteStyle( int32 Index );

	FReply OnNewStyleButtonClicked();

	void OnPropertyValueChanged();

	TWeakObjectPtr<class UBYGRichTextStylesheet> TextStylesheet;

	IDetailLayoutBuilder* MyDetailBuilder;

	FSimpleDelegate OnRebuildBlah;

};


// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "Settings/BYGRichTextStylesheet.h"
#include "Settings/BYGRichTextProperty.h"

struct FBYGStyleStack
{
public:

	const TArray<FName>& GetClassesStack() const
	{
		return StyleStackNames;
	}

	TArray<const UBYGRichTextPropertyBase*> GetHeadProperties() const
	{
		TArray<const UBYGRichTextPropertyBase*> Props;
		for ( auto& Pair : PropertiesStack )
		{
			if ( Pair.Value.Num() > 0 )
			{
				Props.Add( Pair.Value.Top() );
			}
		}
		return Props;
	}

	const UBYGRichTextStyle* GetHeadStyle() const
	{
		return StyleStack.Num() > 0 ? StyleStack.Top() : nullptr;
	}

	bool IsEmpty() const { return StyleStack.Num() == 0; }

	void SetRootProperty( const UBYGRichTextPropertyBase* Prop, bool bCanOverride = false )
	{
		if ( bCanOverride )
		{
			PropertiesStack.Remove( Prop->GetTypeID() );
			PropertiesStack.Add( Prop->GetTypeID() ).Add( Prop );
		}
		else
		{
			ensure( !PropertiesStack.Contains( Prop->GetTypeID() ) );
			PropertiesStack.FindOrAdd( Prop->GetTypeID() ).Add( Prop );
		}
	}

	void PushStyle( const UBYGRichTextStyle* Style )
	{
		//ensure( Style->DisplayType == EBYGStyleDisplayType::Inline );
		StyleStack.Add( Style );
		StyleStackNames.Add( Style->GetID() );
		for ( const UBYGRichTextPropertyBase* Prop : Style->Properties )
		{
			if ( !Prop ) continue;
			PropertiesStack.FindOrAdd( Prop->GetTypeID() ).Add( Prop );
		}
	}

	bool CanPopStyle() const
	{
		return StyleStack.Num() >= 1;
	}

	void PopStyle()
	{
		if ( !CanPopStyle() )
			return;

		for ( const UBYGRichTextPropertyBase* Prop : StyleStack.Top()->Properties )
		{
			if ( !Prop ) continue;
			TArray<const UBYGRichTextPropertyBase*>* Props = PropertiesStack.Find( Prop->GetTypeID() );
			if ( ensure( Props ) )
			{
				if ( ensure( Props->Num() > 0 ) )
				{
					if ( ensure( Props->Top() == Prop ) )
					{
						Props->Pop();
					}
				}
			}
		}

		StyleStack.Pop();
		StyleStackNames.Pop();
	}

protected:
	TMap<FName, TArray<const UBYGRichTextPropertyBase*>> PropertiesStack;

	TArray<const UBYGRichTextStyle*> StyleStack;
	TArray<FName> StyleStackNames;
};


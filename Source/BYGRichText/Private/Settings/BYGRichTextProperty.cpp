// Copyright Brace Yourself Games. All Rights Reserved.

#pragma once

#include "Settings/BYGRichTextProperty.h"
#include "Widget/BYGRichTextBlock.h"

UWidget* UBYGRichTextTooltipProperty::CreateTooltip( UBYGRichTextBlock* OuterBlock )
{
	UWidget* Widget = nullptr;
	if ( UserWidgetClass && OuterBlock )
	{
		Widget = Cast<UUserWidget>( UUserWidget::CreateWidgetInstance( *OuterBlock, UserWidgetClass, "Tooltip" ) );
	}

	if ( Widget 
		&& UKismetSystemLibrary::DoesImplementInterface( Widget, UBYGInlineTextTooltip::StaticClass() ) )
	{
		IBYGInlineTextTooltip* TooltipWidget = Cast<IBYGInlineTextTooltip>( Widget );
		FBYGStyleTagData Data;
		TooltipWidget->InitializeTooltip( Data );
	}

	return Widget;
}

void UBYGRichTextPropertyBase::BeginDestroy()
{
	UE_LOG( LogTemp, Warning, TEXT( "RichTextProperty %s is being destroyed!" ), *GetName() );
	Super::BeginDestroy();
}

#if WITH_EDITOR
void UBYGRichTextPropertyBase::PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	OnPropertyPropertyChangedDelegate.ExecuteIfBound();
}

void UBYGRichTextPropertyBase::PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent )
{
	Super::PostEditChangeChainProperty( PropertyChangedEvent );

	OnPropertyPropertyChangedDelegate.ExecuteIfBound();
}
#endif

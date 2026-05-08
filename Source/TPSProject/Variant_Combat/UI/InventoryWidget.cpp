// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/InventoryWidget.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::InitWidget(UInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
	BP_OnInitWidget();
}

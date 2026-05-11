// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/ItemSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/ItemData.h"

void UItemSlotWidget::InitSlot(const FInventoryItem& Item)
{
	if (!Item.DataAsset) return;

	ItemGuid = Item.ItemGuid;

	// 아이콘 설정
	if (UTexture2D* Icon = Item.DataAsset->ItemData.Icon)
	{
		ItemIcon->SetBrushFromTexture(Icon);
	}

	// 수량 텍스트 (MaxStackSize > 1인 아이템만 표시)
	if (StackCount)
	{
		if (Item.DataAsset->ItemData.MaxStackSize > 1)
		{
			StackCount->SetText(FText::AsNumber(Item.StackCount));
			StackCount->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			StackCount->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	BP_OnInitSlot(Item);
}

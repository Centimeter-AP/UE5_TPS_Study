// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/InventoryGridWidget.h"
#include "UI/ItemSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UInventoryGridWidget::BuildGrid(int32 Width, int32 Height)
{
	if (!GridPanel || !CellClass) return;

	GridPanel->ClearChildren();
	GridPanel->SetMinDesiredSlotWidth(CellSize);
	GridPanel->SetMinDesiredSlotHeight(CellSize);
	GridPanel->SetSlotPadding(FMargin(1.0f));

	for (int32 Row = 0; Row < Height; ++Row)
	{
		for (int32 Col = 0; Col < Width; ++Col)
		{
			UUserWidget* Cell = CreateWidget<UUserWidget>(this, CellClass);
			if (!Cell) continue;

			UUniformGridSlot* GridSlot = GridPanel->AddChildToUniformGrid(Cell, Row, Col);
			if (GridSlot)
			{
				GridSlot->SetHorizontalAlignment(HAlign_Fill);
				GridSlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}
}

void UInventoryGridWidget::RefreshItems(const TArray<FInventoryItem>& Items)
{
	if (!ItemCanvas || !ItemSlotClass) return;

	// 기존 슬롯 제거
	for (UItemSlotWidget* ExistingSlot : ItemSlots)
	{
		if (ExistingSlot) ExistingSlot->RemoveFromParent();
	}
	ItemSlots.Empty();

	// 아이템마다 슬롯 생성 후 CanvasPanel에 절대 좌표로 배치
	for (const FInventoryItem& Item : Items)
	{
		if (!Item.DataAsset) continue;

		UItemSlotWidget* SlotWidget = CreateWidget<UItemSlotWidget>(this, ItemSlotClass);
		if (!SlotWidget) continue;

		SlotWidget->InitSlot(Item);

		UCanvasPanelSlot* ItemCanvasSlot = ItemCanvas->AddChildToCanvas(SlotWidget);
		if (ItemCanvasSlot)
		{
			FIntPoint Size = Item.GetEffectiveSize();
			float PaddedCell = CellSize + 2.0f; // SlotPadding(1px) 양쪽 보정

			ItemCanvasSlot->SetPosition(FVector2D(Item.Position.X * PaddedCell, Item.Position.Y * PaddedCell));
			ItemCanvasSlot->SetSize(FVector2D(Size.X * PaddedCell, Size.Y * PaddedCell));
			ItemCanvasSlot->SetAutoSize(false);
		}

		ItemSlots.Add(SlotWidget);
	}
}

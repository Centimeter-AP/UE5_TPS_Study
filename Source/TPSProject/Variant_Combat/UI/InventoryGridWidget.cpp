// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/InventoryGridWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

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

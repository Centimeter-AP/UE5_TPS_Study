// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	FInventoryTab DefaultTab;
	DefaultTab.TabName = FText::FromString(TEXT("인벤토리"));
	Tabs.Add(DefaultTab);
}

int32 UInventoryComponent::TryAddItem(UItemDataAsset* DataAsset, int32 Count, int32 TabIndex)
{
	if (!DataAsset || Count <= 0 || !Tabs.IsValidIndex(TabIndex)) return 0;

	const FItemData& Data = DataAsset->ItemData;
	int32 Remaining = Count;

	// 1x1 스택 가능 아이템은 기존 슬롯에 먼저 합산
	if (Data.GridSize == FIntPoint(1, 1) && Data.MaxStackSize > 1)
	{
		for (FInventoryItem& Item : Tabs[TabIndex].Items)
		{
			if (Item.DataAsset == DataAsset && Item.StackCount < Data.MaxStackSize)
			{
				int32 Space = Data.MaxStackSize - Item.StackCount;
				int32 ToAdd = FMath::Min(Space, Remaining);
				Item.StackCount += ToAdd;
				Remaining -= ToAdd;
				if (Remaining == 0) break;
			}
		}
	}

	// 남은 수량을 새 슬롯에 배치
	while (Remaining > 0)
	{
		FIntPoint Pos;
		if (!FindFreePosition(TabIndex, Data.GridSize, Pos)) break;

		FInventoryItem NewItem;
		NewItem.DataAsset = DataAsset;
		NewItem.StackCount = FMath::Min(Remaining, Data.MaxStackSize);
		NewItem.Position = Pos;
		NewItem.bIsRotated = false;
		NewItem.ItemGuid = FGuid::NewGuid();
		Tabs[TabIndex].Items.Add(NewItem);
		Remaining -= NewItem.StackCount;
	}

	int32 Added = Count - Remaining;
	if (Added > 0) OnInventoryChanged.Broadcast();
	return Added;
}

int32 UInventoryComponent::RemoveItem(FGuid ItemGuid, int32 Count)
{
	for (FInventoryTab& Tab : Tabs)
	{
		FInventoryItem* Item = FindItemInTab(Tab, ItemGuid);
		if (!Item) continue;

		int32 ToRemove = FMath::Min(Count, Item->StackCount);
		Item->StackCount -= ToRemove;
		if (Item->StackCount <= 0)
		{
			Tab.Items.RemoveAll([&ItemGuid](const FInventoryItem& I) { return I.ItemGuid == ItemGuid; });
		}
		OnInventoryChanged.Broadcast();
		return ToRemove;
	}
	return 0;
}

bool UInventoryComponent::MoveItem(FGuid ItemGuid, int32 TargetTabIndex, FIntPoint TargetPosition, bool bRotated)
{
	if (!Tabs.IsValidIndex(TargetTabIndex)) return false;

	int32 SourceTabIndex = -1;
	for (int32 i = 0; i < Tabs.Num(); ++i)
	{
		if (FindItemInTab(Tabs[i], ItemGuid))
		{
			SourceTabIndex = i;
			break;
		}
	}
	if (SourceTabIndex == -1) return false;

	FInventoryItem* Item = FindItemInTab(Tabs[SourceTabIndex], ItemGuid);
	const FIntPoint& Base = Item->DataAsset->ItemData.GridSize;
	FIntPoint NewSize = bRotated ? FIntPoint(Base.Y, Base.X) : Base;

	// 같은 탭 내 이동이면 자기 자신을 제외하고 충돌 검사
	FGuid ExcludeGuid = (SourceTabIndex == TargetTabIndex) ? ItemGuid : FGuid();
	if (!IsAreaFree(TargetTabIndex, TargetPosition, NewSize, ExcludeGuid)) return false;

	if (SourceTabIndex == TargetTabIndex)
	{
		Item->Position = TargetPosition;
		Item->bIsRotated = bRotated;
	}
	else
	{
		FInventoryItem Copy = *Item;
		Copy.Position = TargetPosition;
		Copy.bIsRotated = bRotated;
		Tabs[SourceTabIndex].Items.RemoveAll([&ItemGuid](const FInventoryItem& I) { return I.ItemGuid == ItemGuid; });
		Tabs[TargetTabIndex].Items.Add(Copy);
	}

	OnInventoryChanged.Broadcast();
	return true;
}

void UInventoryComponent::ExpandGrid(int32 NewWidth, int32 NewHeight)
{
	if (NewWidth <= GridWidth && NewHeight <= GridHeight) return;
	GridWidth = FMath::Max(GridWidth, NewWidth);
	GridHeight = FMath::Max(GridHeight, NewHeight);
	OnGridResized.Broadcast(GridWidth, GridHeight);
}

int32 UInventoryComponent::AddBagTab(FGuid BagItemGuid, FText BagName, UTexture2D* BagIcon)
{
	for (const FInventoryTab& Tab : Tabs)
	{
		if (Tab.SourceBagGuid == BagItemGuid) return -1;
	}

	FInventoryTab NewTab;
	NewTab.TabName = BagName;
	NewTab.TabIcon = BagIcon;
	NewTab.SourceBagGuid = BagItemGuid;
	Tabs.Add(NewTab);

	int32 NewIndex = Tabs.Num() - 1;
	OnTabAdded.Broadcast(NewIndex);
	return NewIndex;
}

void UInventoryComponent::RemoveBagTab(int32 TabIndex)
{
	// 인덱스 0(기본 탭)은 제거 불가
	if (TabIndex <= 0 || !Tabs.IsValidIndex(TabIndex)) return;
	Tabs.RemoveAt(TabIndex);
	OnTabRemoved.Broadcast(TabIndex);
}

FInventoryItem* UInventoryComponent::FindItem(FGuid ItemGuid)
{
	for (FInventoryTab& Tab : Tabs)
	{
		FInventoryItem* Found = FindItemInTab(Tab, ItemGuid);
		if (Found) return Found;
	}
	return nullptr;
}

bool UInventoryComponent::IsAreaFree(int32 TabIndex, FIntPoint TopLeft, FIntPoint Size, FGuid ExcludeGuid) const
{
	if (TopLeft.X < 0 || TopLeft.Y < 0) return false;
	if (TopLeft.X + Size.X > GridWidth || TopLeft.Y + Size.Y > GridHeight) return false;

	for (const FInventoryItem& Item : Tabs[TabIndex].Items)
	{
		if (Item.ItemGuid == ExcludeGuid) continue;

		FIntPoint ItemSize = Item.GetEffectiveSize();
		bool bOverlapX = TopLeft.X < Item.Position.X + ItemSize.X && TopLeft.X + Size.X > Item.Position.X;
		bool bOverlapY = TopLeft.Y < Item.Position.Y + ItemSize.Y && TopLeft.Y + Size.Y > Item.Position.Y;
		if (bOverlapX && bOverlapY) return false;
	}
	return true;
}

bool UInventoryComponent::FindFreePosition(int32 TabIndex, FIntPoint Size, FIntPoint& OutPosition) const
{
	for (int32 Y = 0; Y <= GridHeight - Size.Y; ++Y)
	{
		for (int32 X = 0; X <= GridWidth - Size.X; ++X)
		{
			if (IsAreaFree(TabIndex, FIntPoint(X, Y), Size))
			{
				OutPosition = FIntPoint(X, Y);
				return true;
			}
		}
	}
	return false;
}

FInventoryItem* UInventoryComponent::FindItemInTab(FInventoryTab& Tab, FGuid ItemGuid)
{
	for (FInventoryItem& Item : Tab.Items)
	{
		if (Item.ItemGuid == ItemGuid) return &Item;
	}
	return nullptr;
}

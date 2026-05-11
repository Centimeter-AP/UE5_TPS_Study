// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryComponent.h"
#include "ItemSlotWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 인벤토리 그리드 위에 올라가는 아이템 슬롯 위젯의 C++ 기반 클래스.
 * 아이템 아이콘과 스택 수량을 표시한다.
 * 크기와 위치는 UInventoryGridWidget::RefreshItems()에서 설정한다.
 */
UCLASS(abstract)
class TPSPROJECT_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 아이템 인스턴스 데이터로 슬롯을 초기화한다 */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void InitSlot(const FInventoryItem& Item);

	UFUNCTION(BlueprintPure, Category="Inventory")
	FGuid GetItemGuid() const { return ItemGuid; }

protected:
	/** 아이콘 이미지 (Blueprint 자식에서 이 이름으로 배치) */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemIcon;

	/** 스택 수량 텍스트 (Blueprint 자식에서 이 이름으로 배치) */
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> StackCount;

	/** InitSlot 이후 Blueprint에서 추가 처리가 필요할 때 오버라이드 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory")
	void BP_OnInitSlot(const FInventoryItem& Item);

private:
	FGuid ItemGuid;
};

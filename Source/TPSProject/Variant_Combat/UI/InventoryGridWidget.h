// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryComponent.h"
#include "InventoryGridWidget.generated.h"

class UUniformGridPanel;
class UCanvasPanel;
class UItemSlotWidget;

/**
 * 인벤토리 그리드를 렌더링하는 위젯의 C++ 기반 클래스.
 * BuildGrid() 호출 시 GridPanel에 CellClass 위젯을 Width x Height개 채운다.
 * 레이아웃은 Blueprint 자식(WBP_InventoryGrid)에서 구성한다.
 */
UCLASS(abstract)
class TPSPROJECT_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 그리드를 Width x Height 크기로 구성한다.
	 * 기존 셀을 모두 제거하고 새로 채운다.
	 */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void BuildGrid(int32 Width, int32 Height);

	/** 각 셀에 사용할 위젯 클래스 (WBP_GridCell 할당) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TSubclassOf<UUserWidget> CellClass;

	/** 아이템 슬롯에 사용할 위젯 클래스 (WBP_ItemSlot 할당) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TSubclassOf<UItemSlotWidget> ItemSlotClass;

	/** 셀 하나의 픽셀 크기 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin=10))
	float CellSize = 50.0f;

	/**
	 * 현재 탭의 아이템 목록을 읽어 슬롯을 CanvasPanel 위에 배치한다.
	 * OnInventoryChanged 델리게이트에 바인딩해서 자동 갱신한다.
	 */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void RefreshItems(const TArray<FInventoryItem>& Items);

protected:
	/** Blueprint 자식에서 이 이름으로 UniformGridPanel을 배치해야 한다 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel;

	/** Blueprint 자식에서 이 이름으로 CanvasPanel을 배치해야 한다 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> ItemCanvas;

private:
	TArray<TObjectPtr<UItemSlotWidget>> ItemSlots;
};

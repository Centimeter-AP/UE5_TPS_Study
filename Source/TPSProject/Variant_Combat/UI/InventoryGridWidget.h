// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridWidget.generated.h"

class UUniformGridPanel;

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

	/** 셀 하나의 픽셀 크기 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin=10))
	float CellSize = 50.0f;

protected:
	/** Blueprint 자식에서 이 이름으로 UniformGridPanel을 배치해야 한다 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> GridPanel;
};

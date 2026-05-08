// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;

/**
 * 인벤토리 UI의 C++ 기반 클래스.
 * 레이아웃은 Blueprint 자식(WBP_Inventory)에서 구성하고,
 * 컴포넌트 참조와 델리게이트 바인딩은 여기서 관리한다.
 */
UCLASS(abstract)
class TPSPROJECT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 캐릭터가 위젯 생성 직후 호출해 컴포넌트를 연결한다 */
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void InitWidget(UInventoryComponent* InInventoryComponent);

	UFUNCTION(BlueprintPure, Category="Inventory")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:
	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	/** InitWidget 호출 후 Blueprint에서 추가 초기화가 필요할 때 오버라이드 */
	UFUNCTION(BlueprintImplementableEvent, Category="Inventory")
	void BP_OnInitWidget();
};

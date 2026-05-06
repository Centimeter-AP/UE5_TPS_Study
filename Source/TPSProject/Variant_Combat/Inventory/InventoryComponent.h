// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

/** 인벤토리에 실제로 존재하는 아이템 인스턴스 */
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UItemDataAsset> DataAsset;

	UPROPERTY(BlueprintReadOnly)
	int32 StackCount = 1;

	/** 탭 그리드 내 좌상단 좌표 */
	UPROPERTY(BlueprintReadOnly)
	FIntPoint Position = FIntPoint(0, 0);

	/** true이면 GridSize를 (Y, X)로 해석 */
	UPROPERTY(BlueprintReadOnly)
	bool bIsRotated = false;

	/** 아이템 인스턴스 고유 ID */
	UPROPERTY(BlueprintReadOnly)
	FGuid ItemGuid;

	bool IsValid() const { return DataAsset != nullptr && ItemGuid.IsValid(); }

	/** 회전 여부를 반영한 실제 점유 크기 반환 */
	FIntPoint GetEffectiveSize() const
	{
		if (!DataAsset) return FIntPoint(1, 1);
		const FIntPoint& Size = DataAsset->ItemData.GridSize;
		return bIsRotated ? FIntPoint(Size.Y, Size.X) : Size;
	}
};

/** 인벤토리 탭 하나 (기본 인벤토리 또는 가방) */
USTRUCT(BlueprintType)
struct FInventoryTab
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FText TabName;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> TabIcon;

	/** 이 탭을 생성한 가방 아이템의 Guid (기본 탭은 Invalid) */
	UPROPERTY(BlueprintReadOnly)
	FGuid SourceBagGuid;

	UPROPERTY(BlueprintReadOnly)
	TArray<FInventoryItem> Items;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabAdded, int32, TabIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabRemoved, int32, TabIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGridResized, int32, NewWidth, int32, NewHeight);

/**
 * 탭 기반 그리드 인벤토리 컴포넌트.
 * 모든 탭은 GridWidth x GridHeight 크기를 공유하며,
 * 가방 아이템 획득 시 탭이 추가되고 드롭 시 탭이 제거된다.
 */
UCLASS(ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class TPSPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	/** 모든 탭이 공유하는 그리드 가로 칸 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	int32 GridWidth = 10;

	/** 모든 탭이 공유하는 그리드 세로 칸 수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	int32 GridHeight = 10;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTabAdded OnTabAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTabRemoved OnTabRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnGridResized OnGridResized;

	/**
	 * 지정 탭에 아이템을 Count개 추가한다.
	 * 1x1 스택 가능 아이템은 기존 스택에 먼저 합산하고, 남으면 새 슬롯에 배치.
	 * @return 실제로 추가된 수량
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 TryAddItem(UItemDataAsset* DataAsset, int32 Count = 1, int32 TabIndex = 0);

	/**
	 * 지정 Guid 아이템에서 Count개 제거한다.
	 * @return 실제로 제거된 수량
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItem(FGuid ItemGuid, int32 Count = 1);

	/**
	 * 아이템을 TargetTabIndex의 TargetPosition으로 이동한다.
	 * 탭 간 이동도 지원. 대상 위치가 막혀있으면 false 반환.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool MoveItem(FGuid ItemGuid, int32 TargetTabIndex, FIntPoint TargetPosition, bool bRotated);

	/**
	 * 그리드를 NewWidth x NewHeight로 확장한다. 축소는 불가.
	 * 모든 탭에 즉시 반영된다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ExpandGrid(int32 NewWidth, int32 NewHeight);

	/**
	 * 가방 아이템 획득 시 탭을 추가한다. 같은 가방 Guid의 탭이 이미 있으면 -1 반환.
	 * @return 새 탭의 인덱스, 실패 시 -1
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 AddBagTab(FGuid BagItemGuid, FText BagName, UTexture2D* BagIcon);

	/**
	 * 가방 탭을 제거한다. 탭 내 아이템은 모두 소멸한다.
	 * 기본 탭(인덱스 0)은 제거 불가.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveBagTab(int32 TabIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FInventoryTab>& GetTabs() const { return Tabs; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetTabCount() const { return Tabs.Num(); }

	/** 전체 탭에서 Guid로 아이템을 찾아 포인터 반환. 없으면 nullptr. */
	FInventoryItem* FindItem(FGuid ItemGuid);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<FInventoryTab> Tabs;

	/** TopLeft부터 Size 영역이 비어있는지 확인. ExcludeGuid 아이템은 무시. */
	bool IsAreaFree(int32 TabIndex, FIntPoint TopLeft, FIntPoint Size, FGuid ExcludeGuid = FGuid()) const;

	/** 탭에서 Size를 배치할 수 있는 첫 번째 빈 위치를 찾는다. */
	bool FindFreePosition(int32 TabIndex, FIntPoint Size, FIntPoint& OutPosition) const;

	FInventoryItem* FindItemInTab(FInventoryTab& Tab, FGuid ItemGuid);
};

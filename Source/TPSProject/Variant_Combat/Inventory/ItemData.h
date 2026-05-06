// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable	UMETA(DisplayName = "소모품"),
	Material	UMETA(DisplayName = "재료"),
	Equipment	UMETA(DisplayName = "장착 아이템"),
	Bag			UMETA(DisplayName = "가방"),
	Quest		UMETA(DisplayName = "퀘스트 아이템"),
};

/**
 * 아이템의 기본 정보를 담는 구조체
 * UItemDataAsset 안에 포함되어 에디터에서 편집됩니다.
 */
USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	/** 아이템 고유 식별자 (예: "item_health_potion") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemID;

	/** 인벤토리에 표시될 아이템 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	/** 아이템 설명 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription;

	/** 아이템 종류 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EItemType ItemType = EItemType::Consumable;

	/** 인벤토리 슬롯에 표시할 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon;

	/** 그리드에서 아이템이 차지하는 칸 크기 (X = 가로, Y = 세로) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (ClampMin = 1, ClampMax = 10))
	FIntPoint GridSize = FIntPoint(1, 1);

	/**
	 * 최대 스택 수. GridSize가 1x1인 아이템만 스택을 허용합니다.
	 * 1로 설정하면 스택 불가.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = 1, ClampMax = 99))
	int32 MaxStackSize = 1;

	/** false이면 인벤토리에서 드롭 불가 (퀘스트 아이템 등) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool bIsDroppable = true;
};

/**
 * 에디터에서 아이템을 개별 .uasset 파일로 만들 수 있는 DataAsset
 * 우클릭 -> Miscellaneous -> Data Asset -> UItemDataAsset 으로 생성
 */
UCLASS(BlueprintType)
class UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FItemData ItemData;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Item", ItemData.ItemID);
	}
};

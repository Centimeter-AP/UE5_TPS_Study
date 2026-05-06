// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UItemDataAsset;
class ACombatCharacter;

/**
 * 월드에 배치하는 픽업 가능 아이템 액터.
 * 플레이어가 PickupRadius 안에 들어온 뒤 F키를 누르면 인벤토리에 추가된다.
 * 수량이 0이 되면 자동으로 Destroy된다.
 */
UCLASS()
class TPSPROJECT_API AItemPickup : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

	/** 이 범위 안에 들어온 플레이어가 F키로 픽업 가능 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* PickupRadius;

public:
	AItemPickup();

	/** 이 액터가 나타내는 아이템 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<UItemDataAsset> DataAsset;

	/** 현재 남은 수량 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup", meta=(ClampMin=1))
	int32 Count = 1;

	/**
	 * 캐릭터의 인벤토리에 아이템을 이전한다. CombatCharacter::DoPickup에서 호출.
	 * 전량 이전되면 액터를 Destroy한다.
	 */
	void TryGiveToCharacter(ACombatCharacter* Character);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPickupRadiusBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPickupRadiusEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

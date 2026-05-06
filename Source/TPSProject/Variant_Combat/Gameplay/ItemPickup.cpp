// Copyright Epic Games, Inc. All Rights Reserved.

#include "Gameplay/ItemPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CombatCharacter.h"
#include "Inventory/InventoryComponent.h"

AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	PickupRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickupRadius"));
	PickupRadius->SetupAttachment(RootComponent);
	PickupRadius->SetSphereRadius(100.0f);
	PickupRadius->SetCollisionProfileName(TEXT("Trigger"));
}

void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	PickupRadius->OnComponentBeginOverlap.AddDynamic(this, &AItemPickup::OnPickupRadiusBeginOverlap);
	PickupRadius->OnComponentEndOverlap.AddDynamic(this, &AItemPickup::OnPickupRadiusEndOverlap);
}

void AItemPickup::TryGiveToCharacter(ACombatCharacter* Character)
{
	if (!Character || !DataAsset) return;

	UInventoryComponent* Inv = Character->GetInventory();
	if (!Inv) return;

	int32 Added = Inv->TryAddItem(DataAsset, Count);
	Count -= Added;

	if (Count <= 0)
	{
		Destroy();
	}
}

void AItemPickup::OnPickupRadiusBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACombatCharacter* Character = Cast<ACombatCharacter>(OtherActor))
	{
		Character->SetNearbyPickup(this);
	}
}

void AItemPickup::OnPickupRadiusEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACombatCharacter* Character = Cast<ACombatCharacter>(OtherActor))
	{
		Character->SetNearbyPickup(nullptr);
	}
}

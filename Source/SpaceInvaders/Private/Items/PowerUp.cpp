#include "Items/PowerUp.h"
#include "Ships/PlayerPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"

APowerUp::APowerUp()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionBox->SetGenerateOverlapEvents(true);
	RootComponent = CollisionBox;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);
}

void APowerUp::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpanDuration);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &APowerUp::OnOverlapBegin);
}

void APowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldOffset(FVector(-MoveSpeed * DeltaTime, 0.f, 0.f));
	MeshComponent->AddLocalRotation(FRotator(XSpinRate * DeltaTime, 0.f, ZSpinRate * DeltaTime));
}

void APowerUp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerPawn* Player = Cast<APlayerPawn>(OtherActor);
	if (!Player) return;

	Player->BoostFireRate(FireRateBoost);

	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}

	if (PickupSound)
	{
		const float Volume = ASoundManager::Get(GetWorld()) ? ASoundManager::Get(GetWorld())->GetSFXVolume() : 1.f;
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation(), Volume);
	}

	Destroy();
}

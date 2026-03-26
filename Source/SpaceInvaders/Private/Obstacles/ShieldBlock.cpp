#include "Obstacles/ShieldBlock.h"
#include "Obstacles/ShieldTile.h"
#include "Components/SceneComponent.h"

AShieldBlock::AShieldBlock()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
}

void AShieldBlock::BeginPlay()
{
	Super::BeginPlay();
	SpawnTiles();
}

void AShieldBlock::SpawnTiles()
{
	if (!TileClass) return;

	UWorld* World = GetWorld();
	if (!World) return;

	Tiles.Empty();
	Tiles.Reserve(Rows * Columns);

	// Centre the grid on the actor's origin
	const float HalfWidth  = (Columns - 1) * TileSize * 0.5f;
	const float HalfHeight = (Rows    - 1) * TileSize * 0.5f;

	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Columns; ++Col)
		{
			const FVector LocalOffset(
				-(Row * TileSize - HalfHeight),  // X: Row 0 at front
				Col * TileSize - HalfWidth,       // Y: columns step right
				0.f
			);

			const FVector SpawnLocation = GetActorLocation() + FVector(OffsetX, OffsetY, OffsetZ) + LocalOffset;

			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AShieldTile* Tile = World->SpawnActor<AShieldTile>(TileClass, SpawnLocation, FRotator::ZeroRotator, Params);
			Tiles.Add(Tile);
		}
	}
}

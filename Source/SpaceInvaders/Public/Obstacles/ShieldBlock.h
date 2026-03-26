#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldBlock.generated.h"

class AShieldTile;

UCLASS()
class SPACEINVADERS_API AShieldBlock : public AActor
{
	GENERATED_BODY()

public:
	AShieldBlock();

protected:
	virtual void BeginPlay() override;

private:
	void SpawnTiles();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRoot;

	// Flat array — access via Tiles[Row * Columns + Col]
	UPROPERTY()
	TArray<AShieldTile*> Tiles;

	UPROPERTY(EditAnywhere, Category = "Shield")
	int32 Columns = 4;

	UPROPERTY(EditAnywhere, Category = "Shield")
	int32 Rows = 3;

	// World-space gap between tile centres
	UPROPERTY(EditAnywhere, Category = "Shield")
	float TileSize = 40.f;

	// Offset applied to the grid origin relative to the actor's location
	UPROPERTY(EditAnywhere, Category = "Shield")
	float OffsetX = 0.f;

	UPROPERTY(EditAnywhere, Category = "Shield")
	float OffsetY = 0.f;

	UPROPERTY(EditAnywhere, Category = "Shield")
	float OffsetZ = 0.f;

	// Tile Blueprint to spawn — must be a subclass of AShieldTile
	UPROPERTY(EditAnywhere, Category = "Shield")
	TSubclassOf<AShieldTile> TileClass;
};

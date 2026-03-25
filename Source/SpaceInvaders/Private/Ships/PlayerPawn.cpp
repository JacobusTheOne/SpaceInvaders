#include "Ships/PlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ShootingComponent.h"
#include "Ships/EnemyBase.h"
#include "GameState/SpaceInvaderGameState.h"
#include "GameModes/SpaceInvaderGameModeBase.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

APlayerPawn::APlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RootComponent = MeshComponent;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetCollisionProfileName(TEXT("Player"));
    CollisionComponent->SetupAttachment(RootComponent);
    CollisionComponent->SetGenerateOverlapEvents(true);

    ShootingComponent = CreateDefaultSubobject<UShootingComponent>(TEXT("ShootingComponent"));
    ShootingComponent->SetupAttachment(RootComponent);
}

void APlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerPawn::OnCollisionOverlap);

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(MappingContext, 0);
        }
    }
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnMove);
        EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerPawn::OnMoveStop);
        EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnFire);
        EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerPawn::OnFireStop);
    }
}

void APlayerPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!CurrentMoveInput.IsNearlyZero())
    {
        FVector MoveDir = FVector(CurrentMoveInput.Y, CurrentMoveInput.X, 0.f).GetSafeNormal();
        FVector NewLocation = GetActorLocation() + MoveDir * MoveSpeed * DeltaTime;
        NewLocation.X = FMath::Clamp(NewLocation.X, BoundBottom, BoundTop);
        NewLocation.Y = FMath::Clamp(NewLocation.Y, BoundLeft, BoundRight);
        SetActorLocation(NewLocation);
    }
}

void APlayerPawn::OnMove(const FInputActionValue& Value)
{
    CurrentMoveInput = Value.Get<FVector2D>();
}

void APlayerPawn::OnMoveStop()
{
    CurrentMoveInput = FVector2D(0);
}

void APlayerPawn::OnFire()
{
    if (ShootingComponent)
    {
        ShootingComponent->TryFire();
    }
}

void APlayerPawn::OnFireStop()
{
    //Might not be needed
}

void APlayerPawn::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (Cast<AEnemyBase>(OtherActor))
    {
        OtherActor->Destroy();
        TakeDamage(1.f, FDamageEvent(), nullptr, OtherActor);
    }
}

float APlayerPawn::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f;
    bIsDead = true;

    ASpaceInvaderGameState* GS = GetWorld()->GetGameState<ASpaceInvaderGameState>();
    ASpaceInvaderGameModeBase* GM = GetWorld()->GetAuthGameMode<ASpaceInvaderGameModeBase>();

    if (GS)
    {
        GS->LoseLife();
        UE_LOG(LogTemp, Log, TEXT("PlayerPawn: life lost — lives remaining: %d"), GS->GetLives());

        if (GS->GetLives() > 0)
        {
            if (GM)
            {
                GM->RespawnPlayer(GetController());
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerPawn: no lives remaining — restarting level"));
            UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetMapName()));
        }
    }

    Destroy();
    return DamageAmount;
}

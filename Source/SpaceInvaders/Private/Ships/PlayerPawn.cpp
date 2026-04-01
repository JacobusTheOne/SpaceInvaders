#include "Ships/PlayerPawn.h"
#include "Components/WidgetComponent.h"
#include "UI/StrafeCooldownWidget.h"
#include "UI/DeflectCooldownWidget.h"
#include "Components/PlayerSFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ShootingComponent.h"
#include "Components/Projectile.h"
#include "Ships/EnemyBase.h"
#include "GameState/SpaceInvaderGameState.h"
#include "GameModes/SpaceInvaderGameModeBase.h"
#include "Managers/WaveManager.h"
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

    SFXComponent = CreateDefaultSubobject<UPlayerSFXComponent>(TEXT("SFXComponent"));

    StrafeIndicatorComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StrafeIndicatorComponent"));
    StrafeIndicatorComponent->SetupAttachment(RootComponent);
    StrafeIndicatorComponent->SetWidgetSpace(EWidgetSpace::Screen);
    StrafeIndicatorComponent->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
    StrafeIndicatorComponent->SetDrawSize(FVector2D(100.f, 20.f));

    DeflectIndicatorComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeflectIndicatorComponent"));
    DeflectIndicatorComponent->SetupAttachment(RootComponent);
    DeflectIndicatorComponent->SetWidgetSpace(EWidgetSpace::Screen);
    DeflectIndicatorComponent->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    DeflectIndicatorComponent->SetDrawSize(FVector2D(100.f, 20.f));

    DeflectSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeflectSphere"));
    DeflectSphere->SetupAttachment(RootComponent);
    DeflectSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DeflectSphere->SetRelativeScale3D(DeflectSphereScale);
    DeflectSphere->SetHiddenInGame(true);
}

void APlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerPawn::OnCollisionOverlap);

    if (StrafeCooldownWidgetClass)
    {
        StrafeIndicatorComponent->SetWidgetClass(StrafeCooldownWidgetClass);
    }

    if (DeflectCooldownWidgetClass)
    {
        DeflectIndicatorComponent->SetWidgetClass(DeflectCooldownWidgetClass);
    }

    if (ASpaceInvaderGameState* GS = GetWorld()->GetGameState<ASpaceInvaderGameState>())
    {
        FireRate += GS->GetFireRateBoost();
    }
    ShootingComponent->FireRate = FireRate;

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
        if (KillWaveAction)
        {
            EIC->BindAction(KillWaveAction, ETriggerEvent::Started, this, &APlayerPawn::OnKillWave);
        }
        if (StrafeAction)
        {
            EIC->BindAction(StrafeAction, ETriggerEvent::Started, this, &APlayerPawn::OnStrafe);
        }
        if (DeflectAction)
        {
            EIC->BindAction(DeflectAction, ETriggerEvent::Started, this, &APlayerPawn::OnDeflect);
        }
    }
}

void APlayerPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (StrafeCooldownRemaining > 0.f)
    {
        StrafeCooldownRemaining = FMath::Max(0.f, StrafeCooldownRemaining - DeltaTime);
    }

    if (DeflectCooldownRemaining > 0.f)
    {
        DeflectCooldownRemaining = FMath::Max(0.f, DeflectCooldownRemaining - DeltaTime);
    }

    FVector NewLocation = GetActorLocation();

    if (StrafeBoostRemaining > 0.f)
    {
        StrafeBoostRemaining = FMath::Max(0.f, StrafeBoostRemaining - DeltaTime);
        NewLocation.Y += StrafeBoostDir * StrafeBoostSpeed * DeltaTime;
    }
    else if (!CurrentMoveInput.IsNearlyZero())
    {
        const FVector MoveDir = FVector(CurrentMoveInput.Y, CurrentMoveInput.X, 0.f).GetSafeNormal();
        NewLocation += MoveDir * MoveSpeed * DeltaTime;
    }

    NewLocation.X = FMath::Clamp(NewLocation.X, BoundBottom, BoundTop);
    NewLocation.Y = FMath::Clamp(NewLocation.Y, BoundLeft, BoundRight);
    SetActorLocation(NewLocation);

    if (UStrafeCooldownWidget* W = Cast<UStrafeCooldownWidget>(StrafeIndicatorComponent->GetUserWidgetObject()))
    {
        W->UpdateCooldown(StrafeCooldownRemaining, StrafeCooldown);
    }

    if (UDeflectCooldownWidget* W = Cast<UDeflectCooldownWidget>(DeflectIndicatorComponent->GetUserWidgetObject()))
    {
        W->UpdateCooldown(DeflectCooldownRemaining, DeflectCooldown);
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
    if (!ShootingComponent || !ShootingComponent->CanFire()) return;

    TSubclassOf<AProjectile> ProjClass = ShootingComponent->GetProjectileClass();
    if (!ProjClass) return;

    const FTransform SpawnTransform(ShootingComponent->FireDirection.Rotation(),
                                    ShootingComponent->GetComponentLocation());

    AProjectile* Proj = GetWorld()->SpawnActorDeferred<AProjectile>(ProjClass, SpawnTransform, this, GetInstigator());
    if (Proj)
    {
        if (ProjectileInitialSpeed > 0.f) Proj->SetInitialSpeed(ProjectileInitialSpeed);
        if (ProjectileMaxSpeed     > 0.f) Proj->SetMaxSpeed(ProjectileMaxSpeed);
        if (ProjectileLifeSpan     > 0.f) Proj->SetLifeSpanDuration(ProjectileLifeSpan);
        if (ProjectileDamage       > 0.f) Proj->SetDamage(ProjectileDamage);
        Proj->Tags.Add(FName("PlayerProjectile"));
        UGameplayStatics::FinishSpawningActor(Proj, SpawnTransform);
    }

    SFXComponent->PlayShoot();
    ShootingComponent->StartCooldown();
}

void APlayerPawn::OnFireStop()
{
    //Might not be needed
}

void APlayerPawn::ToggleGodMode()
{
    bGodMode = !bGodMode;
    UE_LOG(LogTemp, Warning, TEXT("PlayerPawn: God Mode %s"), bGodMode ? TEXT("ON") : TEXT("OFF"));
}

void APlayerPawn::OnStrafe(const FInputActionValue& Value)
{
    if (!FMath::IsNearlyZero(CurrentMoveInput.X))
    {
        TryStrafe(FMath::Sign(CurrentMoveInput.X));
    }
}

void APlayerPawn::TryStrafe(float Direction)
{
    if (StrafeCooldownRemaining > 0.f) return;

    StrafeBoostDir        = Direction;
    StrafeBoostRemaining  = StrafeBoostDuration;
    StrafeCooldownRemaining = StrafeCooldown;
}

void APlayerPawn::OnKillWave()
{
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), Enemies);
    UE_LOG(LogTemp, Warning, TEXT("Debug KillWave — destroying %d enemies"), Enemies.Num());
    for (AActor* Enemy : Enemies)
    {
        if (IsValid(Enemy)) Enemy->Destroy();
    }
}

void APlayerPawn::BoostFireRate(float AdditionalRate)
{
    FireRate += AdditionalRate;
    ShootingComponent->FireRate = FireRate;
    UE_LOG(LogTemp, Warning, TEXT("PowerUp collected — FireRate boosted by %.2f, new FireRate: %.2f"), AdditionalRate, FireRate);
}

void APlayerPawn::OnDeflect()
{
    TryDeflect();
}

void APlayerPawn::TryDeflect()
{
    UE_LOG(LogTemp, Warning, TEXT("[Deflect] TryDeflect called — bDeflecting=%d, Cooldown=%.2f"),
        bDeflecting, DeflectCooldownRemaining);

    if (bDeflecting || DeflectCooldownRemaining > 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Deflect] TryDeflect BLOCKED (already active or on cooldown)"));
        return;
    }

    bDeflecting = true;
    UE_LOG(LogTemp, Warning, TEXT("[Deflect] ACTIVATED for %.2f seconds"), DeflectDuration);

    DeflectSphere->SetHiddenInGame(false);

    GetWorldTimerManager().SetTimer(
        DeflectTimerHandle, this, &APlayerPawn::EndDeflect, DeflectDuration, false);
}

void APlayerPawn::EndDeflect()
{
    bDeflecting = false;
    UE_LOG(LogTemp, Warning, TEXT("[Deflect] Shield expired — cooldown started (%.2f s)"), DeflectCooldown);

    DeflectSphere->SetHiddenInGame(true);
    DeflectCooldownRemaining = DeflectCooldown;
}

void APlayerPawn::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
    {
        if (Enemy->ShouldDieOnPlayerContact())
        {
            Enemy->Destroy();
        }
        TakeDamage(1.f, FDamageEvent(), nullptr, OtherActor);
    }
}

float APlayerPawn::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    UE_LOG(LogTemp, Warning, TEXT("[Deflect] TakeDamage — bIsDead=%d bGodMode=%d bDeflecting=%d DamageCauser=%s"),
        bIsDead, bGodMode, bDeflecting,
        DamageCauser ? *DamageCauser->GetName() : TEXT("null"));

    if (bIsDead || bGodMode) return 0.f;

    if (bDeflecting)
    {
        AProjectile* Proj = Cast<AProjectile>(DamageCauser);
        UE_LOG(LogTemp, Warning, TEXT("[Deflect] bDeflecting=true — Cast to AProjectile: %s"),
            Proj ? TEXT("SUCCESS") : TEXT("FAILED (not a projectile actor)"));

        if (Proj)
        {
            const bool bHasTag = Proj->ActorHasTag(FName("EnemyProjectile"));
            UE_LOG(LogTemp, Warning, TEXT("[Deflect] Projectile has 'EnemyProjectile' tag: %d"), bHasTag);

            if (bHasTag)
            {
                UE_LOG(LogTemp, Warning, TEXT("[Deflect] Calling PlayerDeflect on %s"), *Proj->GetName());
                Proj->PlayerDeflect(this);  // pass PlayerPawn as new instigator
                return 0.f;
            }
        }
    }

    bIsDead = true;

    SFXComponent->PlayDeath();

    if (ExplosionEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation());
    }

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
            UE_LOG(LogTemp, Log, TEXT("PlayerPawn: no lives remaining — triggering Game Over UI"));
            if (AWaveManager* WM = Cast<AWaveManager>(
                UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass())))
            {
                WM->TriggerGameOver();
            }
        }
    }

    Destroy();
    return DamageAmount;
}

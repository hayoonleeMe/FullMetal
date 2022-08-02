// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
    _CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    _CollisionComp->InitSphereRadius(5.f);
    _CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    _CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

    // Projectile 위에 올라서지 못하도록 설정
    _CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    _CollisionComp->CanCharacterStepUpOn = ECB_No;

    RootComponent = _CollisionComp;

    _ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    _ProjectileMovement->UpdatedComponent = _CollisionComp;
    _ProjectileMovement->InitialSpeed = 12000.f;
    _ProjectileMovement->MaxSpeed = 15000.f;
    _ProjectileMovement->ProjectileGravityScale = 0;

    InitialLifeSpan = 3.f;

    _Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    _Mesh->SetupAttachment(_CollisionComp);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("StaticMesh'/Game/FirstPersonProjectileMesh.FirstPersonProjectileMesh'"));
    if (SM.Succeeded())
    {
        _Mesh->SetStaticMesh(SM.Object);
    }
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Vanguard.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotAnimInstance.h"
#include "Projectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

// Sets default values
AVanguard::AVanguard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// TODO : 콜리전 범위 수정 필요
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Z축을 제외하고 컨트롤러가 회전할 때 캐릭터가 같이 회전하는 것을 방지한다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->JumpZVelocity = 500.f; // 점프 시 시작속도.
	GetCharacterMovement()->AirControl = 0.25f;	// 공중에서 움직일 수 있는 정도, 1이면 지면과 동일하게 이동.
	GetCharacterMovement()->MaxWalkSpeed = 1000.f; // 지면에서 이동 시 최대 속도, 공중에서 움직일 때에도 영향을 끼친다.
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; // 속도를 직접적으로 낮추는 일정한 반대되는 힘이다.

	// 스켈레탈 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/BattleRobot/Mesh/SK_BattleRobot.SK_BattleRobot'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}

	// 스프링암, 카메라 컴포넌트를 부착하고 기본 값을 설정한다.
	_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	_SpringArm->SetupAttachment(GetCapsuleComponent());
	_SpringArm->TargetArmLength = 1000.f;
	// 스프링암을 컨트롤러 로테이션으로 회전시켜 마우스를 회전했을 때 움직이는 컨트롤러의 회전대로 움직인다. 
	// 따라서 마우스를 회전하는대로 스프림암도 회전한다.
	_SpringArm->bUsePawnControlRotation = true;	
	_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 400.f), FRotator(-40.f, 0.f, 0.f));

	_Camera->SetupAttachment(_SpringArm, USpringArmComponent::SocketName);

	// 카메라는 스프링암과 달리 컨트롤러 로테이션대로 회전시키지 않는다.
	// 따라서 마우스를 회전해도 카메라는 계속 캐릭터를 비춘다.
	_Camera->bUsePawnControlRotation = false;	

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	// MuzzleEffect NiagaraSystem Asset 로드
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ME(TEXT("NiagaraSystem'/Game/Effects/N_MuzzleFlash.N_MuzzleFlash'"));
	if (ME.Succeeded())
	{
		_MuzzleEffect = ME.Object;
	}

	// NiagaraComponent 로드
	_LeftMuzzleFlash = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftMuzzleFlash"));
	_RightMuzzleFlash = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightMuzzleFlash"));

	_LeftMuzzleFlash->SetupAttachment(GetMesh());
	_RightMuzzleFlash->SetupAttachment(GetMesh());
	_LeftMuzzleFlash->SetAsset(_MuzzleEffect);
	_RightMuzzleFlash->SetAsset(_MuzzleEffect);

	// TODO : 점프 시 MuzzleEffect가 따라오지 않는 현상 개선 필요

	_LeftMuzzleFlash->SetAutoActivate(false);
	_RightMuzzleFlash->SetAutoActivate(false);

	//// 소환할 Projectile Class 로드
	//static ConstructorHelpers::FClassFinder<AProjectile> PJ(TEXT("Blueprint'/Game/Blueprints/BP_Projectile.BP_Projectile_C'"));
	//if (PJ.Succeeded())
	//{
	//	_ProjectileClass = PJ.Class;
	//}
}

// Called when the game starts or when spawned
void AVanguard::BeginPlay()
{
	Super::BeginPlay();
	
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	_AnimInstance->PlayAwakeMontage();
}

// Called every frame
void AVanguard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AVanguard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AVanguard::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AVanguard::StopFire);

	PlayerInputComponent->BindAxis(TEXT("ForwardBackward"), this,  &AVanguard::ForwardBackward);
	PlayerInputComponent->BindAxis(TEXT("RightLeft"), this, &AVanguard::RightLeft);

	PlayerInputComponent->BindAxis(TEXT("TurnRightLeft"), this, &AVanguard::TurnRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APawn::AddControllerPitchInput);
}

void AVanguard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	_AnimInstance = Cast<URobotAnimInstance>(GetMesh()->GetAnimInstance());

	if (_AnimInstance)
	{
		_AnimInstance->OnMontageEnded.AddDynamic(this, &AVanguard::OnAwakeMontageEnded);
	}
}

void AVanguard::ForwardBackward(float Value)
{
	_ForwardBackwardValue = Value;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVanguard::RightLeft(float Value)
{
	_RightLeftValue = Value;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVanguard::TurnRightLeft(float Value)
{
	if (!_IsAwakeEnded)
		return;

	AddControllerYawInput(Value);
}

void AVanguard::LookUpDown(float Value)
{
	float Angle = Value * 180;

	AddControllerPitchInput(Value);
}

void AVanguard::OnAwakeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName() == TEXT("BattleRobot_Skeleton_Montage"))
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		_IsAwakeEnded = true;
	}
}

bool AVanguard::GunTrace(FHitResult& Hit)
{
	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);
	FVector End = Location + Rotation.Vector() * 15000.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// ECC_GameTraceChannel1 : Camera
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);	
}

void AVanguard::Fire()
{
	FName LeftMuzzleSocket(TEXT("muzzle_L"));
	FName RightMuzzleSocket(TEXT("muzzle_R"));

	FVector LeftMuzzleLocation = GetMesh()->GetSocketLocation(LeftMuzzleSocket);
	FVector RightMuzzleLocation = GetMesh()->GetSocketLocation(RightMuzzleSocket);

	FHitResult Hit;
	FVector ProjectileTarget;

	bool bResult = GunTrace(Hit);
	if (bResult)
	{
		ProjectileTarget = Hit.ImpactPoint;

		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitActor : %s"), *HitActor->GetName());
		}
	}
	else
	{
		ProjectileTarget = Hit.TraceEnd;
	}

	// 총구에서 발사체 목표 위치까지 바라보는 Rotation
	FRotator LeftRotation = UKismetMathLibrary::FindLookAtRotation(LeftMuzzleLocation, ProjectileTarget);
	FRotator RightRotation = UKismetMathLibrary::FindLookAtRotation(RightMuzzleLocation, ProjectileTarget);

	FTransform LeftTransform = UKismetMathLibrary::MakeTransform(LeftMuzzleLocation, LeftRotation);
	FTransform RightTransform = UKismetMathLibrary::MakeTransform(RightMuzzleLocation, RightRotation);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(_ProjectileClass, LeftTransform, ActorSpawnParams);
	GetWorld()->SpawnActor<AActor>(_ProjectileClass, RightTransform, ActorSpawnParams);

	// 발사 소리 재생
	if (_FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _FireSound, GetActorLocation(), .2f);
	}
}

void AVanguard::StartFire()
{
	if (!_IsAwakeEnded)
		return;

	_AnimInstance->PlayFireMontage(true);

	_LeftMuzzleFlash->Activate(true);
	_RightMuzzleFlash->Activate(true);

	Fire();

	GetWorldTimerManager().SetTimer(_TimerHandle_HandleRefire, this, &AVanguard::Fire, _TimeBetweenShots, true);
}

void AVanguard::StopFire()
{
	_AnimInstance->PlayFireMontage(false);

	_LeftMuzzleFlash->Deactivate();
	_RightMuzzleFlash->Deactivate();

	GetWorldTimerManager().ClearTimer(_TimerHandle_HandleRefire);
}
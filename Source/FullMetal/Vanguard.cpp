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
#include "NiagaraFunctionLibrary.h"
#include "MyStatComponent.h"
#include "MyGameInstance.h"

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

	// 스켈레탈 메시 설정
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/BattleRobot/Mesh/SK_BattleRobot.SK_BattleRobot'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}

	// 스프링암, 카메라 컴포넌트를 부착하고 초기화한다.
	_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

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

	// Muzzle Effect NiagaraSystem Asset 로드
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
	_LeftMuzzleFlash->SetAutoActivate(false);
	_RightMuzzleFlash->SetAutoActivate(false);

	// Impact Effect NiagaraSystem Asset 로드
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> IE(TEXT("NiagaraSystem'/Game/Effects/N_Impact_System.N_Impact_System'"));
	if (IE.Succeeded())
	{
		_ImpactEffect = IE.Object;
	}

	// Stat Component 로드
	_Stat = CreateDefaultSubobject<UMyStatComponent>(TEXT("Stat"));
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
	
	// 총 발사할 때 Muzzle Effect의 위치를 총구로 조정
	// Fire()에서 수행할 경우 발사속도에 따라 실제로 게임에서 위치 조정이 적용되는 속도가 달라지므로 Tick에서 수행
	if (_IsShooting)
	{
		_LeftMuzzleFlash->SetWorldLocation(GetMesh()->GetSocketLocation(_LeftMuzzleSocket));
		_RightMuzzleFlash->SetWorldLocation(GetMesh()->GetSocketLocation(_RightMuzzleSocket));
	}
}

// Called to bind functionality to input
void AVanguard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 점프
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	// 총 발사
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AVanguard::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &AVanguard::StopFire);

	// 앞, 뒤, 좌, 우 이동
	PlayerInputComponent->BindAxis(TEXT("ForwardBackward"), this,  &AVanguard::ForwardBackward);
	PlayerInputComponent->BindAxis(TEXT("RightLeft"), this, &AVanguard::RightLeft);

	// 상, 하, 좌, 우 시점 이동
	PlayerInputComponent->BindAxis(TEXT("TurnRightLeft"), this, &AVanguard::TurnRightLeft);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &AVanguard::LookUpDown);
}

void AVanguard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 점프 시 시작속도.
	GetCharacterMovement()->JumpZVelocity = 500.f; 
	// 공중에서 움직일 수 있는 정도, 1이면 지면과 동일하게 이동.
	GetCharacterMovement()->AirControl = 0.25f;	
	// 지면에서 이동 시 최대 속도, 공중에서 움직일 때에도 영향을 끼친다.
	GetCharacterMovement()->MaxWalkSpeed = _Stat->GetSpeed(); 
	// 속도를 직접적으로 낮추는 일정한 반대되는 힘이다.
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; 

	// 발사 딜레이 설정
	auto MyGameInstance = Cast<UMyGameInstance>((UGameplayStatics::GetGameInstance(GetWorld())));
	if (IsValid(MyGameInstance))
	{
		float MaxTimeBetweenShots = MyGameInstance->GetMaxTimeBetweenShots();
		_TimeBetweenShots = MaxTimeBetweenShots + MaxTimeBetweenShots * (1 - _Stat->GetRpmPercent() / 100);
	}

	// 소켓 FName 캐싱
	_LeftMuzzleSocket = TEXT("muzzle_L");
	_RightMuzzleSocket = TEXT("muzzle_R");

	// 애니메이션 클래스 캐싱
	_AnimInstance = Cast<URobotAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(_AnimInstance))
	{
		_AnimInstance->OnMontageEnded.AddDynamic(this, &AVanguard::OnAwakeMontageEnded);
	}
}

void AVanguard::ForwardBackward(float Value)
{
	_ForwardBackwardValue = Value;

	if ((Controller) && (Value != 0.0f))
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVanguard::RightLeft(float Value)
{
	_RightLeftValue = Value;

	if ((Controller) && (Value != 0.0f))
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVanguard::TurnRightLeft(float Value)
{
	if (!_IsAwakeEnded)
		return;

	if ((Controller) && (Value != 0.0f))
	{
		AddControllerYawInput(Value);
	}
}

void AVanguard::LookUpDown(float Value)
{
	if (!_IsAwakeEnded)
		return;

	if ((Controller) && (Value != 0.0f))
	{
		AddControllerPitchInput(Value);

		// 상, 하 시점에 따라 무기 Bone의 회전을 조정하기 위한 변수 업데이트
		if (_IsAwakeEnded)
		{
			_UpDown = FRotator(0, GetController()->GetControlRotation().Pitch * -1, 0);
		}
	}
}

void AVanguard::OnAwakeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName() == TEXT("Awake_Montage"))
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		_IsAwakeEnded = true;
	}
}

float AVanguard::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	_Stat->OnAttacked(DamageAmount);

	return DamageAmount;
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
	// UNiagaraComponent의 회전 조정
	_LeftMuzzleFlash->SetRelativeRotation(FRotator(0, 0, GetController()->GetControlRotation().Pitch * -1));
	_RightMuzzleFlash->SetRelativeRotation(FRotator(0, 0, GetController()->GetControlRotation().Pitch * -1));
	
	FHitResult Hit;
	FVector ProjectileTargetLocation;

	bool bResult = GunTrace(Hit);
	if (bResult)
	{
		// 총알 발사체의 도착위치를 라인트레이스의 충돌지점으로 설정
		ProjectileTargetLocation = Hit.ImpactPoint;

		// TODO : 총알을 맞았을 때의 상호작용
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitActor : %s"), *HitActor->GetName());

			// 총알을 맞은 오브젝트가 데미지를 받을 수 있는 Pawn이면 데미지를 입힌다.
			auto HitPawn = Cast<APawn>(HitActor);
			if (HitPawn)
			{
				FDamageEvent DamageEvent;
				HitActor->TakeDamage(_Stat->GetPrimaryDamage(), DamageEvent, GetController(), this);
			}
		}

		// 총알이 부딪힌 곳에 Impact Effect 생성
		if (IsValid(_ImpactEffect))
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), _ImpactEffect, Hit.ImpactPoint + Hit.ImpactNormal * 5.f, UKismetMathLibrary::MakeRotFromX(Hit.ImpactNormal));
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), _ImpactEffect, Hit.ImpactPoint + Hit.ImpactNormal * 5.f, UKismetMathLibrary::MakeRotFromX(Hit.ImpactNormal));
		}
	}
	else
	{
		// 총알 발사체의 도착위치를 라인트레이스의 가장 끝부분으로 설정
		ProjectileTargetLocation = Hit.TraceEnd;
	}

	// 총구 소켓의 World Location
	FVector LeftMuzzleLocation = GetMesh()->GetSocketLocation(_LeftMuzzleSocket);
	FVector RightMuzzleLocation = GetMesh()->GetSocketLocation(_RightMuzzleSocket);

	// 총구에서 발사체 목표 위치까지 바라보는 Rotation
	FRotator LeftRotation = UKismetMathLibrary::FindLookAtRotation(LeftMuzzleLocation, ProjectileTargetLocation);
	FRotator RightRotation = UKismetMathLibrary::FindLookAtRotation(RightMuzzleLocation, ProjectileTargetLocation);

	FTransform LeftTransform = UKismetMathLibrary::MakeTransform(LeftMuzzleLocation, LeftRotation);
	FTransform RightTransform = UKismetMathLibrary::MakeTransform(RightMuzzleLocation, RightRotation);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// 총알 동선을 따라가는 총알 생성
	GetWorld()->SpawnActor<AActor>(_ProjectileClass, LeftTransform, ActorSpawnParams);
	GetWorld()->SpawnActor<AActor>(_ProjectileClass, RightTransform, ActorSpawnParams);

	// 발사 소리 재생
	if (IsValid(_FireSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, _FireSound, GetActorLocation(), .2f);
	}
}

void AVanguard::StartFire()
{
	if (!_IsAwakeEnded)
		return;

	_IsShooting = true;

	_AnimInstance->PlayFireMontage(true);

	_LeftMuzzleFlash->Activate(true);
	_RightMuzzleFlash->Activate(true);

	Fire();

	GetWorldTimerManager().SetTimer(_TimerHandle_HandleRefire, this, &AVanguard::Fire, _TimeBetweenShots, true);
}

void AVanguard::StopFire()
{
	if (!_IsAwakeEnded)
		return;

	_IsShooting = false;

	_AnimInstance->PlayFireMontage(false);

	_LeftMuzzleFlash->Deactivate();
	_RightMuzzleFlash->Deactivate();

	GetWorldTimerManager().ClearTimer(_TimerHandle_HandleRefire);
}
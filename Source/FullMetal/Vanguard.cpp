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

// Sets default values
AVanguard::AVanguard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// TODO : �ݸ��� ���� ���� �ʿ�
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Z���� �����ϰ� ��Ʈ�ѷ��� ȸ���� �� ĳ���Ͱ� ���� ȸ���ϴ� ���� �����Ѵ�.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->JumpZVelocity = 500.f; // ���� �� ���ۼӵ�.
	GetCharacterMovement()->AirControl = 0.25f;	// ���߿��� ������ �� �ִ� ����, 1�̸� ����� �����ϰ� �̵�.
	GetCharacterMovement()->MaxWalkSpeed = 1000.f; // ���鿡�� �̵� �� �ִ� �ӵ�, ���߿��� ������ ������ ������ ��ģ��.
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f; // �ӵ��� ���������� ���ߴ� ������ �ݴ�Ǵ� ���̴�.

	// ���̷�Ż �޽� ����
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/BattleRobot/Mesh/SK_BattleRobot.SK_BattleRobot'"));
	if (SM.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM.Object);
	}

	// ��������, ī�޶� ������Ʈ�� �����ϰ� �⺻ ���� �����Ѵ�.
	_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	_SpringArm->SetupAttachment(GetCapsuleComponent());
	_SpringArm->TargetArmLength = 1000.f;
	// ���������� ��Ʈ�ѷ� �����̼����� ȸ������ ���콺�� ȸ������ �� �����̴� ��Ʈ�ѷ��� ȸ����� �����δ�. 
	// ���� ���콺�� ȸ���ϴ´�� �������ϵ� ȸ���Ѵ�.
	_SpringArm->bUsePawnControlRotation = true;	
	_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 400.f), FRotator(-40.f, 0.f, 0.f));

	_Camera->SetupAttachment(_SpringArm, USpringArmComponent::SocketName);

	// ī�޶�� �������ϰ� �޸� ��Ʈ�ѷ� �����̼Ǵ�� ȸ����Ű�� �ʴ´�.
	// ���� ���콺�� ȸ���ص� ī�޶�� ��� ĳ���͸� �����.
	_Camera->bUsePawnControlRotation = false;	

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	//// ��ȯ�� Projectile Class �ε�
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

	//_HUD->InitWidget();
}

void AVanguard::ForwardBackward(float Value)
{
	_ForwardBackwardValue = Value;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//// find out which way is forward
		//const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		//// get forward vector
		//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//AddMovementInput(Direction, Value);

		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVanguard::RightLeft(float Value)
{
	_RightLeftValue = Value;

	if ((Controller != nullptr) && (Value != 0.0f))
	{
		//// find out which way is right
		//const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		//// get right vector 
		//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//// add movement in that direction
		//AddMovementInput(Direction, Value);

		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AVanguard::TurnRightLeft(float Value)
{
	if (!_CanTurnHorizontally)
		return;

	AddControllerYawInput(Value);
}

void AVanguard::OnAwakeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName() == TEXT("BattleRobot_Skeleton_Montage"))
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		_CanTurnHorizontally = true;
		_CanFire = true;
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
	if (!_CanFire)
		return;

	FName LeftMuzzleSocket(TEXT("muzzle_L"));
	FName RightMuzzleSocket(TEXT("muzzle_R"));

	FVector LeftMuzzle = GetMesh()->GetSocketLocation(LeftMuzzleSocket);
	FVector RightMuzzle = GetMesh()->GetSocketLocation(RightMuzzleSocket);

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

	// �ѱ����� �߻�ü�� ���������� �ٶ󺸴� Rotation�� ���Ѵ�.
	FRotator LeftRotation = UKismetMathLibrary::FindLookAtRotation(LeftMuzzle, ProjectileTarget);
	FRotator RightRotation = UKismetMathLibrary::FindLookAtRotation(RightMuzzle, ProjectileTarget);

	FTransform LeftTransform = UKismetMathLibrary::MakeTransform(LeftMuzzle, LeftRotation);
	FTransform RightTransform = UKismetMathLibrary::MakeTransform(RightMuzzle, RightRotation);

	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(_ProjectileClass, LeftTransform, ActorSpawnParams);
	GetWorld()->SpawnActor<AActor>(_ProjectileClass, RightTransform, ActorSpawnParams);

	if (_FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _FireSound, GetActorLocation(), .5f);
	}

	// TODO : ����ü ���� �ִϸ��̼� ��� �ʿ�, ����� _FireAnimation ����ȵ�
	if (_FireAnimation != nullptr && _AnimInstance != nullptr)
	{
		_AnimInstance->Montage_Play(_FireAnimation, 1.f);
	}
}

void AVanguard::StartFire()
{
	Fire();

	GetWorldTimerManager().SetTimer(_TimerHandle_HandleRefire, this, &AVanguard::Fire, _TimeBetweenShots, true);
}

void AVanguard::StopFire()
{
	GetWorldTimerManager().ClearTimer(_TimerHandle_HandleRefire);
}
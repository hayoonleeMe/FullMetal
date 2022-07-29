// Fill out your copyright notice in the Description page of Project Settings.


#include "Vanguard.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotAnimInstance.h"

// Sets default values
AVanguard::AVanguard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// ��Ʈ�ѷ��� ȸ���� �� ĳ���Ͱ� ���� ȸ���ϴ� ���� �����Ѵ�.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĳ���Ͱ� �����ϴ� �������� RotationRate�� �ӵ��� ȸ���ϵ��� �Ѵ�.
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetCharacterMovement()->JumpZVelocity = 500.f; // ���� �� ���ۼӵ�.
	GetCharacterMovement()->AirControl = 0.25f;	// ���߿��� ������ �� �ִ� ����, 1�̸� ����� �����ϰ� �̵�.
	GetCharacterMovement()->MaxWalkSpeed = 500.f; // ���鿡�� �̵� �� �ִ� �ӵ�, ���߿��� ������ ������ ������ ��ģ��.
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
	_SpringArm->TargetArmLength = 800.f;
	// ���������� ��Ʈ�ѷ� �����̼����� ȸ������ ���콺�� ȸ������ �� �����̴� ��Ʈ�ѷ��� ȸ����� �����δ�. 
	// ���� ���콺�� ȸ���ϴ´�� �������ϵ� ȸ���Ѵ�.
	_SpringArm->bUsePawnControlRotation = true;	
	_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 300.f), FRotator(-30.f, 0.f, 0.f));

	_Camera->SetupAttachment(_SpringArm, USpringArmComponent::SocketName);

	// ī�޶�� �������ϰ� �޸� ��Ʈ�ѷ� �����̼Ǵ�� ȸ����Ű�� �ʴ´�.
	// ���� ���콺�� ȸ���ص� ī�޶�� ��� ĳ���͸� �����.
	_Camera->bUsePawnControlRotation = false;	

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
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

	PlayerInputComponent->BindAxis(TEXT("ForwardBackward"), this,  &AVanguard::ForwardBackward);
	PlayerInputComponent->BindAxis(TEXT("RightLeft"), this, &AVanguard::RightLeft);

	PlayerInputComponent->BindAxis(TEXT("TurnRightLeft"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpDown"), this, &APawn::AddControllerPitchInput);
}

void AVanguard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	_AnimInstance = Cast<URobotAnimInstance>(GetMesh()->GetAnimInstance());

	_AnimInstance->OnMontageEnded.AddDynamic(this, &AVanguard::OnAwakeMontageEnded);
}

void AVanguard::ForwardBackward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AVanguard::RightLeft(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AVanguard::OnAwakeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage->GetName() == TEXT("BattleRobot_Skeleton_Montage"))
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

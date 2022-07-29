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

	// 컨트롤러가 회전할 때 캐릭터가 같이 회전하는 것을 방지한다.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캐릭터가 가속하는 방향으로 RotationRate의 속도로 회전하도록 한다.
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetCharacterMovement()->JumpZVelocity = 500.f; // 점프 시 시작속도.
	GetCharacterMovement()->AirControl = 0.25f;	// 공중에서 움직일 수 있는 정도, 1이면 지면과 동일하게 이동.
	GetCharacterMovement()->MaxWalkSpeed = 500.f; // 지면에서 이동 시 최대 속도, 공중에서 움직일 때에도 영향을 끼친다.
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
	_SpringArm->TargetArmLength = 800.f;
	// 스프링암을 컨트롤러 로테이션으로 회전시켜 마우스를 회전했을 때 움직이는 컨트롤러의 회전대로 움직인다. 
	// 따라서 마우스를 회전하는대로 스프림암도 회전한다.
	_SpringArm->bUsePawnControlRotation = true;	
	_SpringArm->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 300.f), FRotator(-30.f, 0.f, 0.f));

	_Camera->SetupAttachment(_SpringArm, USpringArmComponent::SocketName);

	// 카메라는 스프링암과 달리 컨트롤러 로테이션대로 회전시키지 않는다.
	// 따라서 마우스를 회전해도 카메라는 계속 캐릭터를 비춘다.
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

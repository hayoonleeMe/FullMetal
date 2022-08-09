#include "RobotAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vanguard.h"
#include "Kismet/GameplayStatics.h"

URobotAnimInstance::URobotAnimInstance()
{
	// _AwakeMontage를 로드해 설정한다.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Animations/Awake_Montage.Awake_Montage'"));
	if (AM.Succeeded())
	{
		_AwakeMontage = AM.Object;
	}

	// _FootstepSound 로드.
	static ConstructorHelpers::FObjectFinder<USoundBase> FS(TEXT("SoundWave'/Game/Sounds/Footstep.Footstep'"));
	if (FS.Succeeded())
	{
		_FootstepSound = FS.Object;
	}

	// _FireMontage 로드.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> FM(TEXT("AnimMontage'/Game/Animations/Fire_Montage.Fire_Montage'"));
	if (FM.Succeeded())
	{
		_FireMontage = FM.Object;
	}

	// _ReloadMontage 로드.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> RM(TEXT("AnimMontage'/Game/Animations/Reload_Montage.Reload_Montage'"));
	if (RM.Succeeded())
	{
		_ReloadMontage = RM.Object;
	}

	// _ReloadSound 로드
	static ConstructorHelpers::FObjectFinder<USoundBase> RS(TEXT("SoundWave'/Game/Sounds/Reload.Reload'"));
	if (RS.Succeeded())
	{
		_ReloadSound = RS.Object;
	}
}

void URobotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 캐릭터 캐싱.
	if (!_Character)
	{
		auto Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			_Character = Cast<AVanguard>(Pawn);
		}
	}

	// 애니메이션 재생에 필요한 데이터를 수집한다.
	if (_Character)
	{
		_Velocity = _Character->GetVelocity();

		_IsFalling = _Character->GetMovementComponent()->IsFalling();

		FVector Accel = _Character->GetCharacterMovement()->GetCurrentAcceleration();
		_ShouldMove = (_Velocity.Size() > 3.f) && (Accel.X != 0 || Accel.Y != 0);

		_Horizontal = _Character->_RightLeftValue;
		_Vertical = _Character->_ForwardBackwardValue;
		_UpDown = _Character->_UpDown;
	}
}

void URobotAnimInstance::PlayAwakeMontage()
{
	Montage_Play(_AwakeMontage, 1.f);
}

void URobotAnimInstance::PlayFireMontage(bool bIsPlay)
{
	// _FireMontage를 재생한다.
	if (bIsPlay)
	{
		Montage_Play(_FireMontage, 1.f);
	}
	// _FireMontage를 끈다.
	else
	{
		Montage_Stop(1.f, _FireMontage);
	}
}

void URobotAnimInstance::PlayReloadMontage()
{
	// 재장전 사운드를 재생한다.
	if (IsValid(_ReloadSound) && _Character)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _ReloadSound, _Character->GetActorLocation(), .5f);

		Montage_Play(_ReloadMontage, _ReloadMontage->GetPlayLength() / _ReloadSound->GetDuration());
	}
}

void URobotAnimInstance::AnimNotify_Footstep()
{
	if (IsValid(_FootstepSound) && _Character)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _FootstepSound, _Character->GetActorLocation(), .5f);
	}
}
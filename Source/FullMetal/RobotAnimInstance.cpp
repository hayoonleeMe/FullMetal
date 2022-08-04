#include "RobotAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Vanguard.h"
#include "Kismet/GameplayStatics.h"

URobotAnimInstance::URobotAnimInstance()
{
	// _AwakeMontage를 로드해 설정한다.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("AnimMontage'/Game/Animations/BattleRobot_Skeleton_Montage.BattleRobot_Skeleton_Montage'"));
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
}

void URobotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 캐릭터 캐싱.
	if (_Character == nullptr)
	{
		auto Pawn = TryGetPawnOwner();
		if (IsValid(Pawn))
		{
			_Character = Cast<AVanguard>(Pawn);
		}
	}

	// 애니메이션 재생에 필요한 데이터를 수집한다.
	if (_Character != nullptr)
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
	if (!Montage_IsPlaying(_AwakeMontage))
	{
		Montage_Play(_AwakeMontage, 1.f);
	}
}

void URobotAnimInstance::PlayFireMontage(bool bIsPlay)
{
	// _FireMontage를 재생한다.
	if (bIsPlay)
	{
		if (!Montage_IsPlaying(_FireMontage))
		{
			Montage_Play(_FireMontage, 1.f);
		}
	}
	// _FireMontage를 끈다.
	else
	{
		if (Montage_IsPlaying(_FireMontage))
		{
			Montage_Stop(1.f, _FireMontage);
		}
	}
	
}

void URobotAnimInstance::AnimNotify_Footstep()
{
	if (_FootstepSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, _FootstepSound, _Character->GetActorLocation(), .5f);
	}
}
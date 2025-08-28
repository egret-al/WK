// Fill out your copyright notice in the Description page of Project Settings.


#include "WKRootMotionSource_RootMotionWarping.h"

#include "MotionWarpingComponent.h"
#include "BlackWK/Animation/AnimNotify/WKAnimNotifyState_RootMotionWarping.h"
#include "BlackWK/Gameplay/WKGameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

FName FWKRootMotionSource_RootMotionWarping::RootMotionWarpingForceName("RootMotionWarping");
constexpr static float ANIMATION_DISTANCE_THRESHOLD = 10.f;

static TAutoConsoleVariable<int32> CVarDrawRootMotionWarpingDebugInfo(
	TEXT("wk.DrawRootMotionWarpingDebugInfo"),
	0,
	TEXT("显示角色调试信息。0：关闭绘制，1: 开启绘制 \n"),
	ECVF_Default
);

FRootMotionWarpingSection::FRootMotionWarpingSection()
	: StartTime(0.f)
	, EndTime(0.f)
	, AccumulateMode(ERootMotionAccumulateMode::Additive)
	, bIgnoreZAccumulate(false)
	, bMaintainVelocityWhenFinish(false)
	, bKeepAnimationRhythm(true)
{
}

FRootMotionWarpingSection::FRootMotionWarpingSection(float InStartTime, float InEndTime)
	: StartTime(InStartTime)
	, EndTime(InEndTime)
	, AccumulateMode(ERootMotionAccumulateMode::Additive)
	, bIgnoreZAccumulate(false)
	, bMaintainVelocityWhenFinish(false)
	, bKeepAnimationRhythm(true)
{
}

FRootMotionWarpingSection::FRootMotionWarpingSection(float InStartTime, float InEndTime, ERootMotionAccumulateMode InAccumulateMode, bool InbIgnoreZAccumulate, bool InbMaintainVelocityWhenFinish, bool InbKeepAnimationRhythm)
	: StartTime(InStartTime)
	, EndTime(InEndTime)
	, AccumulateMode(InAccumulateMode)
	, bIgnoreZAccumulate(InbIgnoreZAccumulate)
	, bMaintainVelocityWhenFinish(InbMaintainVelocityWhenFinish)
	, bKeepAnimationRhythm(InbKeepAnimationRhythm)
{
}

bool FRootMotionWarpingSection::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << StartTime;
	Ar << EndTime;
	Ar << AccumulateMode;
	Ar.SerializeBits(&bIgnoreZAccumulate, 1);
	Ar.SerializeBits(&bMaintainVelocityWhenFinish, 1);
	Ar.SerializeBits(&bKeepAnimationRhythm,1);
	bOutSuccess = true;
	return bOutSuccess;
}

FRootMotionWarpingSection_Translation::FRootMotionWarpingSection_Translation()
	: WarpingDistance(0.f)
	, Direction(FVector::ZeroVector)
{
}

FRootMotionWarpingSection_Translation::FRootMotionWarpingSection_Translation(float InStartTime, float InEndTime, ERootMotionAccumulateMode InAccumulateMode, bool InbIgnoreZAccumulate, bool InbMaintainVelocityWhenFinish, bool InbKeepAnimationRhythm)
	: FRootMotionWarpingSection(InStartTime,InEndTime,InAccumulateMode,InbIgnoreZAccumulate,InbMaintainVelocityWhenFinish,InbKeepAnimationRhythm)
	, WarpingDistance(0.f)
	, Direction(FVector::ZeroVector)
{
}

bool FRootMotionWarpingSection_Translation::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionWarpingSection::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << WarpingDistance;
	Direction.NetSerialize(Ar, Map, bOutSuccess);
	
	bOutSuccess = true;
	return true;
}

FRootMotionWarpingSection_Rotation::FRootMotionWarpingSection_Rotation()
	: WarpingRotation(0.f)
	, TargetActor(nullptr)
	, RotationAngleSpeed(-1.f)
	, RotationOrientation(EWKRotateOrientation::MinAngle)
{
}

FRootMotionWarpingSection_Rotation::FRootMotionWarpingSection_Rotation(float InStartTime, float InEndTime)
	: FRootMotionWarpingSection(InStartTime, InEndTime)
	, WarpingRotation(0.f)
	, TargetActor(nullptr)
	, RotationAngleSpeed(-1.f)
	, RotationOrientation(EWKRotateOrientation::MinAngle)
{
}

bool FRootMotionWarpingSection_Rotation::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionWarpingSection::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	Ar << WarpingRotation;
	
	if (Ar.IsSaving())
	{
		if (!TargetActor.IsValid())
		{
			TargetActor = nullptr;
		}
	}
	
	Ar << TargetActor;
	Ar << RotationAngleSpeed;
	Ar << RotationOrientation;
	
	bOutSuccess = true;
	return true;
}

FRootMotionWarpingSection_TargetActor::FRootMotionWarpingSection_TargetActor()
	: TargetActor(nullptr)
	, bUpdateTargetPosition(true)
	, MaxDistance(-1.f)
	, WarpingDistanceIfTargetNotFound(-1.f)
	, ChasingSpeed(-1.f)
	, RotationToAbort(-1.f)
	, RotationRate(-1.f)
	, RotationMax(-1.f)
	, LocationOffset(FVector::ZeroVector)
	, RotationOffset(0.f)
	, ZTrajectory(nullptr)
	, RotationOrientation(EWKRotateOrientation::MinAngle)	
	, LastTargetLocation(FVector::ZeroVector)
	, LastWarpingLocation(FVector::ZeroVector)
	, LastDirection(FVector::ZeroVector)
	, StartDeltaZ(0)
	, StopTrace(false)
	, CurrentRotatorAngle(0)
{
}

FRootMotionWarpingSection_TargetActor::FRootMotionWarpingSection_TargetActor(float InStartTime, float InEndTime, ERootMotionAccumulateMode InAccumulateMode, bool InbIgnoreZAccumulate, bool InbMaintainVelocityWhenFinish, bool InbKeepAnimationRhythm)
	: FRootMotionWarpingSection(InStartTime,InEndTime,InAccumulateMode,InbIgnoreZAccumulate,InbMaintainVelocityWhenFinish,InbKeepAnimationRhythm)
	, TargetActor(nullptr)
	, bUpdateTargetPosition(true)
	, MaxDistance(-1.f)
	, WarpingDistanceIfTargetNotFound(-1.f)
	, ChasingSpeed(-1.f)
	, RotationToAbort(-1.f)
	, RotationRate(-1.f)
	, RotationMax(-1.f)
	, LocationOffset(FVector::ZeroVector)
	, RotationOffset(0.f)
	, ZTrajectory(nullptr)
	, RotationOrientation(EWKRotateOrientation::MinAngle)
	, LastTargetLocation(FVector::ZeroVector)
	, LastDirection(FVector::ZeroVector)
	, StartDeltaZ(0)
	, StopTrace(false)
	, CurrentRotatorAngle(0)
{
}

bool FRootMotionWarpingSection_TargetActor::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionWarpingSection::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	enum RepFlag
	{
		REP_HasRotationToAbort = 0,
		REP_HasRotationRate,
		REP_HasRotationMax,
		REP_HasZTrajectory,
		REP_MAX
	};

	uint16 RepBits = 0;

	static_assert(sizeof(RepBits) * 8 >= REP_MAX, "RepBits must be greater than REP_MAX, In Case Bit Flag Not Set");

	if (Ar.IsSaving())
	{
		if (!TargetActor.IsValid())
		{
			TargetActor = nullptr;
		}
		
		if (IsValid(ZTrajectory))
		{
			RepBits |= (1 << REP_HasZTrajectory);
		}
		else
		{
			ZTrajectory = nullptr;
		}

		if (!FMath::IsNearlyZero(RotationToAbort))
		{
			RepBits |= (1 << REP_HasRotationToAbort);
		}

		if (!FMath::IsNearlyZero(RotationRate))
		{
			RepBits |= (1 << REP_HasRotationRate);
		}

		if (!FMath::IsNearlyZero(RotationMax))
		{
			RepBits |= (1 << REP_HasRotationMax);
		}
	}

	Ar.SerializeBits(&RepBits, REP_MAX);
	
	Ar << TargetActor;
	Ar.SerializeBits(&bUpdateTargetPosition,1);
	Ar << MaxDistance;
	Ar << WarpingDistanceIfTargetNotFound;
	Ar << ChasingSpeed;

	SERIALIZE_BY_CONDITION(REP_HasZTrajectory, Ar << ZTrajectory,ZTrajectory = nullptr)
	SERIALIZE_BY_CONDITION(REP_HasRotationToAbort, Ar << RotationToAbort, RotationToAbort = 0)
	SERIALIZE_BY_CONDITION(REP_HasRotationRate, Ar << RotationRate, RotationRate = 0)
	SERIALIZE_BY_CONDITION(REP_HasRotationMax, Ar << RotationMax, RotationMax = 0)

	LocationOffset.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RotationOffset;
	Ar << RotationOrientation;
	
	bOutSuccess = true;
	return true;
}

FRootMotionWarpingSection_Location::FRootMotionWarpingSection_Location()
	: TargetLocation(FVector::ZeroVector)
	, ZTrajectory(nullptr)
	, AnimationTotalDistance(0)
	, StartDeltaZ(0)
{
}

FRootMotionWarpingSection_Location::FRootMotionWarpingSection_Location(float InStartTime, float InEndTime, ERootMotionAccumulateMode InAccumulateMode, bool InbIgnoreZAccumulate, bool InbMaintainVelocityWhenFinish, bool InbKeepAnimationRhythm)
	: FRootMotionWarpingSection(InStartTime, InEndTime, InAccumulateMode, InbIgnoreZAccumulate, InbMaintainVelocityWhenFinish, InbKeepAnimationRhythm)
	, TargetLocation(FVector::ZeroVector)
	, ZTrajectory(nullptr)
	, AnimationTotalDistance(0)
	, StartDeltaZ(0)
{
}

bool FRootMotionWarpingSection_Location::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionWarpingSection::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	TargetLocation.NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		if (!IsValid(ZTrajectory))
		{
			ZTrajectory = nullptr;
		}
	}
	Ar << ZTrajectory;
	
	bOutSuccess = true;
	return true;
}

FWKRootMotionSource_RootMotionWarping::FWKRootMotionSource_RootMotionWarping()
	: Montage(nullptr)
	, AnimationStartPos(0.f)
	, AnimationEndPos(0.f)
	, PlayRate(1.f)
	, CurrentAnimPos(-1.f)
{
	AccumulateMode = ERootMotionAccumulateMode::Override;
	Settings.SetFlag(ERootMotionSourceSettingsFlags::UseSensitiveLiftoffCheck);
	Settings.SetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);
}

FWKRootMotionSource_RootMotionWarping::~FWKRootMotionSource_RootMotionWarping()
{
	LastSection.Reset();
}

FRootMotionSource* FWKRootMotionSource_RootMotionWarping::Clone() const
{
	FWKRootMotionSource_RootMotionWarping* CopyPtr = new FWKRootMotionSource_RootMotionWarping(*this);
	return CopyPtr;
}

bool FWKRootMotionSource_RootMotionWarping::Matches(const FRootMotionSource* Other) const
{
	// We can cast safely here since in FRootMotionSource::Matches() we ensured ScriptStruct equality
	const FWKRootMotionSource_RootMotionWarping* OtherCast = static_cast<const FWKRootMotionSource_RootMotionWarping*>(Other);
	
	if (!FRootMotionSource::Matches(Other))
	{
		if (LocalID != static_cast<uint16>(ERootMotionSourceID::Invalid) &&
			LocalID == OtherCast->LocalID)
		{
			//DSLog(LogRootMotion,Warning,TEXT("FDSRootMotionSource_RootMotionWarping:: Supper Matches LocalID Equal but Result not equal (%s) (%s)"),*ToSimpleString(),*OtherCast->ToSimpleString());
		}
		else
		{
			return false;
		}
	}
	
	const bool Result = Montage == OtherCast->Montage &&
		//FMath::IsNearlyEqual(PlayRate, OtherCast->PlayRate, UE_SMALL_NUMBER)&&
		FMath::IsNearlyEqual(AnimationStartPos ,OtherCast->AnimationStartPos, UE_SMALL_NUMBER)&&
		FMath::IsNearlyEqual(AnimationEndPos,OtherCast->AnimationEndPos,UE_SMALL_NUMBER)&&
		WarpingSections_Translation.Num() == OtherCast->WarpingSections_Translation.Num()&&
		WarpingSections_Rotation.Num() == OtherCast->WarpingSections_Rotation.Num()&&
		WarpingSections_TargetActor.Num() == OtherCast->WarpingSections_TargetActor.Num()&&
		WarpingSections_Location.Num() == OtherCast->WarpingSections_Location.Num();
	
	if (!Result)
	{
		if (LocalID != static_cast<uint16>(ERootMotionSourceID::Invalid) &&
			LocalID == OtherCast->LocalID)
		{
			// DSLog(LogRootMotion,Warning,TEXT("FDSRootMotionSource_RootMotionWarping::Matches LocalID Equal but Result not equal (%s) (%s)"),*ToSimpleString(),*OtherCast->ToSimpleString());
			//LocalID相同，返回true避免客户端报Ensure
			return true;
		}
	}
	
	return Result;
}

void FWKRootMotionSource_RootMotionWarping::PrepareRootMotion(float SimulationTime, float MovementTickTime, const ACharacter& Character, const UCharacterMovementComponent& MoveComponent)
{
	RootMotionParams.Clear();

	if (Character.GetMesh() && Character.GetMesh()->GetAnimInstance())
	{
		//由于PlayRate可能会在蒙太奇运行过程中被修改，所以需要实时取
		if (FAnimMontageInstance* MontageInstance = Character.GetMesh()->GetAnimInstance()->GetActiveInstanceForMontage(Montage))
		{
			PlayRate = MontageInstance->GetPlayRate();
		}
	}

	if (CurrentAnimPos < 0)
	{
		CurrentAnimPos = AnimationStartPos;
	}
	
	if (Duration > SMALL_NUMBER && MovementTickTime > SMALL_NUMBER)
	{
		FTransform DeltaTransform(FTransform::Identity);
		FRotator DeltaRotation = FRotator::ZeroRotator;

		FRootMotionWarpingSection* ActiveSection = nullptr;

		if (Character.GetMesh()->GetAnimInstance()
			&&Character.GetMesh()->GetAnimInstance()->RootMotionMode != ERootMotionMode::IgnoreRootMotion)
		{
			//目标跟踪
			if (!HandleTargetActorWarping(Character,SimulationTime,DeltaTransform,DeltaRotation,ActiveSection))
			{
				//指定Location
				if (!HandleLocationWarping(Character,SimulationTime,DeltaTransform,DeltaRotation,ActiveSection))
				{
					//指定距离
					if (!HandleTranslationWarping(Character,SimulationTime,DeltaTransform,DeltaRotation,ActiveSection))
					{
						//使用原动画位移
						const FVector OriginTranslation = Character.GetMesh()->ConvertLocalRootMotionToWorld(UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, CurrentAnimPos, CurrentAnimPos + SimulationTime * PlayRate)).GetTranslation();
						DeltaTransform.Accumulate(FTransform(OriginTranslation));
					}
				}
				//处理旋转
				HandleRotationWarping(Character,SimulationTime,DeltaRotation);
			}
		}
		
		CurrentAnimPos =  FMath::Clamp(CurrentAnimPos + SimulationTime*PlayRate, AnimationStartPos, AnimationEndPos);

		//float MontagePosition = Character.GetMesh()->GetAnimInstance()->Montage_GetPosition(Montage);
		//DSLog(LogTemp,Warning,TEXT("MontagePosition:%0.2f"),MontagePosition);
		//DSLog(LogTemp,Warning,TEXT("start:%0.2f"),CurrentAnimPos);
		//DSLog(LogTemp,Warning,TEXT("end:%0.2f"),CurrentAnimPos + SimulationTime * PlayRate);
		//DSLog(LogTemp,Warning,TEXT("---------------------------------"));
		
		const FVector Force = DeltaTransform.GetTranslation() / MovementTickTime;
		RootMotionParams.Set(FTransform(DeltaRotation, Force));
		RootMotionParams.ScaleRootMotionTranslation(Character.GetAnimRootMotionTranslationScale());

		HandleRootMotionSourceParams(ActiveSection,MoveComponent);
	}
	else
	{
		checkf(Duration > SMALL_NUMBER, TEXT("FDSRootMotionSource_RootMotionWarping prepared with invalid duration."));
	}
	
	SetTime(GetTime() + SimulationTime);
}

bool FWKRootMotionSource_RootMotionWarping::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (!FRootMotionSource::NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}

	if (Ar.IsSaving())
	{
		if (!IsValid(Montage))
		{
			Montage = nullptr;
		}
	}

	Ar << Montage;
	//Ar << PlayRate;
	Ar << AnimationStartPos;
	Ar << AnimationEndPos;

	constexpr uint8 MaxArraySize = 8;

	if (Ar.IsSaving())
	{
#define CHECK_IN_ARR_SIZE(ARR)\
if (ARR.Num() > MaxArraySize)\
{\
	/*DSLog(LogNetSerialization,Error,TEXT("FDSRootMotionSource_RootMotionWarping::NetSerialize ArraySize > Max Size "#ARR));*/\
}\

		CHECK_IN_ARR_SIZE(WarpingSections_Translation);
		CHECK_IN_ARR_SIZE(WarpingSections_Rotation);
		CHECK_IN_ARR_SIZE(WarpingSections_TargetActor);
		CHECK_IN_ARR_SIZE(WarpingSections_Location);

#undef CHECK_IN_ARR_SIZE
	}
	
	SafeNetSerializeTArray_WithNetSerialize<MaxArraySize>(Ar, WarpingSections_Translation, Map);
	SafeNetSerializeTArray_WithNetSerialize<MaxArraySize>(Ar, WarpingSections_Rotation, Map);
	SafeNetSerializeTArray_WithNetSerialize<MaxArraySize>(Ar, WarpingSections_TargetActor, Map);
	SafeNetSerializeTArray_WithNetSerialize<MaxArraySize>(Ar, WarpingSections_Location, Map);
	
	bOutSuccess = true;
	return true;
}

UScriptStruct* FWKRootMotionSource_RootMotionWarping::GetScriptStruct() const
{
	return GetScriptStruct();
}

FString FWKRootMotionSource_RootMotionWarping::ToSimpleString() const
{
	return FString::Printf(TEXT("[ID:%u]FWKRootMotionSource_RootMotionWarping[Montage:%s,StartPos:%.2f,EndPos:%.2f]"), LocalID, !IsValid(Montage) ? *FString("nullptr") : *Montage->GetName(), AnimationStartPos, AnimationEndPos);
}

bool FWKRootMotionSource_RootMotionWarping::HandleTargetActorWarping(const ACharacter& Character, const float& SimulationTime, FTransform& DeltaTransform, FRotator& DeltaRotation, FRootMotionWarpingSection*& ActiveSection)
{
	FRootMotionWarpingSection_TargetActor* ActiveTargetSection = nullptr;
	for (FRootMotionWarpingSection_TargetActor& TargetSection: WarpingSections_TargetActor)
	{
		if (CurrentAnimPos >= TargetSection.StartTime && CurrentAnimPos <= TargetSection.EndTime)
		{
			ActiveTargetSection = &TargetSection;
			ActiveSection = &TargetSection;
			break;
		}
	}

	//目标跟踪
	if (ActiveTargetSection)
	{
		if (ActiveTargetSection->bKeepAnimationRhythm && !ActiveTargetSection->AnimationTotalDistance.IsSet())
		{
			const FVector AnimationTranslation = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, ActiveTargetSection->StartTime, ActiveTargetSection->EndTime).GetTranslation();
			//目标跟踪只取Y轴位移
			ActiveTargetSection->AnimationTotalDistance = FMath::Abs(AnimationTranslation.Y);
		}
		
		if (ActiveTargetSection->TargetActor.IsValid())
		{
			//目标位置（加上偏移）
			FVector TargetLocation = ActiveTargetSection->TargetActor->GetActorLocation();
			if (!ActiveTargetSection->LocationOffset.IsZero())
			{
				//LocationOffset负位移保护功能. 仅考虑X方向的位移情况。如果距离目标的距离，小于偏移X，则以Owner坐标为目标追踪
				if (FVector::DistSquared2D(Character.GetActorLocation(),TargetLocation) < FMath::Square(ActiveTargetSection->LocationOffset.X))
				{
					//需要加一点点位移来保证朝向有旋转
					const FVector ToTarget = (TargetLocation - Character.GetActorLocation()).GetSafeNormal2D();
					TargetLocation = TargetLocation + ToTarget * 0.1f;
				}
				else
				{
					const FVector ToSource = (Character.GetActorLocation() - TargetLocation).GetSafeNormal2D();
					TargetLocation += ToSource.ToOrientationQuat().RotateVector(ActiveTargetSection->LocationOffset);
				}
			}
			
			if (!ActiveTargetSection->StartLocation.IsSet())
			{
				ActiveTargetSection->StartLocation = Character.GetActorLocation();
				ActiveTargetSection->LastTargetLocation = TargetLocation;
				ActiveTargetSection->LastWarpingLocation = TargetLocation;
				ActiveTargetSection->LastDirection = (TargetLocation - Character.GetActorLocation()).GetSafeNormal();
				ActiveTargetSection->StartDeltaZ = TargetLocation.Z - ActiveTargetSection->StartLocation.GetValue().Z;

				float StartAngle = UWKGameplayStatics::CalculateDirection((TargetLocation - Character.GetActorLocation()).GetSafeNormal2D(), Character.GetActorForwardVector().GetSafeNormal2D());
				//如果一开始角度就超过限制，不追踪
				if (ActiveTargetSection->RotationToAbort >=0 &&FMath::Abs(StartAngle) > ActiveTargetSection->RotationToAbort)
				{
					float Distance = 0.f;
					if (ActiveTargetSection->WarpingDistanceIfTargetNotFound >= 0)
					{
						Distance = ActiveTargetSection->WarpingDistanceIfTargetNotFound;
					}
					else if (ActiveTargetSection->MaxDistance >= 0)
					{
						Distance = ActiveTargetSection->MaxDistance;
					}
					
					ActiveTargetSection->LastTargetLocation = Character.GetActorLocation()+ Character.GetActorForwardVector() * Distance;
					ActiveTargetSection->LastWarpingLocation = ActiveTargetSection->LastTargetLocation;

					ActiveTargetSection->StopTrace = true;
				}
				else
				{
					ActiveTargetSection->StopTrace = false;
				}
			}

			if (!ActiveTargetSection->bUpdateTargetPosition)
			{
				TargetLocation = ActiveTargetSection->LastTargetLocation;
			}

			float Angle = UWKGameplayStatics::CalculateDirection((TargetLocation - Character.GetActorLocation()).GetSafeNormal2D(), Character.GetActorForwardVector().GetSafeNormal2D());
			bool bUpdateTrackLocation = true;
			
			if (ActiveTargetSection->MaxDistance < 0
				&& ActiveTargetSection->ChasingSpeed < 0
				&& ActiveTargetSection->RotationToAbort < 0)
			{
				//全部为负数，则为不做追踪限制，必定追踪到目标
				ActiveTargetSection->LastTargetLocation = TargetLocation;
				ActiveTargetSection->LastWarpingLocation = TargetLocation;
			}
			else
			{
				if ((ActiveTargetSection->RotationToAbort >=0 && FMath::Abs(Angle) > ActiveTargetSection->RotationToAbort)
					||(ActiveTargetSection->RotationMax >= 0 && ActiveTargetSection->CurrentRotatorAngle >= ActiveTargetSection->RotationMax)
					||ActiveTargetSection->StopTrace)
				{
 					bUpdateTrackLocation = false;
				}
				
				if (bUpdateTrackLocation)
				{
					//这一帧追踪的目标位置
					FVector DeltaTargetLocation = TargetLocation;
				
					if (ActiveTargetSection->ChasingSpeed>=0)
					{
						FVector TargetMoveDirection = TargetLocation - ActiveTargetSection->LastTargetLocation;
						if (!TargetMoveDirection.IsZero())
						{
							//这一帧目标位置的最大位移
							const float MaxDeltaDistance = ActiveTargetSection->ChasingSpeed * SimulationTime * PlayRate;
							if (TargetMoveDirection.SizeSquared() > FMath::Square(MaxDeltaDistance))
							{
								DeltaTargetLocation = ActiveTargetSection->LastTargetLocation + MaxDeltaDistance * TargetMoveDirection.GetSafeNormal();
							}
						}
					}
				
					ActiveTargetSection->LastTargetLocation = DeltaTargetLocation;

					//限制最大距离
					if (ActiveTargetSection->MaxDistance >= 0 )
					{
						if (FVector::DistSquared(DeltaTargetLocation,ActiveTargetSection->StartLocation.GetValue()) > FMath::Square(ActiveTargetSection->MaxDistance))
						{
							DeltaTargetLocation = ActiveTargetSection->StartLocation.GetValue() + ActiveTargetSection->MaxDistance * (DeltaTargetLocation - ActiveTargetSection->StartLocation.GetValue()).GetSafeNormal();
						}
					}
				
					ActiveTargetSection->LastWarpingLocation = DeltaTargetLocation;
				}
			}
			
			//缩放位移
			FVector CurrentTargetLocation;
			const float  TimeWarpingScale = FMath::Clamp((CurrentAnimPos + SimulationTime * PlayRate - ActiveTargetSection->StartTime)/ (ActiveTargetSection->EndTime - ActiveTargetSection->StartTime),0.f,1.f);
			
			if (ActiveTargetSection->bKeepAnimationRhythm && ActiveTargetSection->AnimationTotalDistance.GetValue() > ANIMATION_DISTANCE_THRESHOLD)
			{
				//动画有位移，等比缩放
				const FVector RMTranslation = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, ActiveTargetSection->StartTime, CurrentAnimPos + SimulationTime * PlayRate).GetTranslation();
				const float  TranslationScale = FMath::Clamp(FMath::Max(RMTranslation.Y,0) / ActiveTargetSection->AnimationTotalDistance.GetValue(),0,1);
				CurrentTargetLocation = FMath::Lerp<FVector, float>(ActiveTargetSection->StartLocation.GetValue(), ActiveTargetSection->LastWarpingLocation, TranslationScale);

				//加上高度差Z轴曲线偏移
				if (ActiveTargetSection->ZTrajectory && ActiveTargetSection->StartDeltaZ > 0)
				{
					CurrentTargetLocation.Z = CurrentTargetLocation.Z +ActiveTargetSection->StartDeltaZ*(ActiveTargetSection->ZTrajectory->GetFloatValue(TranslationScale));
				}
			}
			else
			{
				//动画没有位移，按时间缩放
				CurrentTargetLocation = FMath::Lerp<FVector, float>(ActiveTargetSection->StartLocation.GetValue(), ActiveTargetSection->LastWarpingLocation, TimeWarpingScale);

				//加上高度差Z轴曲线偏移
				if (ActiveTargetSection->ZTrajectory && ActiveTargetSection->StartDeltaZ > 0)
				{
					CurrentTargetLocation.Z = CurrentTargetLocation.Z +ActiveTargetSection->StartDeltaZ*(ActiveTargetSection->ZTrajectory->GetFloatValue(TimeWarpingScale));
				}
			}
			
			if (CVarDrawRootMotionWarpingDebugInfo.GetValueOnGameThread())
			{
				if (Character.GetNetMode() == ENetMode::NM_DedicatedServer)
				{
					DrawDebugSphere(Character.GetWorld(), TargetLocation, 25.f, 12, FColor::Red, false, 3.f, 0, 2.f);
					DrawDebugSphere(Character.GetWorld(), ActiveTargetSection->LastTargetLocation, 20.f, 12, FColor::Green, false, 3.f, 0, 2.f);
					DrawDebugSphere(Character.GetWorld(), ActiveTargetSection->LastWarpingLocation, 15.f, 12, FColor::Yellow, false, 3.f, 0, 2.f);
					DrawDebugSphere(Character.GetWorld(), CurrentTargetLocation, 10.f, 12, FColor::Blue, false, 3.f, 0, 2.f);
				}
				else
				{
					DrawDebugSphere(Character.GetWorld(), TargetLocation + FVector::UpVector *50, 25.f, 12, FColor::Red, false, 3.f, 0, 2.f);
					DrawDebugSphere(Character.GetWorld(), ActiveTargetSection->LastTargetLocation+ FVector::UpVector *50, 20.f, 12, FColor::Green, false, 3.f, 0, 2.f);
					DrawDebugSphere(Character.GetWorld(), ActiveTargetSection->LastWarpingLocation+ FVector::UpVector *50, 15.f, 12, FColor::Yellow, false, 3.f, 0, 2.f);
					DrawDebugSphere(Character.GetWorld(), CurrentTargetLocation+ FVector::UpVector *50, 10.f, 12, FColor::Blue, false, 3.f, 0, 2.f);
				}
			}
			
			FVector DesiredTranslation = CurrentTargetLocation  - Character.GetActorLocation();

			DeltaTransform.Accumulate(FTransform(DesiredTranslation));
			
			//处理旋转
			FString DebugString ="";
			
			//这一帧需要旋转的角度
			float TargetDeltaAngle = 0;
			
			FVector ToNewTargetDirection = (TargetLocation - Character.GetActorLocation()).GetSafeNormal2D().RotateAngleAxis(ActiveTargetSection->RotationOffset * TimeWarpingScale,FVector::UpVector);

			FRotator CurrentRot = Character.GetActorRotation();
			const float DesiredYaw = ToNewTargetDirection.Rotation().Yaw;
			
			if (ActiveTargetSection->RotationRate >=0)
			{
				if (bUpdateTrackLocation)
				{
					/*float AngleChange = (ActiveTargetSection->RotationRate * SimulationTime * PlayRate);
					TargetDeltaAngle = FDSMath::CalculateDirection(ToNewTargetDirection,Character.GetActorForwardVector().GetSafeNormal2D());
					if (FMath::Abs(TargetDeltaAngle) < AngleChange)
					{
						//直接转向目标点
						DebugString += FString::Printf(TEXT("正在追踪,角度差小于旋转速率"));
					}
					else
					{
						//按旋转速率转向
						TargetDeltaAngle = AngleChange * FMath::Sign(TargetDeltaAngle);
						DebugString += FString::Printf(TEXT("正在追踪，角度差大于旋转速率"));
					}*/

					TargetDeltaAngle = CalculateDeltaAngle(CurrentRot.Yaw,DesiredYaw,ActiveTargetSection->RotationRate,SimulationTime * PlayRate,ActiveTargetSection->RotationOrientation);
					
					ActiveTargetSection->LastDirection = (TargetLocation - Character.GetActorLocation()).GetSafeNormal2D();
				}
				else
				{
					/*FVector ToOldTargetDirection = ActiveTargetSection->LastDirection.RotateAngleAxis(ActiveTargetSection->RotationOffset * WarpingScale,FVector::UpVector);
					TargetDeltaAngle = FDSMath::CalculateDirection(ToOldTargetDirection,Character.GetActorForwardVector().GetSafeNormal2D());
					if (FMath::Abs(TargetDeltaAngle) > AngleChange)
					{
						TargetDeltaAngle = AngleChange * FMath::Sign(TargetDeltaAngle);
					}*/
					TargetDeltaAngle = 0.f;
					DebugString += FString::Printf(TEXT("停止追踪，角度差大于最大放弃追踪角度"));
				}
			}
			else
			{
				if (!ActiveTargetSection->StopTrace)
				{
					DebugString += FString::Printf(TEXT("没有限制旋转速率"));
					TargetDeltaAngle = UWKGameplayStatics::CalculateDirection(ToNewTargetDirection, Character.GetActorForwardVector().GetSafeNormal2D());
				}
			}

			if (CVarDrawRootMotionWarpingDebugInfo.GetValueOnGameThread())
			{
				if (Character.GetNetMode() == ENetMode::NM_DedicatedServer)
				{
					GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("%s，这一帧旋转角度:%.2f"), *DebugString,TargetDeltaAngle));
					UE_LOG(LogRootMotion, Warning, TEXT("%s，这一帧追踪的旋转角度:%.2f"), *DebugString,TargetDeltaAngle);
				}
			}

			ActiveTargetSection->CurrentRotatorAngle += FMath::Abs(TargetDeltaAngle);
			
			//TODO 不除以2会来回反复，原因待查
			DeltaRotation.Yaw = TargetDeltaAngle/2;
		}
		else
		{//如果没有找到目标,按照WarpingDistanceIfTargetNotFound进行缩放
			
			if (ActiveTargetSection->WarpingDistanceIfTargetNotFound >= 0)
			{
				FVector ScaledTranslation = FVector(0, 0,  0);
		
				if (ActiveTargetSection->bKeepAnimationRhythm && ActiveTargetSection->AnimationTotalDistance.GetValue() > ANIMATION_DISTANCE_THRESHOLD )
				{
					// 动画有位移，按动画位移进行缩放
					const FVector DeltaRMTranslation = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, CurrentAnimPos, CurrentAnimPos + SimulationTime * PlayRate).GetTranslation();
					const float  TranslationScale = FMath::Clamp(FMath::Max(DeltaRMTranslation.Y,0) / ActiveTargetSection->AnimationTotalDistance.GetValue(),0,1);
					ScaledTranslation = TranslationScale * ActiveTargetSection->WarpingDistanceIfTargetNotFound * Character.GetActorForwardVector();
				}
				else
				{
					//动画没有位移，按时间缩放
					const float  TranslationScale = FMath::Min(SimulationTime * PlayRate / (ActiveTargetSection->EndTime - ActiveTargetSection->StartTime),1.f);
					ScaledTranslation = TranslationScale * ActiveTargetSection->WarpingDistanceIfTargetNotFound * Character.GetActorForwardVector();
			
				}
				DeltaTransform.Accumulate(FTransform(ScaledTranslation));
			}
			else
			{
				//使用原动画位移
				FVector OriginTranslation = Character.GetMesh()->ConvertLocalRootMotionToWorld(UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, CurrentAnimPos, CurrentAnimPos + SimulationTime * PlayRate)).GetTranslation();
				DeltaTransform.Accumulate(FTransform(OriginTranslation));
			}
			
		}
		return true;
	}
	return false;
}

bool FWKRootMotionSource_RootMotionWarping::HandleLocationWarping(const ACharacter& Character, const float& SimulationTime, FTransform& DeltaTransform, FRotator& DeltaRotation, FRootMotionWarpingSection*& ActiveSection)
{
	FRootMotionWarpingSection_Location* ActiveLocationSection = nullptr;
	for (FRootMotionWarpingSection_Location& LocationSection: WarpingSections_Location)
	{
		if (CurrentAnimPos >= LocationSection.StartTime && CurrentAnimPos <= LocationSection.EndTime)
		{
			ActiveSection = &LocationSection;
			ActiveLocationSection = &LocationSection;
			break;
		}
	}

	if (ActiveLocationSection)
	{
		if (!ActiveLocationSection->StartLocation.IsSet())
		{
			ActiveLocationSection->StartLocation = Character.GetActorLocation();
			ActiveLocationSection->StartDeltaZ = ActiveLocationSection->TargetLocation.Z - ActiveLocationSection->StartLocation.GetValue().Z;
			if (ActiveLocationSection->bKeepAnimationRhythm)
			{
				FVector RMTranslation =UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, ActiveLocationSection->StartTime, ActiveLocationSection->EndTime).GetTranslation();
				//Location不能只取Y轴位移，比如受击动画是8方向位移
				ActiveLocationSection->AnimationTotalDistance = FMath::Abs(RMTranslation.Size2D());
			}
		}
	
		FVector CurrentTargetLocation;
		if (ActiveLocationSection->bKeepAnimationRhythm && ActiveLocationSection->AnimationTotalDistance > ANIMATION_DISTANCE_THRESHOLD)
		{
			//动画有位移，等比缩放
			const FVector RMTranslation = UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, ActiveLocationSection->StartTime, CurrentAnimPos + SimulationTime * PlayRate).GetTranslation();
			const float  TranslationScale = FMath::Clamp(RMTranslation.Size2D() / ActiveLocationSection->AnimationTotalDistance,0,1);
			CurrentTargetLocation = FMath::Lerp<FVector, float>(ActiveLocationSection->StartLocation.GetValue(), ActiveLocationSection->TargetLocation, TranslationScale);

			//加上高度差Z轴曲线偏移
			if (ActiveLocationSection->ZTrajectory && ActiveLocationSection->StartDeltaZ > 0)
			{
				CurrentTargetLocation.Z = CurrentTargetLocation.Z +ActiveLocationSection->StartDeltaZ*(ActiveLocationSection->ZTrajectory->GetFloatValue(TranslationScale));
			}
		}
		else
		{
			const float  TimeWarpingScale = FMath::Clamp((CurrentAnimPos + SimulationTime * PlayRate - ActiveLocationSection->StartTime)/ (ActiveLocationSection->EndTime - ActiveLocationSection->StartTime),0.f,1.f);
			//动画没有位移，按时间缩放
			CurrentTargetLocation = FMath::Lerp<FVector, float>(ActiveLocationSection->StartLocation.GetValue(), ActiveLocationSection->TargetLocation, TimeWarpingScale);

			//加上高度差Z轴曲线偏移
			if (ActiveLocationSection->ZTrajectory && ActiveLocationSection->StartDeltaZ > 0)
			{
				CurrentTargetLocation.Z = CurrentTargetLocation.Z +ActiveLocationSection->StartDeltaZ*(ActiveLocationSection->ZTrajectory->GetFloatValue(TimeWarpingScale));
			}
		}

		FVector DesiredTranslation = CurrentTargetLocation  - Character.GetActorLocation();
		DeltaTransform.Accumulate(FTransform(DesiredTranslation));
		
		return true;
	}
	return false;
}

bool FWKRootMotionSource_RootMotionWarping::HandleTranslationWarping(const ACharacter& Character, const float& SimulationTime, FTransform& DeltaTransform, FRotator& DeltaRotation, FRootMotionWarpingSection*& ActiveSection)
{
	FRootMotionWarpingSection_Translation* ActiveTranslationSection = nullptr;
	for (FRootMotionWarpingSection_Translation& TranslationSection: WarpingSections_Translation)
	{
		if (CurrentAnimPos >= TranslationSection.StartTime && CurrentAnimPos <= TranslationSection.EndTime) // 在窗口内，缩放RM
		{
			ActiveSection = &TranslationSection;
			ActiveTranslationSection = &TranslationSection;
			break;
		}
	}

	if (ActiveTranslationSection)
	{
		if (!ActiveTranslationSection->AnimationTotalDistance.IsSet())
		{
			const FVector AnimationTranslation =Character.GetMesh()->ConvertLocalRootMotionToWorld(UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, ActiveTranslationSection->StartTime, ActiveTranslationSection->EndTime)).GetTranslation();
			ActiveTranslationSection->AnimationTotalDistance = AnimationTranslation.Size2D();
			if (ActiveTranslationSection->Direction == FVector::ZeroVector)
			{
				ActiveTranslationSection->Direction = AnimationTranslation.GetSafeNormal2D();
				if (ActiveTranslationSection->Direction == FVector::ZeroVector)
				{
					ActiveTranslationSection->Direction = Character.GetActorForwardVector();
				}
			}
		}
		
		FVector ScaledTranslation = FVector(0, 0,  0);
		
		if (ActiveTranslationSection->bKeepAnimationRhythm && ActiveTranslationSection->AnimationTotalDistance.GetValue() > ANIMATION_DISTANCE_THRESHOLD )
		{
			// 动画有位移，按动画位移进行缩放
			const FVector DeltaRMTranslation = Character.GetMesh()->ConvertLocalRootMotionToWorld(UMotionWarpingUtilities::ExtractRootMotionFromAnimation(Montage, CurrentAnimPos, CurrentAnimPos + SimulationTime * PlayRate)).GetTranslation();
			
			if (FVector::DotProduct(DeltaRMTranslation.GetSafeNormal2D(),ActiveTranslationSection->Direction)>0)
			{
				//这一帧的位移方向和总位移方向一致
				const float  TranslationScale = DeltaRMTranslation.Size2D() / ActiveTranslationSection->AnimationTotalDistance.GetValue();
				ScaledTranslation = TranslationScale * ActiveTranslationSection->WarpingDistance * ActiveTranslationSection->Direction;
			}
			else
			{
				//这一帧的位移方向和总位移方向不一致,则输出原动画位移，且不计入总位移
				ScaledTranslation = DeltaRMTranslation;
			}
		}
		else
		{
			//动画没有位移，按时间缩放
			
			const float  TranslationScale = FMath::Min(SimulationTime * PlayRate / (ActiveTranslationSection->EndTime - ActiveTranslationSection->StartTime),1.f);
				
			ScaledTranslation = TranslationScale * ActiveTranslationSection->WarpingDistance * ActiveTranslationSection->Direction;
			
		}
		
		DeltaTransform.Accumulate(FTransform(ScaledTranslation));

		return true;
	}
	return false;
}

bool FWKRootMotionSource_RootMotionWarping::HandleRotationWarping(const ACharacter& Character, const float& SimulationTime, FRotator& DeltaRotation)
{
	FRootMotionWarpingSection_Rotation* ActiveRotationSection = nullptr;
	for (FRootMotionWarpingSection_Rotation& RotationSection: WarpingSections_Rotation)
	{
		if (CurrentAnimPos >= RotationSection.StartTime && CurrentAnimPos <= RotationSection.EndTime)
		{
			ActiveRotationSection = &RotationSection;
			break;
		}
	}
				
	if (ActiveRotationSection)
	{
		if (ActiveRotationSection->TargetActor.IsValid())
		{
			FRotator CurrentRot = Character.GetActorRotation();
			const FVector TargetDirection = (ActiveRotationSection->TargetActor->GetActorLocation()- Character.GetActorLocation()).GetSafeNormal2D();
			const float DesiredYaw = TargetDirection.Rotation().Yaw;
			
			if (ActiveRotationSection->RotationAngleSpeed >= 0.f)
			{
				DeltaRotation.Yaw = CalculateDeltaAngle(CurrentRot.Yaw,DesiredYaw,ActiveRotationSection->RotationAngleSpeed,SimulationTime * PlayRate,ActiveRotationSection->RotationOrientation);
				//TODO 不除以2会来回反复，原因待查
				DeltaRotation.Yaw = DeltaRotation.Yaw/2;
			}
			else
			{
				//如果不配角速度，则从通知开始到通知结束刚好转到面向目标的朝向
				const float Scale = FMath::Min(SimulationTime * PlayRate / (ActiveRotationSection->EndTime - CurrentAnimPos),1.f);

				switch (ActiveRotationSection->RotationOrientation)
				{
				case EWKRotateOrientation::MinAngle:
					DeltaRotation.Yaw = FRotator::NormalizeAxis(DesiredYaw - Character.GetActorRotation().Yaw)* Scale;
					break;
				case EWKRotateOrientation::Clockwise:
					DeltaRotation.Yaw = FRotator::ClampAxis(DesiredYaw - Character.GetActorRotation().Yaw)* Scale;
					break;
				case EWKRotateOrientation::Anticlockwise:
					DeltaRotation.Yaw = (FRotator::ClampAxis(DesiredYaw - Character.GetActorRotation().Yaw) - 360)* Scale;
					break;
				}
			}
		}
		else
		{
			if (!ActiveRotationSection->StartRotation.IsSet())
			{  
				ActiveRotationSection->StartRotation = Character.GetActorRotation().Yaw;
			}
					
			float Alpha = FMath::Clamp((CurrentAnimPos + SimulationTime * PlayRate - ActiveRotationSection->StartTime)/(ActiveRotationSection->EndTime - ActiveRotationSection->StartTime),0.f,1.f);

			float CurrentRot = FRotator::NormalizeAxis(ActiveRotationSection->StartRotation.GetValue() + ActiveRotationSection->WarpingRotation*Alpha);

			DeltaRotation.Yaw = FRotator::NormalizeAxis(CurrentRot - Character.GetActorRotation().Yaw);
		}

		return true;
	}
	return false;
}

void FWKRootMotionSource_RootMotionWarping::HandleRootMotionSourceParams(FRootMotionWarpingSection* ActiveSection, const UCharacterMovementComponent& MoveComponent)
{
	if (ActiveSection)
	{
		AccumulateMode = ActiveSection->AccumulateMode;
		if (ActiveSection->bIgnoreZAccumulate)
		{
			Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
		}
		else
		{
			Settings.UnSetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
		}
			
		if (!LastSection.IsValid()
			|| ((LastSection.Get()->StartTime != ActiveSection->StartTime)&&(
				LastSection.Get()->EndTime != ActiveSection->EndTime)))
		{
			LastSection.Reset();
			LastSection = MakeShared<FRootMotionWarpingSection>(*ActiveSection);
		}
	}
	else
	{
		AccumulateMode = ERootMotionAccumulateMode::Override;
		Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
		
		if (LastSection.IsValid())
		{
			if (!LastSection->bMaintainVelocityWhenFinish)
			{
				if (UCharacterMovementComponent* MutableMovementComponent = const_cast<UCharacterMovementComponent*>(&MoveComponent))
				{
					MutableMovementComponent->Velocity = FVector::ZeroVector;
					RootMotionParams.Set(FTransform::Identity);
				}
			}
			LastSection.Reset();
		}
	}
}

float FWKRootMotionSource_RootMotionWarping::CalculateDeltaAngle(float CurrentYaw, float DesiredYaw, float RotationAngleSpeed, float DeltaSeconds, EWKRotateOrientation RotationOrientation)
{
	if (FMath::IsNearlyEqual(CurrentYaw, DesiredYaw))
	{
		return 0;
	}

	float DeltaAngle = 0;
	
	const float AxisDeltaRotation = FMath::Min(FMath::Abs(RotationAngleSpeed * DeltaSeconds), 360.f);

	const float TargetYaw = FMath::FixedTurn(CurrentYaw, DesiredYaw, AxisDeltaRotation);
	switch (RotationOrientation)
	{
	case EWKRotateOrientation::MinAngle:
		DeltaAngle =FRotator::NormalizeAxis(TargetYaw - CurrentYaw);
		break;
	case EWKRotateOrientation::Clockwise:
		if (FMath::Abs(FRotator::ClampAxis(CurrentYaw) - FRotator::ClampAxis(DesiredYaw)) < AxisDeltaRotation)
		{
			//容忍范围内不管方向转向目标，否则会导导致只差一点点也转360度
			DeltaAngle = FRotator::NormalizeAxis(TargetYaw - CurrentYaw);
		}
		else
		{
			DeltaAngle = AxisDeltaRotation;
		}
		break;
	case EWKRotateOrientation::Anticlockwise:
		if (FMath::Abs(FRotator::ClampAxis(CurrentYaw) - FRotator::ClampAxis(DesiredYaw)) < AxisDeltaRotation)
		{
			DeltaAngle = FRotator::NormalizeAxis(TargetYaw - CurrentYaw);
		}
		else
		{
			DeltaAngle = -AxisDeltaRotation;
		}
		break;
	}
	
	return DeltaAngle;
}

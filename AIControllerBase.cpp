// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

AAIControllerBase::AAIControllerBase() : AAIController()
{
	SetGenericTeamId(FGenericTeamId(5));
}

ETeamAttitude::Type AAIControllerBase::GetTeamAttitudeTowards(const AActor& Other) const
{

	if (const APawn* OtherPawn = Cast<APawn>(&Other)) {

		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
		}
	}
	return ETeamAttitude::Neutral;
}

bool AAIControllerBase::CalculateDetection(ACharacter* PawnDetected, ACharacter* Detector)
{
 	UWorld* World = GetWorld();
	if (World == nullptr) return false;
	if (Detector != nullptr && PawnDetected != nullptr)
	{
		FVector TraceStart = Detector->GetActorLocation();
		UMeshComponent* DetectorMesh = Detector->GetMesh();
		UMeshComponent* PawnDetectedMesh = PawnDetected->GetMesh();
		if(!ensure(DetectorMesh != nullptr && PawnDetectedMesh != nullptr)) return false;
		if (DetectorMesh != nullptr)
		{
			TraceStart = DetectorMesh->GetSocketLocation(this->DetectorTraceStartBone);	
		}
		if (BonesDetection.Num() != 0)
		{
			TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

			TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			
			FCollisionObjectQueryParams const DetectionTraceParams = FCollisionObjectQueryParams(TraceObjectTypes);
			
			FHitResult HitResult;
			int LinesHit = 0;
			for (auto const Bone : BonesDetection)
			{
				FVector BoneLocation = PawnDetectedMesh->GetSocketLocation(Bone);
				FCollisionQueryParams TraceParams;
				TraceParams.AddIgnoredActor(Detector);
				World->LineTraceSingleByObjectType(HitResult,TraceStart,BoneLocation,DetectionTraceParams,TraceParams);
				 
				
				 //World->LineTraceSingleByChannel(HitResult,TraceStart,BoneLocation,ECC_Visibility,TraceParams);
				 if(bDetectionDebug)
				 {
				 	DrawDebugLine(World,TraceStart,BoneLocation,FColor::Magenta,true,-1.f,0,1.5f);
				 }
				if(HitResult.GetActor() == PawnDetected)
				{
					LinesHit++;
				}
			}
			if (LinesHit >= DetectionLineSuccessThreshold)
			{
				return true;
			}
		}
	}
	return false;
}

void AAIControllerBase::CalculateAwarenessLevelOnDetection(AActor* PawnDetected, AActor* Detector)
{
	float const  Distance = FVector::Distance(PawnDetected->GetActorLocation(), Detector->GetActorLocation());
	
	if (Distance <= AwarenessThresholds.FindRef(EAwarenessThreshold::Low))
	{
		this->Awareness = AwarenessInitialValues.Low;
	}
	else if (Distance <= AwarenessThresholds.FindRef(EAwarenessThreshold::Medium))
	{
		this->Awareness = this->AwarenessInitialValues.Medium;
	}
	else if ( Distance <= AwarenessThresholds.FindRef(EAwarenessThreshold::High) )
	{
		Awareness = this->AwarenessInitialValues.High;
	}
}

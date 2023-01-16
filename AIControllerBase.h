// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */

class ABaseGlobalCharacter;
UENUM(BlueprintType)
enum class EAwarenessThreshold : uint8
{
	Low UMETA(DisplayName="Low"),
	Medium  UMETA(DisplayName="Medium"),
	High  UMETA(DisplayName="High")
};

USTRUCT(BlueprintType)
struct FAwarenessInitialValues
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float Low;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float Medium;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float High;
};

UCLASS(Blueprintable)
class PERSONAL_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
	AAIControllerBase();
public:
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float Awareness;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float AwarenessMax = 100.f;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Detection")
	TMap<EAwarenessThreshold,float> AwarenessThresholds;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Detection")
	FAwarenessInitialValues AwarenessInitialValues;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Detection")
	TArray<FName> BonesDetection;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Detection")
	FName DetectorTraceStartBone = TEXT("head");

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Detection")
	int DetectionLineSuccessThreshold = 2;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Detection")
	bool bDetectionDebug;
	
	UFUNCTION(BlueprintCallable)
	bool CalculateDetection(ACharacter* PawnDetected, ACharacter* Detector);

	UFUNCTION(BlueprintCallable)
	void CalculateAwarenessLevelOnDetection(AActor* PawnDetected,AActor* Detector);
	
};

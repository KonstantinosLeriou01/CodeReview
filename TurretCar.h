// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretCar.generated.h"

UCLASS()
class PERSONAL_API ATurretCar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurretCar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


#region Variables

    UPROPERTY(BlueprintReadWrite)
    float HeatAiCooldown = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float OverheatCooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	float Heat = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn=true))
	bool bPlayer = false;

	UPROPERTY(VisibleAnywhere)
	FVector TargetLoc;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* StaticMeshh;

	UPROPERTY(VisibleDefaultsOnly)
	FHitResult HitResult;

	UPROPERTY(BlueprintReadWrite)
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShoot = true;

	UPROPERTY(VisibleDefaultsOnly)
	FTimerHandle TimerHandle;

#endregion
#region Events
	UFUNCTION()
	void TurnToTargetCPP();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Events", meta = (DisplayName = "StartingEvent"))
	void StartingEvent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Events", meta = (DisplayName = "StoppingEvent"))
	void StoppingEvent();

	UFUNCTION(BlueprintCallable, Category = "Events", meta = (DisplayName = "ShootEvent"))
	void ShootEvent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Events", meta = (DisplayName = "Shooting"))
	void Shooting();
#endregion
	
};

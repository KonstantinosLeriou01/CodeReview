// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretCar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"

// Sets default values
ATurretCar::ATurretCar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void ATurretCar::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshh = this->FindComponentByClass<UStaticMeshComponent>();
}

// Called every frame
void ATurretCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (HeatAiCooldown == 0.f)
    {
	    if (OverheatCooldown > 0.f)
	    {
		    OverheatCooldown = OverheatCooldown - DeltaTime;
		    if (OverheatCooldown <= 0.f)
		    {
			    OverheatCooldown = 0.f;
		    	Heat = 1.f;
		    }
	    }
	    else
	    {
		    if (Heat > 0.f)
		    {
			    if (bShoot)
			    {
			    	//Heat = UKismetMathLibrary::BMax(Heat - 0.001f, 0.f);
			    	Heat = Heat - 0.01f;
			    }
			    else
			    {
			    	Heat = Heat + 0.01f;
			    	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Reached"));
			    }
		    }
		    else
		    {
			    if (!bShoot)
			    {
			    	Heat = Heat + 0.08f;
			    }
		    }
	    }
	    
    }
    else
    {
    	if (Heat > 0.f)
    	{
    		Heat = UKismetMathLibrary::BMax(Heat - 0.001f, 0.f);
    	}
    	HeatAiCooldown = HeatAiCooldown - GetWorld()->GetDeltaSeconds();
        if (HeatAiCooldown < 0.f)
        {
	        HeatAiCooldown = 0.f;
        }
    }

	if (bPlayer)
	{
		const APlayerController* PlayerContr =  UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerContr != nullptr)
		{
			PlayerContr->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
			TargetLoc = HitResult.Location;
			TurnToTargetCPP();
		}
	}
	else
	{
		if (Target)
		{
			TargetLoc = Target->GetActorLocation();
			TurnToTargetCPP();
			if (UKismetMathLibrary::Vector4_Size(TargetLoc-GetActorLocation()) < 3000.f)
			{
				// TRIGGER EVENT START
				StartingEvent();
			}
			else
			{
				//TRIGGER EVENT STOP
				StoppingEvent();
			}
		}
	}
	if (Heat < 0.f)
	{
		Heat = 0.f;
	}

	//const FString Str = FString::SanitizeFloat(Heat);
	//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, *Str);

}

void ATurretCar::TurnToTargetCPP()
{
	if (StaticMeshh)
	{
		StaticMeshh->SetWorldRotation(UKismetMathLibrary::RLerp(StaticMeshh->GetComponentRotation(),
			UKismetMathLibrary::FindLookAtRotation(StaticMeshh->GetComponentLocation(), TargetLoc),
			GetWorld()->GetDeltaSeconds()*5.f,
			true));
	}
}

void ATurretCar::ShootEvent()
{
	if (!bShoot)
	{
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ATurretCar::ShootEvent, GetWorld()->DeltaTimeSeconds*15);
		if (OverheatCooldown == 0.f)
		{
			//Heat = Heat + 0.08f;
			if (Heat >= 1.f)
			{
				Heat = 1.f;
				OverheatCooldown = 6.f;
			}
			Shooting();
		}
		else
		{
			StoppingEvent();
		}
	}
}

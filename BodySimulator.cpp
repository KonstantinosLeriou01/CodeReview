// Fill out your copyright notice in the Description page of Project Settings.


#include "BodySimulator.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"

// Sets default values
ABodySimulator::ABodySimulator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 0.0167f;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	SetRootComponent(InstancedMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Game/SM_Circle'"));
	InstancedMesh->SetStaticMesh(MeshAsset.Object);

}

// Called when the game starts or when spawned
void ABodySimulator::BeginPlay()
{
	Super::BeginPlay();
	InitBodies();
}

// Called every frame
void ABodySimulator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsGravityEnabled)
		GravityStep(DeltaTime);
	UpdatePositionStep(DeltaTime);
}

void ABodySimulator::InitBodies()
{
	Bodies.SetNumUninitialized(BodyNum);
	Transforms.SetNumUninitialized(BodyNum);
	for (int32 Index = 0; Index < BodyNum; ++Index) {
		FVector2D RandomPosition(FMath::RandPointInCircle(PlacementRadius));
		const float RadialSpeedFactor = PlacementRadius / RandomPosition.Size();
		FVector2D RandomVelocity {FMath::FRandRange(BaseInitialVelocity - 100.0f, BaseInitialVelocity + 100.0f) / RadialSpeedFactor, 0};
		RandomVelocity = RandomVelocity.GetRotated(90.0f + FMath::RadiansToDegrees(FMath::Atan2(RandomPosition.Y, RandomPosition.X)));
		const float Mass = FMath::FRandRange(MinMass, MaxMass);
		const float MeshScale = FMath::Sqrt(Mass) * BodyDisplayScale;
		const FTransform MeshTransform (
			FRotator(),
			TranslationFrom2DCoordinates(RandomPosition),
			FVector(MeshScale, MeshScale, 1.0f)
		);
		Transforms[Index] = MeshTransform;
		Bodies[Index] = FEntity{RandomPosition, RandomVelocity, Mass, Index};
	}
	InstancedMesh->AddInstances(Transforms, false);
}

void ABodySimulator::GravityStep(float DeltaTime)
{
	ParallelFor(Bodies.Num(), [&] (int32 Index) {
		FVector2D Acceleration(0.0f, 0.0f);
		for (const FEntity& AffectingBody: Bodies) {
			if (AffectingBody.Index == Bodies[Index].Index) continue; // exclude self
			float Distance = FVector2D::Distance(Bodies[Index].Position, AffectingBody.Position);
			Distance = FMath::Max(Distance, MinimumGravityDistance); // avoids division by zero
			Acceleration += AffectingBody.Mass / Distance * G / Distance * (AffectingBody.Position - Bodies[Index].Position) / Distance;
		}
		Bodies[Index].Velocity += Acceleration * DeltaTime ;
	});
}



void ABodySimulator::UpdatePositionStep(float DeltaTime)
{
	const FVector2D HalfWorld(WorldWidth * 0.5f, WorldHeight * 0.5f);
	for (FEntity& Body: Bodies) {
		Body.Position += Body.Velocity * DeltaTime;
		Body.Position.X = FMath::Wrap(Body.Position.X, -HalfWorld.X, HalfWorld.X);
		Body.Position.Y = FMath::Wrap(Body.Position.Y, -HalfWorld.Y, HalfWorld.Y);
		Transforms[Body.Index].SetTranslation(TranslationFrom2DCoordinates(Body.Position));
	}
	InstancedMesh->BatchUpdateInstancesTransforms(0, Transforms, false, true);
}

FVector ABodySimulator::TranslationFrom2DCoordinates(const FVector2D& XYCoordinates)
{
	return FVector(XYCoordinates.Y, XYCoordinates.X, 0.0f);
}


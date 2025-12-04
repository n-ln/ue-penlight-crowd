// Fill out your copyright notice in the Description page of Project Settings.


#include "PenLightAreaLight.h"

#include "Components/RectLightComponent.h"
#include "PenLightPCGData.h"
#include "PenLightManager.h"


// Sets default values
APenLightAreaLight::APenLightAreaLight()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0333f;
	
	Parent = CreateDefaultSubobject<USceneComponent>("Parent");
	RootComponent = Parent;
	RectLightDownward = CreateDefaultSubobject<URectLightComponent>("RectLightDownward");
	RectLightDownward->SetupAttachment(Parent);
	RectLightUpward = CreateDefaultSubobject<URectLightComponent>("RectLightUpward");
	RectLightUpward->SetupAttachment(Parent);
	PenLightPCGData = CreateDefaultSubobject<UPenLightPCGData>("PenLightPCGData");
	
	RectLightDownward->BarnDoorAngle = 90.0f;
	RectLightDownward->BarnDoorLength = 0.0f;
	RectLightDownward->SetIntensityUnits(ELightUnits::Lumens);
	RectLightDownward->SetIntensity(10.0f);
	RectLightUpward->BarnDoorAngle = 90.0f;
	RectLightUpward->BarnDoorLength = 0.0f;
	RectLightUpward->SetIntensityUnits(ELightUnits::Lumens);
	RectLightUpward->SetIntensity(10.0f);
	
	RectLightOffset = 0.0f;
	RectLightWaveAmplitude = 0.0f;
	RectLightHeightFromGround = 250.0f;
	UpwardRelativeIntensityInverse = 8.0f;
	DownwardRelativeIntensityInverse = 8.0f;
	
	RectLightDownward->SetRelativeLocation(FVector(0.0f, 0.0f, RectLightHeightFromGround + RectLightOffset));
	RectLightUpward->SetRelativeLocation(FVector(0.0f, 0.0f, RectLightHeightFromGround - RectLightOffset));
}

// Called when the game starts or when spawned
void APenLightAreaLight::BeginPlay()
{
	Super::BeginPlay();
	
	// Construct audience from saved PCG data
	SetupFromPCGData();
	
	// Get PenLightManager
	TWeakObjectPtr<APenLightManager> PenLightManagerInstance = APenLightManager::GetPenLightManager(this);
	if (PenLightManagerInstance.IsValid() && PenLightManagerInstance->GetIsInitialized())
	{
		// If initialized register this area light
		PenLightManager = PenLightManagerInstance;
		PenLightManager->RegisterAreaLight(this);
	}
	else
	{
		// If not initialized, subscribe to delegate and handle registration in the callback
		APenLightManager::OnPenLightManagerInitialized().AddUObject(this, &APenLightAreaLight::OnPenLightManagerInitialized);
	}

}

void APenLightAreaLight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister area light
	PenLightManager->UnregisterAreaLight(this);
	
	Super::EndPlay(EndPlayReason);
}

void APenLightAreaLight::OnPenLightManagerInitialized(TWeakObjectPtr<APenLightManager> PenLightManagerInstance)
{
	if (PenLightManagerInstance.IsValid() && PenLightManagerInstance->GetIsInitialized())
	{
		PenLightManager = PenLightManagerInstance;
		PenLightManager->RegisterAreaLight(this);
	}
}

// Called every frame
void APenLightAreaLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const int32 CurrentFrame = PenLightManager->GetCurrentMasterAnimFrame();
	float Phase = FMath::Frac(static_cast<float>(CurrentFrame) / static_cast<float>(PenLightManager->GetPenLightAnimFrameRate()))
					* 2.0f * PI;
	/* Do not use location offset for light fluctuation
	float Height = RectLightHeightFromGround + -1.0f * FMath::Cos(Phase) * RectLightWaveAmplitude;
	RectLightDownward->SetRelativeLocation(FVector(0.0f, 0.0f, Height + RectLightOffset));
	RectLightUpward->SetRelativeLocation(FVector(0.0f, 0.0f, Height - RectLightOffset));
	*/
	float IntensityFactor = FMath::Lerp(0.25f, 1.0f, -0.5f * FMath::Cos(Phase) + 0.5f);
	RectLightDownward->SetIntensity(IntensityFactor * BaseIntensity / DownwardRelativeIntensityInverse);
	RectLightUpward->SetIntensity(IntensityFactor * BaseIntensity / UpwardRelativeIntensityInverse);
}

void APenLightAreaLight::SavePCGData()
{
	PenLightPCGData->SetSeatVector(TempSeatVector);
	PenLightPCGData->SetLightVector(TempLightVector);
	PenLightPCGData->SetSpawnRotation(TempSpawnRotation);
	PenLightPCGData->SetAudienceSection(TempAudienceSection);
	
	SetupFromPCGData();
}

void APenLightAreaLight::SetupFromPCGData()
{
	// Set up area light from saved PCG data
	SetActorRotation(PenLightPCGData->SpawnRotation);
	RectLightDownward->SetSourceWidth(PenLightPCGData->LightVector.X);
	RectLightUpward->SetSourceWidth(PenLightPCGData->LightVector.X);
	
	RectLightDownward->SetSourceHeight(FMath::Sqrt(FMath::Pow(PenLightPCGData->LightVector.Y, 2) + FMath::Pow(PenLightPCGData->LightVector.Z, 2)));
	RectLightUpward->SetSourceHeight(FMath::Sqrt(FMath::Pow(PenLightPCGData->LightVector.Y, 2) + FMath::Pow(PenLightPCGData->LightVector.Z, 2)));
	
	RectLightDownward->SetRelativeRotation(FRotator(-90.0f, 90.0f, 0.0f));
	RectLightUpward->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	
	RectLightDownward->AddRelativeRotation(FRotator(-FMath::RadiansToDegrees(FMath::Atan2(PenLightPCGData->LightVector.Z, PenLightPCGData->LightVector.Y)), 0.0f, 0.0f));
	RectLightUpward->AddRelativeRotation(FRotator(-FMath::RadiansToDegrees(FMath::Atan2(PenLightPCGData->LightVector.Z, PenLightPCGData->LightVector.Y)), 0.0f, 0.0f));
	
	RectLightDownward->SetAttenuationRadius(PenLightPCGData->LightVector.Length() / 2.0f + 100.0f);
	RectLightUpward->SetAttenuationRadius(PenLightPCGData->LightVector.Length() / 2.0f + 100.0f);
	
	const FVector Scale = PenLightPCGData->LightVector / PenLightPCGData->SeatVector.ComponentMax(FVector(0.0001f));
	RectLightScale = FVector2D(Scale.X, Scale.Y);
}

void APenLightAreaLight::SetEnabled_Implementation(const bool bNewIsEnabled)
{
	bIsEnabled = bNewIsEnabled;
	RectLightDownward->SetVisibility(bIsEnabled);
	RectLightUpward->SetVisibility(bIsEnabled);
}

void APenLightAreaLight::SetPenLightIntensity_Implementation(const float NewPenLightIntensity)
{
	PenLightIntensity = NewPenLightIntensity;
	BaseIntensity = PenLightIntensity * RectLightScale.X * RectLightScale.Y;
	//RectLightDownward->SetIntensity(BaseIntensity / DownwardRelativeIntensityInverse);
	//RectLightUpward->SetIntensity(BaseIntensity / UpwardRelativeIntensityInverse);
}

void APenLightAreaLight::SetPenLightColor_Implementation(const FLinearColor NewPenLightColor)
{
	PenLightColor = NewPenLightColor;
	RectLightDownward->SetLightColor(PenLightColor);
	RectLightUpward->SetLightColor(PenLightColor);
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "PenLightAudience.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "InstancedActorsComponent.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PenLightPCGData.h"
#include "PenLightManager.h"


// Sets default values
APenLightAudience::APenLightAudience()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.0333f;
	
	//bIsFinishedSetup = false;
	
	CharacterVAT = CreateDefaultSubobject<UStaticMeshComponent>("CharacterVAT");
	CharacterVAT->SetGenerateOverlapEvents(false);
	CharacterVAT->SetComponentTickEnabled(false);
	RootComponent = CharacterVAT;
	PenLightVAT = CreateDefaultSubobject<UStaticMeshComponent>("PenLightVAT");
	PenLightVAT->SetGenerateOverlapEvents(false);
	PenLightVAT->SetComponentTickEnabled(false);
	PenLightVAT->SetupAttachment(CharacterVAT);
	PenLightVAT->AddLocalOffset(FVector(0.0f, 0.0f, 100.0f));
	Character = CreateDefaultSubobject<USkeletalMeshComponent>("Character");
	Character->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Character->bPauseAnims = true;
	Character->SetGenerateOverlapEvents(false);
	Character->bUpdateOverlapsOnAnimationFinalize = false;
	Character->SetupAttachment(CharacterVAT);
	PenLight = CreateDefaultSubobject<UStaticMeshComponent>("PenLight");
	PenLight->SetGenerateOverlapEvents(false);
	PenLight->SetupAttachment(Character, FName("HandGrip_L"));
	PointLight = CreateDefaultSubobject<UPointLightComponent>("PointLight");
	PointLight->SetupAttachment(PenLight, FName("Light"));
	PointLight->SetIntensityUnits(ELightUnits::Lumens);
	PointLight->SetIntensity(10.0f);
	
	PenLightPCGData = CreateDefaultSubobject<UPenLightPCGData>("PenLightPCGData");
	InstancedActorsComponent = CreateDefaultSubobject<UInstancedActorsComponent>("InstancedActorsComponent");
	
	// Hide High LOD components by default
	bIsUsingHighLOD = false;
	CharacterVAT->SetVisibility(true);
	PenLightVAT->SetVisibility(true);
	Character->SetVisibility(false);
	PenLight->SetVisibility(false);
	PointLight->SetVisibility(false);
	
}

// Called when the game starts or when spawned
void APenLightAudience::BeginPlay()
{
	Super::BeginPlay();
	
	WorldPtr = GetWorld();
	
	if (ensureMsgf(UGameplayStatics::GetPlayerCameraManager(WorldPtr, 0), TEXT("No PlayerCameraManager found.")))
	{
		PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(WorldPtr, 0);
	}
	
	// Get PenLightManager
	TWeakObjectPtr<APenLightManager> PenLightManagerInstance = APenLightManager::GetPenLightManager(this);
	if (PenLightManagerInstance.IsValid() && PenLightManagerInstance->GetIsInitialized())
	{
		// If initialized register this audience
		PenLightManager = PenLightManagerInstance;
		PenLightManager->RegisterAudience(this);
	}
	else
	{
		// If not initialized, subscribe to delegate and handle registration in the callback
		APenLightManager::OnPenLightManagerInitialized().AddUObject(this, &APenLightAudience::OnPenLightManagerInitialized);
	}
	
	UpdateAnimParameters();
	
	// Temp: Setup some parameters here
	//Character->SetForcedLOD(2);
	// This need to match the rotation baked in the VAT for PenLight
	PenLightRotationOffset = FRotator(0.0f, 0.0f, 45.0f);
	PenLight->AddRelativeRotation(PenLightRotationOffset);
	PointLight->AttenuationRadius = 75.0f;
	PointLight->SourceRadius = 2.0f;
	PointLight->SourceLength = 12.0f;
	
	// Offset the scale of PenLights with the inverse of Actor scale
	ActorScale = GetActorScale3D();
	OffsetScale = FVector::OneVector / ActorScale;
	// We can't apply scale in a straightforward way to the VAT penlight mesh right now,
	// since it will mess up baked transform in the VAT.
	// Instead, we do a gradual scale change of the Skeletal mesh for a smooth transition.
	//PenLight->SetRelativeScale3D(OffsetScale);
	//PenLightVAT->SetRelativeScale3D(OffsetScale);
	
	// Enable Tick
	SetActorTickEnabled(true);
}

void APenLightAudience::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister audience
	PenLightManager->UnregisterAudience(this);
	
	Super::EndPlay(EndPlayReason);
}

void APenLightAudience::OnPenLightManagerInitialized(TWeakObjectPtr<APenLightManager> PenLightManagerInstance)
{
	if (PenLightManagerInstance.IsValid() && PenLightManagerInstance->GetIsInitialized())
	{
		PenLightManager = PenLightManagerInstance;
		PenLightManager->RegisterAudience(this);
	}
}

// Called every frame
void APenLightAudience::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Calculate Animation Position for High LOD the same way as in VAT material
	// TODO Need to fix popping at start of transition if only calculating Pos when bIsUsingHighLOD = true
	const int32 CurrentFrame = FMath::RoundToInt(static_cast<float>(PenLightManager->GetCurrentMasterAnimFrame()) +
								FMath::Lerp(-MaxAnimOffsetFrames, MaxAnimOffsetFrames, 
									FMath::Clamp(FMath::Frac(FMath::Abs(FVector::DotProduct(
											GetActorLocation(), RandomSeed))), 0.0f, 1.0f))
								+ static_cast<float>(PenLightAnimFrameRate)) % PenLightAnimFrameRate;
	const float AnimPos = static_cast<float>(CurrentFrame) / static_cast<float>(PenLightAnimFrameRate);
	Character->SetPosition(AnimPos);
	
	// Get distance to camera
	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	// Use High LOD if within certain distance and in front of camera
	const FVector ActorLocationOffset = FVector(0.0f, 0.0f, 170.0f);
	const float DistanceToCamera = FVector::Dist(GetActorLocation() + ActorLocationOffset, CameraLocation);
	const bool bIsInFrontOfCamera = FVector::DotProduct((GetActorLocation() - CameraLocation).GetSafeNormal(), CameraRotation.Vector()) > 0.0f;
	const bool bShouldUseHighLOD = DistanceToCamera <= 200.0f && bIsInFrontOfCamera;
	if (bShouldUseHighLOD != bIsUsingHighLOD)
	{
		bIsUsingHighLOD = bShouldUseHighLOD;
		bIsTransitioning = true;
		TransitionStartTime = UGameplayStatics::GetRealTimeSeconds(WorldPtr);
		
		if (bIsUsingHighLOD) // Low LOD to High LOD
		{
			UpdateAnimParameters();
			SwitchLOD();
		}
		else  // High LOD to Low LOD
		{
			// Defer switch until HLOD Penlight finish scaling back
		}
	}
	
	if (bIsTransitioning)
	{
		const float ElapsedTime = UGameplayStatics::GetRealTimeSeconds(WorldPtr) - TransitionStartTime;
		if (ElapsedTime < TransitionTime)
		{
			if (bIsUsingHighLOD)
			{
				PenLight->SetRelativeScale3D(FMath::Lerp(FVector::OneVector, OffsetScale, 
										FMath::Clamp(ElapsedTime / TransitionTime, 0.0f, 1.0f)));
			}
			else
			{
				PenLight->SetRelativeScale3D(FMath::Lerp(OffsetScale, FVector::OneVector, 
										FMath::Clamp(ElapsedTime / TransitionTime, 0.0f, 1.0f)));
			}
		}
		else // Finished transition
		{
			if (bIsUsingHighLOD)
			{
				PenLight->SetRelativeScale3D(OffsetScale);
			}
			else
			{
				PenLight->SetRelativeScale3D(FVector::OneVector);
				SwitchLOD();
			}
			bIsTransitioning = false;
		}
	}
}

/** ~Begin IPenLightInterface interface */
void APenLightAudience::SetEnabled_Implementation(const bool bNewIsEnabled)
{
	bIsEnabled = bNewIsEnabled;
	PointLight->SetVisibility(bIsEnabled && bIsUsingHighLOD);
}

void APenLightAudience::SetPenLightIntensity_Implementation(const float NewPenLightIntensity)
{
	PenLightIntensity = NewPenLightIntensity;
	PointLight->SetIntensity(PenLightIntensity);
}

void APenLightAudience::SetPenLightColor_Implementation(const FLinearColor NewPenLightColor)
{
	PenLightColor = NewPenLightColor;
	PointLight->SetLightColor(PenLightColor);
}
/** ~End IPenLightInterface interface */

void APenLightAudience::SwitchLOD()
{
	// Update visibility
	Character->SetVisibility(bIsUsingHighLOD);
	PenLight->SetVisibility(bIsUsingHighLOD);
	PointLight->SetVisibility(bIsEnabled && bIsUsingHighLOD);
	
	CharacterVAT->SetVisibility(!bIsUsingHighLOD);
	PenLightVAT->SetVisibility(!bIsUsingHighLOD);
}

void APenLightAudience::UpdateAnimParameters()
{
	RandomSeed = PenLightManager->GetRandomSeed();
	MaxAnimOffsetFrames = PenLightManager->GetMaxAnimOffsetFrames();
	PenLightAnimFrameRate = PenLightManager->GetPenLightAnimFrameRate();
}

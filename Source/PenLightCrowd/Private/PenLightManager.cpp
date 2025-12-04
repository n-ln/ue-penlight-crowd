
#include "PenLightManager.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "AudienceSection.h"

TWeakObjectPtr<APenLightManager> APenLightManager::PenLightManagerInstance;
FOnPenLightManagerInitialized APenLightManager::PenLightManagerInitializedEvent;

// Sets default values
APenLightManager::APenLightManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

TWeakObjectPtr<APenLightManager> APenLightManager::GetPenLightManager(const UObject* WorldContextObject)
{
	if (PenLightManagerInstance.IsValid())
	{
		return  PenLightManagerInstance;
	}
	else
	{
		if (WorldContextObject)
		{
			TArray<AActor*> PenLightManagers;
			UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APenLightManager::StaticClass(), PenLightManagers);
			if (ensureMsgf(PenLightManagers.Num() != 0, TEXT("No PenLightManager found in the level.")))
			{
				ensureMsgf(PenLightManagers.Num() >= 1, TEXT("More than one PenLightManager found in the level."));
				PenLightManagerInstance = Cast<APenLightManager>(PenLightManagers[0]);
			}
		}
		return PenLightManagerInstance;
	}
}

// Called when the game starts or when spawned
void APenLightManager::BeginPlay()
{
	Super::BeginPlay();
	
	WorldPtr = GetWorld();
	
	PenLightManagerInstance = this;
	
	if (ensureMsgf(MPCPenLight, TEXT("MPC PenLight is not set in PenLightManager")))
	{
		MPCPenLightInstance = WorldPtr->GetParameterCollectionInstance(MPCPenLight);
	}
	if (ensureMsgf(MPCPenLight, TEXT("MPC Bone Animation is not set in PenLightManager")))
	{
		MPCBoneAnimationInstance = WorldPtr->GetParameterCollectionInstance(MPCBoneAnimation);
	}
	
	OnLightsOnChanged();
	OnPenLightIntensityChanged();
	OnEmissiveValueChanged();
	OnPenLightColorChanged();
	OnIsWavingChanged();
	OnPenLightBPMChanged();
	OnPenLightAnimFrameRateChanged();
	OnMaxAnimOffsetFramesChanged();
	
	bIsInitialized = true;
	PenLightManagerInitializedEvent.Broadcast(this);
}

// Called every frame
void APenLightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsWaving)
	{
		CurrentSessionElapsedTime = UGameplayStatics::GetRealTimeSeconds(WorldPtr) - CurrentSessionStartTime;
		// Current animation frame, before individual offsets
		CurrentMasterAnimFrame = FMath::Floor(PenLightAnimFrameRate * FMath::Frac(CurrentSessionElapsedTime * PenLightBPM / 60.0f));
		if (CurrentMasterAnimFrame != PrevMasterAnimFrame)
		{
			PrevMasterAnimFrame = CurrentMasterAnimFrame;
			// Set Current Frame to Material Parameter Collection
			MPCBoneAnimationInstance->SetScalarParameterValue(FName("Frame"), CurrentMasterAnimFrame);
		}
		
	}
}

#if WITH_EDITOR
void APenLightManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName ChangedProperty = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;
}
#endif

void APenLightManager::RegisterAudienceSection(AAudienceSection* NewAudienceSection)
{
	AudienceSections.Add(FName(*NewAudienceSection->GetName()), NewAudienceSection);
}

void APenLightManager::UnregisterAudienceSection(const AAudienceSection* AudienceSection)
{
	AudienceSections.Remove(FName(*AudienceSection->GetName()));
}

void APenLightManager::RegisterAudience(AActor* NewAudienceActor)
{
	Audiences.Add(FName(*NewAudienceActor->GetName()), NewAudienceActor);
	
	InterfaceSetEnabled(NewAudienceActor, bLightsOn);
	InterfaceSetPenLightIntensity(NewAudienceActor, PenLightIntensity);
	InterfaceSetPenLightColor(NewAudienceActor, PenLightColor);
}

void APenLightManager::UnregisterAudience(const AActor* AudienceActor)
{
	Audiences.Remove(FName(*AudienceActor->GetName()));
}

void APenLightManager::RegisterAreaLight(AActor* NewAreaLightActor)
{
	AreaLights.Add(FName(*NewAreaLightActor->GetName()), NewAreaLightActor);
	
	InterfaceSetEnabled(NewAreaLightActor, bLightsOn);
	InterfaceSetPenLightIntensity(NewAreaLightActor, PenLightIntensity);
	InterfaceSetPenLightColor(NewAreaLightActor, PenLightColor);
}

void APenLightManager::UnregisterAreaLight(const AActor* AreaLightActor)
{
	AreaLights.Remove(FName(*AreaLightActor->GetName()));
}

void APenLightManager::SetLightsOn(const bool bNewLightsOn)
{
	if (bLightsOn != bNewLightsOn)
	{
		bLightsOn = bNewLightsOn;
		OnLightsOnChanged();
	}
}

void APenLightManager::SetPenLightIntensity(const float NewPenLightIntensity)
{
	if (!FMath::IsNearlyEqual(PenLightIntensity, NewPenLightIntensity))
	{
		PenLightIntensity = NewPenLightIntensity;
		OnPenLightIntensityChanged();
	}
}

void APenLightManager::SetEmissiveValue(const float NewEmissiveValue)
{
	if (!FMath::IsNearlyEqual(EmissiveValue, NewEmissiveValue))
	{
		EmissiveValue = NewEmissiveValue;
		OnEmissiveValueChanged();
	}
}

void APenLightManager::SetPenLightColor(const FLinearColor NewPenLightColor)
{
	if (PenLightColor != NewPenLightColor)
	{
		PenLightColor = NewPenLightColor;
		OnPenLightColorChanged();
	}
}

void APenLightManager::SetIsWaving(const bool bNewIsWaving)
{
	if (bIsWaving != bNewIsWaving)
	{
		bIsWaving = bNewIsWaving;
		OnIsWavingChanged();
	}
}

void APenLightManager::SetPenLightBPM(const float NewPenLightBPM)
{
	if (!FMath::IsNearlyEqual(PenLightBPM, NewPenLightBPM))
	{
		PenLightBPM = NewPenLightBPM;
		OnPenLightBPMChanged();
	}
}

void APenLightManager::SetPenLightAnimFrameRate(const int32 NewPenLightAnimFrameRate)
{
	if (PenLightAnimFrameRate != NewPenLightAnimFrameRate)
	{
		PenLightAnimFrameRate = NewPenLightAnimFrameRate;
		OnPenLightAnimFrameRateChanged();
	}
}

void APenLightManager::SetMaxAnimOffsetFrames(const int32 NewMaxAnimOffsetFrames)
{
	if (MaxAnimOffsetFrames != NewMaxAnimOffsetFrames)
	{
		MaxAnimOffsetFrames = NewMaxAnimOffsetFrames;
		OnMaxAnimOffsetFramesChanged();
	}
}

void APenLightManager::OnLightsOnChanged()
{
	// Toggle Emissive Material from Material Parameter Collection
	MPCPenLightInstance->SetScalarParameterValue(FName("bIsEnabled"), bLightsOn ? 1.0f : 0.0f);
	// Set enable for all PenLight and AreaLight
	for (TPair PenLight : Audiences)
	{
		if (PenLight.Value.IsValid())
		{
			if (UKismetSystemLibrary::DoesImplementInterface(PenLight.Value.Get(), UPenLightInterface::StaticClass()))
			{
				InterfaceSetEnabled(PenLight.Value, bLightsOn);
			}
		}
	}
	for (TPair AreaLight : AreaLights)
	{
		if (AreaLight.Value.IsValid())
		{
			if (UKismetSystemLibrary::DoesImplementInterface(AreaLight.Value.Get(), UPenLightInterface::StaticClass()))
			{
				InterfaceSetEnabled(AreaLight.Value, bLightsOn);
			}
		}
	}
}

void APenLightManager::OnPenLightIntensityChanged()
{
	// Update for all PenLight and AreaLight
	for (TPair PenLight : Audiences)
	{
		if (PenLight.Value.IsValid())
		{
			if (UKismetSystemLibrary::DoesImplementInterface(PenLight.Value.Get(), UPenLightInterface::StaticClass()))
			{
				InterfaceSetPenLightIntensity(PenLight.Value, PenLightIntensity);
			}
		}
	}
	for (TPair AreaLight : AreaLights)
	{
		if (AreaLight.Value.IsValid())
		{
			if (UKismetSystemLibrary::DoesImplementInterface(AreaLight.Value.Get(), UPenLightInterface::StaticClass()))
			{
				InterfaceSetPenLightIntensity(AreaLight.Value, PenLightIntensity);
			}
		}
	}
}

void APenLightManager::OnEmissiveValueChanged()
{
	// Change Emissive Material Intensity from Material Parameter Collection
	MPCPenLightInstance->SetScalarParameterValue(FName("EmissiveValue"), EmissiveValue);
}

void APenLightManager::OnPenLightColorChanged()
{
	// Change Emissive Material Color from Material Parameter Collection
	MPCPenLightInstance->SetVectorParameterValue(FName("EmissiveColor"), PenLightColor);
	// Update for all PenLight and AreaLight
	for (TPair PenLight : Audiences)
	{
		if (PenLight.Value.IsValid())
		{
			if (UKismetSystemLibrary::DoesImplementInterface(PenLight.Value.Get(), UPenLightInterface::StaticClass()))
			{
				InterfaceSetPenLightColor(PenLight.Value, PenLightColor);
			}
		}
	}
	for (TPair AreaLight : AreaLights)
	{
		if (AreaLight.Value.IsValid())
		{
			if (UKismetSystemLibrary::DoesImplementInterface(AreaLight.Value.Get(), UPenLightInterface::StaticClass()))
			{
				InterfaceSetPenLightColor(AreaLight.Value, PenLightColor);
			}
		}
	}
}

void APenLightManager::OnIsWavingChanged()
{
	if (bIsWaving)
	{
		CurrentSessionStartTime = UGameplayStatics::GetRealTimeSeconds(WorldPtr);
		// Set a new random seed vector for offset in MPC
		RandomSeed = FMath::VRand();
		MPCBoneAnimationInstance->SetVectorParameterValue(FName("RandomSeed"), RandomSeed);
	}
	else
	{
		CurrentSessionElapsedTime = 0.0f;
		CurrentMasterAnimFrame = 0;
	}
	
	MPCBoneAnimationInstance->SetScalarParameterValue(FName("bIsPlaying"), bIsWaving);
}

void APenLightManager::OnPenLightBPMChanged()
{
	MPCBoneAnimationInstance->SetScalarParameterValue(FName("PenLightBPM"), PenLightBPM);
}

void APenLightManager::OnPenLightAnimFrameRateChanged()
{
	MPCBoneAnimationInstance->SetScalarParameterValue(FName("AnimFrameRate"), PenLightAnimFrameRate);
}

void APenLightManager::OnMaxAnimOffsetFramesChanged()
{
	// Need to Fix: Current setup based on ActorPositionWS node does not work with Instanced Static Mesh
	//MPCBoneAnimationInstance->SetScalarParameterValue(FName("MaxAnimOffset"), MaxAnimOffsetFrames);
}

#if WITH_EDITOR
void APenLightManager::RefreshAllAudienceSections()
{
	TArray<AActor*> AllAudienceSections;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAudienceSection::StaticClass(), AllAudienceSections);
	
	for (AActor* AudienceSection : AllAudienceSections)
	{
		//Cast to AAudienceSection and call RefreshPCG
		if (AAudienceSection* CastedAudienceSection = Cast<AAudienceSection>(AudienceSection))
		{
			CastedAudienceSection->RegeneratePCG();
		}
	}
}
#endif

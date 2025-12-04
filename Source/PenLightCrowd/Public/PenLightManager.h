#pragma once

#include "CoreMinimal.h"
#include "PenLightInterface.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "PenLightManager.generated.h"

class AAudienceSection;
class APenLightManager;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPenLightManagerInitialized, TWeakObjectPtr<APenLightManager>);

UCLASS(Blueprintable)
class PENLIGHTCROWD_API APenLightManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APenLightManager();
	
	static TWeakObjectPtr<APenLightManager> GetPenLightManager(const UObject* WorldContextObject);
	static FOnPenLightManagerInitialized& OnPenLightManagerInitialized() { return PenLightManagerInitializedEvent; }
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	bool GetIsInitialized() const { return bIsInitialized; }
	
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void RegisterAudienceSection(AAudienceSection* NewAudienceSection);
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void UnregisterAudienceSection(const AAudienceSection* AudienceSection);
	
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void RegisterAudience(AActor* NewAudienceActor);
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void UnregisterAudience(const AActor* AudienceActor);
	
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void RegisterAreaLight(AActor* NewAreaLightActor);
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void UnregisterAreaLight(const AActor* AreaLightActor);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	bool GetLightsOn() const { return bLightsOn; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetLightsOn(const bool bNewLightsOn);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	float GetPenLightIntensity() const { return PenLightIntensity; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetPenLightIntensity(const float NewPenLightIntensity);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	float GetEmissiveValue() const { return EmissiveValue; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetEmissiveValue(const float NewEmissiveValue);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	FLinearColor GetPenLightColor() const { return PenLightColor; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetPenLightColor(const FLinearColor NewPenLightColor);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	bool GetIsWaving() const { return bIsWaving; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetIsWaving(const bool bNewIsWaving);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	float GetCurrentSessionStartTime() const { return CurrentSessionStartTime; }
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	float GetCurrentSessionElapsedTime() const { return CurrentSessionElapsedTime; }
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	int32 GetCurrentMasterAnimFrame() const { return CurrentMasterAnimFrame; }
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	float GetPenLightBPM() const { return PenLightBPM; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetPenLightBPM(const float NewPenLightBPM);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	int32 GetPenLightAnimFrameRate() const { return PenLightAnimFrameRate; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetPenLightAnimFrameRate(const int32 NewPenLightAnimFrameRate);
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	FVector GetRandomSeed() const { return RandomSeed; }
	
	UFUNCTION(BlueprintPure, Category="Pen Light Manager")
	int32 GetMaxAnimOffsetFrames() const { return MaxAnimOffsetFrames; }
	UFUNCTION(BlueprintCallable, Category="Pen Light Manager")
	void SetMaxAnimOffsetFrames(const int32 NewMaxAnimOffsetFrames);
	
#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category="Pen Light Manager")
	void RefreshAllAudienceSections();
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pen Light Manager")
	UMaterialParameterCollection* MPCPenLight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pen Light Manager")
	UMaterialParameterCollection* MPCBoneAnimation;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	static TWeakObjectPtr<APenLightManager> PenLightManagerInstance;
	static FOnPenLightManagerInitialized PenLightManagerInitializedEvent;
	UPROPERTY()
	bool bIsInitialized = false;
	
	UFUNCTION()
	void OnLightsOnChanged();
	UFUNCTION()
	void OnPenLightIntensityChanged();
	UFUNCTION()
	void OnEmissiveValueChanged();
	UFUNCTION()
	void OnPenLightColorChanged();
	UFUNCTION()
	void OnIsWavingChanged();
	UFUNCTION()
	void OnPenLightBPMChanged();
	UFUNCTION()
	void OnPenLightAnimFrameRateChanged();
	UFUNCTION()
	void OnMaxAnimOffsetFramesChanged();
	
	UFUNCTION()
	static void InterfaceSetEnabled(const TWeakObjectPtr<AActor> Target, const bool bNewEnabled) { IPenLightInterface::Execute_SetEnabled(Target.Get(), bNewEnabled); }
	UFUNCTION()
	static void InterfaceSetPenLightIntensity(const TWeakObjectPtr<AActor> Target, const float NewPenLightIntensity) { IPenLightInterface::Execute_SetPenLightIntensity(Target.Get(), NewPenLightIntensity); }
	UFUNCTION()
	static void InterfaceSetPenLightColor(const TWeakObjectPtr<AActor> Target, const FLinearColor NewPenLightColor) { IPenLightInterface::Execute_SetPenLightColor(Target.Get(), NewPenLightColor); }
	
	UPROPERTY()
	UWorld* WorldPtr;
	
	UPROPERTY(EditAnywhere, Category="Pen Light Manager")
	bool bLightsOn = true;
	UPROPERTY(EditAnywhere, Category="Pen Light Manager", meta=(clampMin="0.0", UIMin="0.0", UIMax="1000.0", unit="lm"))
	float PenLightIntensity = 10.0f;
	UPROPERTY(EditAnywhere, Category="Pen Light Manager", meta=(clampMin="0.0", UIMin="0.0", UIMax="1000.0", unit="nit"))
	float EmissiveValue = 10.0f;
	UPROPERTY(EditAnywhere, Category="Pen Light Manager")
	FLinearColor PenLightColor = FLinearColor(1.0f, 1.0f, 1.0f);
	UPROPERTY(EditAnywhere, Category="Pen Light Manager")
	bool bIsWaving = false;
	
	UPROPERTY()
	float CurrentSessionStartTime = 0.0f;
	UPROPERTY()
	float CurrentSessionElapsedTime = 0.0f;
	UPROPERTY()
	int32 CurrentMasterAnimFrame = 0;
	UPROPERTY()
	int32 PrevMasterAnimFrame = 0;
	
	UPROPERTY()
	TMap<FName, TWeakObjectPtr<AAudienceSection>> AudienceSections;
	UPROPERTY()
	TMap<FName, TWeakObjectPtr<AActor>> Audiences;
	UPROPERTY()
	TMap<FName, TWeakObjectPtr<AActor>> AreaLights;
	
	UPROPERTY(BlueprintReadOnly, Category="Pen Light Manager", meta=(AllowPrivateAccess="true"))
	UMaterialParameterCollectionInstance* MPCPenLightInstance;
	UPROPERTY(BlueprintReadOnly, Category="Pen Light Manager", meta=(AllowPrivateAccess="true"))
	UMaterialParameterCollectionInstance* MPCBoneAnimationInstance;
	
	UPROPERTY(EditAnywhere, Category="Pen Light Manager", meta=(AllowPrivateAccess="true"))
	float PenLightBPM = 60.0f;
	UPROPERTY(EditAnywhere, Category="Pen Light Manager", meta=(AllowPrivateAccess="true"))
	int32 PenLightAnimFrameRate = 30;
	
	UPROPERTY()
	FVector RandomSeed = FMath::VRand();
	UPROPERTY(EditAnywhere, Category="Pen Light Manager", meta=(AllowPrivateAccess="true"))
	int32 MaxAnimOffsetFrames = 5;
};

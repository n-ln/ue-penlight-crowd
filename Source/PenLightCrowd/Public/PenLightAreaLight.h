// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PenLightInterface.h"
#include "GameFramework/Actor.h"
#include "PenLightAreaLight.generated.h"

class URectLightComponent;
class UPenLightPCGData;
class APenLightManager;

UCLASS()
class PENLIGHTCROWD_API APenLightAreaLight : public AActor, public IPenLightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APenLightAreaLight();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, CallInEditor, Category="PenLight Area Light")
	void SavePCGData();
	UFUNCTION(BlueprintCallable, CallInEditor, Category="PenLight Area Light")
	void SetupFromPCGData();
	
	/** ~Begin IPenLightInterface interface */
	void SetEnabled_Implementation(const bool bNewIsEnabled) override;
	void SetPenLightIntensity_Implementation(const float NewPenLightIntensity) override;
	void SetPenLightColor_Implementation(const FLinearColor NewPenLightColor) override;
	/** ~End IPenLightInterface interface */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnPenLightManagerInitialized(TWeakObjectPtr<APenLightManager> PenLightManagerInstance);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> Parent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URectLightComponent> RectLightDownward;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	TObjectPtr<URectLightComponent> RectLightUpward;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPenLightPCGData> PenLightPCGData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<APenLightManager> PenLightManager;
	
	// Variables controlled by IPenLightInterface (PenLightManager)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	bool bIsEnabled;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float PenLightIntensity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	FLinearColor PenLightColor;
	
	// Rect light properties
	// The "actual" scale inferred from light dimension, which might differ from the scale set in Audience Section.
	// Used for intensity calculation.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	FVector2D RectLightScale = FVector2D(8.0f, 8.0f);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float BaseIntensity = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float RectLightOffset = 5.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float UpwardRelativeIntensityInverse = 8.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float DownwardRelativeIntensityInverse = 8.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float RectLightHeightFromGround = 170.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Area Light", meta=(AllowPrivateAccess="true"))
	float RectLightWaveAmplitude = 30.0f;
	
	
	// Variables used to receive data from PCG graph 
	// Should NOT be used at runtime, persistent data are saved in PenLightPCGData component
	UPROPERTY()
	FVector TempSeatVector;
	UPROPERTY()
	FVector TempLightVector;
	UPROPERTY()
	FRotator TempSpawnRotation;
	UPROPERTY()
	AActor* TempAudienceSection;
	
};

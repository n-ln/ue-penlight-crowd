// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PenLightInterface.h"
#include "GameFramework/Actor.h"
#include "PenLightAudience.generated.h"

class UPointLightComponent;
class UPenLightPCGData;
class APenLightManager;
class UInstancedActorsComponent;
class APlayerCameraManager;

UCLASS()
class PENLIGHTCROWD_API APenLightAudience : public AActor, public IPenLightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APenLightAudience();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** ~Begin IPenLightInterface interface */
	void SetEnabled_Implementation(const bool bNewIsEnabled) override;
	void SetPenLightIntensity_Implementation(const float NewPenLightIntensity) override;
	void SetPenLightColor_Implementation(const FLinearColor NewPenLightColor) override;
	/** ~End IPenLightInterface interface */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PenLight Audience")
	bool bIsUsingHighLOD = false;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnPenLightManagerInitialized(TWeakObjectPtr<APenLightManager> PenLightManagerInstance);
	UFUNCTION()
	void SwitchLOD();
	UFUNCTION()
	void UpdateAnimParameters();
	
	UPROPERTY()
	UWorld* WorldPtr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> PenLight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPointLightComponent> PointLight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> CharacterVAT;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> PenLightVAT;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPenLightPCGData> PenLightPCGData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInstancedActorsComponent> InstancedActorsComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	TWeakObjectPtr<APenLightManager> PenLightManager;
	
	// Variables controlled by IPenLightInterface (PenLightManager)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	bool bIsEnabled;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	float PenLightIntensity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLight Audience", meta=(AllowPrivateAccess="true"))
	FLinearColor PenLightColor;

	UPROPERTY()
	FVector ActorScale;
	UPROPERTY()
	FVector OffsetScale;
	UPROPERTY()
	float TransitionTime = 0.2f;
	UPROPERTY()
	float TransitionStartTime = 0.0f;
	UPROPERTY()
	bool bIsTransitioning = false;
	
	UPROPERTY()
	FRotator PenLightRotationOffset = FRotator::ZeroRotator;
	
	UPROPERTY()
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager;
	
	// Animation Parameters for High LOD
	UPROPERTY()
	int32 MaxAnimOffsetFrames = 0;
	UPROPERTY()
	FVector RandomSeed = FVector::ZeroVector;
	UPROPERTY()
	int32 PenLightAnimFrameRate = 30;
	
};

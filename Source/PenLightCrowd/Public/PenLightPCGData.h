// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PenLightPCGData.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PENLIGHTCROWD_API UPenLightPCGData : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPenLightPCGData();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLightPCGData")
	FVector SeatVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLightPCGData")
	FVector LightVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLightPCGData")
	FRotator SpawnRotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="PenLightPCGData")
	AActor* AudienceSection;
	
	UFUNCTION(BlueprintCallable, Category="PenLightPCGData")
	void SetSeatVector(const FVector NewSeatVector) { SeatVector = NewSeatVector; }
	
	UFUNCTION(BlueprintCallable, Category="PenLightPCGData")
	void SetLightVector(const FVector NewLightVector) { LightVector = NewLightVector; }
	
	UFUNCTION(BlueprintCallable, Category="PenLightPCGData")
	void SetSpawnRotation(const FRotator NewSpawnRotation) { SpawnRotation = NewSpawnRotation; }
	
	UFUNCTION(BlueprintCallable, Category="PenLightPCGData")
	void SetAudienceSection(AActor* NewAudienceSection) { AudienceSection = NewAudienceSection; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	
};

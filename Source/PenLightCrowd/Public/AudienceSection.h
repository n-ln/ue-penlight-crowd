#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudienceSection.generated.h"

class APenLightManager;
class USplineComponent;
class UPCGComponent;

UCLASS(Blueprintable)
class PENLIGHTCROWD_API AAudienceSection : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAudienceSection();
	
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, CallInEditor, Category="Audience Section")
	void RegeneratePCG();
#endif 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audience Section", meta=(clampMin="0.0", UIMin="0.0", UIMax="200.0", unit="cm"))
	FVector SeatVector = FVector(60.0f, 70.0f, 40.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audience Section", meta=(clampMin="1", UIMin="1", UIMax="100"))
	int32 RowCount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audience Section", meta=(clampMin="1.0", UIMin="1.0", UIMax="50.0"))
	FVector2D RectLightScale = FVector2D(16.0f, 16.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audience Section")
	bool bIsCurved = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audience Section", meta=(clampMin="0.0", UIMin="0.0", UIMax="200.0", unit="cm"))
	float ExtensionStart = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audience Section", meta=(clampMin="0.0", UIMin="0.0", UIMax="200.0", unit="cm"))
	float ExtensionEnd = 0.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	UFUNCTION()
	void OnPenLightManagerInitialized(TWeakObjectPtr<APenLightManager> PenLightManagerInstance);
	
	UFUNCTION()
	void UpdatePCGParameters();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Audience Section", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USplineComponent> SplineComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Audience Section", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPCGComponent> PCGComponent;
	
	UPROPERTY()
	TWeakObjectPtr<APenLightManager> PenLightManager;
};

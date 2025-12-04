#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PenLightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPenLightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PENLIGHTCROWD_API IPenLightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="PenLight Interface")
	bool GetEnabled();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="PenLight Interface")
	void SetEnabled(bool bNewEnabled);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="PenLight Interface")
	float GetPenLightIntensity();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="PenLight Interface")
	void SetPenLightIntensity(float NewPenLightIntensity);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="PenLight Interface")
	FLinearColor GetPenLightColor();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="PenLight Interface")
	void SetPenLightColor(FLinearColor NewPenLightColor);
};

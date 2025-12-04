
#include "AudienceSection.h"
#include "PCGComponent.h"
#include "PCGGraph.h"
#include "Components/SplineComponent.h"
#include "PenLightManager.h"

// Sets default values
AAudienceSection::AAudienceSection()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	PCGComponent = CreateDefaultSubobject<UPCGComponent>("PCGComponent");
	
	PCGComponent->GenerationTrigger = EPCGComponentGenerationTrigger::GenerateOnDemand;
}

// Called when the game starts or when spawned
void AAudienceSection::BeginPlay()
{
	Super::BeginPlay();

	// Get PenLightManager
	TWeakObjectPtr<APenLightManager> PenLightManagerInstance = APenLightManager::GetPenLightManager(this);
	if (PenLightManagerInstance.IsValid() && PenLightManagerInstance->GetIsInitialized())
	{
		// If initialized register this audience section
		PenLightManager = PenLightManagerInstance;
		PenLightManager->RegisterAudienceSection(this);
	}
	else
	{
		// If not initialized, subscribe to delegate and handle registration in the callback
		APenLightManager::OnPenLightManagerInitialized().AddUObject(this, &AAudienceSection::OnPenLightManagerInitialized);
	}
}

void AAudienceSection::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PenLightManager->UnregisterAudienceSection(this);
	
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void AAudienceSection::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	UpdatePCGParameters();
	
	// Only regenerate if not in game
	if (!GetWorld()->IsGameWorld())
	{
		PCGComponent->Cleanup();
		PCGComponent->Generate();
	}
}

void AAudienceSection::RegeneratePCG()
{
	UpdatePCGParameters();
	PCGComponent->Cleanup();
	PCGComponent->Generate();
}
#endif

void AAudienceSection::OnPenLightManagerInitialized(TWeakObjectPtr<APenLightManager> PenLightManagerInstance)
{
	if (PenLightManagerInstance.IsValid() && PenLightManagerInstance->GetIsInitialized())
	{
		PenLightManager = PenLightManagerInstance;
		PenLightManager->RegisterAudienceSection(this);
	}
}

void AAudienceSection::UpdatePCGParameters()
{
	UPCGGraphInstance* PCGGraph = PCGComponent->GetGraphInstance();
	if (ensureMsgf(PCGGraph, TEXT("PCGComponent has no graph instance")))
	{
		PCGGraph->SetGraphParameter(FName(TEXT("SeatVector")), SeatVector.ComponentMax(FVector(10.0f, 10.0f, 0.0f)));
		PCGGraph->SetGraphParameter(FName(TEXT("RowCount")), FMath::Max(RowCount, 1));
		PCGGraph->SetGraphParameter(FName(TEXT("RectLightScale")), RectLightScale.ComponentMax(FVector2D(1.0f, 1.0f)));
		PCGGraph->SetGraphParameter(FName(TEXT("bIsCurved")), bIsCurved);
		PCGGraph->SetGraphParameter(FName(TEXT("PCGActor")), StaticCast<UObject*>(this));
		PCGGraph->SetGraphParameter(FName(TEXT("ExtensionStart")), FMath::Max(ExtensionStart, 0.0f));
		PCGGraph->SetGraphParameter(FName(TEXT("ExtensionEnd")), FMath::Max(ExtensionEnd, 0.0f));
	}
}


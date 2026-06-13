#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TechyMonPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class TECHYMON_API ATechyMonPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

protected:
    virtual void BeginPlay() override;
};

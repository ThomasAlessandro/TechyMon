#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "InputActionValue.h"
#include "TechyMonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPaperFlipbook;
class UInputAction;

UENUM(BlueprintType)
enum class EFacingDirection : uint8
{
    Down,
    Up,
    Left,
    Right
};

UCLASS()
class TECHYMON_API ATechyMonCharacter : public APaperCharacter
{
    GENERATED_BODY()

public:
    ATechyMonCharacter();

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditAnywhere, Category = "Sprites")
    TObjectPtr<UPaperFlipbook> FlipbookDown;

    UPROPERTY(EditAnywhere, Category = "Sprites")
    TObjectPtr<UPaperFlipbook> FlipbookUp;

    UPROPERTY(EditAnywhere, Category = "Sprites")
    TObjectPtr<UPaperFlipbook> FlipbookLeft;

    UPROPERTY(EditAnywhere, Category = "Sprites")
    TObjectPtr<UPaperFlipbook> FlipbookRight;

    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed = 200.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraOrthoWidth = 512.f;

    EFacingDirection CurrentFacing = EFacingDirection::Down;
    FVector2D PreviousInput = FVector2D::ZeroVector;

    enum class ELastAxis : uint8 { None, Horizontal, Vertical };
    ELastAxis LastPressedAxis = ELastAxis::None;

    void HandleMove(const FInputActionValue& Value);
    UPaperFlipbook* FlipbookForDirection() const;
};

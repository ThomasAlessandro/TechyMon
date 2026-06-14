#include "TechyMonCharacter.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"

ATechyMonCharacter::ATechyMonCharacter()
{
    // Spring arm — zero length, no 3D collision, looks straight down
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.f;
    SpringArm->bDoCollisionTest = false;
    SpringArm->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    // Camera — attached directly to root, positioned in -Y looking in +Y (Paper2D front view)
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(RootComponent);
    Camera->SetRelativeLocation(FVector(0.f, -500.f, 0.f));
    Camera->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
    Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
    Camera->OrthoWidth = CameraOrthoWidth;

    // Movement — top-down 2D, no gravity, constrained to XY plane
    GetCharacterMovement()->GravityScale = 0.f;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
    GetCharacterMovement()->MaxFlySpeed = WalkSpeed;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
}

void ATechyMonCharacter::BeginPlay()
{
    Super::BeginPlay();
    // Super::BeginPlay resets movement mode to DefaultLandMovementMode — re-apply flying here
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
    Camera->SetRelativeLocation(FVector(0.f, -500.f, 0.f));
    Camera->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
}

void ATechyMonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATechyMonCharacter::HandleMove);
        EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATechyMonCharacter::HandleMove);
    }
}

void ATechyMonCharacter::HandleMove(const FInputActionValue& Value)
{
    FVector2D Input = Value.Get<FVector2D>();

    const bool bHasX = !FMath::IsNearlyZero(Input.X);
    const bool bHasY = !FMath::IsNearlyZero(Input.Y);

    // Track which axis was most recently pressed
    if (bHasX && FMath::IsNearlyZero(PreviousInput.X)) LastPressedAxis = ELastAxis::Horizontal;
    if (bHasY && FMath::IsNearlyZero(PreviousInput.Y)) LastPressedAxis = ELastAxis::Vertical;
    PreviousInput = Input;
    // LastPressedAxis intentionally persists across key releases — last-direction bias
    // is standard Pokémon-style behaviour (re-pressing the same key continues that direction).

    // Resolve diagonal input — only one axis moves at a time
    FVector2D MoveInput = FVector2D::ZeroVector;
    if (bHasX && bHasY)
    {
        MoveInput = (LastPressedAxis == ELastAxis::Horizontal)
            ? FVector2D(Input.X, 0.f)
            : FVector2D(0.f, Input.Y);
    }
    else if (bHasX) MoveInput = FVector2D(Input.X, 0.f);
    else if (bHasY) MoveInput = FVector2D(0.f, Input.Y);

    if (!MoveInput.IsNearlyZero())
    {
        AddMovementInput(FVector(MoveInput.X, 0.f, MoveInput.Y));

        if (!FMath::IsNearlyZero(MoveInput.Y))
            CurrentFacing = (MoveInput.Y > 0.f) ? EFacingDirection::Up : EFacingDirection::Down;
        else
            CurrentFacing = (MoveInput.X > 0.f) ? EFacingDirection::Right : EFacingDirection::Left;

        GetSprite()->SetFlipbook(FlipbookForDirection());
    }
    else
    {
        GetCharacterMovement()->StopMovementImmediately();
    }
}

UPaperFlipbook* ATechyMonCharacter::FlipbookForDirection() const
{
    UPaperFlipbook* Target = nullptr;
    switch (CurrentFacing)
    {
        case EFacingDirection::Down:  Target = FlipbookDown;  break;
        case EFacingDirection::Up:    Target = FlipbookUp;    break;
        case EFacingDirection::Left:  Target = FlipbookLeft;  break;
        case EFacingDirection::Right: Target = FlipbookRight; break;
    }
    return Target ? Target : GetSprite()->GetFlipbook();
}

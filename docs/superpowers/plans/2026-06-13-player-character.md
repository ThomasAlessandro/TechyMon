# Player Character Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a 4-directional top-down player character using APaperCharacter, Enhanced Input, and an orthographic camera.

**Architecture:** `ATechyMonPlayerController` activates the input mapping context on BeginPlay. `ATechyMonCharacter` owns movement, sprite direction tracking, and camera. Input binding happens in `SetupPlayerInputComponent`. No game logic (battle, inventory) touches either class.

**Tech Stack:** UE5.7, C++, Paper2D, Enhanced Input system. EnhancedInput and Paper2D are already in Build.cs.

---

### Task 1: Create TechyMonPlayerController

**Files:**
- Create: `Source/TechyMon/Player/TechyMonPlayerController.h`
- Create: `Source/TechyMon/Player/TechyMonPlayerController.cpp`

- [ ] **Step 1: Create the header**

Create `Source/TechyMon/Player/TechyMonPlayerController.h` with this exact content:

```cpp
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
```

- [ ] **Step 2: Create the implementation**

Create `Source/TechyMon/Player/TechyMonPlayerController.cpp` with this exact content:

```cpp
#include "TechyMonPlayerController.h"
#include "EnhancedInputSubsystems.h"

void ATechyMonPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}
```

- [ ] **Step 3: Commit**

```bash
git add Source/TechyMon/Player/TechyMonPlayerController.h
git add Source/TechyMon/Player/TechyMonPlayerController.cpp
git commit -m "feat: add TechyMonPlayerController with Enhanced Input mapping context"
```

---

### Task 2: Create TechyMonCharacter Header

**Files:**
- Create: `Source/TechyMon/Player/TechyMonCharacter.h`

- [ ] **Step 1: Create the header**

Create `Source/TechyMon/Player/TechyMonCharacter.h` with this exact content:

```cpp
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
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere)
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

    EFacingDirection CurrentFacing = EFacingDirection::Down;
    FVector2D PreviousInput = FVector2D::ZeroVector;

    enum class ELastAxis : uint8 { None, Horizontal, Vertical };
    ELastAxis LastPressedAxis = ELastAxis::None;

    void HandleMove(const FInputActionValue& Value);
    UPaperFlipbook* FlipbookForDirection() const;
};
```

- [ ] **Step 2: Commit**

```bash
git add Source/TechyMon/Player/TechyMonCharacter.h
git commit -m "feat: add TechyMonCharacter header"
```

---

### Task 3: Create TechyMonCharacter Implementation

**Files:**
- Create: `Source/TechyMon/Player/TechyMonCharacter.cpp`

- [ ] **Step 1: Create the implementation**

Create `Source/TechyMon/Player/TechyMonCharacter.cpp` with this exact content:

```cpp
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
    SpringArm->TargetArmLength = 0.f;
    SpringArm->bDoCollisionTest = false;
    SpringArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));

    // Camera — orthographic, top-down
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
    Camera->OrthoWidth = 512.f;

    // Movement — top-down 2D, no gravity, constrained to XY plane
    GetCharacterMovement()->GravityScale = 0.f;
    GetCharacterMovement()->MaxWalkSpeed = 200.f;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 1.f));
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
        AddMovementInput(FVector(MoveInput.X, MoveInput.Y, 0.f));

        if (!FMath::IsNearlyZero(MoveInput.Y))
            CurrentFacing = (MoveInput.Y > 0.f) ? EFacingDirection::Up : EFacingDirection::Down;
        else
            CurrentFacing = (MoveInput.X > 0.f) ? EFacingDirection::Right : EFacingDirection::Left;

        GetSprite()->SetFlipbook(FlipbookForDirection());
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
```

- [ ] **Step 2: Commit**

```bash
git add Source/TechyMon/Player/TechyMonCharacter.cpp
git commit -m "feat: add TechyMonCharacter implementation with 4-directional movement and camera"
```

---

### Task 4: Compile

**Files:** No new files — compile check only.

- [ ] **Step 1: Compile in UE5 editor**

In the UE5 editor: **Tools → Compile** (or press Ctrl+Alt+F11).

Expected: "Compile Complete!" with no errors. If errors appear, check:
- `#include` paths match exactly the headers listed in Task 2 and 3
- `TECHYMON_API` macro matches your module name (check `TechyMon.h` — it uses `TECHYMON_API`)

- [ ] **Step 2: Verify classes appear in editor**

In the UE5 Content Browser, right-click → Blueprint Class → search for `TechyMonCharacter`. It should appear as a parent class option. If it doesn't appear, the compile failed silently — check the Output Log.

---

### Task 5: Create Input Assets in Editor

**Files:**
- Create: `Content/Input/IA_Move.uasset` (editor)
- Create: `Content/Input/IMC_Default.uasset` (editor)

These must be created inside the UE5 editor — they are data assets, not C++ files.

- [ ] **Step 1: Create the Input folder**

In the Content Browser, right-click `Content` → New Folder → name it `Input`.

- [ ] **Step 2: Create IA_Move**

Inside `Content/Input`, right-click → **Input → Input Action**.
Name it `IA_Move`.
Open it. Set **Value Type** to `Axis2D (Vector2D)`. Save (Ctrl+S).

- [ ] **Step 3: Create IMC_Default**

Inside `Content/Input`, right-click → **Input → Input Mapping Context**.
Name it `IMC_Default`.
Open it. Click **+** to add a mapping. Set the action to `IA_Move`.

Add these key mappings under `IA_Move`:
| Key | Modifiers |
|-----|-----------|
| W | None |
| S | Negate (Y axis) |
| A | Swizzle (YXZ), Negate |
| D | Swizzle (YXZ) |
| Up Arrow | None |
| Down Arrow | Negate (Y axis) |
| Left Arrow | Swizzle (YXZ), Negate |
| Right Arrow | Swizzle (YXZ) |

Save (Ctrl+S).

> **Why the modifiers?** `IA_Move` is Axis2D: X = horizontal, Y = vertical. W/S naturally map to Y; A/D need Swizzle to push their value into X. Negate reverses direction for S and A/Left Arrow.

---

### Task 6: Create Blueprint Character and Wire Assets

**Files:**
- Create: `Content/Blueprints/BP_TechyMonCharacter.uasset` (editor)
- Create: `Content/Blueprints/BP_TechyMonPlayerController.uasset` (editor)
- Create: `Content/Blueprints/BP_TechyMonGameMode.uasset` (editor)

- [ ] **Step 1: Create BP_TechyMonCharacter**

In Content Browser, right-click `Content` → New Folder → `Blueprints`.
Right-click `Blueprints` → Blueprint Class → search parent class `TechyMonCharacter` → select it.
Name it `BP_TechyMonCharacter`. Open it.

In the Details panel (with the root selected), find **Input → Move Action** and assign `IA_Move`.

Save and compile the Blueprint.

- [ ] **Step 2: Create BP_TechyMonPlayerController**

Right-click `Content/Blueprints` → Blueprint Class → search `TechyMonPlayerController` → select it.
Name it `BP_TechyMonPlayerController`. Open it.

In Details panel, find **Input → Default Mapping Context** and assign `IMC_Default`.

Save and compile.

- [ ] **Step 3: Create BP_TechyMonGameMode**

Right-click `Content/Blueprints` → Blueprint Class → search `Game Mode Base` → select it.
Name it `BP_TechyMonGameMode`. Open it.

In Details panel:
- **Default Pawn Class** → `BP_TechyMonCharacter`
- **Player Controller Class** → `BP_TechyMonPlayerController`

Save and compile.

---

### Task 7: Create Test Level and Verify Movement

**Files:** No C++ — in-editor level setup and manual testing.

- [ ] **Step 1: Create a test level**

File → New Level → Empty Level. Save it as `Content/Maps/TestMovement`.

- [ ] **Step 2: Set up the floor**

In the level, add a **Plane** (Place Actors → Shapes → Plane). Scale it to 20×20. This gives the character something to stand on.

Add a **Directional Light** so you can see.

- [ ] **Step 3: Set the Game Mode**

In the World Settings panel (Window → World Settings), set **Game Mode Override** to `BP_TechyMonGameMode`.

- [ ] **Step 4: Play and verify**

Press **Play** (Alt+P).

Checklist:
- [ ] Character spawns in the level
- [ ] W / Up Arrow moves the character upward on screen
- [ ] S / Down Arrow moves the character downward
- [ ] A / Left Arrow moves the character left
- [ ] D / Right Arrow moves the character right
- [ ] Holding two keys at once (e.g. W+D) moves in only one direction, not diagonally
- [ ] Character does not fall through the floor
- [ ] Camera follows the character and stays top-down

- [ ] **Step 5: Commit**

```bash
git add .gitignore
git commit -m "feat: player character movement verified — test level setup complete"
```

> Note: The Blueprint and level assets are binary UE5 files. Committing them requires LFS or accepting binary diffs. If your project doesn't use LFS, skip committing the `.uasset` files for now and commit only the `.gitignore` update if any.

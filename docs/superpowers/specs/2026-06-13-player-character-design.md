# Player Character Design

**Date:** 2026-06-13
**Status:** Approved

## Overview

A human kid/teenager player character for TechyMon, implemented as an `APaperCharacter` subclass with 4-directional movement, fast walk speed, and an orthographic top-down camera. Placeholder sprites are used initially; real flipbook assets slot in from the editor later.

## Architecture

**Files to create:**
- `Source/TechyMon/Player/TechyMonCharacter.h/.cpp` — player character class
- `Source/TechyMon/Player/TechyMonPlayerController.h/.cpp` — player controller
- `Content/Input/IMC_Default.uasset` — Input Mapping Context (created in editor)
- `Content/Input/IA_Move.uasset` — Input Action for movement (created in editor)

**Class hierarchy:**
```
APaperCharacter
  └── ATechyMonCharacter
        ├── UPaperFlipbook* (sprite component, inherited)
        ├── UCameraComponent (orthographic, top-down)
        └── USpringArmComponent (attaches camera)
```

**Responsibility split:**
- `ATechyMonCharacter` — movement, sprite direction, camera
- `ATechyMonPlayerController` — input setup and mapping context activation
- No game logic (battles, inventory) in this class — purely locomotion

## Movement & Input

**Movement:**
- Speed: 200 units/sec (tweakable via `UPROPERTY`)
- 4-directional: X axis (left/right), Y axis (up/down)
- No diagonal movement — most recently pressed axis wins; resolved via `LastMoveDirection` tracking
- `GravityScale = 0`, no falling — pure top-down 2D

**Input (Enhanced Input):**
- `IA_Move` — `Value Type: Axis2D`, WASD + arrow keys
- `IMC_Default` — active from game start
- `ATechyMonPlayerController::BeginPlay()` adds `IMC_Default` to the local player subsystem
- `ATechyMonCharacter::SetupPlayerInputComponent()` binds `IA_Move` → `HandleMove(FInputActionValue)`

**Direction tracking:**
```cpp
enum class EFacingDirection : uint8 { Down, Up, Left, Right };
EFacingDirection CurrentFacing;
```
Updated on input, drives sprite selection.

## Sprite

- Placeholder: single solid-color `UPaperSprite` (no art required to start)
- 4 `UPROPERTY(EditAnywhere)` flipbook pointers: `FlipbookDown`, `FlipbookUp`, `FlipbookLeft`, `FlipbookRight`
- On movement input: `GetSprite()->SetFlipbook(FlipbookForDirection())` swaps to matching flipbook
- On stop: flipbook freezes on last frame (idle pose)
- Null-safe: `FlipbookForDirection()` returns current flipbook if target is null — no crash on missing asset

## Camera

- `USpringArmComponent`: `TargetArmLength = 0`, `bDoCollisionTest = false`
- `UCameraComponent`: `ProjectionMode = Orthographic`, `OrthoWidth = 512`
- Camera looks straight down Z axis (`-90° pitch`)
- Camera follows character directly — no offset, no lag

## Testing

Manual in-editor playtesting:
1. Place character in a blank level with a flat plane
2. Verify 4-directional WASD movement
3. Verify no falling (gravity off)
4. Verify camera follows and stays top-down
5. Sprite direction testing deferred until real flipbooks are assigned

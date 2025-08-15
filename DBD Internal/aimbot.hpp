#pragma once
#include <core/game/utils/classes/classes.hpp>
#include <dependencies/includes/includes.hpp>
#include <xor.h>
#include <cmath>
#include <numbers>
#include <Windows.h>

void MoveMouseTo(float dx, float dy);
void AimAtTargetMouse(const FVector& CameraLocation, const FRotator& CameraRotation, const FVector& TargetLocation, float Smooth);

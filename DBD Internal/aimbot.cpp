#pragma once
#include <core/game/utils/classes/classes.hpp>
#include <dependencies/includes/includes.hpp>
#include <xor.h>
#include <cmath>
#include <numbers>
#include <Windows.h>

void MoveMouseTo(float dx, float dy)
{
    INPUT input = { 0 };

    input.type = INPUT_MOUSE;

    input.mi.dx = static_cast<LONG>(dx);

    input.mi.dy = static_cast<LONG>(dy);

    input.mi.dwFlags = MOUSEEVENTF_MOVE;

    SendInput(1, &input, sizeof(INPUT));
}

void AimAtTargetMouse(const FVector& CameraLocation, const FRotator& CameraRotation, const FVector& TargetLocation, float Smooth)
{
    FRotator TargetRotation = UE->KismetMathLibrary->FindLookAtRotation(CameraLocation, TargetLocation);

    float deltaPitch = TargetRotation.Pitch - CameraRotation.Pitch;
    float deltaYaw = TargetRotation.Yaw - CameraRotation.Yaw;

    if (deltaYaw > 180.f) deltaYaw -= 360.f;
    if (deltaYaw < -180.f) deltaYaw += 360.f;

    if (deltaPitch > 180.f) deltaPitch -= 360.f;
    if (deltaPitch < -180.f) deltaPitch += 360.f;

    deltaPitch /= Smooth;
    deltaYaw /= Smooth;

    const float Sensitivity = 4.f;
    float dx = deltaYaw * Sensitivity;
    float dy = -deltaPitch * Sensitivity;

    MoveMouseTo(dx, dy);
}
#pragma once

#include "CoreMinimal.h"
#include "ShipTypes.generated.h"

/**
 * @enum EShipStatus
 * @brief Describes the current operational status of a ship.
 *
 * Intended for use in AShipActor to drive visual feedback — for example
 * changing the ship mesh material or colour depending on whether the ship
 * is actively moving or stationary.
 *
 * @note This enum is defined but not yet consumed by any class. It is
 * intended for use in a future update when per-ship visual state is
 * implemented.
 *
 * @see FShipRecord::Speed
 * @see AShipActor
 */
UENUM(BlueprintType)
enum class EShipStatus : uint8
{
    Active      UMETA(DisplayName = "Active"),
    Stationary  UMETA(DisplayName = "Stationary"),
    Unknown     UMETA(DisplayName = "Unknown")
};
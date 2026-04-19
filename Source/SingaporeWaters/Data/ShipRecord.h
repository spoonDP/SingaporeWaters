#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShipRecord.generated.h"

/**
 * @struct FShipRecord
 * @brief Represents a single positional snapshot of a ship at a point in time.
 *
 * Each row in the imported CSV corresponds to one FShipRecord, capturing the
 * ship's geographic coordinates, movement data, and the Unix timestamp of the
 * reading. Records are loaded at runtime by UCSVShipDataProvider and consumed
 * by AShipManager to drive the playback timeline.
 *
 * Inherits from FTableRowBase so it can be used as a UDataTable row type,
 * enabling Unreal's built-in CSV importer to populate instances directly from
 * a CSV file at runtime.
 *
 * @note CSV column names must match property names exactly, including
 * capitalisation, for the DataTable importer to map them correctly.
 *
 * Expected CSV format:
 * @code
 * Id,DateTime,LongitudeDegrees,LatitudeDegrees,Speed,Course,Heading,LastUpdateTimestamp
 * @endcode
 */
USTRUCT(BlueprintType)
struct FShipRecord : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString DateTime;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double LongitudeDegrees = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double LatitudeDegrees = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Speed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Course = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Heading = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int64 LastUpdateTimestamp = 0;
};
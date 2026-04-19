#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/ShipRecord.h"
#include "ShipDataProvider.generated.h"

/**
 * @class UShipDataProvider
 * @brief Unreal reflection wrapper required by the UINTERFACE macro.
 *
 * This class exists solely to satisfy Unreal's interface declaration
 * pattern and should never be used directly. All functionality is
 * defined in IShipDataProvider.
 *
 */
UINTERFACE(MinimalAPI)
class UShipDataProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * @class IShipDataProvider
 * @brief Abstract interface for any class that supplies ship positional records
 *        to AShipManager.
 *
 * Decouples AShipManager from any specific data source. Any class that
 * implements this interface can be used as a drop-in data source without
 * requiring changes to AShipManager or AShipActor.
 *
 * Current implementations:
 * - UCSVShipDataProvider — reads from a user-selected CSV file at runtime
 *
 * Planned implementations:
 * - URESTShipDataProvider — polls a REST API endpoint for live AIS data
 *
 */
class SINGAPOREWATERS_API IShipDataProvider
{
    GENERATED_BODY()

public:
    /**
     * @brief Returns a reference to the provider's array of ship records.
     *
     * Should only be called after IsDataReady returns true. The returned
     * reference is owned by the implementing class and remains valid for
     * the lifetime of that object.
     *
     * @return Reference to a TArray of FShipRecord sorted ascending by
     *         FShipRecord::LastUpdateTimestamp.
     */
    virtual TArray<FShipRecord>& GetShipRecords() = 0;

    /**
     * @brief Returns whether the provider has data ready to be consumed.
     *
     * AShipManager checks this before calling GetShipRecords to avoid
     * operating on an empty or partially loaded dataset. Implementations
     * should return false until data has been fully loaded and is safe
     * to read.
     *
     * @return true if GetShipRecords will return a valid populated array,
     *         false otherwise.
     */
    virtual bool IsDataReady() const = 0;
};

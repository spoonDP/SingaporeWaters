#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/ShipDataProvider.h"
#include "CSVShipDataProvider.generated.h"

/**
 * @class UCSVShipDataProvider
 * @brief Concrete implementation of IShipDataProvider that loads ship
 *        positional data from a CSV file on disk at runtime.
 *
 * Reads a CSV file selected by the user via the playback UI, parses it into
 * an array of FShipRecord instances using Unreal's UDataTable importer, and
 * exposes the results through the IShipDataProvider interface.
 *
 * The CSV is parsed synchronously on the game thread. For the current dataset
 * size (~1200 rows) this is acceptable. If datasets grow significantly, this
 * class should be updated to parse asynchronously using AsyncTask to avoid
 * blocking the game thread.
 *
 * @note This class is not a UActorComponent and cannot be placed in the level.
 *       It is instantiated programmatically by AShipManager via NewObject.
 */
UCLASS()
class SINGAPOREWATERS_API UCSVShipDataProvider : public UObject, public IShipDataProvider
{
    GENERATED_BODY()

public:
    /**
     * @brief Reads a CSV file from disk and parses its contents into FShipRecord instances.
     *
     * Loads the entire file into memory as a string, creates a temporary UDataTable
     * using FShipRecord as the row struct, and uses Unreal's built-in CSV importer
     * (UDataTable::CreateTableFromCSVString) to populate it. Rows are then copied
     * out of the DataTable into CachedRecords and sorted ascending by
     * FShipRecord::LastUpdateTimestamp to guarantee correct playback order
     * regardless of row order in the source file.
     *
     * Sets bDataReady to true on success. If the file cannot be read or the path
     * is invalid, bDataReady remains false and CachedRecords will be empty.
     *
     * @param FilePath Absolute path to the CSV file on disk.
     *                 Example: "C:/User/Documents/ships.csv"
     *
     * @note The CSV first column must be named Id and is used as the DataTable
     *       row key. Remaining column names must match FShipRecord property names
     *       exactly including capitalisation.
     */
    void LoadFromFilePath(const FString& FilePath);

    /**
     * @brief Returns a reference to the array of parsed ship records.
     *
     * Returns the cached records populated by the most recent call to
     * LoadFromFilePath. Records are sorted ascending by
     * FShipRecord::LastUpdateTimestamp.
     *
     * @return Reference to the internal TArray of FShipRecord. The array will
     *         be empty if LoadFromFilePath has not been called or failed.
     */
    virtual TArray<FShipRecord>& GetShipRecords() override;

    /**
     * @brief Returns whether the provider has valid data ready to be consumed.
     *
     * Returns true only after a successful call to LoadFromFilePath where the
     * file was read and at least one row was parsed. Returns false before any
     * load attempt or if the last load failed.
     *
     * AShipManager checks this before calling GetShipRecords to avoid operating
     * on an empty dataset.
     *
     * @return true if CachedRecords is populated and ready for use, false otherwise.
     */
    virtual bool IsDataReady() const override;

private:
    TArray<FShipRecord> CachedRecords;
    bool bDataReady = false;
};

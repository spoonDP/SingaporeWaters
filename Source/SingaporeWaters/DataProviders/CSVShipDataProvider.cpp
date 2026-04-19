#include "DataProviders/CSVShipDataProvider.h"
#include "Misc/FileHelper.h"
#include "Engine/DataTable.h"

void UCSVShipDataProvider::LoadFromFilePath(const FString& FilePath)
{
    CachedRecords.Empty();
    bDataReady = false;
    UE_LOG(LogTemp, Log, TEXT("CSVShipDataProvider: Loading file: %s"), *FilePath);

    double TotalStart = FPlatformTime::Seconds();

    // read file from disk
    double ReadStart = FPlatformTime::Seconds();
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("CSVShipDataProvider: Failed to read file"));
        return;
    }
    double ReadMs = (FPlatformTime::Seconds() - ReadStart) * 1000.0;

    // (just in case) tab-to-comma replace + UDataTable parse
    double ParseStart = FPlatformTime::Seconds();
    FileContent.ReplaceInline(TEXT("\t"), TEXT(","));
    UDataTable* TempTable = NewObject<UDataTable>(GetTransientPackage());
    TempTable->AddToRoot();
    TempTable->RowStruct = FShipRecord::StaticStruct();
    TempTable->CreateTableFromCSVString(FileContent);
    double ParseMs = (FPlatformTime::Seconds() - ParseStart) * 1000.0;

    UE_LOG(LogTemp, Log, TEXT("CSVShipDataProvider: Table has %d rows"), TempTable->GetRowMap().Num());

    // copy rows into CachedRecords
    double CopyStart = FPlatformTime::Seconds();
    for (auto& Pair : TempTable->GetRowMap())
    {
        FShipRecord* Row = reinterpret_cast<FShipRecord*>(Pair.Value);
        if (Row) CachedRecords.Add(*Row);
    }
    TempTable->RemoveFromRoot();
    double CopyMs = (FPlatformTime::Seconds() - CopyStart) * 1000.0;

    // sort if needed, low overhead
    double SortStart = FPlatformTime::Seconds();
    CachedRecords.Sort([](const FShipRecord& A, const FShipRecord& B)
        {
            return A.LastUpdateTimestamp < B.LastUpdateTimestamp;
        });
    double SortMs = (FPlatformTime::Seconds() - SortStart) * 1000.0;

    bDataReady = true;

    double TotalMs = (FPlatformTime::Seconds() - TotalStart) * 1000.0;
    UE_LOG(LogTemp, Log, TEXT("CSVShipDataProvider: Loaded %d records | Read: %.2f ms | Parse: %.2f ms | Copy: %.2f ms | Sort: %.2f ms | Total: %.2f ms"),
        CachedRecords.Num(), ReadMs, ParseMs, CopyMs, SortMs, TotalMs);

    if (CachedRecords.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("CSVShipDataProvider: First record Lat: %.6f Lon: %.6f"),
            CachedRecords[0].LatitudeDegrees, CachedRecords[0].LongitudeDegrees);
        UE_LOG(LogTemp, Log, TEXT("CSVShipDataProvider: Last record Lat: %.6f Lon: %.6f"),
            CachedRecords.Last().LatitudeDegrees, CachedRecords.Last().LongitudeDegrees);
    }
}

TArray<FShipRecord>& UCSVShipDataProvider::GetShipRecords()
{
    return CachedRecords;
}

bool UCSVShipDataProvider::IsDataReady() const
{
    return bDataReady;
}
#include "Managers/ShipManager.h"
#include "Ships/ShipActor.h"
#include "DataProviders/CSVShipDataProvider.h"

AShipManager::AShipManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AShipManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("ShipManager: Ready, use LoadFromFile to load data"));
}

void AShipManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsPlaying) return;
    if (!DataProvider.GetObject()) return;
    if (!DataProvider->IsDataReady()) return;

    CurrentPlaybackTime += DeltaTime * PlaybackSpeed;

    // stop at end of data
    if (CurrentPlaybackTime >= TotalDurationSeconds)
    {
        CurrentPlaybackTime = TotalDurationSeconds;
        bIsPlaying = false;
        UE_LOG(LogTemp, Log, TEXT("ShipManager: Playback complete"));
    }

    UpdateShipPosition();
}

void AShipManager::Play()
{
    if (!DataProvider.GetObject() || !DataProvider->IsDataReady())
    {
        UE_LOG(LogTemp, Warning, TEXT("ShipManager: No data loaded yet"));
        return;
    }
    bIsPlaying = true;
    UE_LOG(LogTemp, Log, TEXT("ShipManager: Playing"));
}

void AShipManager::Pause()
{
    bIsPlaying = false;
    UE_LOG(LogTemp, Log, TEXT("ShipManager: Paused"));
}

void AShipManager::SeekToTime(float TimeSeconds)
{
    CurrentPlaybackTime = FMath::Clamp(TimeSeconds, 0.0f, TotalDurationSeconds);
    UpdateShipPosition();
}

void AShipManager::LoadFromFile(const FString& FilePath)
{
    UE_LOG(LogTemp, Log, TEXT("ShipManager: LoadFromFile called"));

    // stop and destroy existing ship if any
    bIsPlaying = false;
    CurrentPlaybackTime = 0.0f;
    if (Ship)
    {
        Ship->Destroy();
        Ship = nullptr;
    }

    UCSVShipDataProvider* NewProvider = NewObject<UCSVShipDataProvider>(this);
    NewProvider->LoadFromFilePath(FilePath);

    ActiveDataProvider = NewProvider;
    DataProvider = TScriptInterface<IShipDataProvider>(NewProvider);

    if (!DataProvider->IsDataReady())
    {
        UE_LOG(LogTemp, Error, TEXT("ShipManager: Data failed to load"));
        return;
    }

    TArray<FShipRecord>& Records = DataProvider->GetShipRecords();

    if (Records.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("ShipManager: No records found"));
        return;
    }

    // records are sorted by timestamp so first and last give us the range
    FirstTimestamp = Records[0].LastUpdateTimestamp;
    TotalDurationSeconds = (float)(Records.Last().LastUpdateTimestamp - FirstTimestamp);

    UE_LOG(LogTemp, Log, TEXT("ShipManager: %d records loaded, duration %.0f seconds"),
        Records.Num(), TotalDurationSeconds);

    SpawnShip();

    // position ship at start immediately
    UpdateShipPosition();
}

void AShipManager::SpawnShip()
{
    if (!ShipActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShipManager: ShipActorClass not set"));
        return;
    }

    FActorSpawnParameters Params;
    //Params.Name = TEXT("Ship");
    // name is causing some issues

    Ship = GetWorld()->SpawnActor<AShipActor>(
        ShipActorClass,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        Params
    );

    if (Ship)
    {
        UE_LOG(LogTemp, Log, TEXT("ShipManager: Ship spawned"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ShipManager: Failed to spawn ship"));
    }
}

void AShipManager::UpdateShipPosition()
{
    if (!Ship) return;

    FShipRecord Interpolated = InterpolateRecord(CurrentPlaybackTime);
    Ship->SetPositionFromRecord(Interpolated);
}

FShipRecord AShipManager::InterpolateRecord(float TimeSeconds) const
{
    TArray<FShipRecord>& Records = DataProvider->GetShipRecords();
    if (Records.Num() == 0) return FShipRecord();
    if (Records.Num() == 1) return Records[0];

    double TargetTimestamp = (double)FirstTimestamp + (double)TimeSeconds;

    int32 PrevIndex = 0;
    for (int32 i = 0; i < Records.Num() - 1; i++)
    {
        if ((double)Records[i].LastUpdateTimestamp <= TargetTimestamp)
            PrevIndex = i;
        else
            break;
    }

    if (PrevIndex >= Records.Num() - 1)
        return Records.Last();

    const FShipRecord& A = Records[PrevIndex];
    const FShipRecord& B = Records[PrevIndex + 1];

    float SegmentDuration = (float)(B.LastUpdateTimestamp - A.LastUpdateTimestamp);
    float Alpha = 0.0f;
    if (SegmentDuration > 0.0f)
    {
        Alpha = (float)(TargetTimestamp - (double)A.LastUpdateTimestamp) / SegmentDuration;
        Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
    }

    FShipRecord Result;
    Result.LatitudeDegrees = FMath::Lerp(A.LatitudeDegrees, B.LatitudeDegrees, Alpha);
    Result.LongitudeDegrees = FMath::Lerp(A.LongitudeDegrees, B.LongitudeDegrees, Alpha);
    Result.Speed = FMath::Lerp(A.Speed, B.Speed, Alpha);
    Result.Course = FMath::Lerp(A.Course, B.Course, Alpha);

    // handle rotations going from 360 to 0
    float HeadingDelta = B.Heading - A.Heading;
    while (HeadingDelta > 180.0f)  HeadingDelta -= 360.0f;
    while (HeadingDelta < -180.0f) HeadingDelta += 360.0f;
    Result.Heading = A.Heading + HeadingDelta * Alpha;
    if (Result.Heading < 0.0f)    Result.Heading += 360.0f;
    if (Result.Heading >= 360.0f) Result.Heading -= 360.0f;

    Result.LastUpdateTimestamp = (int64)TargetTimestamp;
    return Result;
}
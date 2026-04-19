#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ShipDataProvider.h"
#include "Data/ShipRecord.h"
#include "Ships/ShipActor.h"
#include "ShipManager.generated.h"

/**
 * @class AShipManager
 * @brief Central actor responsible for loading ship data, spawning the ship
 *        actor, and driving the playback timeline.
 *
 * AShipManager acts as the coordinator between the data layer and the
 * visual layer. It accepts a CSV file path at runtime via LoadFromFile,
 * delegates parsing to UCSVShipDataProvider, spawns a single AShipActor,
 * and advances the playback clock each frame to move the ship through its
 * recorded journey.
 *
 * Position at any given playback time is calculated by interpolating between
 * the two FShipRecord entries whose timestamps bracket the current time,
 * producing smooth movement between recorded GPS snapshots.
 *
 * This actor should be placed in the level with ShipActorClass assigned to
 * BP_ShipActor in the Details panel before pressing Play.
 *
 */
UCLASS()
class SINGAPOREWATERS_API AShipManager : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief Sets up the actor with ticking enabled.
     */
    AShipManager();

    /**
     * @brief Called when the level starts. Logs that the manager is ready
     *        and waiting for a file to be loaded via LoadFromFile.
     */
    virtual void BeginPlay() override;

    /**
     * @brief Called every frame. Advances CurrentPlaybackTime by
     *        DeltaTime * PlaybackSpeed and updates the ship's position
     *        when playback is active. Stops automatically when the end
     *        of the recorded data is reached.
     *
     * @param DeltaTime Time in seconds since the last frame.
     */
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Ships")
    TSubclassOf<AShipActor> ShipActorClass;

    UPROPERTY(BlueprintReadOnly, Category = "Playback")
    float CurrentPlaybackTime = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Playback")
    float TotalDurationSeconds = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Playback")
    float PlaybackSpeed = 1.0f;

    /**
     * @brief Begins advancing the playback clock each frame.
     *
     * Has no effect if no data has been loaded yet. Safe to call
     * from Blueprints or the playback UI widget.
     */
    UFUNCTION(BlueprintCallable, Category = "Playback")
    void Play();

    /**
     * @brief Halts playback without resetting CurrentPlaybackTime.
     *
     * The ship remains at its current position. Call Play to resume
     * from the same point.
     */
    UFUNCTION(BlueprintCallable, Category = "Playback")
    void Pause();

    /**
     * @brief Jumps playback to a specific time and updates the ship position
     *        immediately.
     *
     * TimeSeconds is clamped to the range [0, TotalDurationSeconds].
     * Can be called while playing or paused.
     *
     * @param TimeSeconds Target playback time in seconds from the start of
     *                    the recorded journey.
     */
    UFUNCTION(BlueprintCallable, Category = "Playback")
    void SeekToTime(float TimeSeconds);

    /**
     * @brief Loads ship positional data from a CSV file and begins the
     *        journey from the start.
     *
     * Destroys any existing ship actor, creates a new UCSVShipDataProvider,
     * parses the CSV, calculates TotalDurationSeconds from the first and last
     * record timestamps, spawns a new AShipActor, and positions it at the
     * first record immediately.
     *
     * Called by UPlaybackWidget when the user confirms a file in the
     * file picker dialog.
     *
     * @param FilePath Absolute path to the CSV file on disk.
     *
     */
    UFUNCTION(BlueprintCallable, Category = "Data")
    void LoadFromFile(const FString& FilePath);

    UPROPERTY()
    AShipActor* Ship;

private:
    UPROPERTY()
    UObject* ActiveDataProvider;

    TScriptInterface<IShipDataProvider> DataProvider;

    bool bIsPlaying = false;
    int64 FirstTimestamp = 0;

    /**
     * @brief Spawns an AShipActor instance of ShipActorClass into the level.
     *
     * Called internally by LoadFromFile after data has been successfully parsed.
     * Logs an error if ShipActorClass is not set in the Details panel.
     */
    void SpawnShip();

    /**
     * @brief Calculates the interpolated FShipRecord for the current playback
     *        time and passes it to the ship actor to update its position.
     *
     * Called each frame by Tick when playing, and once immediately by
     * SeekToTime regardless of play state.
     */
    void UpdateShipPosition();

    /**
     * @brief Returns a FShipRecord interpolated between the two records whose
     *        timestamps bracket the given playback time.
     *
     * Converts TimeSeconds to an absolute Unix timestamp using FirstTimestamp
     * as the origin, then performs a linear search through the sorted records
     * to find the surrounding pair. Latitude, longitude, speed, heading and
     * course are all linearly interpolated. Heading and course use
     * FMath::LerpAngle to correctly handle the 0/360 degree wraparound.
     *
     * Returns the first record if TimeSeconds is before the first timestamp,
     * and the last record if TimeSeconds is past the last timestamp.
     *
     * @param TimeSeconds Playback time in seconds from the start of the journey.
     * @return Interpolated FShipRecord representing the ship's state at the
     *         given time.
     */
    FShipRecord InterpolateRecord(float TimeSeconds) const;
};
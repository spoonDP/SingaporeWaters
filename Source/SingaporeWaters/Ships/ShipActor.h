#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ShipRecord.h"
#include "ShipActor.generated.h"

/**
 * @class AShipActor
 * @brief Actor that represents a single ship on the Cesium globe, positioning
 *        itself at real-world geographic coordinates each frame.
 *
 * AShipActor is spawned and driven by AShipManager. Each frame the manager
 * calls SetPositionFromRecord with an interpolated FShipRecord, and the actor
 * forwards the latitude, longitude and heading to its UCesiumGlobeAnchorComponent
 * which handles the conversion from WGS84 coordinates to Unreal world space.
 *
 * The visual representation is decoupled from the positioning logic — the ship
 * mesh is held in a UChildActorComponent whose Child Actor Class is set to the
 * ship mesh Blueprint in the editor. This allows the mesh to be swapped without
 * touching any C++ code.
 *
 * MeshRotationOffset accounts for differences between the mesh's forward axis
 * and Unreal's default forward direction, and should be adjusted per mesh asset
 * in the Details panel.
 *
 */
UCLASS()
class SINGAPOREWATERS_API AShipActor : public AActor
{
    GENERATED_BODY()

public:
    /**
     * @brief Initialises the SceneRoot, ShipVisual and GlobeAnchor components
     *        and sets up the component attachment hierarchy.
     */
    AShipActor();

    /**
     * @brief Called when the level starts. Reserved for future initialisation.
     */
    virtual void BeginPlay() override;

    /**
     * @brief Called every frame. Reserved for future per-frame logic such as
     *        smooth client-side interpolation if needed.
     *
     * @param DeltaTime Time in seconds since the last frame.
     */
    virtual void Tick(float DeltaTime) override;

    /**
     * @brief Updates the ship's position, heading and display state from an
     *        interpolated FShipRecord.
     *
     * Caches the record's latitude, longitude, speed, heading and datetime
     * into the public display properties, then calls
     * UCesiumGlobeAnchorComponent::MoveToLongitudeLatitudeHeight to reposition
     * the actor on the globe. Heading is applied via SetActorRelativeRotation
     * with MeshRotationOffset added to align the mesh's forward axis with the
     * ship's true heading.
     *
     * Called by AShipManager::UpdateShipPosition every frame during playback
     * and once immediately after a file is loaded to place the ship at the
     * journey's starting position.
     *
     * @param Record Interpolated FShipRecord for the current playback time,
     *               produced by AShipManager::InterpolateRecord.
     */
    void SetPositionFromRecord(const FShipRecord& Record);

    UPROPERTY(VisibleAnywhere, Category = "Components")
    USceneComponent* SceneRoot;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UChildActorComponent* ShipVisual;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UCesiumGlobeAnchorComponent* GlobeAnchor;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float InterpolationSpeed = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MeshRotationOffset = 90.0f;

    // Exposed for UI to read
    double CurrentLatitude = 0.0;
    double CurrentLongitude = 0.0;
    float CurrentSpeed = 0.0f;
    float CurrentHeading = 0.0f;
    FString CurrentDateTime{};
};
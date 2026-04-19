#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlaybackWidget.generated.h"

/**
 * @class UPlaybackWidget
 * @brief UMG widget that provides the runtime UI for controlling ship
 *        playback and displaying live positional data.
 *
 * Displayed as an overlay on the viewport when the level starts. Locates
 * AShipManager automatically in NativeConstruct by searching the level, then
 * delegates all user interactions to it.
 *
 * Provides the following controls:
 * - Load File — opens a file picker dialog for the user to select a CSV
 * - Play / Pause — starts and halts the playback clock
 * - Timeline slider — scrubs to any point in the recorded journey
 *
 * The following ship telemetry values are updated every frame via NativeTick:
 * - Current and total journey time
 * - Latitude, longitude, speed and heading
 *
 * The C++ class defines behaviour only. The visual layout — widget positions,
 * fonts, colours and sizing — is configured in the paired Widget Blueprint
 * WBP_Playback. Each UPROPERTY marked with meta=(BindWidget) must have a
 * widget of the matching name and type present in WBP_Playback or the Blueprint
 * will fail to compile.
 *
 */
UCLASS()
class SINGAPOREWATERS_API UPlaybackWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /**
     * @brief Called when the widget is constructed and added to the viewport.
     *
     * Binds all button and slider delegates to their respective handler
     * functions. Searches the level for an AShipManager instance using
     * UGameplayStatics::GetActorOfClass and caches it in ShipManager.
     * Logs an error if no ShipManager is found in the level.
     */
    virtual void NativeConstruct() override;

    /**
     * @brief Called every frame to keep the UI in sync with playback state.
     *
     * Updates the timeline slider position, current and total time text,
     * and the ship telemetry display (latitude, longitude, speed, heading)
     * by reading directly from AShipManager and AShipActor each frame.
     * The slider is not updated while bSliderBeingDragged is true to prevent
     * the playback position fighting the user's drag input.
     *
     * @param MyGeometry Layout geometry of this widget.
     * @param DeltaTime  Time in seconds since the last frame.
     */
    virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

    UPROPERTY(BlueprintReadWrite, Category = "Playback")
    class AShipManager* ShipManager;

protected:
    UPROPERTY(meta = (BindWidget))
    class UButton* PlayButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* PauseButton;

    UPROPERTY(meta = (BindWidget))
    class UButton* LoadFileButton;

    UPROPERTY(meta = (BindWidget))
    class USlider* TimelineSlider;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CurrentTimeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TotalTimeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LatitudeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* LongitudeText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* SpeedText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HeadingText;

    /**
     * @brief Bound to PlayButton::OnClicked. Calls AShipManager::Play.
     */
    UFUNCTION()
    void OnPlayClicked();

    /**
     * @brief Bound to PauseButton::OnClicked. Calls AShipManager::Pause.
     */
    UFUNCTION()
    void OnPauseClicked();

    /**
     * @brief Bound to LoadFileButton::OnClicked. Opens a system file picker
     *        dialog filtered to CSV files and passes the selected path to
     *        AShipManager::LoadFromFile.
     *
     * Uses IDesktopPlatform::OpenFileDialog with a nullptr parent window
     * handle to avoid a deadlock that occurs when parenting the dialog to
     * the editor window during Play In Editor sessions. In a packaged build
     * the dialog behaves correctly with or without a parent handle.
     *
     * Does nothing if the user cancels the dialog or if ShipManager is null.
     */
    UFUNCTION()
    void OnLoadFileClicked();

    /**
     * @brief Bound to TimelineSlider::OnValueChanged. Seeks playback to the
     *        position corresponding to the slider value.
     *
     * Converts the normalised slider value (0.0–1.0) to an absolute time in
     * seconds by multiplying by AShipManager::TotalDurationSeconds, then calls
     * AShipManager::SeekToTime. Sets bSliderBeingDragged to true for the
     * duration of the call to prevent NativeTick from overwriting the slider
     * position while the user is dragging.
     *
     * @param Value Normalised slider position in the range [0.0, 1.0].
     */
    UFUNCTION()
    void OnTimelineSliderChanged(float Value);

private:
    bool bSliderBeingDragged = false;

    /**
     * @brief Converts a time value in seconds to a formatted HH:MM:SS string.
     *
     * Used to populate CurrentTimeText and TotalTimeText each frame.
     *
     * @param TimeSeconds Time in seconds to format.
     * @return Formatted string in the form "HH:MM:SS".
     */
    FString FormatTime(float TimeSeconds) const;
};
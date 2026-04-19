#include "UI/PlaybackWidget.h"
#include "Managers/ShipManager.h"
#include "Ships/ShipActor.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"

void UPlaybackWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (PlayButton)
        PlayButton->OnClicked.AddDynamic(this, &UPlaybackWidget::OnPlayClicked);

    if (PauseButton)
        PauseButton->OnClicked.AddDynamic(this, &UPlaybackWidget::OnPauseClicked);

    if (LoadFileButton)
        LoadFileButton->OnClicked.AddDynamic(this, &UPlaybackWidget::OnLoadFileClicked);

    if (TimelineSlider)
        TimelineSlider->OnValueChanged.AddDynamic(
            this, &UPlaybackWidget::OnTimelineSliderChanged);

    AActor* FoundActor = UGameplayStatics::GetActorOfClass(
        GetWorld(), AShipManager::StaticClass());

    if (FoundActor)
    {
        ShipManager = Cast<AShipManager>(FoundActor);
        UE_LOG(LogTemp, Log, TEXT("PlaybackWidget: ShipManager found"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlaybackWidget: ShipManager not found in level"));
    }
}

void UPlaybackWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);

    if (!ShipManager) return;

    float Duration = ShipManager->TotalDurationSeconds;
    float Current = ShipManager->CurrentPlaybackTime;

    // update slider position
    if (TimelineSlider && Duration > 0.0f && !bSliderBeingDragged)
    {
        TimelineSlider->SetValue(FMath::Clamp(Current / Duration, 0.0f, 1.0f));
    }

    // update time labels
    if (CurrentTimeText)
        CurrentTimeText->SetText(FText::FromString(FormatTime(Current)));

    if (TotalTimeText)
        TotalTimeText->SetText(FText::FromString(FormatTime(Duration)));

    // update ship coordinate display
    if (ShipManager->Ship)
    {
        AShipActor* S = ShipManager->Ship;

        if (LatitudeText)
            LatitudeText->SetText(FText::FromString(
                FString::Printf(TEXT("Lat: %.6f"), S->CurrentLatitude)));

        if (LongitudeText)
            LongitudeText->SetText(FText::FromString(
                FString::Printf(TEXT("Lon: %.6f"), S->CurrentLongitude)));

        if (SpeedText)
            SpeedText->SetText(FText::FromString(
                FString::Printf(TEXT("Speed: %.2f kts"), S->CurrentSpeed)));

        if (HeadingText)
            HeadingText->SetText(FText::FromString(
                FString::Printf(TEXT("Heading: %.1f deg"), S->CurrentHeading)));
    }
}

void UPlaybackWidget::OnPlayClicked()
{
    if (ShipManager) ShipManager->Play();
}

void UPlaybackWidget::OnPauseClicked()
{
    if (ShipManager) ShipManager->Pause();
}

void UPlaybackWidget::OnLoadFileClicked()
{
    UE_LOG(LogTemp, Log, TEXT("PlaybackWidget: Load file clicked"));
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (!DesktopPlatform)
    {
        UE_LOG(LogTemp, Error, TEXT("PlaybackWidget: DesktopPlatform not available"));
        return;
    }
    TArray<FString> OutFiles;
    bool bOpened = DesktopPlatform->OpenFileDialog(
        nullptr,
        TEXT("Select Ship Data CSV"),
        FPaths::ProjectUserDir(),
        TEXT(""),
        TEXT("CSV Files (*.csv)|*.csv"),
        EFileDialogFlags::None,
        OutFiles
    );
    UE_LOG(LogTemp, Log, TEXT("PlaybackWidget: Dialog %s, files: %d"),
        bOpened ? TEXT("confirmed") : TEXT("cancelled"), OutFiles.Num());
    if (bOpened && OutFiles.Num() > 0 && ShipManager)
    {
        double StartTime = FPlatformTime::Seconds();

        ShipManager->LoadFromFile(OutFiles[0]);

        double ElapsedMs = (FPlatformTime::Seconds() - StartTime) * 1000.0;
        UE_LOG(LogTemp, Log, TEXT("PlaybackWidget: File load took %.2f ms"), ElapsedMs);
    }
    else if (!ShipManager)
    {
        UE_LOG(LogTemp, Error, TEXT("PlaybackWidget: ShipManager is null"));
    }
}

void UPlaybackWidget::OnTimelineSliderChanged(float Value)
{
    if (!ShipManager) return;

    bSliderBeingDragged = true;
    ShipManager->SeekToTime(Value * ShipManager->TotalDurationSeconds);
    bSliderBeingDragged = false;
}

FString UPlaybackWidget::FormatTime(float TimeSeconds) const
{
    int32 Hours = FMath::FloorToInt(TimeSeconds / 3600);
    int32 Minutes = FMath::FloorToInt(FMath::Fmod(TimeSeconds, 3600.0f) / 60.0f);
    int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeSeconds, 60.0f));
    return FString::Printf(TEXT("%02d:%02d:%02d"), Hours, Minutes, Seconds);
}
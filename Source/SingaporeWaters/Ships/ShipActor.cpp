#include "Ships/ShipActor.h"
#include "Components/ChildActorComponent.h"
#include "CesiumGlobeAnchorComponent.h"

AShipActor::AShipActor()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    ShipVisual = CreateDefaultSubobject<UChildActorComponent>(TEXT("ShipVisual"));
    ShipVisual->SetupAttachment(RootComponent);

    GlobeAnchor = CreateDefaultSubobject<UCesiumGlobeAnchorComponent>(TEXT("GlobeAnchor"));
}

void AShipActor::BeginPlay()
{
    Super::BeginPlay();
}

void AShipActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AShipActor::SetPositionFromRecord(const FShipRecord& Record)
{
    CurrentLatitude = Record.LatitudeDegrees;
    CurrentLongitude = Record.LongitudeDegrees;
    CurrentSpeed = Record.Speed;
    CurrentHeading = Record.Heading;
    CurrentDateTime = Record.DateTime;

    if (!GlobeAnchor)
    {
        UE_LOG(LogTemp, Error, TEXT("ShipActor: GlobeAnchor is null"));
        return;
    }

    GlobeAnchor->MoveToLongitudeLatitudeHeight(
        FVector(CurrentLongitude, CurrentLatitude, 0.0)
    );

    GetRootComponent()->SetRelativeRotation(FRotator(0.0f, -CurrentHeading - 90.0f, 0.0f));
}
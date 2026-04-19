# SingaporeWaters

An Unreal Engine 5 application that visualises ship movement across Singapore's waters using real-world geographic data. Ships are rendered on a Cesium globe and animated through a sequence of GPS coordinates loaded from a CSV file at runtime.

---

## Prerequisites

### Unreal Engine 5
Download and install Unreal Engine 5.4 or later through the [Epic Games Launcher](https://www.unrealengine.com).

### Cesium for Unreal (required)
This project requires the **Cesium for Unreal** plugin to render the globe and handle geographic coordinate conversion.

1. Open the Epic Games Launcher
2. Go to **Unreal Engine → Library → Vault**
3. Search for **Cesium for Unreal**
4. Click **Install to Engine** for your UE5 version
5. Once installed, enable it in your project under **Edit → Plugins → search "Cesium" → enable**

Without this plugin the project will not compile.

---

## Project structure

```
SingaporeWaters/
├── Source/SingaporeWaters/
│   ├── Data/
│   │   └── ShipRecord.h              — FShipRecord struct matching CSV columns
│   ├── Interfaces/
│   │   └── ShipDataProvider.h        — IShipDataProvider interface
│   ├── DataProviders/
│   │   └── CSVShipDataProvider.h/.cpp — reads and parses CSV at runtime
│   ├── Ships/
│   │   └── ShipActor.h/.cpp          — actor that moves on the Cesium globe
│   ├── Managers/
│   │   └── ShipManager.h/.cpp        — spawns ship, drives playback timeline
│   └── UI/
│       └── PlaybackWidget.h/.cpp     — UMG widget for playback controls
└── Content/SingaporeWaters/
    ├── Ships/
    │   └── BP_ShipActor              — Blueprint subclass of AShipActor
    └── UI/
        └── WBP_Playback              — Widget Blueprint for the playback UI
```

---

## Getting started

### 1. Clone the repository

```
git clone https://github.com/spoonDP/SingaporeWaters.git
```

### 2. Generate Visual Studio project files
 
Right-click **SingaporeWaters.uproject** in Windows Explorer and select **Generate Visual Studio project files**.

### 3. Build and open the project
 
Double-click **SingaporeWaters.uproject** to open the project in the editor.

---

## CSV data format

The application accepts CSV files exported from Excel. The file must have the following column headers in the first row, slightly modified from the original CSV headers:

```
Id,DateTime,LongitudeDegrees,LatitudeDegrees,Speed,Course,Heading,LastUpdateTimestamp
```

---

## Usage

1. Press **Play** in the Unreal Editor
2. Click **Import** in the playback UI
3. Navigate to your CSV file and confirm
4. Once loaded the ship will appear at its starting position on the globe
5. Click **Play** to begin playback. The ship will travel through all coordinates in sequence
6. Use the **timeline slider** to scrub to any point in the journey
7. Current latitude, longitude, speed and heading are displayed in real time

### Camera controls (CesiumFlyCamera)

| Input | Action |
|---|---|
| Right mouse + WASD | Fly through the scene |
| Scroll wheel | Increase or decrease fly speed |
| Right mouse drag | Look around |

---

## Future work

- REST API integration via `URESTShipDataProvider`
- Multi-ship support from multiple CSV files

---

## Dependencies

| Dependency | Version | Purpose |
|---|---|---|
| Unreal Engine | 5.4 | Game engine and editor |
| Cesium for Unreal | 2.0+ | Globe rendering and georeferencing |
| Visual Studio | 2022 | C++ compilation |
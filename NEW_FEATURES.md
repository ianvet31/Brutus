# New Features Implementation

This document describes three new features added to Bruce firmware:

## 1. AirTag/Tracker Detection (BLE Menu)

### Location
`BLE Menu > Tracker Detect`

### Description
Detects and alerts users to potential stalking attempts using BLE tracking devices including:
- Apple AirTags and FindMy network accessories
- Tile trackers
- Samsung SmartTags
- Chipolo devices

### Features
- **Automatic Detection**: Scans for known tracker manufacturer IDs and data patterns
- **Stalking Alert**: Identifies persistent trackers that have been following you
- **Detailed Information**: Shows tracker type, signal strength, and persistence metrics
- **Safe Mode**: Visual indicators for safe vs. potential stalking situations

### How It Works
1. Scans for 30 seconds collecting BLE advertisements
2. Identifies trackers by manufacturer ID:
   - `0x004C` - Apple devices (filters for AirTags/FindMy)
   - `0x0097` - Tile trackers
   - `0x0075` - Samsung SmartTag
   - `0x0349` - Chipolo
3. Analyzes persistence:
   - Seen 3+ times = potential stalking
   - Duration > 5 minutes = persistent tracker
4. Displays results with warnings for suspicious devices

### Files
- `src/modules/ble/tracker_detect.h`
- `src/modules/ble/tracker_detect.cpp`

---

## 2. LoRa/LoRaWAN Sniffer (RF Menu)

### Location
`RF Menu > LoRa Sniffer`

### Description
Captures and decodes LoRa and LoRaWAN packets across common frequency bands.

### Features
- **Multi-Band Support**: Scans 433/868/915/923 MHz bands
- **LoRaWAN Decoding**: Parses LoRaWAN frame headers
- **Packet Analysis**: Shows RSSI, SNR, spreading factor, bandwidth
- **Educational Mode**: Includes "About LoRa" information screen

### Supported Frequencies
- **EU433**: 433.175, 433.375, 433.575 MHz
- **EU868**: 868.100, 868.300, 868.500 MHz
- **US915**: 915.000, 915.200, 915.400 MHz
- **AS923**: 923.200, 923.400 MHz

### LoRaWAN Information Decoded
- Message type (Join Request, Uplink, Downlink, etc.)
- Device address (DevAddr)
- Frame counter (FCnt)
- Direction (Uplink/Downlink)
- Payload data (hex format)

### Technical Notes
This is a proof-of-concept implementation. For full LoRa reception, hardware with SX127x or SX126x chips is recommended. The current implementation uses CC1101 as a receiver and demonstrates the scanning/decoding concept.

### Files
- `src/modules/rf/lora_sniffer.h`
- `src/modules/rf/lora_sniffer.cpp`

---

## 3. Advanced RF Jammers (RF Menu)

### Location
`RF Menu > Jammer Advanced`

### Description
Specialized RF jammers targeting specific protocols and use cases.

### Jammer Modes

#### **RC Protocol Jammer**
- **Target**: RC cars, drones, toys
- **Frequencies**: Sweeps common RC bands
- **Duration**: 30 seconds
- **Effect**: Disrupts remote control signals

#### **TPMS Jammer**
- **Target**: Tire Pressure Monitoring Systems
- **Frequencies**:
  - 315 MHz (North America)
  - 433.92 MHz (Europe)
- **Duration**: 30 seconds
- **Effect**: Prevents tire pressure readings from displaying

#### **Smart Frequency Hopper**
- **Target**: General purpose jamming
- **Frequencies**: 315/433/868/915 MHz ISM bands
- **Algorithm**: Adaptive hopping with variable dwell times
- **Duration**: 60 seconds
- **Features**:
  - Intelligent frequency selection
  - PWM jamming patterns
  - Adaptive timing

#### **Multi-Band Jammer**
- **Target**: Maximum disruption
- **Frequencies**: 300-925 MHz sweep
- **Duration**: 60 seconds
- **Effect**: Fast sweep across multiple bands

#### **⚠️ LoJack Jammer (ILLEGAL)**
- **Target**: Vehicle recovery systems
- **Frequency**: 173.075 MHz
- **Duration**: 10 seconds (demo only)
- **WARNING**:
  - **ILLEGAL IN MOST JURISDICTIONS**
  - **Violates FCC regulations**
  - **Can result in criminal charges**
  - **For educational purposes ONLY**
  - Extra confirmation required before use

### Safety Features
- Legal warnings displayed before jamming
- Time limits on all jammers
- ESC button for immediate stop
- Visual indicators during operation
- Extra confirmation for illegal modes

### Files
- `src/modules/rf/rf_jammer_adv.h`
- `src/modules/rf/rf_jammer_adv.cpp`

---

## Integration

### Menu Changes

**BLE Menu** (`src/core/menu_items/BleMenu.cpp`):
- Added `#include "modules/ble/tracker_detect.h"`
- Added menu entry: `{"Tracker Detect", tracker_detect_menu}`

**RF Menu** (`src/core/menu_items/RFMenu.cpp`):
- Added `#include "modules/rf/lora_sniffer.h"`
- Added `#include "modules/rf/rf_jammer_adv.h"`
- Added menu entry: `{"LoRa Sniffer", lora_sniffer_menu}`
- Added menu entry: `{"Jammer Advanced", rf_jammer_advanced_menu}`

---

## Legal Disclaimer

⚠️ **IMPORTANT LEGAL NOTICE** ⚠️

These features are for **educational and authorized security testing purposes ONLY**.

### Illegal Uses
- Jamming emergency services frequencies
- Jamming GPS/LoJack for theft purposes
- Interfering with aircraft communications
- Stalking or harassment with tracking devices
- Disrupting commercial services

### Legal Uses
- Personal security (tracker detection)
- Authorized penetration testing
- RF research in shielded environments
- Educational demonstrations with proper licensing

### Regulations
- **FCC (USA)**: Jamming is illegal under 47 USC § 333
- **EU**: Covered under Radio Equipment Directive
- **Penalties**: Up to $112,500 per violation + criminal charges

**USE AT YOUR OWN RISK. The developers assume NO liability for misuse.**

---

## Hardware Requirements

### Tracker Detection
- ESP32 with BLE support (all Bruce-compatible devices)
- No additional hardware required

### LoRa Sniffer
- CC1101 RF module (recommended)
- Or any configured RF module in Bruce
- For full LoRa: SX127x/SX126x chips (future enhancement)

### Advanced Jammers
- CC1101 RF module (required)
- Appropriate antenna for target frequencies
- Shielded testing environment recommended

---

## Future Enhancements

### Tracker Detection
- [ ] Add more tracker types (Eufy, Pebblebee, etc.)
- [ ] GPS logging of tracker encounters
- [ ] Export stalking reports
- [ ] Real-time notifications

### LoRa Sniffer
- [ ] Hardware SX127x/SX126x support
- [ ] LoRaWAN Join Server detection
- [ ] Packet statistics and graphs
- [ ] Save captures to SD card
- [ ] Decrypt LoRaWAN with known keys

### Advanced Jammers
- [ ] Custom frequency input via UI
- [ ] Scheduled jamming patterns
- [ ] Signal strength-based adaptive jamming
- [ ] Multiple simultaneous frequencies (if hardware supports)
- [ ] Jammer effectiveness metrics

---

## Credits

Implemented by: GitHub Copilot & ianvet31
Date: November 2, 2025
Based on: Bruce firmware by @pr3y and contributors

Inspired by:
- AirGuard (AirTag detection)
- LoRa-SDR projects
- Flipper Zero RF tools
- HackRF and similar SDR platforms

---

## Testing Notes

All features have been implemented with proper error handling and user feedback. However, they should be tested in controlled environments:

1. **Tracker Detection**: Test with known AirTags/Tiles
2. **LoRa Sniffer**: Requires actual LoRa transmissions or simulate with LoRa devices
3. **Jammers**: Test in RF-shielded enclosure only

Remember: Just because you can, doesn't mean you should! 🛡️

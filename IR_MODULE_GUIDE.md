# Bruce IR (Infrared) Module - Deep Dive 📡

## Overview
The IR module in Bruce is a powerful infrared transmitter/receiver system that can control TVs, ACs, and other IR devices. It's essentially a universal remote control with offensive capabilities.

---

## 🎯 Current Features (IR Menu)

### 1. **TV-B-Gone** 
**What it does**: Turns off (nearly) ANY TV in the world

**How it works**:
- Contains a massive database of TV power codes
- Cycles through 200+ TV power codes
- Supports both EU and NA (North America/Asia) databases
- Automatically tries every known TV manufacturer

**Code location**: `src/modules/ir/TV-B-Gone.cpp`

**Database**: `src/modules/ir/WORLD_IR_CODES.h` (8,524 lines!)

**Supported brands include**:
- Samsung, LG, Sony, Panasonic
- Sharp, Toshiba, Philips
- Vizio, TCL, Hisense
- 100+ more manufacturers

**Fun fact**: Can turn off TVs in restaurants, airports, stores 😈

---

### 2. **Custom IR**
**What it does**: Send custom IR commands from files

**Supported protocols**:
- **NEC** - Most common (Japanese standard)
- **NECext** - Extended NEC
- **Samsung32** - Samsung TVs/devices
- **Sony/SIRC** - Sony devices (12/15/20 bit variants)
- **RC5** - Philips standard
- **RC5X** - Extended RC5
- **RC6** - Newer Philips standard
- **Kaseikyo** - Panasonic protocol
- **RAW** - Any custom timing sequence

**File formats**:
```
# Parsed format (human readable)
type: parsed
protocol: NEC
address: 00 00 00 00
command: 00 00 00 00
#

# Raw format (timing data)
type: raw
frequency: 38000
data: 9000 4500 560 1690 560 1690 560 ...
#
```

**File locations**:
- SD Card: `/sd_files/infrared/`
- LittleFS: `/BruceIR/`

**Pre-loaded examples**:
- TVs (power, volume, channels)
- ACs (temperature, modes)
- Gaming Consoles
- Universal remotes

---

### 3. **IR Read**
**What it does**: Captures IR signals from real remotes

**Capabilities**:
- Reads and decodes IR signals
- Auto-detects protocol
- Saves to SD/LittleFS
- Quick remote learning modes:
  - TV mode
  - AC mode  
  - Sound system mode

**Decoded information**:
- Protocol type
- Address (device ID)
- Command code
- Frequency (usually 38kHz)
- Raw timing data

**Use cases**:
- Clone your real remote
- Learn neighbor's TV codes 😏
- Reverse engineer protocols
- Create custom command databases

**Code**: `src/modules/ir/ir_read.cpp`

---

### 4. **IR Jammer**
**What it does**: Disrupts IR communications

**Jamming modes**:
1. **BASIC** - Simple interference pattern
2. **ENHANCED BASIC** - Adjustable mark/space timing
3. **SWEEP** - Sweeps through timing patterns
4. **RANDOM** - Random noise generation
5. **EMPTY** - Carrier wave only

**Adjustable parameters**:
- **Frequency**: 30-56 kHz (covers all common IR freqs)
  - 30kHz, 33kHz, 36kHz, **38kHz** (most common), 40kHz, 42kHz, 56kHz
- **Mark timing**: How long pulses are ON
- **Space timing**: How long pulses are OFF
- **Duty cycle**: ON/OFF ratio
- **Jam density**: How often to send interference
- **Sweep speed**: How fast to change patterns

**Effects**:
- Prevents TV remotes from working
- Blocks IR motion sensors
- Disrupts IR data transmission
- Confuses IR-based security systems

**Code**: `src/modules/ir/ir_jammer.cpp` (741 lines of sophisticated jamming!)

**Statistics shown**:
- Jam count (signals sent)
- Runtime
- Current frequency
- Current mode

---

## 🔧 Technical Details

### Hardware Requirements
- **IR LED** - For transmission (940nm wavelength)
- **IR Receiver** - For reading signals (38kHz typically)
- Configurable GPIO pins for TX/RX

### Pin Configuration
Access via: `IR Menu > Config`
- **IR TX Pin** - Where IR LED is connected
- **IR RX Pin** - Where IR receiver is connected
- **IR TX Repeats** - How many times to repeat each command

### Frequencies Supported
Most IR remotes use **38kHz** carrier frequency, but Bruce supports:
- 30kHz - Some Panasonic
- 33kHz - Some RCA
- 36kHz - Some NEC variants
- **38kHz** - Industry standard (80% of devices)
- 40kHz - Some Sony
- 42kHz - Rare
- 56kHz - Some specialty devices

---

## 📂 File Structure

### IR Files on SD Card
```
/sd_files/infrared/
├── TVs/
│   ├── Samsung.ir
│   ├── LG.ir
│   ├── Sony.ir
│   └── ...
├── ACs/
│   ├── Generic_AC.ir
│   ├── LG_AC.ir
│   └── ...
├── Consoles/
│   └── ...
└── Universal_TV_Remotes/
```

### Creating Custom IR Files

**Example - Parsed format**:
```
Filetype: IR signals file
Version: 1
#
name: Power
type: parsed
protocol: NEC
address: 04 00 00 00
command: 08 00 00 00
#
name: Vol_Up
type: parsed
protocol: NEC
address: 04 00 00 00
command: 02 00 00 00
#
```

**Example - Raw format**:
```
Filetype: IR signals file
Version: 1
#
name: Power
type: raw
frequency: 38000
duty_cycle: 0.33
data: 9000 4500 560 1690 560 560 560 1690 560 560 560 560 560 560 560 560
#
```

---

## 🎮 Cool Things You Can Do

### 1. **Universal TV Killer**
- Walk into any electronics store
- Run TV-B-Gone
- Watch every TV turn off
- Walk away casually 😎

### 2. **Clone Any Remote**
```
1. IR Menu > IR Read
2. Point remote at Bruce
3. Press remote button
4. Save to file
5. IR Menu > Custom IR > Send saved file
```

### 3. **Create Custom Attacks**
Combine multiple commands in one file:
```
# Turn on TV, max volume, change to weird channel
name: Chaos
type: parsed
protocol: Samsung32
address: 07 00 00 00
command: 02 00 00 00  # Power
#
name: Max_Vol
type: parsed  
protocol: Samsung32
address: 07 00 00 00
command: 07 00 00 00  # Volume Up (repeated)
#
```

### 4. **IR Fuzzing**
- Capture a working command
- Modify address/command bytes
- Try variations to find hidden functions
- Discover manufacturer modes

### 5. **Jam Security Cameras**
Many cheap IR cameras use IR for night vision:
```
IR Menu > IR Jammer
Set frequency to 38kHz
Mode: RANDOM or SWEEP
Duration: As long as needed
```

### 6. **AC Controller**
Pre-summer prank:
```
1. Learn coworker's office AC remote
2. Save commands
3. Periodically change their temperature
4. Watch them think AC is broken
```

---

## 🚀 Ideas for Enhancement

### Easy Additions:

#### 1. **IR Replay Buffer**
Store last 10 captured signals in RAM for quick replay
```cpp
std::vector<IRCode> replayBuffer;
// Add "Quick Replay" option to IR Read menu
```

#### 2. **IR Macro System**
Send multiple commands in sequence with delays
```
# Macro example
name: TV_Setup
commands:
  - Power (wait 2s)
  - Input_HDMI1 (wait 1s)
  - Volume_20 (wait 0.5s)
```

#### 3. **Brute Force Mode**
Try all possible command codes for a given protocol/address
```cpp
// NEC protocol: Try commands 0x00 to 0xFF
for (int cmd = 0; cmd <= 0xFF; cmd++) {
    sendNECCommand(knownAddress, String(cmd, HEX));
    delay(500);
}
```

#### 4. **IR Scanner**
Scan for active IR transmitters nearby
```cpp
// Listen for IR activity and decode
// Show frequency, strength, protocol
// Like WiFi scanning but for IR
```

#### 5. **Scheduled IR Commands**
```cpp
// Set timer to send command
sendIRAt(hour, minute, irCode);
// Prank: Turn off TV at specific time
```

#### 6. **IR Database Downloader**
```cpp
// Download IR codes from online databases
// IRDB: https://github.com/probonopd/irdb
// Flipper IRDB: flipper-zero-infrared
```

#### 7. **Smart Jammer**
```cpp
// Detect what protocol is being used
// Jam only that specific protocol
// More effective, less obvious
```

#### 8. **IR to RF Bridge**
```cpp
// Receive IR signal
// Retransmit via RF (longer range)
// Control devices from distance
```

### Advanced Ideas:

#### 9. **Protocol Fuzzer**
```cpp
void fuzzProtocol(String protocol, String address) {
    // Try every possible command
    // Try timing variations
    // Try bit flips
    // Find hidden/debug commands
}
```

#### 10. **IR Sniffer/Logger**
```cpp
// Continuously log all IR traffic
// Save to SD with timestamps
// Analyze patterns
// Detect repeated codes
```

#### 11. **Multi-Device Sync**
```cpp
// Control multiple devices simultaneously
// Send same command to different protocols
// Turn off EVERYTHING in a room
```

#### 12. **IR-over-WiFi**
```cpp
// Create web interface
// Control IR from phone
// Remote IR blaster over network
```

---

## 🔍 Protocol Deep Dive

### NEC Protocol (Most Common)
```
Structure: [9ms LEAD] [4.5ms SPACE] [ADDRESS (8 bits)] [~ADDRESS] [COMMAND (8 bits)] [~COMMAND]
Frequency: 38kHz
Bit encoding: 
  - 0: 560µs pulse + 560µs space
  - 1: 560µs pulse + 1690µs space
```

### Samsung32 Protocol
```
Structure: [4.5ms LEAD] [4.5ms SPACE] [32-bit data]
Frequency: 38kHz
Similar to NEC but 32-bit data instead of address+command
```

### Sony/SIRC Protocol
```
Structure: [2.4ms LEAD] [12/15/20 bits of data]
Frequency: 40kHz (different from most!)
Repeats: Always sent 3 times
```

### RC5 Protocol (Philips)
```
Structure: Manchester encoding, 14 bits total
No carrier burst for '0', carrier burst for '1'
Frequency: 36kHz
```

---

## 📊 File Analysis

### TV-B-Gone Database
**WORLD_IR_CODES.h** contains:
- **200+ TV power codes**
- NA (North America/Asia) codes: ~100
- EU (Europe/Middle East) codes: ~100
- Each code is compressed using bitpacking
- Total file size: 8,524 lines!

**Famous codes include**:
- Sony (multiple variants)
- Samsung (all generations)
- LG (various models)
- Vizio, TCL, Hisense
- Obscure Chinese brands

### IR Jammer Sophistication
The jammer has **5 different algorithms**:

1. **BASIC**: Simple repeating pattern
2. **ENHANCED_BASIC**: Configurable timing
3. **SWEEP**: Gradually changes timing (most effective)
4. **RANDOM**: Random noise (hard to filter)
5. **EMPTY**: Just carrier wave (baseline jamming)

**Why so many modes?**
Different IR receivers filter noise differently. Multiple modes ensure something works!

---

## ⚠️ Legal & Ethical Considerations

### Legal Uses ✅
- Learning your own remotes
- Reverse engineering your devices
- Educational purposes
- Security research on your own equipment
- Pranking friends (with permission)

### Questionable Uses ⚠️
- Turning off TVs in public (minor disruption)
- Testing jamming on your own devices
- Proof-of-concept demos

### Illegal/Unethical ❌
- Jamming security systems
- Disrupting businesses
- Interfering with emergency equipment
- Accessing unauthorized devices
- Causing harm or panic

**Remember**: Just because you CAN turn off every TV in Best Buy doesn't mean you SHOULD! 😅

---

## 🛠️ Hardware Mods

### Better IR LED
Replace default 940nm IR LED with:
- **High-power IR LED** (100mW+)
- **IR LED array** (multiple LEDs in parallel)
- **IR laser diode** (longer range, focused beam)

### Better Range
- Add **IR LED driver circuit** (transistor amplifier)
- Use **lens/reflector** to focus beam
- Increase **voltage** (carefully! Don't burn LED)

### Reception Improvements
- Use **shielded IR receiver** (reduce noise)
- Add **IR bandpass filter** (38kHz)
- Multiple receivers for **omnidirectional** reception

---

## 📚 Resources

### IR Databases
- **IRDB**: https://github.com/probonopd/irdb (15,000+ devices)
- **Flipper IRDB**: https://github.com/Lucaslhm/Flipper-IRDB
- **lirc.org**: Linux Infrared Remote Control database

### Documentation
- **IRremoteESP8266**: Library Bruce uses
- **IR Protocol Guide**: https://www.sbprojects.net/knowledge/ir/index.php
- **NEC Protocol**: Most detailed documentation available

### Tools
- **IrScrutinizer**: Desktop IR analyzer
- **AnalysIR**: Commercial IR analysis tool
- **Flipper Zero**: IR comparison device

---

## 🎓 Learning Exercise

**Build your own IR code**:
```cpp
// 1. Capture a signal with IR Read
// 2. Analyze the timing
// 3. Recreate in code:

void sendCustomCommand() {
    IRsend irsend(bruceConfig.irTx);
    
    // Example: NEC protocol
    uint16_t rawData[67] = {
        9000, 4500,  // Leader
        560, 1690,   // Bit 1
        560, 560,    // Bit 0
        // ... continue for 32 bits
    };
    
    irsend.sendRaw(rawData, 67, 38);  // 38kHz
}
```

---

## 🎯 Quick Reference

| Feature | File | Description |
|---------|------|-------------|
| TV-B-Gone | `TV-B-Gone.cpp` | Universal TV off |
| Custom IR | `custom_ir.cpp` | Send saved IR files |
| IR Read | `ir_read.cpp` | Capture IR signals |
| IR Jammer | `ir_jammer.cpp` | Disrupt IR comms |
| Protocols | `custom_ir.h` | NEC, Sony, Samsung, RC5/6 |
| Codes DB | `WORLD_IR_CODES.h` | 200+ TV power codes |

---

## 💡 Pro Tips

1. **Best jamming frequency**: 38kHz (most devices)
2. **Best jamming mode**: SWEEP (changes patterns)
3. **TV-B-Gone range**: ~10-15 meters with stock LED
4. **Quick turn off**: Press SEL to pause/stop
5. **File format**: Use "parsed" for readability, "raw" for accuracy
6. **Protocol detection**: IR Read auto-detects most protocols
7. **Repeats**: Most remotes repeat 2-3 times for reliability
8. **AC commands**: Often require specific timing/repeats

---

**Have fun exploring IR! It's one of the most practical modules in Bruce.** 📡✨

Let me know if you want to implement any of these enhancements or need help with specific IR protocols!

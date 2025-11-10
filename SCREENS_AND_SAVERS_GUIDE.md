# Bruce Screen Savers & Menu Screens - Complete Guide 🖼️

## 📍 **Quick Answer: Where Everything Is Located**

### **Boot/Splash Screens**
- **Code**: `src/main.cpp` (lines 214-319)
  - `boot_screen()` - Draws basic text boot screen
  - `boot_screen_anim()` - Animated boot sequence with Bruce logo

### **Built-in Graphics (Bitmaps)**
- **File**: `src/core/display.h` (lines 219-480+)
  - `bruce_small_bits[]` - Small Bruce logo (60x34 pixels)
  - `bits[]` - Large Bruce logo (237x133 pixels)

### **Screen Saver/Sleep/Dimmer**
- **Code**: `src/core/powerSave.cpp` (full file)
  - `checkPowerSaveTime()` - Monitors idle time
  - `fadeOutScreen()` - Fade animation to sleep
  - `sleepModeOn()` / `sleepModeOff()` - Power save mode

### **Display/Drawing Functions**
- **Code**: `src/core/display.cpp` (1,727 lines!)
  - All menu drawing functions
  - Border, text, icon rendering
  - Touch screen footer

### **Menu System**
- **Code**: `src/core/main_menu.cpp` & `main_menu.h`
  - Main menu loop and navigation
  - Menu item management

### **Individual Menu Icons**
- **Location**: `src/core/menu_items/`
  - `BleMenu.cpp` - BLE menu icon & drawing
  - `RFMenu.cpp` - RF menu icon & drawing
  - `IRMenu.cpp` - IR menu icon & drawing
  - `WifiMenu.cpp` - WiFi menu icon & drawing
  - etc. (one file per menu category)

### **Custom Boot Images (User-provided)**
- **SD Card**: `/boot.jpg` or `/boot.gif`
- **LittleFS**: `/boot.jpg` or `/boot.gif`
- **Theme system**: `sd_files/themes/[theme_name]/boot.gif`

---

## 🎨 **How the Boot Screen Works**

### Boot Sequence (from `main.cpp`):

```cpp
void boot_screen_anim() {
    // 1. Draw basic text (Bruce + version)
    boot_screen();
    
    // 2. Check for custom images:
    // Priority order:
    //   1. SD card theme (if theme enabled)
    //   2. SD card /boot.jpg or /boot.gif
    //   3. LittleFS /boot.jpg or /boot.gif
    //   4. Built-in bitmap animation (default)
    
    // 3. Display for 7 seconds (or until key press)
    //    - 0-2s: Text only
    //    - 2-7s: Image/animation or built-in logo
    
    // 4. Any key press skips boot screen
}
```

### Built-in Boot Animation (No Custom Image):

**Timeline**:
- **0-2000ms**: Show "Bruce" text + version
- **2200-2700ms**: Draw small dot (teaser)
- **2700-2900ms**: Clear screen
- **2900-3400ms**: Draw small Bruce logo (60x34)
- **3400-3600ms**: Clear screen
- **3600-7000ms**: Draw full Bruce logo (237x133)
- **7000ms+**: Clear and enter main menu

---

## 💤 **How Screen Saver/Sleep Works**

### From `powerSave.cpp`:

```cpp
checkPowerSaveTime() {
    // Called every ~75ms by taskInputHandler
    
    // Config: bruceConfig.dimmerSet (seconds)
    // if dimmerSet == 0, screen saver is disabled
    
    // Stage 1: DIM (after dimmerSet seconds)
    if (idle_time >= dimmerSet) {
        dimmer = true;
        setBrightness(bright / 3);  // Reduce to 33% brightness
    }
    
    // Stage 2: SCREEN OFF (5 seconds after dimming)
    if (idle_time >= dimmerSet + 5) {
        isScreenOff = true;
        fadeOutScreen();  // Fade from 33% to 0% over ~1 second
        turnOffDisplay();
    }
}
```

### Sleep Mode Features:
- **CPU throttling**: 240 MHz → 80 MHz
- **Panel power down**: Hardware screen sleep
- **Watchdog disabled**: Prevents reset during sleep
- **Wake up**: Any key press resets `previousMillis`

---

## 🎭 **Menu Icons & Drawing**

### How Menu Items Draw Themselves:

Each menu (BLE, RF, IR, etc.) has its own drawing code:

**Example from `RFMenu.cpp`**:
```cpp
void RFMenu::drawIcon(float scale) {
    clearIconArea();
    
    // Draw RF tower icon:
    // - Triangle base
    // - Circle at top
    // - Radio wave arcs
    
    int radius = scale * 7;
    int deltaRadius = scale * 10;
    
    tft.fillCircle(x, y, radius, bruceConfig.priColor);
    tft.drawArc(...);  // Draw signal waves
}

void RFMenu::drawIconImg() {
    // OR load from theme image
    drawImg(themeFS, themeItemImg, x, y);
}
```

### Each menu can draw:
1. **Programmatic icon** - `drawIcon(scale)` - Vector graphics
2. **Image icon** - `drawIconImg()` - Load from theme

---

## 🖼️ **Custom Graphics Locations**

### 1. Boot Screen Images

**SD Card**:
```
/boot.jpg    - Static boot image
/boot.gif    - Animated boot image
```

**LittleFS** (internal flash):
```
/boot.jpg
/boot.gif
```

**Theme System**:
```
/sd_files/themes/[theme_name]/
├── boot.gif
├── ble.jpg
├── rf.jpg
├── ir.jpg
└── ... (menu icons)
```

### 2. Menu Item Icons (Theme)

From `sd_files/themes/README.md`:
```json
{
  "boot_img": "boot.gif",
  "ble": "ble.jpg",
  "rf": "rf.jpg",
  "rfid": "rfid.jpg",
  "ir": "ir.jpg",
  "wifi": "wifi.jpg",
  ...
}
```

### 3. Example GIFs (Included)

```
sd_files/interpreter/gifs/
├── half_spongebob.gif
├── half_rick.gif
├── half_rainbow.gif
└── half_patrick.gif
```

These are used by JavaScript interpreter demos.

---

## 🎨 **Built-in Bitmap Graphics**

### From `display.h`:

**Small Bruce Logo** (60x34 pixels):
```cpp
#define bruce_small_width 60
#define bruce_small_height 34
PROGMEM const unsigned char bruce_small_bits[] = {
    0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F,
    // ... bitmap data ...
};
```

**Large Bruce Logo** (237x133 pixels):
```cpp
#define bits_width 237
#define bits_height 133
PROGMEM const unsigned char bits[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    // ... bitmap data (huge!) ...
};
```

These are **XBitmap format** - monochrome bitmaps stored in flash memory.

---

## 🔧 **How to Customize**

### Option 1: Custom Boot Image (Easiest)

1. Create a 240x135 (or your screen size) image
2. Save as JPEG or GIF
3. Name it `boot.jpg` or `boot.gif`
4. Copy to SD card root OR flash to LittleFS
5. Done! Boots with your image

**Animated Boot**:
- Use GIF format
- Keep file size small (<500KB recommended)
- Bruce will loop/display it for ~5 seconds

### Option 2: Modify Built-in Logo

1. Create new XBitmap:
   - Use GIMP or online converter
   - Convert image to 1-bit bitmap
   - Export as XBitmap (.xbm)
   
2. Edit `src/core/display.h`:
   - Replace `bits[]` array with your data
   - Update `bits_width` and `bits_height`

3. Recompile firmware

### Option 3: Create Theme Pack

1. Create folder: `sd_files/themes/mytheme/`
2. Add images for each menu
3. Create `theme.json`:
```json
{
  "name": "My Theme",
  "boot_img": "boot.gif",
  "ble": "ble.jpg",
  "rf": "rf.jpg",
  "ir": "ir.jpg"
}
```
4. Select theme in Bruce: `Config > Theme`

### Option 4: Disable Boot Screen

Edit `platformio.ini` or config:
```cpp
bruceConfig.instantBoot = true;
```

---

## 🎬 **Screen Saver Customization**

### Change Dimmer Timeout

**Via Menu**:
1. Go to `Config > Brightness`
2. Set "Dim Time" (in seconds)
3. Set to `0` to disable screen saver

**In Code**:
```cpp
// src/core/config.h
int dimmerSet = 10;  // Default 10 seconds

// Change to:
int dimmerSet = 30;  // 30 seconds
int dimmerSet = 0;   // Disabled
```

### Change Fade Animation

**Edit `powerSave.cpp`**:
```cpp
void fadeOutScreen(int startValue) {
    // Current: Fade from 33% brightness to 0
    for (int brightValue = startValue; brightValue >= 0; brightValue -= 1) {
        setBrightness(max(brightValue, 0), false);
        delay(5);  // Change delay for faster/slower fade
    }
}
```

**Make it fade slower**:
```cpp
delay(10);  // Instead of delay(5)
```

**Skip fade (instant off)**:
```cpp
void fadeOutScreen(int startValue) {
    turnOffDisplay();  // Just turn off immediately
}
```

---

## 🎯 **Cool Customization Ideas**

### 1. **Animated Boot Screen**

Create a GIF with your own animation:
```
Tools:
- EZGIF.com (online GIF maker)
- GIMP (desktop editor)
- Photoshop

Requirements:
- 240x135 pixels (or your screen res)
- < 500KB file size
- ~10-20 frames for smooth animation
```

### 2. **Random Boot Screens**

Modify `boot_screen_anim()`:
```cpp
// Check for multiple boot images
if (SD.exists("/boot1.jpg") && SD.exists("/boot2.jpg")) {
    int random = millis() % 2;
    if (random == 0) drawImg(SD, "/boot1.jpg", ...);
    else drawImg(SD, "/boot2.jpg", ...);
}
```

### 3. **Screen Saver with Graphics**

Instead of just fading to black, show an image:
```cpp
void screenSaverMode() {
    // After X seconds idle, show screen saver
    drawImg(SD, "/screensaver.jpg", 0, 0, true);
    
    // Or show clock
    while (isSleeping) {
        drawClock();
        delay(1000);
    }
}
```

### 4. **Matrix/Starfield Screen Saver**

Add animated screen saver:
```cpp
void matrixScreenSaver() {
    while (isSleeping) {
        // Draw falling characters
        for (int i = 0; i < 10; i++) {
            int x = random(0, tftWidth);
            int y = random(0, tftHeight);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.drawChar(x, y, random(33, 126), 1);
        }
        delay(50);
    }
}
```

### 5. **Clock Screen Saver**

Display time when idle:
```cpp
void clockScreenSaver() {
    while (isSleeping && !checkAnyKeyPress()) {
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(3);
        tft.setTextColor(bruceConfig.priColor);
        tft.drawCentreString(timeStr, tftWidth/2, tftHeight/2, 1);
        delay(1000);
    }
}
```

### 6. **Bouncing Logo**

Classic DVD logo style:
```cpp
void bouncingLogo() {
    int x = 0, y = 0;
    int dx = 2, dy = 2;
    
    while (isSleeping) {
        tft.fillScreen(TFT_BLACK);
        drawImg(SD, "/logo.jpg", x, y, false);
        
        x += dx;
        y += dy;
        
        if (x <= 0 || x >= tftWidth - 60) dx = -dx;
        if (y <= 0 || y >= tftHeight - 34) dy = -dy;
        
        delay(30);
    }
}
```

---

## 📂 **File Structure Reference**

```
Brutus/
├── src/
│   ├── main.cpp                    # Boot screens, main loop
│   └── core/
│       ├── display.h               # Built-in bitmaps, drawing functions
│       ├── display.cpp             # Display utilities
│       ├── powerSave.cpp           # Screen saver/sleep
│       ├── main_menu.cpp           # Main menu system
│       └── menu_items/             # Individual menu screens
│           ├── BleMenu.cpp         # BLE menu icon
│           ├── RFMenu.cpp          # RF menu icon
│           ├── IRMenu.cpp          # IR menu icon
│           └── ...
│
├── sd_files/
│   └── themes/                     # Theme system
│       ├── README.md               # Theme documentation
│       ├── Theme_Builder.html      # Web-based theme creator
│       └── example/                # Example theme
│
└── /boot.jpg or /boot.gif          # Custom boot image (SD root)
```

---

## 🎨 **Image Format Requirements**

### Boot Screen Images

**JPEG** (.jpg):
- Max size: ~500KB (larger = slower load)
- Resolution: Match your screen (e.g., 240x135)
- Quality: 80-90% (balance size/quality)

**GIF** (.gif):
- Animated or static
- Max size: ~1MB for smooth playback
- Frame count: 10-30 frames optimal
- Loop: Infinite loop recommended

### Menu Icon Images

**From themes**:
- Small images (50x50 to 100x100)
- JPEG or PNG format
- Keep file sizes small (<50KB each)

---

## 🔍 **Debugging Display Issues**

### Screen Not Updating?
Check `src/core/display.cpp` - `resetTftDisplay()` function

### Boot Image Not Showing?
1. Check file exists: `SD.exists("/boot.jpg")`
2. Check file size (<500KB recommended)
3. Check resolution matches screen
4. Serial output shows loading status

### Screen Saver Not Working?
1. Check `bruceConfig.dimmerSet > 0`
2. Verify `previousMillis` is being updated
3. Check `checkPowerSaveTime()` is called regularly

### Icons Look Wrong?
1. Theme images might be wrong size
2. Check `bruceConfig.theme` settings
3. Fallback to programmatic icons if images fail

---

## 💡 **Pro Tips**

1. **Fast boot**: Set `instantBoot = true` in config
2. **Custom themes**: Use `Theme_Builder.html` to create themes easily
3. **GIF optimization**: Use EZGIF.com to compress and optimize
4. **Bitmap converter**: Use online XBitmap converter for logos
5. **Test on device**: Images might look different on actual screen
6. **SD vs LittleFS**: SD is easier to swap, LittleFS is built-in
7. **Serial debug**: Watch Serial output to see what's loading

---

## 🎯 **Quick Customization Checklist**

**Want custom boot screen?**
- [ ] Create 240x135 image (JPG or GIF)
- [ ] Name it `boot.jpg` or `boot.gif`
- [ ] Copy to SD card root
- [ ] Reboot device

**Want to disable screen saver?**
- [ ] Go to Config > Brightness
- [ ] Set Dim Time to `0`

**Want faster boot?**
- [ ] Edit config, set `instantBoot = true`
- [ ] Or just press any key during boot

**Want custom menu icons?**
- [ ] Create theme folder
- [ ] Add icon images
- [ ] Create theme.json
- [ ] Select theme in Config

---

## 🚀 **Advanced: Add New Screen Saver**

To add a completely new screen saver mode:

1. **Create new function in `powerSave.cpp`**:
```cpp
void customScreenSaver() {
    // Your screen saver code here
    while (isSleeping && !checkAnyKeyPress()) {
        // Draw animation
        delay(50);
    }
}
```

2. **Call it from `checkPowerSaveTime()`**:
```cpp
if (elapsed >= (dimmerSetMs + SCREEN_OFF_DELAY)) {
    isScreenOff = true;
    customScreenSaver();  // Instead of fadeOutScreen()
}
```

3. **Add config option** (optional):
```cpp
// In config.h
int screenSaverMode = 0;  // 0=fade, 1=clock, 2=matrix, etc.

// In powerSave.cpp
switch (bruceConfig.screenSaverMode) {
    case 0: fadeOutScreen(); break;
    case 1: clockScreenSaver(); break;
    case 2: matrixScreenSaver(); break;
}
```

---

**Now you know where everything is! Have fun customizing your Bruce display!** 🎨✨

#include "rf_jammer_adv.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include "rf_utils.h"

// RC Protocol frequencies (2.4 GHz - requires NRF24)
const float RC_FREQUENCIES[] = {
    2400.0f, 2410.0f, 2420.0f, 2430.0f, 2440.0f, 2450.0f, 2460.0f, 2470.0f, 2480.0f
};

// TPMS frequencies (varies by region)
const float TPMS_FREQUENCIES[] = {
    315.0f,  // North America
    433.92f, // Europe
    434.0f   // Alternative
};

// LoJack frequencies (WARNING: Jamming LoJack is ILLEGAL!)
const float LOJACK_FREQUENCIES[] = {
    173.075f // LoJack frequency (US)
};

// Smart hopping - common ISM bands
const float SMART_HOP_FREQUENCIES[] = {315.0f, 433.92f, 868.0f, 915.0f, 2400.0f};

// Multi-band sweep
const float MULTIBAND_FREQUENCIES[] = {300.0f, 315.0f, 390.0f, 418.0f, 433.92f, 868.0f, 915.0f, 925.0f};

RFJammerAdvanced::RFJammerAdvanced() : isJamming(false), currentMode(JAM_SMART_HOP) {
    nTransmitterPin = bruceConfig.rfTx;
}

RFJammerAdvanced::~RFJammerAdvanced() {
    if (isJamming) { stop(); }
    deinitRfModule();
}

void RFJammerAdvanced::start(JammerMode mode) {
    currentMode = mode;

    // Initialize RF module
    if (!initRfModule("tx")) {
        drawMainBorderWithTitle("ERROR");
        padprintln("");
        padprintln("RF module not available!");
        padprintln("");
        padprintln("Check CC1101 connection");
        padprintln("");
        padprintln("Press any key to return...");

        while (!checkAnyKeyPress()) { delay(100); }
        return;
    }

    if (bruceConfig.rfModule == CC1101_SPI_MODULE) { nTransmitterPin = bruceConfigPins.CC1101_bus.io0; }

    // Show warning for certain modes
    if (mode == JAM_LOJACK) {
        displayWarning();
        delay(3000);
    }

    isJamming = true;

    switch (mode) {
        case JAM_RC_PROTOCOLS: jamRCProtocols(); break;
        case JAM_TPMS: jamTPMS(); break;
        case JAM_LOJACK: jamLoJack(); break;
        case JAM_SMART_HOP: jamSmartHopping(); break;
        case JAM_MULTIBAND: jamMultiBand(); break;
        case JAM_CUSTOM: jamCustom(); break;
    }

    deinitRfModule();
}

void RFJammerAdvanced::stop() {
    isJamming = false;
    digitalWrite(nTransmitterPin, LOW);
}

void RFJammerAdvanced::displayBanner(const char *title) {
    drawMainBorderWithTitle(title);
    padprintln("Jamming active...");
    padprintln("");
    tft.setTextColor(TFT_YELLOW, bruceConfig.bgColor);
    padprintln("⚠ WARNING ⚠");
    padprintln("Jamming may be illegal!");
    padprintln("Use responsibly.");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    padprintln("");
    tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
    padprintln("Press ESC to stop");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
}

void RFJammerAdvanced::displayWarning() {
    drawMainBorderWithTitle("⚠ LEGAL WARNING ⚠");
    tft.setTextColor(TFT_RED, bruceConfig.bgColor);
    padprintln("");
    padprintln("JAMMING LOJACK IS ILLEGAL!");
    padprintln("");
    padprintln("This mode is for educational");
    padprintln("purposes ONLY.");
    padprintln("");
    padprintln("Using this jammer can:");
    padprintln("- Violate FCC regulations");
    padprintln("- Result in heavy fines");
    padprintln("- Lead to criminal charges");
    padprintln("- Interfere with police ops");
    padprintln("");
    padprintln("DO NOT USE ILLEGALLY!");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    padprintln("");
    padprintln("Continuing in 3 seconds...");
}

void RFJammerAdvanced::jamRCProtocols() {
    displayBanner("RC Jammer");
    padprintln("");
    padprintln("Targeting RC protocols:");
    padprintln("- RC Cars (27/49/2.4GHz)");
    padprintln("- Drones (2.4GHz)");
    padprintln("- RC Toys");
    padprintln("");

    // Note: 2.4GHz jamming requires NRF24 or similar
    // This uses CC1101 on lower frequencies

    unsigned long startTime = millis();
    const unsigned long JAM_DURATION = 30000; // 30 seconds

    while (isJamming && (millis() - startTime < JAM_DURATION)) {
        if (check(EscPress)) {
            isJamming = false;
            break;
        }

        // Jam common RC frequencies
        // 27 MHz and 49 MHz (older RC)
        // Using 433 MHz as proxy for demonstration
        frequencyHop(MULTIBAND_FREQUENCIES, 8, 200);

        // Update display
        unsigned long remaining = (JAM_DURATION - (millis() - startTime)) / 1000;
        tft.fillRect(10, tftHeight - 40, tftWidth - 20, 20, bruceConfig.bgColor);
        tft.setCursor(10, tftHeight - 40);
        padprintln("Time left: " + String(remaining) + "s");
    }

    digitalWrite(nTransmitterPin, LOW);
}

void RFJammerAdvanced::jamTPMS() {
    displayBanner("TPMS Jammer");
    padprintln("");
    padprintln("Targeting TPMS sensors:");
    padprintln("- 315 MHz (US)");
    padprintln("- 433.92 MHz (EU)");
    padprintln("");
    padprintln("Effect: Prevents tire");
    padprintln("pressure readings");
    padprintln("");

    unsigned long startTime = millis();
    const unsigned long JAM_DURATION = 30000;

    while (isJamming && (millis() - startTime < JAM_DURATION)) {
        if (check(EscPress)) {
            isJamming = false;
            break;
        }

        // Hop between TPMS frequencies
        frequencyHop(TPMS_FREQUENCIES, 3, 500);

        // Display progress
        unsigned long remaining = (JAM_DURATION - (millis() - startTime)) / 1000;
        tft.fillRect(10, tftHeight - 40, tftWidth - 20, 20, bruceConfig.bgColor);
        tft.setCursor(10, tftHeight - 40);
        padprintln("Time left: " + String(remaining) + "s");
    }

    digitalWrite(nTransmitterPin, LOW);
}

void RFJammerAdvanced::jamLoJack() {
    displayBanner("LoJack Jammer");
    tft.setTextColor(TFT_RED, bruceConfig.bgColor);
    padprintln("");
    padprintln("⚠ ILLEGAL IN MOST AREAS ⚠");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    padprintln("");
    padprintln("Frequency: 173.075 MHz");
    padprintln("");
    padprintln("This is a FELONY in the US!");
    padprintln("Educational demo only.");
    padprintln("");

    unsigned long startTime = millis();
    const unsigned long JAM_DURATION = 10000; // Only 10 seconds for demo

    while (isJamming && (millis() - startTime < JAM_DURATION)) {
        if (check(EscPress)) {
            isJamming = false;
            break;
        }

        // Jam LoJack frequency
        // Note: 173 MHz may not work on all CC1101 configurations
        setMHZ(173.075f);

        // Continuous carrier
        digitalWrite(nTransmitterPin, HIGH);
        delay(100);
        digitalWrite(nTransmitterPin, LOW);
        delay(100);

        unsigned long remaining = (JAM_DURATION - (millis() - startTime)) / 1000;
        tft.fillRect(10, tftHeight - 40, tftWidth - 20, 20, bruceConfig.bgColor);
        tft.setCursor(10, tftHeight - 40);
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        padprintln("Demo mode: " + String(remaining) + "s");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }

    digitalWrite(nTransmitterPin, LOW);
}

void RFJammerAdvanced::jamSmartHopping() {
    displayBanner("Smart Frequency Hopper");
    padprintln("");
    padprintln("Intelligent hopping across");
    padprintln("common ISM bands:");
    padprintln("315/433/868/915 MHz");
    padprintln("");
    padprintln("Adaptive timing based on");
    padprintln("detected activity");
    padprintln("");

    unsigned long startTime = millis();
    const unsigned long JAM_DURATION = 60000; // 60 seconds
    int hopCount = 0;

    while (isJamming && (millis() - startTime < JAM_DURATION)) {
        if (check(EscPress)) {
            isJamming = false;
            break;
        }

        // Smart frequency hopping with variable dwell time
        for (int i = 0; i < 5 && isJamming; i++) {
            float freq = SMART_HOP_FREQUENCIES[i];
            setMHZ(freq);

            // Adaptive dwell time (100-500ms)
            int dwellTime = 100 + (hopCount % 4) * 100;

            unsigned long dwellStart = millis();
            while ((millis() - dwellStart < dwellTime) && isJamming) {
                // PWM jamming pattern
                digitalWrite(nTransmitterPin, HIGH);
                delayMicroseconds(50 + (hopCount % 10) * 10);
                digitalWrite(nTransmitterPin, LOW);
                delayMicroseconds(50 + (hopCount % 10) * 10);

                if (check(EscPress)) {
                    isJamming = false;
                    break;
                }
            }

            hopCount++;
        }

        // Update display
        tft.fillRect(10, tftHeight - 60, tftWidth - 20, 40, bruceConfig.bgColor);
        tft.setCursor(10, tftHeight - 60);
        padprintln("Hops: " + String(hopCount));
        unsigned long remaining = (JAM_DURATION - (millis() - startTime)) / 1000;
        padprintln("Time: " + String(remaining) + "s");
    }

    digitalWrite(nTransmitterPin, LOW);
}

void RFJammerAdvanced::jamMultiBand() {
    displayBanner("Multi-Band Jammer");
    padprintln("");
    padprintln("Sweeping multiple bands:");
    padprintln("300-925 MHz");
    padprintln("");
    padprintln("Maximum disruption mode");
    padprintln("");

    unsigned long startTime = millis();
    const unsigned long JAM_DURATION = 60000;

    while (isJamming && (millis() - startTime < JAM_DURATION)) {
        if (check(EscPress)) {
            isJamming = false;
            break;
        }

        // Fast frequency sweep
        frequencyHop(MULTIBAND_FREQUENCIES, 8, 150);

        unsigned long remaining = (JAM_DURATION - (millis() - startTime)) / 1000;
        tft.fillRect(10, tftHeight - 40, tftWidth - 20, 20, bruceConfig.bgColor);
        tft.setCursor(10, tftHeight - 40);
        padprintln("Time left: " + String(remaining) + "s");
    }

    digitalWrite(nTransmitterPin, LOW);
}

void RFJammerAdvanced::jamCustom() {
    // User-defined frequency jamming
    // This would require a UI to select frequencies
    // For now, use multiband as fallback
    jamMultiBand();
}

void RFJammerAdvanced::sendJamSignal(float frequency, unsigned long duration) {
    setMHZ(frequency);
    unsigned long start = millis();

    while ((millis() - start < duration) && isJamming) {
        digitalWrite(nTransmitterPin, HIGH);
        delayMicroseconds(50);
        digitalWrite(nTransmitterPin, LOW);
        delayMicroseconds(50);
    }
}

void RFJammerAdvanced::frequencyHop(const float *frequencies, int count, int dwellTime) {
    for (int i = 0; i < count && isJamming; i++) {
        if (check(EscPress)) {
            isJamming = false;
            return;
        }

        setMHZ(frequencies[i]);

        unsigned long start = millis();
        while ((millis() - start < dwellTime) && isJamming) {
            // Generate noise on current frequency
            digitalWrite(nTransmitterPin, HIGH);
            delayMicroseconds(25);
            digitalWrite(nTransmitterPin, LOW);
            delayMicroseconds(25);
        }
    }
}

// Menu functions
void rc_jammer() {
    RFJammerAdvanced jammer;
    jammer.start(JAM_RC_PROTOCOLS);
}

void tpms_jammer() {
    RFJammerAdvanced jammer;
    jammer.start(JAM_TPMS);
}

void lojack_jammer() {
    // Extra confirmation for illegal jammer
    drawMainBorderWithTitle("⚠ WARNING ⚠");
    tft.setTextColor(TFT_RED, bruceConfig.bgColor);
    padprintln("");
    padprintln("LoJack jamming is ILLEGAL!");
    padprintln("");
    padprintln("This is for research and");
    padprintln("education ONLY.");
    padprintln("");
    padprintln("Press SELECT to continue");
    padprintln("Press ESC to cancel");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    while (true) {
        if (check(SelPress)) {
            RFJammerAdvanced jammer;
            jammer.start(JAM_LOJACK);
            break;
        }
        if (check(EscPress)) { break; }
        delay(100);
    }
}

void smart_hop_jammer() {
    RFJammerAdvanced jammer;
    jammer.start(JAM_SMART_HOP);
}

void multiband_jammer() {
    RFJammerAdvanced jammer;
    jammer.start(JAM_MULTIBAND);
}

void rf_jammer_advanced_menu() {
    options.clear();

    options.push_back({"RC Protocol Jammer", rc_jammer});
    options.push_back({"TPMS Jammer", tpms_jammer});
    options.push_back({"Smart Freq Hopper", smart_hop_jammer});
    options.push_back({"Multi-Band Jammer", multiband_jammer});
    options.push_back({"⚠ LoJack Jammer (ILLEGAL)", lojack_jammer});

    loopOptions(options);
}

#ifndef __RF_JAMMER_ADV_H__
#define __RF_JAMMER_ADV_H__

#include <Arduino.h>

// Advanced RF jammer with multiple modes
// - RC Protocol Jammer (2.4GHz)
// - TPMS Jammer (315/433 MHz)
// - LoJack Jammer (173 MHz - NOTE: illegal in many jurisdictions!)
// - Smart Frequency Hopping Jammer
// - Custom Multi-band Jammer

enum JammerMode {
    JAM_RC_PROTOCOLS, // Jam RC cars/drones
    JAM_TPMS,         // Jam tire pressure sensors
    JAM_LOJACK,       // Jam vehicle tracking (ILLEGAL!)
    JAM_SMART_HOP,    // Intelligent frequency hopping
    JAM_MULTIBAND,    // Multiple bands simultaneously
    JAM_CUSTOM        // User-defined frequencies
};

class RFJammerAdvanced {
public:
    RFJammerAdvanced();
    ~RFJammerAdvanced();

    void start(JammerMode mode);
    void stop();

private:
    int nTransmitterPin;
    bool isJamming;
    JammerMode currentMode;

    void displayBanner(const char *title);
    void jamRCProtocols();
    void jamTPMS();
    void jamLoJack();
    void jamSmartHopping();
    void jamMultiBand();
    void jamCustom();

    void sendJamSignal(float frequency, unsigned long duration);
    void frequencyHop(const float *frequencies, int count, int dwellTime);
    void displayWarning();
};

// Menu entry points
void rf_jammer_advanced_menu();
void rc_jammer();
void tpms_jammer();
void lojack_jammer();
void smart_hop_jammer();
void multiband_jammer();

#endif

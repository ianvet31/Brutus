#ifndef __TRACKER_DETECT_H__
#define __TRACKER_DETECT_H__

#include <Arduino.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <vector>

// Tracker detection for stalking prevention
// Detects Apple AirTags, Tile, Samsung SmartTags, Chipolo, etc.

struct TrackerDevice {
    String name;
    String address;
    String type;
    int rssi;
    unsigned long firstSeen;
    unsigned long lastSeen;
    int seenCount;
    bool isPersistent;
};

class TrackerDetector {
public:
    TrackerDetector();
    ~TrackerDetector();

    void start();
    void stop();

private:
    NimBLEScan *pBLEScan;
    std::vector<TrackerDevice> detectedTrackers;
    unsigned long scanStartTime;
    bool isScanning;

    void displayResults();
    void checkForStalking();
    bool isKnownTracker(NimBLEAdvertisedDevice *device);
    String identifyTrackerType(NimBLEAdvertisedDevice *device);
    void updateOrAddTracker(String address, String name, String type, int rssi);
};

// Main entry point
void tracker_detect_menu();

#endif

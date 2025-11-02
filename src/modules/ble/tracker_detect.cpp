#include "tracker_detect.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"

// Known tracker device identifiers
// Apple AirTag uses Apple's company ID (0x004C) and specific data patterns
// Tile uses company ID 0x0097
// Samsung SmartTag uses 0x0075
// Chipolo uses 0x0349

#define SCAN_TIME 30           // Scan for 30 seconds
#define PERSISTENT_THRESHOLD 3 // Seen 3+ times = potential stalking
#define TIME_THRESHOLD 300000  // 5 minutes in milliseconds

static std::vector<TrackerDevice> globalTrackers;

class TrackerScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice *advertisedDevice) {
        // Check if this is a known tracker type
        String address = advertisedDevice->getAddress().toString().c_str();
        String name = advertisedDevice->getName().c_str();
        int rssi = advertisedDevice->getRSSI();

        // Check manufacturer data for tracker identification
        if (advertisedDevice->haveManufacturerData()) {
            std::string mfgData = advertisedDevice->getManufacturerData();
            if (mfgData.length() >= 2) {
                uint16_t companyId = (uint8_t)mfgData[1] << 8 | (uint8_t)mfgData[0];
                String type = "Unknown";
                bool isTracker = false;

                // Apple devices (AirTag, AirPods, FindMy network)
                if (companyId == 0x004C) {
                    if (mfgData.length() >= 3) {
                        uint8_t appleType = (uint8_t)mfgData[2];
                        // AirTag and FindMy accessories use type 0x12 or 0x07
                        if (appleType == 0x12 || appleType == 0x07) {
                            type = "Apple AirTag/FindMy";
                            isTracker = true;
                        } else if (appleType == 0x01) {
                            type = "Apple Device";
                            isTracker = false; // iPhones, iPads - not trackers
                        }
                    }
                }
                // Tile trackers
                else if (companyId == 0x0097) {
                    type = "Tile Tracker";
                    isTracker = true;
                }
                // Samsung SmartTag
                else if (companyId == 0x0075) {
                    type = "Samsung SmartTag";
                    isTracker = true;
                }
                // Chipolo
                else if (companyId == 0x0349) {
                    type = "Chipolo Tracker";
                    isTracker = true;
                }

                if (isTracker) {
                    // Update or add to global tracker list
                    bool found = false;
                    unsigned long now = millis();

                    for (auto &tracker : globalTrackers) {
                        if (tracker.address == address) {
                            tracker.lastSeen = now;
                            tracker.seenCount++;
                            tracker.rssi = rssi;
                            if (!tracker.name.length() && name.length() > 0) { tracker.name = name; }
                            found = true;
                            break;
                        }
                    }

                    if (!found && globalTrackers.size() < 50) {
                        TrackerDevice newTracker;
                        newTracker.address = address;
                        newTracker.name = name.length() > 0 ? name : "Unknown";
                        newTracker.type = type;
                        newTracker.rssi = rssi;
                        newTracker.firstSeen = now;
                        newTracker.lastSeen = now;
                        newTracker.seenCount = 1;
                        newTracker.isPersistent = false;
                        globalTrackers.push_back(newTracker);
                    }
                }
            }
        }
    }
};

TrackerDetector::TrackerDetector() : pBLEScan(nullptr), isScanning(false) { scanStartTime = millis(); }

TrackerDetector::~TrackerDetector() {
    if (isScanning) { stop(); }
}

void TrackerDetector::start() {
    drawMainBorderWithTitle("Tracker Detection");
    padprintln("Scanning for BLE trackers...");
    padprintln("");
    padprintln("Looking for:");
    padprintln("- Apple AirTags");
    padprintln("- Tile Trackers");
    padprintln("- Samsung SmartTags");
    padprintln("- Chipolo Devices");
    padprintln("");
    tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
    padprintln("Scanning for " + String(SCAN_TIME) + " seconds...");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    delay(1000);

    // Clear previous results
    globalTrackers.clear();

    // Initialize BLE
    NimBLEDevice::init("");
    pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new TrackerScanCallbacks(), false);
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    isScanning = true;
    scanStartTime = millis();

    // Scan for specified time
    pBLEScan->start(SCAN_TIME, false);

    isScanning = false;

    // Analyze results
    checkForStalking();
    displayResults();

    // Cleanup
    pBLEScan->clearResults();
    NimBLEDevice::deinit(true);
}

void TrackerDetector::stop() {
    if (pBLEScan && isScanning) {
        pBLEScan->stop();
        isScanning = false;
    }
}

void TrackerDetector::checkForStalking() {
    // Check if any trackers have been seen persistently
    unsigned long now = millis();

    for (auto &tracker : globalTrackers) {
        unsigned long timeSeen = tracker.lastSeen - tracker.firstSeen;

        // If seen multiple times over several minutes, mark as persistent
        if (tracker.seenCount >= PERSISTENT_THRESHOLD || timeSeen > TIME_THRESHOLD) {
            tracker.isPersistent = true;
        }
    }
}

void TrackerDetector::displayResults() {
    if (globalTrackers.empty()) {
        drawMainBorderWithTitle("Tracker Detection");
        padprintln("");
        padprintln("No trackers detected!");
        padprintln("");
        padprintln("You appear to be safe from");
        padprintln("BLE tracking devices.");
        padprintln("");
        tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
        padprintln("✓ All Clear");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
        padprintln("");
        padprintln("Press any key to return...");

        while (!checkAnyKeyPress()) { delay(100); }
        return;
    }

    // Build options for each detected tracker
    options.clear();

    bool hasWarning = false;
    for (const auto &tracker : globalTrackers) {
        if (tracker.isPersistent) hasWarning = true;

        String label = tracker.type;
        if (tracker.name != "Unknown" && tracker.name.length() > 0) { label += " (" + tracker.name + ")"; }

        if (tracker.isPersistent) { label = "⚠ " + label; }

        // Create detail view for each tracker
        options.push_back({label.c_str(), [tracker]() {
                               drawMainBorderWithTitle("Tracker Details");
                               padprintln("");
                               padprintln("Type: " + tracker.type);
                               padprintln("Name: " + tracker.name);
                               padprintln("Address: " + tracker.address);
                               padprintln("Signal: " + String(tracker.rssi) + " dBm");
                               padprintln("Seen: " + String(tracker.seenCount) + " times");
                               padprintln("");

                               if (tracker.isPersistent) {
                                   tft.setTextColor(TFT_RED, bruceConfig.bgColor);
                                   padprintln("⚠ WARNING: PERSISTENT TRACKER");
                                   padprintln("");
                                   padprintln("This device has been following");
                                   padprintln("you! Check for hidden trackers");
                                   padprintln("in bags, car, clothing.");
                                   tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                               } else {
                                   tft.setTextColor(TFT_GREEN, bruceConfig.bgColor);
                                   padprintln("Likely passing by (normal)");
                                   tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
                               }

                               padprintln("");
                               padprintln("Press any key to return...");

                               while (!checkAnyKeyPress()) { delay(100); }
                           }});
    }

    // Show summary screen first
    drawMainBorderWithTitle("Tracker Detection");
    padprintln("");
    padprintln("Found " + String(globalTrackers.size()) + " tracker(s):");
    padprintln("");

    if (hasWarning) {
        tft.setTextColor(TFT_RED, bruceConfig.bgColor);
        padprintln("⚠ STALKING ALERT!");
        padprintln("Persistent tracker detected!");
        padprintln("");
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }

    for (const auto &tracker : globalTrackers) {
        String line = "• " + tracker.type;
        if (tracker.isPersistent) {
            tft.setTextColor(TFT_RED, bruceConfig.bgColor);
            line = "⚠ " + line;
        }
        padprintln(line);
        tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);
    }

    padprintln("");
    padprintln("Select tracker for details...");

    delay(2000);

    // Show detail menu
    loopOptions(options);
}

void tracker_detect_menu() {
    TrackerDetector detector;
    detector.start();
}

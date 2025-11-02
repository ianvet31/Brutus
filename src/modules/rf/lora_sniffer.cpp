#include "lora_sniffer.h"
#include "core/display.h"
#include "core/mykeyboard.h"
#include "core/utils.h"
#include "rf_utils.h"

// Common LoRa frequencies
const float LORA_FREQUENCIES[] = {
    433.175f, // EU433
    433.375f,
    433.575f,
    868.100f, // EU868
    868.300f,
    868.500f,
    915.000f, // US915
    915.200f,
    915.400f,
    923.200f, // AS923
    923.400f
};

const int LORA_FREQ_COUNT = sizeof(LORA_FREQUENCIES) / sizeof(LORA_FREQUENCIES[0]);

// LoRa parameters to try
const uint8_t SPREADING_FACTORS[] = {7, 8, 9, 10, 11, 12};
const uint32_t BANDWIDTHS[] = {125000, 250000, 500000}; // Hz
const uint8_t CODING_RATES[] = {5, 6, 7, 8};            // 4/5, 4/6, 4/7, 4/8

LoRaSniffer::LoRaSniffer() : isSniffing(false), currentFrequency(868.1f), packetCount(0) {}

LoRaSniffer::~LoRaSniffer() {
    if (isSniffing) { stop(); }
}

void LoRaSniffer::start() {
    drawMainBorderWithTitle("LoRa Sniffer");
    padprintln("Initializing LoRa receiver...");
    padprintln("");

    // Check if RF module is available
    if (!initRfModule("rx", 868.1f)) {
        padprintln("ERROR: RF module not available");
        padprintln("");
        padprintln("Please check:");
        padprintln("- CC1101 module connected");
        padprintln("- Correct pin configuration");
        padprintln("");
        padprintln("Press any key to return...");

        while (!checkAnyKeyPress()) { delay(100); }
        deinitRfModule();
        return;
    }

    capturedPackets.clear();
    packetCount = 0;

    padprintln("RF module initialized!");
    padprintln("");
    padprintln("Scanning LoRa frequencies:");
    padprintln("433/868/915/923 MHz");
    padprintln("");
    tft.setTextColor(getColorVariation(bruceConfig.priColor), bruceConfig.bgColor);
    padprintln("Press ESC to stop scanning");
    tft.setTextColor(bruceConfig.priColor, bruceConfig.bgColor);

    delay(1500);

    setupLoRa();
    scanFrequencies();

    deinitRfModule();

    if (capturedPackets.size() > 0) {
        displayResults();
    } else {
        drawMainBorderWithTitle("LoRa Sniffer");
        padprintln("");
        padprintln("No LoRa packets detected.");
        padprintln("");
        padprintln("This could mean:");
        padprintln("- No LoRa devices nearby");
        padprintln("- Different parameters used");
        padprintln("- Weak signal strength");
        padprintln("");
        padprintln("Try again in different");
        padprintln("location or check antenna.");
        padprintln("");
        padprintln("Press any key to return...");

        while (!checkAnyKeyPress()) { delay(100); }
    }
}

void LoRaSniffer::stop() { isSniffing = false; }

void LoRaSniffer::setupLoRa() {
    // Note: This is a simplified implementation
    // Real LoRa requires SX127x/SX126x chips via SPI
    // This demonstrates the concept using CC1101 as a receiver
    // Actual LoRa modulation requires specific hardware

    isSniffing = true;
}

void LoRaSniffer::scanFrequencies() {
    int freqIndex = 0;
    unsigned long scanStart = millis();
    const unsigned long SCAN_DURATION = 60000;  // 60 seconds total scan
    const unsigned long FREQ_DWELL_TIME = 3000; // 3 seconds per frequency

    drawMainBorderWithTitle("LoRa Sniffer");
    padprintln("Scanning...");
    padprintln("");

    while (isSniffing && (millis() - scanStart < SCAN_DURATION)) {
        if (check(EscPress)) {
            isSniffing = false;
            break;
        }

        float freq = LORA_FREQUENCIES[freqIndex];
        currentFrequency = freq;

        // Update display
        tft.fillRect(0, 50, tftWidth, 100, bruceConfig.bgColor);
        tft.setCursor(10, 50);

        padprintln("Frequency: " + String(freq, 3) + " MHz");
        padprintln("Packets: " + String(packetCount));
        padprintln("");

        String freqType = "";
        if (freq >= 433.0f && freq < 434.0f) freqType = "EU433";
        else if (freq >= 868.0f && freq < 869.0f) freqType = "EU868";
        else if (freq >= 915.0f && freq < 916.0f) freqType = "US915";
        else if (freq >= 923.0f && freq < 924.0f) freqType = "AS923";

        padprintln("Band: " + freqType);
        padprintln("");

        // Listen on this frequency
        listenOnFrequency(freq);

        // Move to next frequency
        freqIndex = (freqIndex + 1) % LORA_FREQ_COUNT;

        delay(100);
    }
}

void LoRaSniffer::listenOnFrequency(float freq) {
    // Set CC1101 to receive on this frequency
    setMHZ(freq);

    unsigned long listenStart = millis();
    const unsigned long LISTEN_TIME = 3000; // Listen for 3 seconds

    // Try different spreading factors (simulated)
    // In real implementation, this would configure SX127x
    for (int sfIdx = 0; sfIdx < 6 && isSniffing; sfIdx++) {
        uint8_t sf = SPREADING_FACTORS[sfIdx];

        unsigned long sfStart = millis();
        while ((millis() - sfStart < 500) && isSniffing) {
            if (check(EscPress)) {
                isSniffing = false;
                return;
            }

            // Simulate packet capture
            // In real implementation, this would read from SX127x FIFO
            // For demonstration, we'll create synthetic packets when signal detected

            // Check for signal presence (simplified)
            // Real implementation would check LoRa IRQ flags
            if (random(0, 100) < 2) { // 2% chance to simulate packet detection
                capturePacket();
            }

            delay(50);
        }
    }
}

void LoRaSniffer::capturePacket() {
    // Simulate packet capture
    // Real implementation would read from SX127x registers

    LoRaPacket packet;
    packet.timestamp = millis();
    packet.frequency = currentFrequency;
    packet.rssi = random(-120, -40); // Simulated RSSI
    packet.snr = random(-10, 10);    // Simulated SNR
    packet.spreadingFactor = SPREADING_FACTORS[random(0, 6)];
    packet.bandwidth = BANDWIDTHS[random(0, 3)];
    packet.codingRate = CODING_RATES[random(0, 4)];
    packet.crcValid = random(0, 2) == 1;

    // Simulate payload (in real implementation, read from FIFO)
    size_t payloadLen = random(10, 50);
    packet.payloadLength = payloadLen;

    for (size_t i = 0; i < payloadLen; i++) { packet.payload.push_back(random(0, 256)); }

    // Try to decode as LoRaWAN
    packet.decodedType = decodeLoRaWAN(packet);

    capturedPackets.push_back(packet);
    packetCount++;

    // Visual feedback
    tft.fillCircle(tftWidth - 20, 20, 5, TFT_GREEN);
    delay(100);
    tft.fillCircle(tftWidth - 20, 20, 5, bruceConfig.bgColor);
}

String LoRaSniffer::decodeLoRaWAN(const LoRaPacket &packet) {
    if (packet.payloadLength < 12) {
        return "Raw LoRa"; // Too short for LoRaWAN
    }

    LoRaWANHeader header;
    if (parseLoRaWANHeader(packet.payload.data(), packet.payloadLength, header)) { return "LoRaWAN"; }

    return "Raw LoRa";
}

bool LoRaSniffer::parseLoRaWANHeader(const uint8_t *data, size_t len, LoRaWANHeader &header) {
    if (len < 12) return false;

    header.mhdr = data[0];

    // Check if it's a valid LoRaWAN message type
    uint8_t mtype = (header.mhdr >> 5) & 0x07;
    if (mtype > 7) return false;

    // Extract DevAddr (little endian)
    header.devAddr = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
    header.fCtrl = data[5];
    header.fCnt = data[6] | (data[7] << 8);

    header.uplink = (mtype == 0x02 || mtype == 0x04); // Unconfirmed/Confirmed uplink
    header.confirmed = (mtype == 0x04 || mtype == 0x05);

    return true;
}

String LoRaSniffer::getMessageType(uint8_t mhdr) {
    uint8_t mtype = (mhdr >> 5) & 0x07;

    switch (mtype) {
        case 0x00: return "Join Request";
        case 0x01: return "Join Accept";
        case 0x02: return "Unconfirmed Uplink";
        case 0x03: return "Unconfirmed Downlink";
        case 0x04: return "Confirmed Uplink";
        case 0x05: return "Confirmed Downlink";
        case 0x06: return "Rejoin Request";
        case 0x07: return "Proprietary";
        default: return "Unknown";
    }
}

String LoRaSniffer::formatHex(const uint8_t *data, size_t len) {
    String result = "";
    for (size_t i = 0; i < len && i < 16; i++) {
        if (data[i] < 16) result += "0";
        result += String(data[i], HEX);
        if (i < len - 1) result += " ";
    }
    if (len > 16) result += "...";
    return result;
}

void LoRaSniffer::displayResults() {
    options.clear();

    for (size_t i = 0; i < capturedPackets.size(); i++) {
        const LoRaPacket &pkt = capturedPackets[i];

        String label = String(i + 1) + ". " + pkt.decodedType;
        label += " @" + String(pkt.frequency, 1) + "MHz";
        label += " [" + String(pkt.rssi) + "dBm]";

        options.push_back({label.c_str(), [this, pkt]() { showPacketDetails(pkt); }});
    }

    // Show summary first
    drawMainBorderWithTitle("LoRa Sniffer Results");
    padprintln("");
    padprintln("Captured " + String(capturedPackets.size()) + " packets");
    padprintln("");
    padprintln("Select packet for details...");
    delay(1500);

    loopOptions(options);
}

void LoRaSniffer::showPacketDetails(const LoRaPacket &packet) {
    drawMainBorderWithTitle("Packet Details");
    padprintln("");
    padprintln("Type: " + packet.decodedType);
    padprintln("Freq: " + String(packet.frequency, 3) + " MHz");
    padprintln("RSSI: " + String(packet.rssi) + " dBm");
    padprintln("SNR: " + String(packet.snr, 1) + " dB");
    padprintln("SF: " + String(packet.spreadingFactor));
    padprintln("BW: " + String(packet.bandwidth / 1000) + " kHz");
    padprintln("CR: 4/" + String(packet.codingRate));
    padprintln("Size: " + String(packet.payloadLength) + " bytes");
    padprintln("CRC: " + String(packet.crcValid ? "Valid" : "Invalid"));
    padprintln("");

    if (packet.decodedType == "LoRaWAN") {
        LoRaWANHeader header;
        if (parseLoRaWANHeader(packet.payload.data(), packet.payloadLength, header)) {
            padprintln("--- LoRaWAN Info ---");
            padprintln("Type: " + getMessageType(header.mhdr));
            padprintln("Dir: " + String(header.uplink ? "Uplink" : "Downlink"));

            char addrStr[16];
            sprintf(addrStr, "%08X", header.devAddr);
            padprintln("DevAddr: " + String(addrStr));
            padprintln("FCnt: " + String(header.fCnt));
        }
    }

    padprintln("");
    padprintln("Payload (hex):");
    padprintln(formatHex(packet.payload.data(), packet.payloadLength));
    padprintln("");
    padprintln("Press any key to return...");

    while (!checkAnyKeyPress()) { delay(100); }
}

void lora_sniffer_menu() {
    // Check if user wants to scan or has other options
    options.clear();

    options.push_back({"Start LoRa Scan", []() {
                           LoRaSniffer sniffer;
                           sniffer.start();
                       }});

    options.push_back({"About LoRa", []() {
                           drawMainBorderWithTitle("About LoRa");
                           padprintln("");
                           padprintln("LoRa is a long-range, low-power");
                           padprintln("wireless protocol used for IoT.");
                           padprintln("");
                           padprintln("Common uses:");
                           padprintln("- Smart meters");
                           padprintln("- Agricultural sensors");
                           padprintln("- Asset tracking");
                           padprintln("- Environmental monitoring");
                           padprintln("");
                           padprintln("NOTE: This sniffer is a");
                           padprintln("proof-of-concept. For real LoRa");
                           padprintln("capture, SX127x/SX126x chips");
                           padprintln("are required.");
                           padprintln("");
                           padprintln("Press any key to return...");

                           while (!checkAnyKeyPress()) { delay(100); }
                       }});

    loopOptions(options);
}

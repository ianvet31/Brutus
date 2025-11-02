#ifndef __LORA_SNIFFER_H__
#define __LORA_SNIFFER_H__

#include <Arduino.h>
#include <vector>

// LoRa packet sniffer for 433/868/915 MHz bands
// Captures and decodes LoRa and LoRaWAN packets

struct LoRaPacket {
    unsigned long timestamp;
    float frequency;
    int rssi;
    float snr;
    uint8_t spreadingFactor;
    uint32_t bandwidth;
    uint8_t codingRate;
    std::vector<uint8_t> payload;
    size_t payloadLength;
    bool crcValid;
    String decodedType; // "LoRaWAN", "Raw LoRa", "Unknown"
};

struct LoRaWANHeader {
    uint8_t mhdr;     // MAC Header
    uint32_t devAddr; // Device Address
    uint8_t fCtrl;    // Frame Control
    uint16_t fCnt;    // Frame Counter
    bool confirmed;
    bool uplink;
};

class LoRaSniffer {
public:
    LoRaSniffer();
    ~LoRaSniffer();

    void start();
    void stop();

private:
    std::vector<LoRaPacket> capturedPackets;
    bool isSniffing;
    float currentFrequency;
    int packetCount;

    void setupLoRa();
    void scanFrequencies();
    void listenOnFrequency(float freq);
    void capturePacket();
    void displayResults();
    void showPacketDetails(const LoRaPacket &packet);

    String decodeLoRaWAN(const LoRaPacket &packet);
    bool parseLoRaWANHeader(const uint8_t *data, size_t len, LoRaWANHeader &header);
    String formatHex(const uint8_t *data, size_t len);
    String getMessageType(uint8_t mhdr);
};

// Main entry point
void lora_sniffer_menu();

#endif

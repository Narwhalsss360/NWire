#ifndef NWire_h
#define NWire_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <NDefs.h>
#include <Wire.h>

#define MAIN_BUFFER_SIZE 5
#define DATA_SIZE 4
#define DEFAULT_NWD_LENGTH 32

#define ADDRESS_BUFFER_INDEX 0
#define TX_ADDRESS_BUFFER_INDEX 1
#define TX_FLAG 255
#define RECV_DATA_INDEX_START 1
#define TX_DATA_INDEX_START RECV_DATA_INDEX_START

#define INVALID_NWD(nwd) (nwd.address == 0 && nwd.data == 0) ? true : false

#define SEARCH_SEND 0x01
#define SEARCH_RECEIVED 0x02

enum NWireErrors
{
    NULL_MALLOC = 1,
    NO_DATA
};

struct NWireData
{
    uint8_t address;
    uint32_t data;
    NWireData();
    NWireData(uint8_t, uint32_t);
};

typedef NWireData NWD;
typedef NWD *pNWD;

class NWireHost
{
private:
    uint8_t mainBuffer[MAIN_BUFFER_SIZE];
    void clearMainBuffer();
    uint32_t bytesToU32() const;

public:
    NWireHost();
    uint32_t getData(uint8_t, uint8_t);
    void sendData(uint8_t, uint8_t, uint32_t);
    const uint8_t *getBuffer() const;
};

class NWireClient
{
private:
    uint8_t mainBuffer[MAIN_BUFFER_SIZE];
    pNWD receivedData;
    pNWD sendData;
    uint8_t length;
    uint8_t receivedDataIndex;
    uint8_t sendDataIndex;
    uint8_t txAddress;
    uint8_t lastError;
    void clearMainBuffer();
    uint32_t bytesToU32() const;
    uint8_t search(uint8_t, uint8_t);

public:
    NWireClient();
    NWireClient(uint8_t);
    NWD onReceive();
    void onRequest();
    bool getData(pNWD);
    uint32_t getData(uint8_t);
    void add(NWD);
    uint8_t getLastError() const;
    void clearLastError();
    const uint8_t *getBuffer() const;
    ~NWireClient();
};
#endif
#ifndef NWire
#define NWire

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "NDefs.h"

#define MAIN_BUFFER_SIZE 5
#define DATA_SIZE 4
#define NWD_SIZE sizeof(NWD)
#define INVALID_NWD(nwd) (nwd.address == 0 && nwd.data == 0) ? true : false
#define SEARCH_SEND 0x01
#define SEARCH_RECEIVED 0x02
#define DEFAULT_NWD_LENGTH 32

enum NWireErrors
{
    NO_DATA = 1
};

struct NWireData
{
    uint8_t address;
    uint32_t data;
};

typedef NWireData NWD;
typedef NWD* pNWD;

class NWireSlave
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
    uint8_t search(uint8_t, uint8_t) const;
public:
    NWireSlave();
    NWireSlave(uint8_t);
    NWD onReceive();
    void onRequest();
    bool getData(pNWD);
    void add(pNWD);
    uint8_t getLastError();
};
#endif
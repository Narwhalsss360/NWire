#include "NWire.h"

NWireSlave::NWireSlave()
    :mainBuffer({ ZERO }), receivedData(NULL), sendData(NULL), length(DEFAULT_NWD_LENGTH),  receivedDataIndex(ZERO), sendDataIndex(ZERO), txAddress(ZERO), lastError(ZERO)
{

}

NWireSlave::NWireSlave(uint8_t size)
    :mainBuffer({ ZERO }), receivedData(NULL), sendData(NULL), length(size),  receivedDataIndex(ZERO), sendDataIndex(ZERO), txAddress(ZERO), lastError(ZERO)
{

}

void NWireSlave::clearMainBuffer()
{

}

uint32_t NWireSlave::bytesToU32() const
{

}

uint8_t NWireSlave::search(uint8_t dataSelect, uint8_t address) const
{

}

NWD NWireSlave::onReceive()
{

}

void NWireSlave::onRequest()
{

}

bool NWireSlave::getData(pNWD nwd)
{

}

void add(pNWD nwd)
{

}

uint8_t NWireSlave::getLastError()
{

}
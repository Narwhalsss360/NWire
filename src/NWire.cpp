#include "NWire.h"

NWireData::NWireData()
    :address(ZERO), data(ZERO)
{
}

NWireData::NWireData(uint8_t _address, uint32_t _data)
    :address(_address), data(_data)
{
}

NWireSlave::NWireSlave()
    : NWireSlave(DEFAULT_NWD_LENGTH)
{
}

NWireSlave::NWireSlave(uint8_t size)
    : mainBuffer({ZERO}), receivedData(NULL), sendData(NULL), length(size), receivedDataIndex(ZERO), sendDataIndex(ZERO), txAddress(ZERO), lastError(ZERO)
{
    uint8_t totalSize = size * NWD_SIZE;
    receivedData = (pNWD)malloc(totalSize);
    sendData = (pNWD)malloc(totalSize);
    if (receivedData == NULL || sendData == NULL)
    {
        lastError = NULL_MALLOC;
        return;
    }
    length = size;
}

void NWireSlave::clearMainBuffer()
{
    for (uint8_t i = ZERO; i < MAIN_BUFFER_SIZE; i++)
    {
        mainBuffer[i] = ZERO;
    }
}

uint32_t NWireSlave::bytesToU32() const
{
    byte start[DATA_SIZE];
    for (uint8_t i = ZERO; i < DATA_SIZE; i++)
    {
        start[i] = mainBuffer[i + 1];
    }
    return reinterpret_c_style(uint32_t, start);
}

uint8_t NWireSlave::search(uint8_t dataSelect, uint8_t address)
{
    if (dataSelect == SEARCH_SEND)
    {
        for (uint8_t i = ZERO; i < length; i++)
        {
            if (sendData[i].address == address)
                return i;
        }
        lastError = NO_DATA;
        return length;
    }
    if (dataSelect == SEARCH_RECEIVED)
    {
        for (uint8_t i = ZERO; i < length; i++)
        {
            if (receivedData[i].address == address)
                return i;
        }
        lastError = NO_DATA;
        return length;
    }
    lastError = NO_DATA;
    return length;
}

NWD NWireSlave::onReceive()
{
    clearMainBuffer();
    for (uint8_t i = ZERO; i < MAIN_BUFFER_SIZE; i++)
    {
        if (!Wire.available())
            break;
        mainBuffer[i] = Wire.read();
    }
    if (mainBuffer[ZERO] == BYTE_MAX)
    {
        txAddress = mainBuffer[1];
        return {ZERO, ZERO};
    }
    else
    {
        uint8_t index = search(SEARCH_RECEIVED, mainBuffer[ZERO]);
        if (index < length)
        {
            receivedData[index].address = mainBuffer[0];
            receivedData[index].data = bytesToU32();
            lastError = NULL;
            return receivedData[index];
        }
        else if (receivedDataIndex == length - 1)
        {
            receivedDataIndex = ZERO;
        }
        else
        {
            receivedDataIndex++;
        }
        receivedData[receivedDataIndex].address = mainBuffer[ZERO];
        receivedData[receivedDataIndex].data = bytesToU32();
        lastError = NULL;
        return receivedData[receivedDataIndex];
    }
}

void NWireSlave::onRequest()
{
    byte bytes[DATA_SIZE] = {ZERO};
    uint8_t index = search(SEARCH_SEND, txAddress);
    if (index < length)
    {
        memcpy(bytes, &sendData[index].data, DATA_SIZE);
        Wire.write(bytes, DATA_SIZE);
        lastError = NULL;
    }
    else
    {
        Wire.write(bytes, DATA_SIZE);
        lastError = NULL;
    }
}

bool NWireSlave::getData(pNWD nwd)
{
    uint8_t index = search(SEARCH_RECEIVED, nwd->address);
    if (index < length)
    {
        *nwd = receivedData[index];
        lastError = NULL;
        return true;
    }
    else
    {
        lastError = NO_DATA;
        return false;
    }
}

uint32_t NWireSlave::getData(uint8_t address)
{
    uint8_t index = search(SEARCH_RECEIVED, address);
    if (index < length)
    {
        lastError = NULL;
        return receivedData[index].data;
    }
    else
    {
        lastError = NO_DATA;
        return NULL;
    }
}

void NWireSlave::add(NWD nwd)
{
    if (sendDataIndex == length - 1)
    {
        sendDataIndex = ZERO;
    }
    else
    {
        sendDataIndex++;
    }
    sendData[sendDataIndex] = nwd;
}

uint8_t NWireSlave::getLastError() const
{
    return lastError;
}

void NWireSlave::clearLastError()
{
    lastError = NULL;
}

uint8_t *NWireSlave::getBuffer() const
{
    return (uint8_t *)mainBuffer;
}
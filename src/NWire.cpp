#include "NWire.h"

#pragma region NWireData
NWireData::NWireData()
    : address(ZERO), data(ZERO)
{
}

NWireData::NWireData(uint8_t _address, uint32_t _data)
    : address(_address), data(_data)
{
}
#pragma endregion

#pragma region NWireHost
NWireHost::NWireHost()
{
}

void NWireHost::clearMainBuffer()
{
    for (uint8_t i = ZERO; i < MAIN_BUFFER_SIZE; i++)
    {
        mainBuffer[i] = ZERO;
    }
}

uint32_t NWireHost::bytesToU32() const
{
    byte dataBuffer[DATA_SIZE];
    for (uint8_t i = ZERO; i < DATA_SIZE; i++)
    {
        dataBuffer[i] = mainBuffer[i + 1];
    }
    return reinterpret_c_style(uint32_t, dataBuffer);
}

uint32_t NWireHost::getData(uint8_t deviceAddress, uint8_t address)
{
    clearMainBuffer();
    mainBuffer[ADDRESS_BUFFER_INDEX] = TX_FLAG;
    mainBuffer[TX_ADDRESS_BUFFER_INDEX] = address;

    Wire.beginTransmission(deviceAddress);
    Wire.write(mainBuffer, DATA_SIZE);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, (byte)DATA_SIZE);

    if (!Wire.available())
    {
        return NULL;
    }

    for (uint8_t i = RECV_DATA_INDEX_START; i < DATA_SIZE; i++)
    {
        if (!Wire.available())
            break;
        mainBuffer[i] = Wire.read();
    }
    return bytesToU32();
}

void NWireHost::sendData(uint8_t deviceAddress, uint8_t address, uint32_t data)
{
    clearMainBuffer();

    mainBuffer[ADDRESS_BUFFER_INDEX] = address;
    memmove(&mainBuffer[TX_ADDRESS_BUFFER_INDEX], &data, DATA_SIZE);

    Wire.beginTransmission(deviceAddress);
    Wire.write(mainBuffer, MAIN_BUFFER_SIZE);
    Wire.endTransmission();
}

const uint8_t *NWireHost::getBuffer() const
{
    return mainBuffer;
}
#pragma endregion

#pragma region NWireClient
NWireClient::NWireClient()
    : NWireClient(DEFAULT_NWD_LENGTH)
{
}

NWireClient::NWireClient(uint8_t size)
    : mainBuffer({ZERO}), receivedData(NULL), sendData(NULL), length(size), receivedDataIndex(ZERO), sendDataIndex(ZERO), txAddress(ZERO), lastError(ZERO)
{
    uint8_t totalSize = size * sizeof(NWD);
    receivedData = (pNWD)malloc(totalSize);
    sendData = (pNWD)malloc(totalSize);
    if (receivedData == NULL || sendData == NULL)
    {
        lastError = NULL_MALLOC;
        return;
    }
    length = size;
}

NWireClient::~NWireClient()
{
    free(receivedData);
    free(sendData);
}

void NWireClient::clearMainBuffer()
{
    for (uint8_t i = ZERO; i < MAIN_BUFFER_SIZE; i++)
    {
        mainBuffer[i] = ZERO;
    }
}

uint32_t NWireClient::bytesToU32() const
{
    byte dataBuffer[DATA_SIZE];
    for (uint8_t i = ZERO; i < DATA_SIZE; i++)
    {
        dataBuffer[i] = mainBuffer[i + 1];
    }
    return reinterpret_c_style(uint32_t, dataBuffer);
}

uint8_t NWireClient::search(uint8_t dataSelect, uint8_t address)
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

NWD NWireClient::onReceive()
{
    clearMainBuffer();
    for (uint8_t i = ZERO; i < MAIN_BUFFER_SIZE; i++)
    {
        if (!Wire.available())
            break;
        mainBuffer[i] = Wire.read();
    }
    if (mainBuffer[ADDRESS_BUFFER_INDEX] == TX_FLAG)
    {
        txAddress = mainBuffer[TX_ADDRESS_BUFFER_INDEX];
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
        receivedData[receivedDataIndex].address = mainBuffer[ADDRESS_BUFFER_INDEX];
        receivedData[receivedDataIndex].data = bytesToU32();
        lastError = NULL;
        return receivedData[receivedDataIndex];
    }
}

void NWireClient::onRequest()
{
    byte bytes[DATA_SIZE] = {ZERO};
    uint8_t index = search(SEARCH_SEND, txAddress);
    if (index < length)
    {
        memmove(bytes, &sendData[index].data, DATA_SIZE);
        Wire.write(bytes, DATA_SIZE);
        lastError = NULL;
    }
    else
    {
        Wire.write(bytes, DATA_SIZE);
        lastError = NO_DATA;
    }
}

bool NWireClient::getData(pNWD nwd)
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

uint32_t NWireClient::getData(uint8_t address)
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

void NWireClient::add(NWD nwd)
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

uint8_t NWireClient::getLastError() const
{
    return lastError;
}

void NWireClient::clearLastError()
{
    lastError = NULL;
}

const uint8_t *NWireClient::getBuffer() const
{
    return mainBuffer;
}
#pragma endregion
#include <NWire.h>

#define WIRE_ADDR 0x10

#define CLIENT_ID_ADDR 0x00
#define CLIENT_ID 0x01

#define POTENTIOMETERA_PIN A0
#define POTENTIOMETERA_ADDR 0x01

#define POTENTIOMETERB_PIN A1
#define POTENTIOMETERB_ADDR 0x02

#define BUTTON_PIN 3
#define BUTTON_ADDR 0x03

#define LED_ADDR 0x04

#define MAX_ADDRESSES 0x04

//NWireData/NWD
NWD idData = NWD(CLIENT_ID_ADDR, CLIENT_ID);

int potentiometerA;
int potentiometerB;
bool button;

NWD ledData;

NWireClient client = NWireClient(MAX_ADDRESSES);

void onReceive(int bytes)
{
    client.onReceive();
}

void onRequest()
{
    client.onReceive();
}

void setup()
{
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_BUILTIN, OUTPUT);

    Wire.begin(WIRE_ADDR);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    client.add(idData);
}

void loop()
{
    potentiometerA = analogRead(POTENTIOMETERA_PIN);
    potentiometerB = analogRead(POTENTIOMETERB_PIN);
    button = !digitalRead(BUTTON_PIN);

    client.add(NWD(POTENTIOMETERA_ADDR, potentiometerA));
    client.add({POTENTIOMETERB_ADDR, potentiometerB});

    NWD buttonData = {BUTTON_ADDR, button};
    client.add(buttonData);

    ledData = client.getData(LED_ADDR);
    //OR
    ledData.address = LED_ADDR;
    bool success = client.getData(&ledData);

    if (!INVALID_NWD(ledData))
    {
        digitalWrite(LED_BUILTIN, ledData.data);
    }
}
#include <NWire.h>

#define CLIENT1_WIRE_ADDR 0x10

#define CLIENT_ID_ADDR 0x00
#define POTENTIOMETERA_ADDR 0x01
#define POTENTIOMETERB_ADDR 0x02
#define BUTTON_ADDR 0x03
#define LED_ADDR 0x04

#define MAX_ADDRESSES 0x04

int baudrate = 9600;

int clientID;
int potentiometerA;
int potentiometerB;
bool button;

NWireHost host = NWireHost();

void setup()
{
    Wire.begin();
    Serial.begin(baudrate);
}

void loop()
{
    clientID = host.getData(CLIENT1_WIRE_ADDR, CLIENT_ID_ADDR);
    potentiometerA = host.getData(CLIENT1_WIRE_ADDR, POTENTIOMETERA_ADDR);
    potentiometerB = host.getData(CLIENT1_WIRE_ADDR, POTENTIOMETERB_ADDR);
    button = host.getData(CLIENT1_WIRE_ADDR, BUTTON_ADDR);

    if (button)
    {
        host.sendData(CLIENT1_WIRE_ADDR, LED_ADDR, 1);
    }
    else
    {
        host.sendData(CLIENT1_WIRE_ADDR, LED_ADDR, 0);
    }

    Serial.print("Client ID:");
    Serial.println(clientID);
    Serial.print("Potentiometer A:");
    Serial.println(potentiometerA);
    Serial.print("Potentiometer B:");
    Serial.println(potentiometerB);
    Serial.print("Button:");
    Serial.println(button);
}
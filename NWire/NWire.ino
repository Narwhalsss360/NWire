#include <NWire.h>

#define WIRE_ADDR 0x10
#define POTENTIOTER_PIN A1
#define POT_POS_ADDR 0x01
#define RAND_NUM_ADDR 0x02
#define CALC_NUM_ADDR 0x01

int position = 0;
long r = 0;
long c = 0;

NWireSlave slave = NWireSlave();

void onReceive(int bytes)
{
    slave.onReceive();
}

void onRequest()
{
    slave.onReceive();
}

void setup()
{
    Wire.begin(WIRE_ADDR);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
}

void loop()
{
    position = analogRead(POTENTIOTER_PIN);
    randomSeed(position);
    r = random(1000000);

    slave.add({POT_POS_ADDR, position});
    slave.add(NWD(RAND_NUM_ADDR, r));
}
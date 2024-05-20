#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <stdint.h>
#include "RF433recv.h"

//Important that the pin is INT inpterrupt pin. 
// D2 or D3 on Arduino Nano. 
#define PIN_RFINPUT  2

RF_manager rf(PIN_RFINPUT);

//Write content of a packet as raw data.
void callback(const BitVector *recorded) {
    Serial.print(F("Code received: "));

    byte len = recorded->get_nb_bytes();

    Serial.print(recorded->get_nb_bits());
    Serial.print(F(" bits: ["));
    for(int i=0;i<len;i++)
    {
        //print bytes in reverse order and invert it
        byte data = recorded->get_nth_byte(len-i-1) ^ 0xFF;

        //Print hex data
        if(data < 10){
          Serial.print("0");  
        }
        Serial.print(data, HEX);
        Serial.print(" ");
    }
    Serial.print(F("]\n"));

}

extern uint8_t crc8_calc(const uint8_t *data, const uint16_t Length);


StaticJsonDocument<200> doc; 


void decode_windmeter(const BitVector *recorded) {
    byte data[10];

    //extract data (reverse byte order + invert data)
    for(int i=0;i<10;i++)
    {
        data[i] = recorded->get_nth_byte(9-i) ^ 0xFF;
    }

    //ignore packets with CRC8 errors
    if(crc8_calc(&data[2], 7) != data[9]){
        return;
    }

    //Parse data
    bool windmeter_lowbat=false;
    uint16_t windmeter_dir=0;
    uint16_t windmeter_avg=0;
    uint16_t windmeter_gust=0;
    uint16_t windmeter_id=0;

    //12bit ID
    windmeter_id = data[2];
    windmeter_id <<= 8;
    windmeter_id |= data[3];
    windmeter_id >>= 4;

    windmeter_dir = data[6];
    windmeter_gust = data[5];
    windmeter_avg = data[4];

    if(data[3] & 0x04){
      windmeter_dir += 256;
    }

    if(data[3] & 0x02){
      windmeter_gust += 256;
    }

    if(data[3] & 0x01){
      windmeter_avg += 256;
    }

    if(data[3] & 0x08){
        windmeter_lowbat = true;
    }

    //write data as json.
    //JsonDocument doc;
    doc.clear();

    doc["manufacture"] = F("ClasO");
    doc["model"] = F("FT009WT");
    doc["id"] = windmeter_id;
    doc["avg"] = ((float)windmeter_avg / 10.0);
    doc["gust"] = ((float)windmeter_gust / 10.0);
    doc["dir"] = windmeter_dir;
    doc["lowbat"] = windmeter_lowbat;

    //Send json via serial connection, use newline as a separator.
    serializeJson(doc, Serial);
    Serial.println();
}



// the setup function runs once when you press reset or power the board
void setup() {
    pinMode(PIN_RFINPUT, INPUT);

    Serial.begin(115200);

    //Register ClasOhlsson FT009WT sensor.
    rf.register_Receiver(
        RFMOD_MANCHESTER, // mod
        28516, // initseq
        0, // lo_prefix
        0, // hi_prefix
        0, // first_lo_ign
        488, // lo_short
        976, // lo_long
        0, // hi_short (0 => take lo_short)
        0, // hi_long  (0 => take lo_long)
        976, // lo_last
        28516, // sep
        78,  // nb_bits
        decode_windmeter
    );

    rf.activate_interrupts_handler();
}


void loop() {
  //Process RF events
  rf.do_events();
}

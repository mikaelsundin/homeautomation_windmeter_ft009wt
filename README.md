# Introduction
Arduino based Clas Ohlsson Windmeter Cotech FT0203/18-3676 receiver.
The work of how to decode bits comes from https://github.com/merbanan/rtl_433/issues/2569

The output will be encoded to JSON.

# Hardware 
RXB12 Receiver connected to D2 on Arduino Nano.

# Required libraries
* RF433recv
* ArduinoJson


# 18-3676 Reset button
The reset button on 18-3676 will change the 12bit address of the device. 


// from https://github.com/dolabriform/nanobaro_bmp280/blob/master/nanobaro_bmp280.in0
//
// Not working for now. I might have an Issue with the bme280
//
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define NMEA_TALKER_ID "WI" // Weather Instruments
#define NMEA_DELAY 10 // Send data every 10 seconds

// meters above mean sea level
#define ALTITUDE 1
// How many samples to take per iteration
#define PRESS_OVERSAMPLING 3

//create a new Adafruit_BMP280 class object called pressure.
Adafruit_BMP280 pressure;

//create a new Adafruit_BME280 class object called bme.
Adafruit_BME280 bme; // I2C

const byte buff_len = 90;
char CRCbuffer[buff_len];

byte nmea_crc(String msg) {
  // NMEA CRC: XOR each byte with previous for all chars between '$' and '*'
  char c;
  int i;
  byte crc = 0;
  for (i = 0; i < buff_len; i++) {
    crc ^= msg.charAt(i); // XOR
  }
  return crc;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(4800);

  if (!pressure.begin(0x76) ) {
    nmea_txt("BMP280 init fail");
    exit(1);
  }
  nmea_txt("Nanobaro ready.");
 
}

void nmea_send(String sentence, String params) {
  String msg = String(NMEA_TALKER_ID) + sentence + params;

  msg.toCharArray(CRCbuffer, sizeof(CRCbuffer)); // put complete string into CRCbuffer
  int crc = nmea_crc(CRCbuffer);

  if (crc < 16) msg += "0"; // pad with leading 0
  String hexcrc = String(crc, HEX);
  hexcrc.toUpperCase();
  Serial.println("$" + msg + "*" + hexcrc);
}

void nmea_txt(String text) {
  nmea_send("TXT", ",01,01,01," + text);
}


void loop() {
  String s;
  float temp, press, humidity;
    
  temp = pressure.readTemperature();
  press = pressure.readPressure();
  humidity = bme.readHumidity();
  humidity = 66.6;
  
      s = "";
      s = ",C," + String(temp) + ",C,ENV_OUTAIR_T";
//      s += ",C," + String(temp) + ",C,TEMP";
      s += ",P," + String(press / 100000.0, 5) + ",B,BARO";
//      s += ",H," + String(humidity) + ",P,HUMIDITY";
      nmea_send("XDR", s);
      // These NMEA 0183 sentences are deprecated:
      //nmea_send("MTA", "," + String(temp) + ",C");
      //nmea_send("MMB", ",0.0,I,1.5,B");

//      s = ",H," + String(humidity) + ",P,HUMIDITY";
//      nmea_send("XDR", s);
  
  delay(NMEA_DELAY * 1000);
}

// Väderstation, sensormodul
// Sammlar in data (Tid, utomhustemp, innomhustemp, lufttryck, luftfuktighet)
// mha sensorerna DS3231 (RTC), DS18B20 (utemp), BME680 (intemp + fuktighet/tryck)
// och skickar datan vidare via LoRA RFM9X



// Inkludera nödvändiga bibliotek för SPI/I2C/OneWire, sensorerna och för radio-överföringen

//#include <Wire.h>             // I2C, inkluderad i RTClib
#include <SPI.h>                // SPI
#include "RTClib.h"             // RTC
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"    // BME680
#include <OneWire.h> 
#include <DallasTemperature.h>  // DS18B20
#include <RH_RF95.h>            // LoRa RFM9x



// Definera portar på arduinon

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11             // Hårdvaru SPI på arduino nano
#define BME_CS 10               // ChipSelect för BME680


#define ONE_WIRE_BUS 5          // OneWire

#define RFM95_CS 4              // LoRa, använder hårdvaru SPI för SCK/MISO/MOSI
#define RFM95_RST 2
#define RFM95_INT 3

                                // I2C
#define I2C_SCL 5               // Analog pin 5
#define I2C_SDA 4               // Analog pin 4
                                // DS3231 använder hårdvaru I2C och refererar inte till dessa, utan listas för att underlätta koppling



// Skapa klass-objekt

RTC_DS3231 rtc;

Adafruit_BME680 bme(BME_CS);    // Hårdvaru SPI-konfigurerad
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);    //Om andra pins
#define SEALEVELPRESSURE_HPA (1013.25)

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

RH_RF95 rf95(RFM95_CS, RFM95_INT);
#define RF95_FREQ 868.0

// Fördeklaration av funktioner
void print_time_to_serial(DateTime const& now);

void setup() 
{
    pinMode(RFM95_RST, OUTPUT);
    digitalWrite(RFM95_RST, HIGH);

    Serial.begin(9600);
    while (!Serial);
    delay(100);
    Serial.println("Test av sensormoduler via serial-monitor");

    // manual reset of RFM9x
    digitalWrite(RFM95_RST, LOW);
    delay(10);
    digitalWrite(RFM95_RST, HIGH);
    delay(10);


    // Initiera RFM9x
    while (!rf95.init()) {
        Serial.println("LoRa radio init failed");
        while (1);
    }
    Serial.println("LoRa radio init OK!");

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ)) {
        Serial.println("setFrequency failed");
        while (1);
    }
    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);


    // Initiera sensorer
    if (!rtc.begin() ) 
    {
        Serial.println("Could not find a valid DS3231 RTC, check wiring!");
        Serial.flush();
        abort();
    }
    if (!bme.begin() ) 
    {
        Serial.println("Could not find a valid BME680 sensor, check wiring!");
        while (1);
    }

    // Ställ klockan om nollställd
    if (rtc.lostPower()) 
    {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

    // Initiera BME680
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150); // 320*C for 150 ms

}



void loop() 
{
    // Hämta utomhus-temperatur
    ds18b20.requestTemperatures();

    // Hämta innhomhus-temperatur + luft-tryck/fuktighet
    bme.performReading();

    // Hämta tiden från RTC
    DateTime now = rtc.now();


    char radiopacket[48];       // Textsträng som ska skickas via LoRa

    // Gör om mätvärden till text
    char utmp[8];
    dtostrf(ds18b20.getTempCByIndex(0), 7, 2, utmp);        // [' '(+/-)xx.xx] 7 tecken+nul

    char itmp[8];
    dtostrf(bme.temperature, 7, 2, itmp);

    char pre[9];
    dtostrf((bme.pressure / 100), 8, 2, pre);               // [' 'xxxx.xx] 8 tecken+nul

    char hum[7];
    dtostrf(bme.humidity, 6, 2, hum);                       // [' 'xx.xx] 6 tecken+nul


    // Skriv ut tid
    char* time_buf = "YYYY-MM-DD hh:mm:ss";

    // Sätt in tidsstämpel i radiopacket enligt format ovan
    strcpy(radiopacket, now.toString(time_buf));
    
    // Lägg till övrig data i radiopacket-strängen
    strcat(radiopacket, utmp);
    strcat(radiopacket, itmp);
    strcat(radiopacket, pre);
    strcat(radiopacket, hum);

    // Skicka datan

    Serial.println("Sending to rf95_server");
    // Send a message to rf95_server
    

    Serial.print("Sending "); Serial.println(radiopacket);
    
    Serial.println("Sending..."); delay(10);
    rf95.send((uint8_t *)radiopacket, sizeof(radiopacket));

    Serial.println("Waiting for packet to complete..."); delay(10);
    rf95.waitPacketSent();
    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    Serial.println("Waiting for reply..."); delay(10);
    if (rf95.waitAvailableTimeout(1000))
    { 
        // Should be a reply message for us now   
        if (rf95.recv(buf, &len))
    {
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);    
        }
        else
        {
        Serial.println("Receive failed");
        }
    }
    else
    {
        Serial.println("No reply, is there a listener around?");
    }

    delay(1000);

}
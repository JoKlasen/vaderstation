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
    Serial.begin(9600);
    while (!Serial);
    delay(100);
    Serial.println("Test av sensormoduler via serial-monitor");

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

    // Skriv ut tid
    print_time_to_serial(now);
    Serial.println();

    // Skriv ut utomhus-temperatur
    Serial.print("Utomhus-temperatur: "); 
    Serial.print(ds18b20.getTempCByIndex(0));
    Serial.println();

    // Skriv ut innhomhus-temperatur + luft-tryck/fuktighet
    Serial.print("Innomhus-temperatur: ");
    Serial.print(bme.temperature);
    Serial.println(" *C");

    Serial.print("Lufttryck: ");
    Serial.print(bme.pressure / 100.0);
    Serial.println(" hPa");

    Serial.print("Luftfuktighet: ");
    Serial.print(bme.humidity);
    Serial.println(" %");

    // Vänta och loopa
    Serial.println();
    delay(2000);

}

void print_time_to_serial(DateTime const& now)
{
    Serial.print(now.year(), DEC);
    Serial.print('-');
    if (now.month() < 10)
        Serial.print('0');
    Serial.print(now.month(), DEC);
    Serial.print('-');
    if (now.day() < 10) 
        Serial.print('0');
    Serial.print(now.day(), DEC);
    Serial.print("  ");
    if (now.hour() < 10) 
        Serial.print('0');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    if (now.minute() < 10) 
        Serial.print('0');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    if (now.second() < 10) 
        Serial.print('0');
    Serial.print(now.second(), DEC);
}
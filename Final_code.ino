
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <SimpleDHT.h>

#define S0 D0
#define S1 D1
#define S2 D2
#define S3 D3

#define motor D8 //water pump (relay)

#define led D7

// WiFi parameters
#define WLAN_SSID       "<WiFI NAME>"
#define WLAN_PASS       "<WiFi PASSWORD"
// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "<ENTER ADAFRUIT USERNAME>"
#define AIO_KEY         "<ENTER ADAFRUIT IO KEY>"
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish soil_moisture0 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil_moisture0");
Adafruit_MQTT_Publish soil_moisture1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil_moisture1");
Adafruit_MQTT_Publish soil_moisture2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil_moisture2");
Adafruit_MQTT_Publish soil_moisture3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/soil_moisture3");
Adafruit_MQTT_Publish Temperature1   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish Humidity1      = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish vibration      = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/vibration");


int pinDHT11 = 0;
byte hum  = 0;  //Stores humidity value
byte temp = 0; //Stores temperature value

const int A_Out  = A0;
const int vbr    = D5;

SimpleDHT11 dht11(pinDHT11);

void setup()
{
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);

    pinMode(D6, INPUT);
    pinMode(D5, INPUT);
    pinMode(D8, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    Serial.println(F("Adafruit IO Example"));
    
    // Connect to WiFi access point.
    delay(10);
    Serial.print(F("Connecting to "));
    Serial.println(WLAN_SSID);
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }

    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());

    // connect to adafruit io
    connect();
}


void connect()
{
    Serial.print(F("Connecting to Adafruit IO... "));
    int8_t ret;
    while ((ret = mqtt.connect()) != 0)
    {
        switch (ret)
        {
        case 1:
            Serial.println(F("Wrong protocol"));
            break;
        case 2:
            Serial.println(F("ID rejected"));
            break;
        case 3:
            Serial.println(F("Server unavail"));
            break;
        case 4:
            Serial.println(F("Bad user/pass"));
            break;
        case 5:
            Serial.println(F("Not authed"));
            break;
        case 6:
            Serial.println(F("Failed to subscribe"));
            break;
        default:
            Serial.println(F("Connection failed"));
            break;
        }

        if (ret >= 0)
            mqtt.disconnect();

        Serial.println(F("Retrying connection..."));
        delay(10000);
    }
    Serial.println(F("Adafruit IO Connected!"));
}

void loop()
{
    // ping adafruit io a few times to make sure we remain connected
    if (!mqtt.ping(3))
    {
        // reconnect to adafruit io
        if (!mqtt.connected())
            connect();
    }

    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);

    int data0 = analogRead(A_Out);

    Serial.println(data0);

    if(!soil_moisture0.publish(data0)) //Publish to Adafruit
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("Sent!"));
    }

    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);

    int data1 = analogRead(A_Out);

    Serial.println(data1);

    delay(2000);

    if(!soil_moisture1.publish(data1)) //Publish to Adafruit
    {   
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("Sent!"));
    }
    digitalWrite(S0, LOW);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);

    int data2 = analogRead(A_Out);

    Serial.println(data2);

    delay(2000);

    if (!soil_moisture2.publish(data2)) //Publish to Adafruit
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("Sent!"));
    }
    digitalWrite(S0, HIGH);
    digitalWrite(S1, HIGH);
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);

    int data3 = analogRead(A_Out);

    Serial.println(data3);

    delay(2000);

    if (!soil_moisture3.publish(data3)) //Publish to Adafruit
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("Sent!"));
    }

    dht11.read(&temp, &hum, NULL);
    Serial.print((int)temp);
    Serial.print(" *C, ");
    Serial.print((int)hum);
    Serial.println(" H");
    delay(2000);

    if (!Temperature1.publish(temp)) //Publish to Adafruit
    {
        Serial.println(F("Failed"));
    }
    if (!Humidity1.publish(hum))    //Publish to Adafruit
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("Sent!"));
    }
    
    int vbr_data = vibrate();   //user define fun.

    Serial.println(vbr_data);
    Serial.print(" VBR ");

    if (vbr_data > 50)
    {
        digitalWrite(LED_BUILTIN, LOW);
    }
    else
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }

    delay(2000);

    if (!vibration.publish(vbr_data)) //Publish to Adafruit
    {
        Serial.println(F("Failed"));
    }
    else
    {
        Serial.println(F("Sent!"));
    }
}
int vibrate()
{
    int reading = pulseIn(D5, HIGH);
    return reading;
}

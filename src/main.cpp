/*
    LilyGo Ink Screen Series u8g2Fonts Test
        - Created by Lewis he
*/

// According to the board, cancel the corresponding macro definition
#define LILYGO_T5_V213
// #define LILYGO_T5_V22
// #define LILYGO_T5_V24
// #define LILYGO_T5_V28
// #define LILYGO_T5_V102
// #define LILYGO_T5_V266
// #define LILYGO_EPD_DISPLAY_102
// #define LILYGO_EPD_DISPLAY_154

#include <boards.h>
#define EINKDISPLAY
#include <GxEPD.h>
#include <ESP32Time.h>


#if defined(LILYGO_T5_V102) || defined(LILYGO_EPD_DISPLAY_102)
#include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
#elif defined(LILYGO_T5_V266)
#include <GxDEPG0266BN/GxDEPG0266BN.h> // 2.66" b/w   form DKE GROUP
#elif defined(LILYGO_T5_V213)
#include <GxDEPG0213BN/GxDEPG0213BN.h> // 2.13" b/w  form DKE GROUP
// #include <GxGDE0213B1.h>
#else
// #include <GxGDGDEW0102T4/GxGDGDEW0102T4.h> //1.02" b/w
// #include <GxGDEW0154Z04/GxGDEW0154Z04.h>  // 1.54" b/w/r 200x200
// #include <GxGDEW0154Z17/GxGDEW0154Z17.h>  // 1.54" b/w/r 152x152
// #include <GxGDEH0154D67/GxGDEH0154D67.h>  // 1.54" b/w
// #include <GxDEPG0150BN/GxDEPG0150BN.h>    // 1.51" b/w   form DKE GROUP
// #include <GxDEPG0266BN/GxDEPG0266BN.h>    // 2.66" b/w   form DKE GROUP
// #include <GxDEPG0290R/GxDEPG0290R.h>      // 2.9" b/w/r  form DKE GROUP
// #include <GxDEPG0290B/GxDEPG0290B.h>      // 2.9" b/w    form DKE GROUP
// #include <GxGDEW029Z10/GxGDEW029Z10.h>    // 2.9" b/w/r  form GoodDisplay
// #include <GxGDEW0213Z16/GxGDEW0213Z16.h>  // 2.13" b/w/r form GoodDisplay
// #include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w  old panel , form GoodDisplay
// #include <GxGDEM0213B74/GxGDEM0213B74.h>  // 2.13" b/w  form GoodDisplay 4-color
// #include <GxGDEW0213M21/GxGDEW0213M21.h>  // 2.13"  b/w Ultra wide temperature , form GoodDisplay
// #include <GxDEPG0213BN/GxDEPG0213BN.h>    // 2.13" b/w  form DKE GROUP
// #include <GxGDEW027W3/GxGDEW027W3.h>      // 2.7" b/w   form GoodDisplay
// #include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r form GoodDisplay
// #include <GxGDEH029A1/GxGDEH029A1.h>      // 2.9" b/w   form GoodDisplay
// #include <GxDEPG0750BN/GxDEPG0750BN.h>    // 7.5" b/w   form DKE GROUP
#endif

#include GxEPD_BitmapExamples
// FreeFonts from Adafruit_GFX
#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/Tiny3x3a2pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#include <Wifi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJSON.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

#define SERIAL_DEBUG_ON
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5           /* Time ESP32 will go to sleep (in seconds) */

ESP32Time rtc;
GxIO_Class io(SPI, EPD_CS, EPD_DC, EPD_RSET);
GxEPD_Class display(io, EPD_RSET, EPD_BUSY);

void testWiFi();
//void LilyGo_logo();
void printPanel();
String printLocalTime();
void reconnect();
void printBatStatus(float battery_proc);
void printHeader();
int getWifiStrenght();
void drawWifiStrenght(int bars);
void SecureClientRead();
void batteryRead(void);
String CheckString(String name, String str);

/* Put your SSID & Password */
const char *ssid = "AC0571";         // Enter SSID here
const char *password = "120479wifi"; // Enter Password here
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7200;
const int daylightOffset_sec = 3600;
String wifi_networks;
String weather_server = "https://www.meteoromania.ro/wp-json/meteoapi/v2/starea-vremii";
                         
WiFiClientSecure client; // or WiFiClientSecure for HTTPS
HTTPClient http;

const char *test_root_ca = "-----BEGIN CERTIFICATE-----\n"
                           "MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n"
                           "iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n"
                           "cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n"
                           "BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n"
                           "MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n"
                           "BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n"
                           "aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n"
                           "dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n"
                           "AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n"
                           "3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n"
                           "tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n"
                           "Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n"
                           "VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n"
                           "79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n"
                           "c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n"
                           "Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n"
                           "c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n"
                           "UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n"
                           "Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n"
                           "BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n"
                           "A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n"
                           "Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n"
                           "VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n"
                           "ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n"
                           "8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n"
                           "iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n"
                           "Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n"
                           "XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n"
                           "qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n"
                           "VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n"
                           "L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n"
                           "jjxDah2nGN59PRbxYvnKkKj9\n"
                           "-----END CERTIFICATE-----\n";
String data_meteo;
String temperatura_locala;
String presiunea_locala;
String viteza_vantului_locala;
String nebulozitate;
String umezeala;
String fenomene_electrice;
String zapada;
String actualizat;

String busvoltage_s;
float busvoltage_f;
Adafruit_INA219 ina219;

void setup(void)
{
#ifdef SERIAL_DEBUG_ON  
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#ifdef SERIAL_DEBUG_ON
  Serial.println("Connecting");
#endif
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
#ifdef SERIAL_DEBUG_ON
    Serial.print(".");
#endif
  }
#ifdef SERIAL_DEBUG_ON
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
#endif
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
#ifdef SERIAL_DEBUG_ON
  Serial.println(printLocalTime());
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S")); // (String) returns time with specified format
#endif
#if defined(LILYGO_EPD_DISPLAY_102)
  pinMode(EPD_POWER_ENABLE, OUTPUT);
  digitalWrite(EPD_POWER_ENABLE, HIGH);
#endif /*LILYGO_EPD_DISPLAY_102*/
#if defined(LILYGO_T5_V102)
  pinMode(POWER_ENABLE, OUTPUT);
  digitalWrite(POWER_ENABLE, HIGH);
#endif /*LILYGO_T5_V102*/

  SPI.begin(EPD_SCLK, EPD_MISO, EPD_MOSI);
  display.init(); // enable diagnostic output on Serial

#ifdef SERIAL_DEBUG_ON
  Serial.println("setup done");
#endif
  printPanel();
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1)
    {
      delay(10);
    }
  }

  SecureClientRead();
}

void printPanel()
{

  printHeader();
  /*               x  y width height           */
  /* Title*/
  display.drawRect(0, 25, 250, 20, GxEPD_BLACK);

  /*Content*/
  display.drawRect(0, 45, 250, 65, GxEPD_BLACK);

  /*footer*/
  display.drawRect(0, 108, 250, 20, GxEPD_BLACK);
}

void printHeader()
{
  /*               x  y width height           */
  display.drawRect(0, 6, 100, 20, GxEPD_BLACK);
  display.drawRect(100, 6, 170, 20, GxEPD_BLACK);
  display.drawRect(230, 6, 20, 20, GxEPD_BLACK);
  printBatStatus(busvoltage_f/6.0*100.0);
}

void printBatStatus(float battery_proc)
{
  int number_of_lines_to_draw = (int)round((battery_proc / 100.0 * 12.0));
#ifdef SERIAL_DEBUG_DISPLAY_ON
  Serial.printf("Nr de linii : %d", number_of_lines_to_draw);
#endif

  /* Draw empty battery*/
  /*               x  y width height           */
  display.drawRect(235, 10, 10, 14, GxEPD_BLACK);
  display.drawFastHLine(238, 8, 4, GxEPD_BLACK);
  display.drawFastHLine(238, 9, 4, GxEPD_BLACK);

  /* Fill battery based on battery_proc parameter */
  for (int i = 0; i < number_of_lines_to_draw; i++)
  {
    display.drawFastHLine(235, 22 - i, 10, GxEPD_BLACK);
  }
}

void drawWifiStrenght(int bars)
{
  /*               x  y width height           */
  for (int i = 1; i <= bars; i++)
  {
    if (i == 5)
    {
      display.fillRect(204 + (i * 4), 26 - (i * 4) + 2, 3, (i * 4) - 5, GxEPD_BLACK);
#ifdef SERIAL_DEBUG_DISPLAY_ON
      Serial.printf("\nHeight max: %d", (i * 4) - 5);
      #endif
    }
    else
    {
      display.fillRect(204 + (i * 4), 26 - (i * 4), 3, (i * 4) - 3, GxEPD_BLACK);
#ifdef SERIAL_DEBUG_DISPLAY_ON
      Serial.printf("\nHeight: %d", (i * 4) - 3);
      #endif
    }
  }
}

void loop()
{
  SecureClientRead();
  batteryRead();
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMono9pt7b);
  display.setCursor(2, 20);
  display.println(printLocalTime().c_str());
  display.setCursor(2, 40);
  // testWiFi();
  display.setCursor(110, 20);
  if (WiFi.status() == WL_CONNECTED)
  {
    // display.println("Wifi Connected");
    int bars = getWifiStrenght();
    display.printf("%s(%d)", WiFi.SSID().c_str(), getWifiStrenght());
    drawWifiStrenght(bars);
  }
  else
    display.println("NC");

  printPanel();

  /*Title */
  display.setFont(&FreeMono9pt7b);
  display.setCursor(2, 40);

  // display.printf("(%d dBm) (%d)", WiFi.RSSI(), getWifiStrenght());
  if((data_meteo !="null")/* || (data_meteo.isEmpty()==false)*/)
  {
    display.println("Sibiu:" + rtc.getTime("%Y-%m-%d"));

    /* Content*/
    display.setFont(&FreeMono9pt7b);
    display.println(CheckString("Temperatura:" + temperatura_locala + " C", temperatura_locala));
    display.setCursor(2, 73);
    display.println(CheckString(presiunea_locala, presiunea_locala) );
    display.setCursor(2, 87);

    display.println(CheckString(viteza_vantului_locala, viteza_vantului_locala) +
                    CheckString(" Umezeala:" + umezeala + "%", umezeala));

    //display.println("" + viteza_vantului_locala + " Umezeala:" + umezeala + "%" );

    /* Footer*/
    display.setFont(&FreeMono9pt7b);
    display.setCursor(2, 123);
    display.print("");
    if(busvoltage_f>1.0)
    {
      display.println(CheckString(nebulozitate, nebulozitate) + " V:" + busvoltage_s);
    }
    else
    {
      display.println(CheckString(nebulozitate, nebulozitate));
    }
  }
  else
  {

   display.println("Sibiu:" + rtc.getTime("%d-%m-%Y"));
   display.println("Informatiile meteo sunt indisponibile ! ");
  }
  display.update();

  delay(10000);
}


void testWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();

  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    display.print(n);
    display.println(" networks found");

    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      if (i < 4)
      {
        display.print(WiFi.SSID(i) + "(" + WiFi.RSSI(i) + ")");
        display.println(" ");
      }
      delay(10);
    }
  }
  Serial.println("");
  reconnect();
}

String printLocalTime()
{
  struct tm timeinfo;
  char timeDate[36];
  if (!getLocalTime(&timeinfo))
  {
#ifdef SERIAL_DEBUG_ON
    Serial.println("Failed to obtain time");
#endif
    return rtc.getTime("%H:%M:%S");
    //String("Failed to obtain time");
  }
  else
  {
#ifdef SERIAL_DEBUG_ON
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  char timeHMS[12];
  strftime(timeHour, 3, "%H", &timeinfo);
  strftime(timeHMS, 11, "%H:%M:%S", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
#endif

  strftime(timeDate, 36, "%H:%M:%S", &timeinfo);
  rtc.setTimeStruct(timeinfo);
  return rtc.getTime("%H:%M:%S"); // String(timeDate);
  }
  
}

void reconnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
#ifdef SERIAL_DEBUG_ON
    Serial.println("Reconnecting");
#endif
    delay(1000);
#ifdef SERIAL_DEBUG_ON
    Serial.print(".");
#endif
  }
#ifdef SERIAL_DEBUG_ON
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
#endif

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

int getWifiStrenght()
{
  int bars;
  int RSSI_strgh = 0;
  //  int bars = map(RSSI,-80,-44,1,6); // this method doesn't refelct the Bars well
  // simple if then to set the number of bars
  RSSI_strgh = WiFi.RSSI();
  if (RSSI_strgh > -55)
  {
    bars = 5;
  }
  else if ((RSSI_strgh < -55) && (RSSI_strgh > -65))
  {
    bars = 4;
  }
  else if ((RSSI_strgh < -65) && (RSSI_strgh > -70))
  {
    bars = 3;
  }
  else if ((RSSI_strgh < -70) && (RSSI_strgh > -78))
  {
    bars = 2;
  }
  else if ((RSSI_strgh < -78) & (RSSI_strgh > -82))
  {
    bars = 1;
  }
  else
  {
    bars = 0;
  }
  return bars;
}

void SecureClientRead()
{
  WiFiClientSecure *clientSec = new WiFiClientSecure;
  if (clientSec)
  {
    clientSec->setCACert(test_root_ca );

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;
      https.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*clientSec, weather_server ))
      { // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0)
        {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
          {
           // String payload = https.getString();
           // Serial.println(payload);
            // Parse response
            DynamicJsonDocument doc(65000);
            DynamicJsonDocument doc_sibiu(2000);
            deserializeJson(doc, https.getStream());
            //Serial.println(doc["features"][1].as<String>());
            doc_sibiu = doc["features"][72];
            deserializeJson(doc_sibiu, doc["features"][72].as<String>());
            // Read values
            data_meteo = doc["date"].as<String>();
#ifdef SERIAL_DEBUG_ON
            Serial.println(doc["features"][72].as<String>());
            Serial.println(doc_sibiu["properties"]["tempe"].as<String>());
            Serial.println(doc["date"].as<String>());
 #endif
            temperatura_locala = doc_sibiu["properties"]["tempe"].as<String>();
            presiunea_locala = doc_sibiu["properties"]["presiunetext"].as<String>();
            viteza_vantului_locala = doc_sibiu["properties"]["vant"].as<String>();
            nebulozitate = doc_sibiu["properties"]["nebulozitate"].as<String>();
            umezeala = doc_sibiu["properties"]["umezeala"].as<String>();
            fenomene_electrice = doc_sibiu["properties"]["fenomen_e"].as<String>();
            zapada = doc_sibiu["properties"]["zapada"].as<String>();
            actualizat = doc_sibiu["properties"]["actualizat"].as<String>();
                                          }
        }
        else
        {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
      }
      else
      {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
    delete clientSec;
  }
  else
  {
    Serial.println("Unable to create client");
  }
}
void batteryRead(void)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  busvoltage_s = String(loadvoltage);
  busvoltage_f = loadvoltage;
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(shuntvoltage);
  Serial.println(" mV");
  Serial.print("Load Voltage:  ");
  Serial.print(loadvoltage);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.println("");

  delay(2000);
}

String CheckString(String name, String str)
{
  if ((str == "null") || (str.isEmpty() == true) || (str == "indisponibil"))
  {
    return "Info indisponibila!";
  }
  else
  {
    return name;
  }

}
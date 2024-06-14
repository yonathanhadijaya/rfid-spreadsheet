//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ESP32_RFID_Google_Spreadsheet_Attendance
//----------------------------------------Including the libraries.
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WiFi.h"
#include <HTTPClient.h>
//----------------------------------------

// Defines SS/SDA PIN and Reset PIN for RFID-RC522.
#define SS_PIN  5  
#define RST_PIN 4

// Defines the button PIN.
#define BTN_PIN 15


//----------------------------------------SSID and PASSWORD of your WiFi network.
const char* ssid = "";  //--> Your wifi name
const char* password = ""; //--> Your wifi password
//----------------------------------------

// Google script Web_App_URL.
String Web_App_URL = ""; // Google App Script Link

String reg_Info = "";

String atc_Info = "";
String atc_Name = "";
String atc_Date = "";
String atc_Time_In = "";
String atc_Time_Out = "";

// Variables for the number of columns and rows on the oled.
#define SCREEN_WIDTH 128 // OLED width,  in pixels
#define SCREEN_HEIGHT 64 // OLED height, in pixels

// Variable to read data from RFID-RC522.
int readsuccess;
char str[32] = "";
String UID_Result = "--------";

String modes = "atc";

// create an OLED display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



// Create MFRC522 object as "mfrc522" and set SS/SDA PIN and Reset PIN.
MFRC522 mfrc522(SS_PIN, RST_PIN);  //--> Create MFRC522 instance.

//________________________________________________________________________________http_Req()
// Subroutine for sending HTTP requests to Google Sheets.
void http_Req(String str_modes, String str_uid) {

  if (WiFi.status() == WL_CONNECTED) {
    String http_req_url = "";

    //----------------------------------------Create links to make HTTP requests to Google Sheets.
    if (str_modes == "atc") {
      http_req_url  = Web_App_URL + "?sts=atc";
      http_req_url += "&uid=" + str_uid;
    }
    if (str_modes == "reg") {
      http_req_url = Web_App_URL + "?sts=reg";
      http_req_url += "&uid=" + str_uid;
    }
    //----------------------------------------

    

    //----------------------------------------Sending HTTP requests to Google Sheets.
    Serial.println();
    Serial.println("-------------");
    Serial.println("Sending request to Google Sheets...");
    Serial.print("URL : ");
    Serial.println(http_req_url);

    

    Serial.println("test serial");
    delay(2000);
    
    // Create an HTTPClient object as "http".
    HTTPClient http;

    // HTTP GET Request.
    http.begin(http_req_url.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // Gets the HTTP status code.
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    // Getting response from google sheet.
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);  
    }
    
    Serial.println("-------------");
    http.end();
    //----------------------------------------

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Example :                                                                                              //
    // Sending an http request to fill in "Time In" attendance.                                               //
    // User data :                                                                                            //
    // - Name : Adam                                                                                          //
    // - UID  : A01                                                                                           //
    // So the payload received if the http request is successful and the parameters are correct is as below : //
    // OK,Adam,29/10/2023,08:30:00 ---> Status,Name,Date,Time_In                                              //
    //                                                                                                        //
    // So, if you want to retrieve "Status", then getValue(payload, ',', 0);                                  //
    // String sts_Res = getValue(payload, ',', 0);                                                            //
    // So the value of sts_Res is "OK".                                                                       //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    String sts_Res = getValue(payload, ',', 0);

    //----------------------------------------Conditions that are executed are based on the payload response from Google Sheets (the payload response is set in Google Apps Script).
    if (sts_Res == "OK") {
      //..................
      if (str_modes == "atc") {
        atc_Info = getValue(payload, ',', 1);
        
        if (atc_Info == "TI_Successful") {
          atc_Name = getValue(payload, ',', 2);
          atc_Date = getValue(payload, ',', 3);
          atc_Time_In = getValue(payload, ',', 4);

          // //::::::::::::::::::Create a position value for displaying "Name" on the oled so that it is centered.
          // int name_Lenght = atc_Name.length();
          // int pos = 0;
          // if (name_Lenght > 0 && name_Lenght <= 20) {
          //   pos = map(name_Lenght, 1, 20, 0, (20 / 2) - 1);
          //   pos = ((20 / 2) - 1) - 0;
          // } else if (name_Lenght > 20) {
          //   atc_Name = atc_Name.substring(0, 20);
          // }
          // //::::::::::::::::::

          oled.clearDisplay();
          oled.display();
          delay(500);
          oled.setCursor(4,0);
          oled.print(atc_Name);
          oled.setCursor(0,20);
          oled.print("Date    : ");
          oled.print(atc_Date);
          oled.setCursor(0,30);
          oled.print("Time IN :   ");
          oled.print(atc_Time_In);
          oled.setCursor(0,40);
          oled.print("Time Out:   ");
          oled.display();
          delay(5000);
          oled.clearDisplay();
          oled.display();
          delay(500);
        }

        if (atc_Info == "TO_Successful") {
          atc_Name = getValue(payload, ',', 2);
          atc_Date = getValue(payload, ',', 3);
          atc_Time_In = getValue(payload, ',', 4);
          atc_Time_Out = getValue(payload, ',', 5);

          // //::::::::::::::::::Create a 0ition value for displaying "Name" on the oled so that it is centered.
          // int name_Lenght = atc_Name.length();
          // int 0 = 0;
          // if (name_Lenght > 0 && name_Lenght <= oledColumns) {
          //   0 = map(name_Lenght, 1, oledColumns, 0, (oledColumns / 2) - 1);
          //   0 = ((oledColumns / 2) - 1) - 0;
          // } else if (name_Lenght > oledColumns) {
          //   atc_Name = atc_Name.substring(0, oledColumns);
          // }
          // //::::::::::::::::::

          oled.clearDisplay();
          oled.display();
          delay(500);
          oled.setCursor(4,0);
          oled.print(atc_Name);
          oled.setCursor(0,20);
          oled.print("Date    : ");
          oled.print(atc_Date);
          oled.setCursor(0,30);
          oled.print("Time IN :   ");
          oled.print(atc_Time_In);
          oled.setCursor(0,40);
          oled.print("Time Out:   ");
          oled.print(atc_Time_Out);
          oled.display();
          delay(5000);
          oled.clearDisplay();
          oled.display();
          delay(500);
        }

        if (atc_Info == "atcInf01") {
          oled.clearDisplay();
          oled.display();
          delay(500);
          oled.setCursor(4,0);
          oled.print(atc_Name);
          oled.setCursor(0,20);
          oled.print("You have completed");
          oled.setCursor(4,30);
          oled.print("your  attendance");
          oled.setCursor(4,40);
          oled.print("record for today.");
          oled.display();
          delay(5000);
          oled.clearDisplay();
          oled.display();
          delay(500);
        }

        if (atc_Info == "atcErr01") {
          oled.clearDisplay();
          oled.display();
          delay(500);
          oled.setCursor(6,0);
          oled.print("Error !");
          oled.setCursor(4,20);
          oled.print("Your card or");
          oled.setCursor(4,30);
          oled.print("key chain is");
          oled.setCursor(4,40);
          oled.print("not registered!");
          oled.display();
          delay(5000);
          oled.clearDisplay();
          oled.display();
          delay(500);
        }

        atc_Info = "";
        atc_Name = "";
        atc_Date = "";
        atc_Time_In = "";
        atc_Time_Out = "";
      }
      //..................

      //..................
      if (str_modes == "reg") {
        reg_Info = getValue(payload, ',', 1);
        
        if (reg_Info == "R_Successful") {
          oled.clearDisplay();
          oled.display();
          delay(500);
          oled.setCursor(2,0);
          oled.print("Success!");
          oled.setCursor(0,20);
          oled.print("The UID of your");
          oled.setCursor(4,30);
          oled.print("card or keychain has");
          oled.setCursor(4,40);
          oled.print("been successfully");
          oled.setCursor(4,50);
          oled.print("uploaded.");
          oled.display();
          delay(5000);
          oled.clearDisplay();
          oled.display();
          delay(500);
        }

        if (reg_Info == "regErr01") {
          oled.clearDisplay();
          oled.display();
          delay(500);
          oled.setCursor(2,0);
          oled.print("Error!");
          oled.setCursor(0,20);
          oled.print("The UID of your card");
          oled.setCursor(4,30);
          oled.print("or keychain has been");
          oled.setCursor(4,40);
          oled.print("registered!");
          oled.display();
          delay(5000);
          oled.clearDisplay();
          oled.display();
          delay(500);
        }

        reg_Info = "";
      }
      //..................
    }
    //----------------------------------------
  } else {
    oled.clearDisplay();
    delay(500);
    oled.setCursor(6,0);
    oled.print("Error!");
    oled.setCursor(2,20);
    oled.print("WiFi disconnected");
    oled.display();
    delay(3000);
    oled.clearDisplay();
    oled.display();
    delay(500);
  }
}
//________________________________________________________________________________

//________________________________________________________________________________getValue()
// String function to process the data (Split String).
// I got this from : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________ 

//________________________________________________________________________________getUID()
// Subroutine to obtain UID/ID when RFID card or RFID keychain is tapped to RFID-RC522 module.
int getUID() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
  
  byteArray_to_string(mfrc522.uid.uidByte, mfrc522.uid.size, str);
  UID_Result = str;
  
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  return 1;
}
//________________________________________________________________________________

//________________________________________________________________________________byteArray_to_string()
void byteArray_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';
}
//________________________________________________________________________________

//________________________________________________________________________________VOID SETUP()
void setup(){
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Hello intialize setup");
  delay(1000);

  pinMode(BTN_PIN, INPUT_PULLUP);
  
  // initialize OLED display with I2C address 0x3C
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1);
  }
  Serial.println("Hello again inilialize setup");
  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0,0);
  oled.print("RFID");
  oled.display();

  delay(500);

  // Init SPI bus.
  SPI.begin();      
  // Init MFRC522.
  mfrc522.PCD_Init(); 


  

  //----------------------------------------Set Wifi to STA mode
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  //---------------------------------------- 

  //----------------------------------------Connect to Wi-Fi (STA).
  Serial.println();
  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);

  //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.

  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    oled.clearDisplay();
    oled.display();
    oled.setCursor(5,0);
    oled.print("ESP32 RFID");
    oled.setCursor(0,20);
    oled.print("Google  Sheets");
    oled.setCursor(4,30);
    oled.print("Attendance  System");
    oled.setCursor(0,50);
    oled.print("Made by  YONATHAN HS");
    oled.display();
    delay(3000);
    oled.clearDisplay();
    oled.display();

    oled.setCursor(0,0);
    oled.print("Connecting to SSID");
    oled.display();
    delay(250);

    oled.clearDisplay();
    oled.display();
    delay(250);
    
    if (connecting_process_timed_out > 0) connecting_process_timed_out--;
    if (connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("------------");

  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.print("WiFi connected");
  oled.display();
  delay(2000);
  //::::::::::::::::::
  //----------------------------------------

  oled.clearDisplay();
  oled.display();
  delay(500);
}
//________________________________________________________________________________

//________________________________________________________________________________VOID LOOP()
void loop(){
  // put your main code here, to run repeatedly:

  //----------------------------------------Switches modes when the button is pressed.
  // modes = "reg" means the mode for new user registration.
  // modes = "atc" means the mode for filling in attendance (Time In and Time Out).

  int BTN_State = digitalRead(BTN_PIN);

  if (BTN_State == LOW) {
    oled.clearDisplay();
    oled.display();
    
    if (modes == "atc") {
      modes = "reg";
    } else if (modes == "reg") {
      modes = "atc";
    }
    
    delay(500);
  }
  //----------------------------------------

  // Detect if reading the UID from the card or keychain was successful.
  readsuccess = getUID();

  //----------------------------------------Conditions that are executed if modes == "atc".
  if (modes == "atc") {
    oled.setCursor(5,0);
    oled.print("ATTENDANCE");
    oled.setCursor(0,10);
    oled.print("");
    oled.setCursor(0,20);
    oled.print("Please tap your card");
    oled.setCursor(4,30);
    oled.print("or key chain");
    oled.display();

    if (readsuccess){
      oled.clearDisplay();
      oled.display();
      delay(500);
      oled.setCursor(4,0);
      oled.print("Please wait...");
      oled.setCursor(0,20);
      oled.print("Getting  UID");
      oled.setCursor(4,30);
      oled.print("Successfully");
      oled.setCursor(0,20);
      oled.display();

      http_Req(modes, UID_Result);
    }
  }
  //----------------------------------------

  //----------------------------------------Conditions that are executed if modes == "reg".
  if (modes == "reg") {
    oled.setCursor(4,0);
    oled.print("REGISTRATION");
    oled.setCursor(0,10);
    oled.print("");
    oled.setCursor(0,20);
    oled.print("Please tap your card");
    oled.setCursor(4,30);
    oled.print("or key chain");
    oled.display();

    if (readsuccess){
      oled.clearDisplay();
      oled.display();
      oled.setCursor(4,0);
      oled.print("Please wait...");
      delay(500);
      oled.setCursor(0,20);
      oled.print("Getting  UID");
      oled.setCursor(4,30);
      oled.print("Successfully");
      oled.setCursor(0,40);
      oled.print("UID : ");
      oled.print(UID_Result);
      oled.display();

      http_Req(modes, UID_Result);
    }
  }
  //----------------------------------------

  delay(10);
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
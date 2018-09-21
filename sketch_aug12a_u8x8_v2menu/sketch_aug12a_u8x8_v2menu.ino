//Libarys
#include <HX711_ADC.h> //Scale, Olav Kallhovd
#include <U8g2lib.h> //Display, olikraus
#include <Wire.h> //I2C to display
#include <SPI.h>//to NFC
#include <Adafruit_PN532.h>//NFC

//Defines
  //Pins
    //Scale
      #define hx711_data_pin A2
      #define hx711_clock_pin A3
    //Buttons
      #define bUp 6
      #define bSelect 7
      #define bDown 8
    //NFC
      #define PN532_SCK  (2)
      #define PN532_MOSI (3)
      #define PN532_SS   (4)
      #define PN532_MISO (5)
  //else
    //#define debug
    #define optsLenght 8
    #define curs "->"

//Constants
  //Menu text
    //Main menu
      static const char menu[optsLenght][15] {"Info", "Read NFC", "Save to NFC", "Init NFC", "Reset scale", "Cal. scale",""};
    //Need to be 4 char long each, this is the sector sizeof the NFC (for easyer update the data)
    //Same used in menu
      static const char opts[optsLenght][5] {"WGHT", "TYPE", "HEND", "HBED", "PFAN", "FLOW",""};
    //Types for update (menu)
      static const char filaments[optsLenght][5] {"PLA ", "PETG", "ABS ", "PVA ",""};
  //Text to nfc
    static const char filamentsTXT[optsLenght][50] {"WGHT----TYPEPLA HEND 210HBED  60PFAN 100FLOW 100",
                                                    "WGHT----TYPEPETGHEND 230HBED  85PFAN  60FLOW 100", 
                                                    "WGHT----TYPEABS HEND 240HBED 105PFAN   0FLOW 100",
                                                    "WGHT----TYPEPVA HEND 210HBED  60PFAN 100FLOW 100"};
    static const char settings[optsLenght][15] {"+100", "+10", "+1","-1","-10","-100","Save",""};
    static const char defValue[5] {"----"};

//Variables
//Times
long t_led;
long t_blink; //millis beforte led toggle
long t_scale;
long t_disp;
long t_dispReset;
long t_nfc;
//Menu
static int menuStatus[4];
//NFC
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
uint8_t data[32];
uint8_t dataLength;
//Opts
static char optsValue[optsLenght][5];
char tmpOpt[5];
char tmpVal[5];
char tmpVal2[5];
int spoolWeight;

// Consructor List
//HX711 scale
HX711_ADC LoadCell(hx711_data_pin, hx711_clock_pin);
//Display
//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
//NFC
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(bUp, INPUT_PULLUP);
  pinMode(bSelect, INPUT_PULLUP);
  pinMode(bDown, INPUT_PULLUP);
  t_blink = 500;

  //Display
  menuReset();
  u8x8.begin();
  //u8x8.setFont(u8x8_font_victoriabold8_r );
  u8x8.setFont(u8x8_font_victoriamedium8_r  );
  u8x8.draw2x2String(0, 3, "Start();");

  //NFC
  nfc.begin();
  nfc.SAMConfig();
  optReset();

  //Debug/Printer
#ifdef debug
  Serial.begin(9600);
  strcpy(tmpOpt, "TYPE");
  strcpy(tmpVal, "PLA ");
  optSave(tmpOpt, tmpVal);
  strcpy(tmpOpt, "HEND");
  strcpy(tmpVal, " 210");
  optSave(tmpOpt, tmpVal);
  strcpy(tmpOpt, "HBED");
  strcpy(tmpVal, "  60");
  optSave(tmpOpt, tmpVal);
  strcpy(tmpOpt, "PFAN");
  strcpy(tmpVal, " 100");
  optSave(tmpOpt, tmpVal);
#endif

  //HX711 scale
  LoadCell.begin(64);
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  LoadCell.start(stabilisingtime);
  LoadCell.setCalFactor(225.0); // user set calibration factor (float)

  //Clear disp
  u8x8.clear();
}

// the loop function runs over and over again forever
void loop() {
  //Led
  if (millis() > t_led + t_blink) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    t_led = millis();
  }
  //Scale
  LoadCell.update();
#ifdef debug
  if (millis() > t_scale + 500) {
    //float i = LoadCell.getData();
    //Serial.print("Load_cell output val: ");
    //Serial.println(i);
    t_scale = millis();
  }
#endif
  //Disp
  if (millis() > t_dispReset + 10000) {
    u8x8.clear();
    t_dispReset = millis();
  }
  if (millis() > t_disp + 200) {
    dispUpdate();
    t_disp = millis();
  }

}

//Functions
//  In tabs

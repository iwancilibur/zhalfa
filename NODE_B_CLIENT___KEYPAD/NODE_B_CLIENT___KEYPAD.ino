#define periodaKeypad         2000

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//char ssid[] = "TOWER_AP";               // SSID of your home WiFi
//char pass[] = "TOWER_AP_PASS";               // password of your home WiFi
char ssid[] = "iwan";               // SSID of your home WiFi
char pass[] = "cilibur2019";               // password of your home WiFi

unsigned long askTimer = 0;

IPAddress server(192,168,43,3);       // the fix IP address of the server
WiFiClient client;
String datax="Menunggu Pesan";
String answer;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {D1, D2, D3, D4}; 
byte colPins[COLS] = {D5, D6, D7}; 

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
int waktu_sebelumnya = 0;
int waktu_sebelumnya2 = 0;
 
char pad[11][11] = {
  "0 ",
  "1?!*#:=/+-",
  "2AaBbCc",
  "3DdEeFf",
  "4GgHhIi",
  "5JjKkLl",
  "6MmNnOo",
  "7PpQqRrSs",
  "8TtUuVvWw",
  "9XxYyZz",
};
 
byte padCounter;
char padChar;
bool padDitekan;
byte charCounter;
byte keySebelumnya;
char bufferKeypad[17];
char *bufferKeypadPtr;
 
long millisKeypad;

void setup() {
  Serial.begin(115200);               // only for debug
  lcd.begin();
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }
  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());
  resetInput();
  //lcd.setCursor(0,0); lcd.print("Menunggu Pesan");
}

void loop () {
unsigned long waktu_sekarang = millis();
unsigned long waktu_sekarang2 = millis();

if(waktu_sekarang - waktu_sebelumnya >= 10){
  waktu_sebelumnya = waktu_sekarang;
  client.connect(server, 80);   // Connection to the server
  //Serial.println(".");
  //client.println("Hello server! Are you sleeping?\r");  // sends the message to the server
  client.println(datax);
  answer = client.readStringUntil('\r');   // receives the answer from the sever
  //Serial.println("from server: " + answer);
   lcd.setCursor(0,0); lcd.print("                ");
   lcd.setCursor(0,0); lcd.print(answer);
   //lcd.setCursor(padCounter, 1);
   //lcd.blink();
  client.flush();
}

if(waktu_sekarang2 - waktu_sebelumnya2 >= 0){
  waktu_sebelumnya2 = waktu_sekarang2;
 panggilKeypad();
}

}


void panggilKeypad() {
  char key = keypad.getKey();
 
  if (key) {
    switch (key)
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
 
        millisKeypad = millis() + periodaKeypad;
        if ((key == keySebelumnya) || (keySebelumnya == 0))
        {
          padChar = pad[key - '0'][charCounter];
          keySebelumnya = key;
        }
        else if ((padDitekan) && (padCounter < sizeof(bufferKeypad) - 1))
        {
          *bufferKeypadPtr++ = padChar;
          keySebelumnya = key;
          charCounter = 0;
          padCounter++;
          padChar = pad[key - '0'][charCounter];
        }
 
        padDitekan = true;
 
        lcd.setCursor(padCounter, 1);
        lcd.print(padChar);
        Serial.println(padChar);
        lcd.setCursor(padCounter, 1);
 
        charCounter++;
        if (!pad[key - '0'][charCounter])
        {
          charCounter = 0;
        }
        break;
 
      case '*':
        if (padCounter)
        {
          if (keySebelumnya)
          {
            keySebelumnya = 0;
          }
          lcd.setCursor(padCounter, 1);
          lcd.print(' ');
          charCounter = 0;
          padCounter--;
          bufferKeypadPtr--;
          padChar = *bufferKeypadPtr;
 
          lcd.setCursor(padCounter, 1);
        }
        else
        {
          resetInput();
        }
        break;
      case '#':
        if ((padDitekan) && (padCounter < sizeof(bufferKeypad) - 1))
        {
          *bufferKeypadPtr++ = padChar;
        }
        *bufferKeypadPtr = 0;
 
        Serial.print("String input = ");
        Serial.println(bufferKeypad);
        datax=bufferKeypad;
        lcd.clear();
        //lcd.noBlink();
        lcd.setCursor(0, 0);
        lcd.print("Kirim Pesan = ");
        lcd.setCursor(0, 1);
        lcd.print(bufferKeypad);
        delay(3000);
 
        resetInput();
        break;
    }
    
  }
 
  if ((padDitekan) && (padCounter < sizeof(bufferKeypad) - 1))
  {
    if (millisKeypad < millis())
    {
      *bufferKeypadPtr++ = padChar;
      keySebelumnya = key;
      charCounter = 0;
      padCounter++;
      padDitekan = false;
 
      lcd.setCursor(padCounter, 1);
      lcd.print(' ');
      lcd.setCursor(padCounter, 1);
    }
  }
}
 
void resetInput()
{
  bufferKeypadPtr = bufferKeypad;
  charCounter = 0;
  padCounter = 0;
  keySebelumnya = 0;
  padDitekan = false;
 
  lcd.clear();
  //lcd.print("Masukkan string");
  //lcd.setCursor(0,0); lcd.print("Menunggu Pesan");
  //lcd.setCursor(padCounter, 1);
  //lcd.blink();
}

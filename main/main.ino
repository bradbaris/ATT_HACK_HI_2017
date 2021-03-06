/**************************************************************************/
/*! 
    @file      main.ino
    @author    Bradford Baris
    @author    Adafruit Industries
    @license   BSD (see license.txt)

    This example will wait for any ISO14443A card or tag, and
    depending on the size of the UID will attempt to read from it.
   
    If the card has a 4-byte UID it is probably a Mifare
    Classic card, and the following steps are taken:
   
    - Authenticate block 4 (the first block of Sector 1) using
      the default KEYA of 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
    - If authentication succeeds, we can then read any of the
      4 blocks in that sector (though only block 4 is read here)

*/
/**************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <Adafruit_RGBLCDShield.h>

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Define color constants
#define RED 0x1
#define GREEN 0x2
#define YELLOW 0x3
#define BLUE 0x4
#define VIOLET 0x5
#define TEAL 0x6
#define WHITE 0x7

// Declare LCD Shield and RFID Shield
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {

  // Start devices
  Serial.begin(115200);
  lcd.begin(16, 2);
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  //Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  //Serial.println("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  //Serial.print("."); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // Configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("System running. Waiting for an ISO14443A Card ...");
  lcd.setBacklight(WHITE);
  lcd.print("SWIPE CARD");
  lcd.print("");

}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  char rfiduid[15];

  // Wait for an ISO14443A type card. When one is found
  // 'uid' will be populated with the UID. A 40byte uidLength indicates a Mifare Classic card.
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    sprintf( rfiduid, "%02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3] );
  
    String str(rfiduid);
    nfc.PrintHex(uid, uidLength);
    lcd.setBacklight(YELLOW);
                          // lol kludge
    if (uidLength == 4 && str.indexOf("413035") == -1)
    {
      Serial.println("Appears to be a Mifare Classic card (4 byte UID)");
    
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Attempting to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    
      // Start with block 4 (the first block of sector 1) since sector 0
      // contains the manufacturer data and it's probably better just
      // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
    
      if (success)
      {
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        uint8_t data[16];
        lcd.setBacklight(GREEN);
        
        // If you want to write something to block 4 to test with, uncomment
        // the following line and this text should be read back in a minute
        // memcpy(data, (const uint8_t[]){ 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0 }, sizeof data);
        
        // Write '255' to block 4 on the card
        // memcpy(data, (const uint8_t[]){ 'f', 'f', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, sizeof data);
        // success = nfc.mifareclassic_WriteDataBlock (4, data);
        
        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(4, data);

        // Data seems to have been read
        if (success)
        {
          lcd.setBacklight(GREEN);
          lcd.setCursor(0, 0);
          if (str == "8BAD0ADD") {
            lcd.print("BRADFORD BARIS  ");
            Serial.println("NAME=Bradford Baris");
          }
          if (str == "FB1611DD") {
            lcd.print("YUKIO YAMAMOTO  ");
            Serial.println("NAME=Yukio Yamamoto");
          }
          char wat[] = { data[0], data[1] };
          int number = (int) strtol( wat, NULL, 16);
          (number < 3) ? number = 0 : number -= 3;
          String balance = String(number, HEX);
          wat[0] = balance[0];
          wat[1] = balance[1];
          memcpy(data, wat, 2);
          success = nfc.mifareclassic_WriteDataBlock (4, data);
          lcd.setCursor(0, 1);
          if (number == 0) {
            lcd.setBacklight(RED);
            lcd.print("Insufficient $$");
            Serial.println("FUNDS=NO");
          } else {
            lcd.print("Balance: $");
            lcd.print(number);
            Serial.print("BAL="); Serial.print(number);
            Serial.println("");
            Serial.println("SEND");
            Serial.println("");
          }
          nfc.PrintHexChar(data, 16);
          Serial.println("");
      
          // Wait a bit before reading the card again
          delay(3000);
          lcd.clear();
          lcd.setBacklight(WHITE);
          lcd.setCursor(0, 0);
          lcd.print("READER:ACTIVE   ");
          lcd.setCursor(0, 1);
          lcd.print("                ");
        }
        else
        {
          Serial.println("Unable to read requested block!");
          lcd.setBacklight(RED);
        }
      }
    } else {
      // lol massive kludge
      if (str.indexOf("45D06A") != -1) {
        lcd.setBacklight(GREEN);
        lcd.setCursor(0, 0);
        lcd.print("AISIS ZANE      ");
        Serial.println("NAME=Aisis Zane");
        int number = 50;
        lcd.setCursor(0, 1);
        lcd.print("Balance: $50");
        Serial.print("BAL=50");
        Serial.println("");
        Serial.println("SEND");
        Serial.println("");
    
        // Wait a bit before reading the card again
        delay(3000);
        lcd.clear();
        lcd.setBacklight(WHITE);
        lcd.setCursor(0, 0);
        lcd.print("READER:ACTIVE   ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
                
      } else {
        Serial.println("Authentication failure!");
        lcd.setBacklight(RED);
      }    
    }
  }
}


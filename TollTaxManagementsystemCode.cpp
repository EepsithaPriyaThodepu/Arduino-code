#include <SPI.h>
#include <MFRC522.h> 
#include <Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Correct library for I2C LCD

#define SS_PIN 10 
#define RST_PIN 9 
#define BUZZER_PIN 3
#define RELAY_PIN 4
#define SERVO_PIN 5 
#define RED_LED_PIN 6 
#define GREEN_LED_PIN 7

MFRC522 mfrc522(SS_PIN, RST_PIN); 
Servo gateServo; 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize LCD with I2C address 0x27

// Predefined RFID Card UIDs
String validUIDs[] = {"A1 B2 C3 D4", "E5 F6 G7 H8"};
const int NUM_UIDS = sizeof(validUIDs) / sizeof(validUIDs[0]);

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    gateServo.attach(SERVO_PIN);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Toll Booth Ready");

    gateServo.write(0); // Close gate initially
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    String readUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        readUID += String(mfrc522.uid.uidByte[i], HEX);
        if (i < mfrc522.uid.size - 1) readUID += " ";
    }
    readUID.toUpperCase();
    Serial.println("Card UID: " + readUID);

    if (isAuthorized(readUID)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Granted");
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        delay(200);
        digitalWrite(BUZZER_PIN, LOW);
        openGate();
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied");
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
        delay(1000);
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RED_LED_PIN, LOW);
    }

    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Toll Booth Ready");
}

bool isAuthorized(String uid) {
    for (int i = 0; i < NUM_UIDS; i++) {
        if (uid == validUIDs[i]) {
            return true;
        }
    }
    return false;
}

void openGate() {
    digitalWrite(RELAY_PIN, HIGH);
    gateServo.write(90);
    delay(5000); // Keep gate open for 5 seconds
    gateServo.write(0);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
}


 

     

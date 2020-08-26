#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial3);
char buffer[3];
char bufferlcd[20];
String strlcd;
int i, j, id, check, p;
void setup() {
  pinMode(23, OUTPUT);
  digitalWrite(23, LOW);
  Serial.begin(9600);
  finger.begin(57600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Welcome (^_$)'");
}

void loop() {
  if (Serial.available()) {
    for (i = 0; i <= 2; i++) {
      buffer[i] = "\0";
    }
    i = 0;
    while (Serial.available()) {
      buffer[i++] = (char)Serial.read();
      delay(50);
    }
    id = atoi(buffer);
    switch (id) {
      case 987: {
          break;
        }
      case 999: {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Nhap van tay...");
          for (i = 0; i <= 2; i++) {
            buffer[i] = "\0";
          }
          while (1) {
            if (Serial.available()) {
              for (i = 0; i < 20; i++) {
                bufferlcd[i] = "\0";
              }
              i = 0;
              while (Serial.available()) {
                bufferlcd[i++] = (char)Serial.read();
                delay(50);
              }
              if (i <= 3) {
                check = atoi(bufferlcd);
                if (check == 987) {
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Nhap van tay...");
                  break;
                }
              } else {
                for (j = 0; j < i; j++) {
                  strlcd += bufferlcd[j];
                }
                lcd.clear();
                lcd.setCursor(2, 1);
                lcd.print("Xin cam on!!");
                lcd.setCursor(0, 0);
                lcd.print(strlcd);
                strlcd = "";
              }
            }
            p = finger.getImage();
            if (p == FINGERPRINT_OK) {
              p = finger.image2Tz();
              if (p == FINGERPRINT_OK) {
                p = finger.fingerFastSearch();
                if (p == FINGERPRINT_OK) {
                  Serial.print(finger.fingerID);
                  digitalWrite(23, HIGH);
                  delay(150);
                  digitalWrite(23, LOW);
                }
              }
            }
            delay(50);
          }
          break;
        }
      default: {
          for (i = 0; i <= 2; i++) {
            buffer[i] = "\0";
          }
          lcd.clear();
          lcd.setCursor(5, 0);
          lcd.print("ID: ");
          lcd.print(id);
          lcd.setCursor(0, 1);
          lcd.print("Nhap van tay...");
          while (1) {
            if (finger.getImage() == FINGERPRINT_OK) {
              digitalWrite(23, HIGH);
              delay(150);
              digitalWrite(23, LOW);
              digitalWrite(23, HIGH);
              delay(150);
              digitalWrite(23, LOW);
              delay(250);
              digitalWrite(23, HIGH);
              delay(150);
              digitalWrite(23, LOW);
              finger.image2Tz(1);
              lcd.clear();
              lcd.setCursor(5, 0);
              lcd.print("ID: ");
              lcd.print(id);
              lcd.setCursor(0, 1);
              lcd.print("Nhap lai VT...");
              while (1) {
                if (finger.getImage() == FINGERPRINT_OK) {
                  digitalWrite(23, HIGH);
                  delay(150);
                  digitalWrite(23, LOW);
                  finger.image2Tz(2);
                  finger.createModel();
                  finger.storeModel(id);
                  Serial.print("987");
                  lcd.clear();
                  lcd.setCursor(5, 0);
                  lcd.print("ID: ");
                  lcd.print(id);
                  lcd.setCursor(0, 1);
                  lcd.print("Nhap thanh cong!");
                  break;
                }
                delay(50);
              }
              break;
            }
            delay(50);
          }
          break;
        }
    }
  }
  delay(50);
}

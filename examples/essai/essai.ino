#include <FeetechServo.h>
#include <INST.h>
#include <SCS.h>
#include <SCSCL.h>
#include <SCSerial.h>
#include <SCServo.h>
#include <SMS_STS.h>

/*
恒速模式例子在STS3215中测试通过，舵机出厂速度单位V=0为停止状态
*/

#include <SCServo.h>
HardwareSerial Serial1(PA10, PA9);
SMS_STS sms_sts;
// int LED_BUILTIN = 13;
const byte SERVO_ID_1       = 1;
const byte SERVO_ID_2       = 2;
const int  PAS_PAR_TOUR   = 4096;
const float  NB_TOURS       = 4;
const int  DEPLACEMENT    = NB_TOURS * PAS_PAR_TOUR;  // 8192 pas
const int  VITESSE        = 4000;  // pas/s (~29 tr/min)
const byte ACCELERATION   = 50;    // x100 pas/s²
const uint8_t circonference = 3.14*70; // roue 70 mm de diametre
uint16_t current_position;
uint8_t cp_low;
uint8_t cp_high;

void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial1.begin(1000000);
  Serial.begin(115200);
  sms_sts.pSerial = &Serial1;
  delay(1000);
  // sts.unLockEprom(1);
  sms_sts.writeByte(SERVO_ID_1, SMS_STS_MODE, 3);
  sms_sts.writeByte(SERVO_ID_2, SMS_STS_MODE, 3);

  // sts.LockEprom(1);
  // lire_tous_les_registres(1);
  sms_sts.writeByte(SERVO_ID_1, 0x09, 0); // Butée min à 0
  sms_sts.writeByte(SERVO_ID_1, 0x0A, 0); // Butée min à 0
  sms_sts.writeByte(SERVO_ID_1, 0x0B, 0); // Buté max à 0, désactivation de la butée
  sms_sts.writeByte(SERVO_ID_1, 0x0C, 0); // Buté max à 0, désactivation de la butée
  sms_sts.writeByte(SERVO_ID_2, 0x09, 0); // Butée min à 0
  sms_sts.writeByte(SERVO_ID_2, 0x0A, 0); // Butée min à 0
  sms_sts.writeByte(SERVO_ID_2, 0x0B, 0); // Buté max à 0, désactivation de la butée
  sms_sts.writeByte(SERVO_ID_2, 0x0C, 0); // Buté max à 0, désactivation de la butée
  // sms_sts.WritePosEx(SERVO_ID_1, DEPLACEMENT, VITESSE, ACCELERATION);
  // lire_tous_les_registres(SERVO_ID_1);
  // lire_tous_les_registres(SERVO_ID_2);
  // avancer(1000);
  sms_sts.WritePosEx(SERVO_ID_1, -20000, VITESSE, ACCELERATION);
  // attendreFinMouvement();
}

int lire_registre(int SERVO_ID_1, int registre_adresse){
  return sms_sts.readByte(SERVO_ID_1, registre_adresse); // Resolution à 1
}

void lire_tous_les_registres(int SERVO_ID_1){
  char message[200]="";
  for (int i=0; i<0x46; i++){
    sprintf(message, "registre 0x%x: %d\n", i, lire_registre(SERVO_ID_1, i));
    Serial.print(message);
  }

}

// Bloque tant que le servo n'a pas fini son deplacement
void attendreFinMouvement() {
  delay(20);  // laisse le temps au drapeau "moving" de passer a 1
  while (((sms_sts.ReadMove(SERVO_ID_1) == 1)) || (sms_sts.ReadMove(SERVO_ID_2) == 1)) {
    delay(20);
  }
}

void avancer(uint16_t distance_mm){
  uint8_t nb_tours = distance_mm/circonference;
  int deplacement = nb_tours*PAS_PAR_TOUR;
  sms_sts.WritePosEx(SERVO_ID_1, -deplacement, VITESSE, ACCELERATION);
  sms_sts.WritePosEx(SERVO_ID_2, deplacement, VITESSE, ACCELERATION);
}

void reculer(uint16_t distance_mm){
  uint8_t nb_tours = distance_mm/circonference;
  int deplacement = nb_tours*PAS_PAR_TOUR;
  sms_sts.WritePosEx(SERVO_ID_1, deplacement, VITESSE, ACCELERATION);
  sms_sts.WritePosEx(SERVO_ID_2, -deplacement, VITESSE, ACCELERATION);
}

void tourner_droite(uint16_t degre){
  float nb_tours = (float)degre/360;
  float correcttion = 1.245;
  int deplacement = nb_tours*PAS_PAR_TOUR*correcttion;
  Serial.printf("degre = %d\n", degre);
  Serial.println(nb_tours);
  Serial.println(deplacement);
  sms_sts.WritePosEx(SERVO_ID_1, -deplacement, VITESSE, ACCELERATION);
  sms_sts.WritePosEx(SERVO_ID_2, -deplacement, VITESSE, ACCELERATION);
}

void tourner_gauche(uint16_t degre){
  float nb_tours = (float)degre/360;
  float correcttion = 1.245;
  int deplacement = nb_tours*PAS_PAR_TOUR*correcttion;
  Serial.printf("degre = %d\n", degre);
  Serial.println(nb_tours);
  Serial.println(deplacement);
  sms_sts.WritePosEx(SERVO_ID_1, deplacement, VITESSE, ACCELERATION);
  sms_sts.WritePosEx(SERVO_ID_2, deplacement, VITESSE, ACCELERATION);
}

void stop(void){
  sms_sts.writeByte(SERVO_ID_1, 0x28, 0); // disable torque motor 1;
  sms_sts.writeByte(SERVO_ID_2, 0x28, 0); // disable torque motor 2;
}
void loop()
{

  // lire_tous_les_registres(SERVO_ID_1);
  // lire_tous_les_registres(SERVO_ID_2);
  digitalWrite(LED_BUILTIN, HIGH);
  avancer(500);
  attendreFinMouvement();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
  reculer(500);
  attendreFinMouvement();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  tourner_droite(90);
  delay(1000);
  tourner_droite(90);
  delay(1000);
  tourner_droite(90);
  delay(1000);
  tourner_droite(90);
  delay(1000);
  tourner_gauche(360);

  delay(10000);
}

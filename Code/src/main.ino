#include <Arduino.h>

unsigned char start = 0;
int pattern = 0;
// uint16_t sensorValue[8];
// uint16_t sensorPID[8];
// uint16_t black_value[8] = {964,962,934,934,977,955,952,967};
// uint16_t white_value[8] = {762,756,737,720,794,759,808,797};
// uint16_t compare_value[8];
unsigned int sensorValue[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int sensorPID[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int black_value[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int white_value[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int compare_value[8] = {0, 0, 0, 0, 0, 0, 0, 0};
float kp;
int kd;
volatile int lastPos;
volatile unsigned char isCalib = 0;
volatile int servoPwm;
volatile unsigned char sensor;
volatile int cnt = 0; // Đổi thành volatile để an toàn với ISR
int RememberLine = 0; // Biến mới để nhớ hướng line mất
int LineCount = 0;    // Biến đếm số line

#define BUZZER_PIN 13
#define SW1_PIN 12
#define SW2_PIN 9
#define DIPSTW1_PIN 10
#define DIPSTW2_PIN 11
#define NPN_SENSOR_PIN 2
#define AIN1_PIN 8
#define AIN2_PIN 7
#define PWMA_PIN 6
#define PWMB_PIN 5
#define BIN1_PIN 4
#define BIN2_PIN 3
void test1();
void test2();
void speed_run(int speedDC_left, int speedDC_right);
void handleAndSpeed(int angle, int speed1);
void read_sensor_v1();
void read_sensor();
unsigned char sensorMask(unsigned char mask);
unsigned char sensorE();
void runforwardline(int tocdo);
void update_sensor();
void beep(int duration);
void analog_raw();

void timer_init()
{
  // Timer/Counter 2 initialization
  ASSR = (0 << EXCLK) | (0 << AS2);
  TCCR2A = (0 << COM2A1) | (0 << COM2A0) | (0 << COM2B1) | (0 << COM2B0) | (0 << WGM21) | (0 << WGM20);
  TCCR2B = (0 << WGM22) | (1 << CS22) | (1 << CS21) | (1 << CS20);
  TCNT2 = 0xB2;
  OCR2A = 0x00;
  OCR2B = 0x00;
  // Timer/Counter 2 Interrupt(s) initialization
  TIMSK2 = (0 << OCIE2B) | (0 << OCIE2A) | (1 << TOIE2);
}
ISR(TIMER2_OVF_vect)
{
  TCNT2 = 0xB2;
  read_sensor();
  cnt++;
}

void setup()
{
  // for (int i = 0; i < 8; i++)
  // {
  //   compare_value[i] = (black_value[i] + white_value[i]) / 2;
  // }
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);     // Turn the buzzer off
  pinMode(SW1_PIN, INPUT_PULLUP);     // left switch
  pinMode(SW2_PIN, INPUT_PULLUP);     // right switch
  pinMode(DIPSTW1_PIN, INPUT_PULLUP); // upper switch
  pinMode(DIPSTW2_PIN, INPUT_PULLUP); // lower switch
  pinMode(NPN_SENSOR_PIN, INPUT);
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(PWMB_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  timer_init();
  isCalib = 1;
  pattern = 0;
  start = 0;
}
void loop1()
{
  speed_run(100, 0);
}
void loop()
{
  // while (start == 0)
  // {
  //   update_sensor();
  //   if (digitalRead(DIPSTW1_PIN) == LOW && digitalRead(DIPSTW2_PIN) == LOW){
  //     if (digitalRead(SW1_PIN) == LOW)
  //     {
  //       isCalib = 0;
  //       start = 1;
  //       pattern = 9;
  //       cnt = 0;
  //     }
  //   }
  //   else if (digitalRead(DIPSTW1_PIN) == LOW && digitalRead(DIPSTW2_PIN) == HIGH){
  //     isCalib = 0;
  //     Serial.print(sensor, BIN);
  //     for(int i=0;i<8;i++){
  //       Serial.print(" | S");
  //       Serial.print(i);
  //       Serial.print(": ");
  //       Serial.print(sensorPID[i]);
  //     }
  //     Serial.print(" | Servo PWM: ");
  //     Serial.println(servoPwm);
  //   }
  //   else if (digitalRead(DIPSTW1_PIN) == HIGH && digitalRead(DIPSTW2_PIN) == LOW)
  //   {
  //     isCalib = 1;
  //     update_sensor();
  //   }
  //   else
  //   {
  //     if (digitalRead(SW1_PIN) == LOW)
  //     {
  //       isCalib = 0;
  //       start = 1;
  //       pattern = 10;
  //       cnt = 0;
  //     }
  //     if (digitalRead(SW2_PIN) == LOW)
  //     {
  //       isCalib = 0;
  //       start = 1;
  //       pattern = 11;
  //       cnt = 0;
  //     }
  //   }
  // }
  while (start == 0)
  {
    update_sensor();
    if(digitalRead(DIPSTW1_PIN)== HIGH && digitalRead(DIPSTW2_PIN)== HIGH)
     {
    if (digitalRead(SW1_PIN) == LOW)
    {
      isCalib = 0;
      start = 1;
      pattern = 10;
      cnt = 0;
    }
    if (digitalRead(SW2_PIN) == LOW)
    {
      isCalib = 0;
      start = 1;
      pattern = 11;
      cnt = 0;
    }
  }
  else if(digitalRead(DIPSTW1_PIN)== LOW && digitalRead(DIPSTW2_PIN)== LOW)
     {
    if (digitalRead(SW1_PIN) == LOW)
    {
      isCalib = 0;
      start = 1;
      pattern = 14;
      cnt = 0;
    }
    if (digitalRead(SW2_PIN) == LOW)
    {
      isCalib = 0;
      start = 1;
      pattern = 341;
      cnt = 0;
    }
  }
}
  // setservotime(nang, ServoPWMChannel3);
  // speed_run(0  ,0);
  switch (pattern)
  {
  case 9:
    runforwardline(0);
    break;
  case 10:
    if (cnt >= 450)
    { // Scale từ 90 cho 5ms (giữ ~450ms)
      pattern = 11;
      cnt = 0; // Reset cnt
      break;
    }
    // if (sensorMask(0x0e) == 0x0e)
    // {
    //   int oldCnt = cnt;
    //   while (cnt - oldCnt < 60)
    //   {                    // Giữ trong 100 ticks (khoảng 0.5 giây)
    //     speed_run(100, 100); // di chuyển thẳng
    //   }
    // }
    runforwardline(150);
    break;
  case 11:
    runforwardline(150);
    if (sensorMask(0x1f) == 0x1f || sensorMask(0x0f) == 0x0f)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 13;
      break;
    }
    break;
  case 13:
    speed_run(100, 25);
    if (cnt >= 80)
    {
      cnt = 0;
      pattern = 214;
      break;
    }
    break;
  case 214:
    speed_run(100, -50);
    if (sensorMask(0x3c) != 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 215;
      break;
    }
    break;
  case 215:
    speed_run(100, -50);
    if (sensorMask(0xfc) != 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 216;
      break;
    }
    break;
  case 216: // hoan thanh quet phai
    runforwardline(180);
    if (cnt >= 100)
    {
      cnt = 0;
      pattern = 14;
      break;
    }
    break;
  case 14:
    runforwardline(180);
    if (sensorMask(0x1f) == 0x1f || sensorMask(0x0f) == 0x0f || sensorMask(0xf0) == 0xf0 || sensorMask(0xf8) == 0xf8)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 15;
      break;
    }
    break;
  case 15: // quet trai nga 4
    speed_run(25, 100);
    if (cnt >= 80)
    {
      cnt = 0;
      pattern = 151;
      break;
    }
    break;
  case 151:
    speed_run(-50, 100);
    if (sensorMask(0x3c) != 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 141;
      break;
    }
    break;
  case 141: // hoan thanh quet

    runforwardline(180);
    if (sensorMask(0x1f) == 0x1f || sensorMask(0x0f) == 0x0f || sensorMask(0x07) == 0x07)
    {
      cnt = 0;
      pattern = 142;
      break;
    }
    if (digitalRead(NPN_SENSOR_PIN) == LOW) // object detected
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 100; // stop
      break;
    }
    break;
  case 142:
    speed_run(150, 25);
    if (cnt >= 80)
    {
      cnt = 0;
      pattern = 143;
      break;
    }
    break;
  case 143:
    speed_run(150, -75);
    if (sensorMask(0xc3) != 0x00)
    {
      cnt = 0;
      pattern = 16;
      break;
    }
    break;
  case 16:

    runforwardline(100);
    if (sensorE() == 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 17;
      break;
    }
    break;
  case 17:
    speed_run(100, -50);
    if (cnt >= 50)
    {
      cnt = 0;
      pattern = 18;
      break;
    }
    break;
  case 18:
    speed_run(100, -100);
    if (sensorMask(0x3c) != 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 19;
      break;
    }
    break;
  case 19:
    runforwardline(150);
    if (sensorE() == 0xf0)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 100;
      break;
    }
    if(cnt >= 100){
      cnt = 0;
      pattern = 34;
      break;
    }
    break;
  case 20:
    speed_run(50, 50);
    if (cnt >= 80)
    {
      cnt = 0;
      pattern = 21;
      break;
    }
    break;
  case 21: // check point so 4
    speed_run(-150, 75);
    if (sensorMask(0x3c) != 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 34;
      break;
    }
    break;
  case 34:
    runforwardline(180);
    if (sensorMask(0x1f) == 0x1f || sensorMask(0x0f) == 0x0f || sensorMask(0xf0) == 0xf0 || sensorMask(0xf8) == 0xf8)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 35;
      break;
    }
    break;
  case 35: // quet trai nga 4
    speed_run(25, 100);
    if (cnt >= 80)
    {
      cnt = 0;
      pattern = 351;
      break;
    }
    break;
  case 351:
    speed_run(-50, 100);
    if (sensorMask(0x3c) != 0x00)
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 341;
      break;
    }
    break;
  case 341: // hoan thanh quet
    runforwardline(100);
    if (digitalRead(NPN_SENSOR_PIN) == LOW) // object detected
    {
      speed_run(0, 0);
      beep(50);
      cnt = 0;
      pattern = 342; // stop
      break;
    }
    break;
  case 342:
    speed_run(-70,-70);
    if (cnt >= 80)
    {
      cnt = 0;
      pattern = 343;
      break;
    }
    break;
  case 343:
    speed_run(70, -70);
    if (cnt >= 50)
    {
      cnt = 0;
      pattern = 344;
      break;
    }
    break;
  case 344:
    speed_run(70,70);
    if (cnt >= 200)
    {
      cnt = 0;
      pattern = 345;
      break;
    }
    break;
  case 345:
    speed_run(-70,70);
    if(cnt >=70){
      cnt =0;
      pattern =348;
      break;
    }
    break;
  case 348:
    speed_run(70,70);
    if(sensorMask(0x3c) != 0x00){
      cnt =0;
      pattern =100;
      break;
    }
    break;
  case 349:
    speed_run(50,50);
    if(cnt >=70){
      cnt =0;
      pattern =348;
      break;
    }
    break;
  case 350:
    speed_run(70,-70);
    if(sensorMask(0x18) == 0x18){
      cnt =0;
      pattern =352;
      break;
    }
    break;
  case 352:
    runforwardline(100);
    break;
  case 100:
    speed_run(0, 0);
    break;
  default:
    break;
}
}
void test1()
{
  // Serial.println("A"); // Read NPN sensor value
  if (digitalRead(SW1_PIN) == LOW)
  {
    speed_run(20, 20); // Move forward
  }
  else if (digitalRead(SW2_PIN) == LOW)
  {
    speed_run(-20, -20); // Move backward
  }
  else
  {
    speed_run(0, 0); // Stop
  }
}
void test2()
{
  if (digitalRead(SW1_PIN) == LOW)
  {
    speed_run(-70, 0); // Turn right
  }
  else if (digitalRead(SW2_PIN) == LOW)
  {
    speed_run(0, -70); // Turn left
  }
  else
  {
    speed_run(0, 0); // Stop
  }
}
void speed_run(int speedDC_left, int speedDC_right) // hàm truyền vào tốc độ động cơ trái + phải
{
  if (speedDC_left > 255)
  {
    speedDC_left = 255;
  }
  else if (speedDC_left < -255)
  {
    speedDC_left = -255;
  }
  if (speedDC_right > 255)
  {
    speedDC_right = 255;
  }
  else if (speedDC_right < -255)
  {
    speedDC_right = -255;
  }
  // Debugging: Print input values
  // Serial.print("Speed Left: ");
  // Serial.print(speedDC_left);
  // Serial.print(", Speed Right: ");
  // Serial.println(speedDC_right);

  if (speedDC_left < 0)
  {
    analogWrite(PWMA_PIN, -speedDC_left);
    digitalWrite(AIN1_PIN, HIGH);
    digitalWrite(AIN2_PIN, LOW);
  }
  else if (speedDC_left > 0)
  {
    analogWrite(PWMA_PIN, speedDC_left);
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, HIGH);
  }
  else
  {
    analogWrite(PWMA_PIN, 0);
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, LOW);
  }
  if (speedDC_right > 0)
  {
    analogWrite(PWMB_PIN, speedDC_right);
    digitalWrite(BIN1_PIN, HIGH);
    digitalWrite(BIN2_PIN, LOW);
  }
  else if (speedDC_right < 0)
  {
    analogWrite(PWMB_PIN, -speedDC_right);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, HIGH);
  }
  else
  {
    analogWrite(PWMB_PIN, 0);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, LOW);
  }

  // Debugging: Confirm PWM values written
  // Serial.println("PWM values written.");
}
void handleAndSpeed(int angle, int speed1)
{
  int speedLeft;
  int speedRight;
  // if ((speed1 + angle) > 255)
  // {
  //   speed1 = 255 - angle;
  // }
  // if ((speed1 - angle) > 255)
  // {
  //   speed1 = 255 + angle;
  // }
  speedLeft = speed1 - angle;
  speedRight = speed1 + angle;
  speed_run(speedLeft, speedRight);
}
void read_sensor()
{
  unsigned char temp = 0;
  unsigned int sum = 0;
  unsigned long avg = 0;
  int i, iP, iD;
  int iRet;
  sensorValue[7] = analogRead(A0);
  sensorValue[6] = analogRead(A2);
  sensorValue[5] = analogRead(A1);
  sensorValue[4] = analogRead(A4);
  sensorValue[3] = analogRead(A3);
  sensorValue[2] = analogRead(A6);
  sensorValue[1] = analogRead(A5);
  sensorValue[0] = analogRead(A7);
  for (int j = 0; j < 8; j++)
  {
    if (isCalib == 0)
    {
      if (sensorValue[j] < black_value[j])
        sensorValue[j] = black_value[j];
      if (sensorValue[j] > white_value[j])
        sensorValue[j] = white_value[j];
      sensorPID[j] = map(sensorValue[j], black_value[j], white_value[j], 0, 1000);
    }
    temp = temp << 1;
    if (sensorValue[j] > compare_value[j])
    {
      temp |= 0x01; // nếu cảm biến j có giá trị lớn hơn giá trị tham chiếu thì gán bit j của temp = 1
      // temp = temp |
    }
    else
    {
      temp &= 0xfe;
    }
    sensor = temp;
  }
  for (int j = 0; j < 8; j++)
  {
    avg += (long)(sensorPID[j]) * ((j) * 1000);
    sum += sensorPID[j];
  }
  i = (int)((avg / sum) - 3500);
  kp = 2;  // 1.2 //2
  kd = 15; // 12 //15
  iP = kp * i;
  iD = kd * (lastPos - i);
  iRet = (iP - iD);
  if ((iRet < -4000))
  {
    iRet = 0;
  }
  servoPwm = iRet / 25; // 12 //25
  lastPos = i;
}
void read_sensor_v1() // hàm đọc cảm biến
{
  unsigned char temp = 0;
  unsigned int sum = 0;
  unsigned long avg = 0;
  int i, iP, iD;
  int iRet;
  sensorValue[0] = analogRead(A0);
  sensorValue[1] = analogRead(A2);
  sensorValue[2] = analogRead(A1);
  sensorValue[3] = analogRead(A4);
  sensorValue[4] = analogRead(A3);
  sensorValue[5] = analogRead(A6);
  sensorValue[6] = analogRead(A5);
  sensorValue[7] = analogRead(A7);
  for (int j = 0; j < 8; j++)
  {
    sensorPID[j] = map(sensorValue[j], black_value[j], white_value[j], 0, 1000);
    sensorPID[j] = constrain(sensorPID[j], 0, 1000);
    temp = temp << 1;
    if (sensorValue[j] > compare_value[j])
    {
      temp |= 0x01; // nếu cảm biến j có giá trị lớn hơn giá trị tham chiếu thì gán bit j của temp = 1
      // temp = temp |
    }
    else
    {
      temp &= 0xfe;
    }
    sensor = temp;
  }
  for (int j = 0; j < 8; j++)
  {
    avg += (long)(sensorPID[j]) * ((j) * 1000);
    sum += sensorPID[j];
  }
  i = (int)((avg / sum) - 3500);
  kp = 1;  // 1.2
  kd = 10; // 12
  iP = kp * i;
  iD = kd * (lastPos - i);
  iRet = (iP - iD);
  if ((iRet < -4000))
  {
    iRet = 0;
  }
  servoPwm = iRet / 20; // 12
  lastPos = i;
}
unsigned char sensorMask(unsigned char mask)
{
  return sensor & mask;
}
unsigned char sensorE()
{
  return sensor;
}
void runforwardline(int tocdo)
{
  handleAndSpeed(servoPwm, tocdo);
}
void update_sensor()
{
  // sensorValue[0] =  analogRead(A0);
  // sensorValue[1] =  analogRead(A2);
  // sensorValue[2] =  analogRead(A1);
  // sensorValue[3] =  analogRead(A4);
  // sensorValue[4] =  analogRead(A3);
  // sensorValue[5] =  analogRead(A6);
  // sensorValue[6] =  analogRead(A5);
  // sensorValue[7] =  analogRead(A7);
  // for (int i = 0; i < 8; i++)
  // {
  //   Serial.print(sensorValue[i]);
  //   if (i < 7)
  //   {
  //     Serial.print(",");
  //   }
  // }
  // Serial.println();
  Serial.println(sensor, BIN);
  for (int i = 0; i < 8; i++)
  {
    // Serial.print(sensorValue[i]);
    // Serial.print("  ");
    if (black_value[i] == 0)
      black_value[i] = 1100;
    if (sensorValue[i] < black_value[i])
      black_value[i] = sensorValue[i];
    if (sensorValue[i] > white_value[i])
      white_value[i] = sensorValue[i];
    compare_value[i] = (black_value[i] + white_value[i]) / 2;
  }
  // Serial.println();
}
void analog_raw()
{
  Serial.print("Sensor Raw Values: ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(sensorValue[i]);
    if (i < 7)
    {
      Serial.print(", ");
    }
  }
  Serial.println();
}
void beep(int duration)
{
  digitalWrite(BUZZER_PIN, LOW); // Turn the buzzer on
  delay(duration);
  digitalWrite(BUZZER_PIN, HIGH); // Turn the buzzer off
}
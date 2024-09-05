#include <DS1302.h>
#include <Wire.h>                                                                // I2C 통신을 위한 라이브러리
#include <U8g2lib.h>                                                             // OLED 디스플레이를 제어하기 위한 라이브러리

#define PUMP1_PIN 2
#define PUMP2_PIN 3
#define FAN_PIN 4
#define SW1_PIN 8
#define SW2_PIN 9
#define SW3_PIN 10
#define GAS_PIN 12
#define VOLT_PIN A0
#define WATER1_PIN A1
#define WATER2_PIN A2
#define IR_PIN A3
#define SDA 20
#define SCL 21
#define CLK 51
#define DAT 49
#define RST 47

#define MAIN_FIRST_PAGE 0
#define MAIN_LAST_PAGE 1
#define DISCHARGE_FIRST_PAGE 2
#define DISCHARGE_LAST_PAGE 2
#define PUTBET_FIRST_PAGE 3
#define PUTBET_LAST_PAGE 5
#define CELLCEHCK_FIRST_PAGE 6
#define CELLCEHCK_LAST_PAGE 6
#define CELLOVER_FIRST_PAGE 7
#define CELLOVER_LAST_PAGE 7
#define MAIN2_FIRST_PAGE 8
#define MAIN2_LAST_PAGE 8
#define MAIN3_FIRST_PAGE 9
#define MAIN3_LAST_PAGE 9

#define LIION_CELL_VOLT 4.2
#define LIPO_CELL_VOLT 4.2
#define NIMH_CELL_VOLT 1.45
#define MAX_VOLT 30

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // 128x32 해상도의 SSD1306 OLED 디스플레이를 I2C 인터페이스로 초기화
DS1302 rtc(RST, DAT, CLK); 
Time now_time;
Time start_time;

void Sensor_refresh();
void Sensor_log();

int sw1_value = 0;
int sw2_value = 0;
int sw3_value = 0;

int gas_value = 0;
int ir_value = 0;

int water1_value = 0;
int water2_value = 0;

int volt_value = 0;
float Bat_voltage = 0.0;
int cell_count = 1;
float cell_voltage = 0.0;
float before_cell_voltage = 0.0;
int battery_type = 0; // 1 = Li-ion | 2 = Li-po | 3= Ni-MH
int percent_volt = 0;
int display_volt = 0;

int page = 0;

bool sw1_flag = false;
bool sw2_flag = false;
bool sw3_flag = false;

bool main_page = true; // 시작할때 main page에서 시작
bool discharge_page = false;
bool setup_page = false;
bool putbet_page = false;
bool select_page = false;
bool cellcheck_page = false;
bool cellover_page = false;
bool pump_page = false;
bool Discharging_page = false;

int page_blink = 0;

int timer_sec = 0;
int timer_min = 0;
int timer_hour = 0;
int timer_flag1 = 0;
int timer_flag2 = 0;

void setup() {
  rtc.halt(false);
  rtc.writeProtect(false);
  u8g2.begin();
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP1_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(SW1_PIN, INPUT);
  pinMode(SW2_PIN, INPUT);
  pinMode(SW3_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  Serial.begin(9600);
  StartPage();
}

void loop() {
  MainPage();
  SensorRefresh();
  SensorLog();
  if(sw1_value == LOW && sw1_flag == false)
  {
    sw1_flag = true;

    if(page < MAIN_LAST_PAGE && main_page) page++;
    if(page < DISCHARGE_LAST_PAGE && discharge_page) page++;
    if(page < PUTBET_LAST_PAGE && putbet_page) page++;
    if(select_page) cell_count++;
  }
  else sw1_flag = false;

  if(sw3_value == LOW && sw3_flag == false)
  {
    sw3_flag = true;
    if(page > MAIN_FIRST_PAGE && main_page) page--;
    if(page > DISCHARGE_FIRST_PAGE && discharge_page) page--;
    if(page > PUTBET_FIRST_PAGE && putbet_page) page--;
    if(cell_count > 1 && select_page) cell_count--;
  }
  else sw3_flag = false;

  if(sw2_value == LOW && sw2_flag == false)
  {
    sw2_flag = true;
    switch(page)
    {
      case 0: discharge_page = true; main_page = false; page = 2; break;
      case 1: setup_page = true; main_page = false; page = 99; break;
      case 2: putbet_page = true; discharge_page = false; page = 3; break;
      case 3: battery_type = 1; select_page = true; putbet_page = false; page = 6; break;
      case 4: battery_type = 2; select_page = true; putbet_page = false; page = 6; break;
      case 5: battery_type = 3; select_page = true; putbet_page = false; page = 6; break;
      case 6:
        select_page = false;
        switch(battery_type)
        {
          case 1:
            if(MAX_VOLT < cell_count * LIION_CELL_VOLT)
            {
              cellover_page = true;
              page = CELLOVER_FIRST_PAGE;
            }
            else
            {
              start_time = now_time;
              cellcheck_page = true;
              page = MAIN2_FIRST_PAGE;
            }
            break;
          case 2:
            if(MAX_VOLT < cell_count * LIPO_CELL_VOLT)
            {
              cellover_page = true;
              page = CELLOVER_FIRST_PAGE;
            }
            else
            {
              start_time = now_time;
              cellcheck_page = true;
              page = MAIN2_FIRST_PAGE;
            }
            break;
          case 3:
            if(MAX_VOLT < cell_count * NIMH_CELL_VOLT)
            {
              cellover_page = true;
              page = CELLOVER_FIRST_PAGE;
            }
            else
            {
              start_time = now_time;
              cellcheck_page = true;
              page = MAIN2_FIRST_PAGE;
            }
            break;
        }
    }
  }
  else sw2_flag = false;
  before_cell_voltage = cell_voltage;
}

void SensorRefresh()
{
  now_time = rtc.getTime();
  sw1_value = digitalRead(SW1_PIN);
  sw2_value = digitalRead(SW2_PIN);
  sw3_value = digitalRead(SW3_PIN);
  gas_value = digitalRead(GAS_PIN);
  ir_value = analogRead(IR_PIN);
  water1_value = analogRead(WATER1_PIN);
  water2_value = analogRead(WATER2_PIN);
  volt_value = analogRead(VOLT_PIN); // 전압 아날로그 값
  Bat_voltage = ((volt_value * 5.0) / 1024.0) * 5.0; // 전압 아날로그 값을 실 변환 값으로 변환, 저항 비 = 30000 : 7500
  cell_voltage = Bat_voltage / cell_count;
}

void SensorLog()
{
  Serial.print("SW 1 : ");
  Serial.print(sw1_value);
  Serial.print(", SW 2 : ");
  Serial.print(sw2_value);
  Serial.print(", SW 3 : ");
  Serial.print(sw3_value);
  Serial.print(", gas : ");
  Serial.print(gas_value);
  Serial.print(", IR : ");
  Serial.print(ir_value);
  Serial.print(", water 1 : ");
  Serial.print(water1_value);
  Serial.print(", water 2 : ");
  Serial.print(water1_value);
  Serial.print(", volt : ");
  Serial.print(Bat_voltage);
  Serial.print(", cell_volt : ");
  Serial.print(cell_voltage);
  Serial.print(", page :");
  Serial.print(page);
  Serial.print(", page blink time(ms) :");
  Serial.print(page_blink);
  Serial.print(", year : ");
  Serial.print(now_time.year);
  Serial.print(", mon : ");
  Serial.print(now_time.mon);
  Serial.print(", date : ");
  Serial.print(now_time.date);
  Serial.print(", hour : ");
  Serial.print(now_time.hour);
  Serial.print(", min : ");
  Serial.print(now_time.min);
  Serial.print(", sec : ");
  Serial.println(now_time.sec);
}


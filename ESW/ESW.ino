#include <DS1302.h>                                                              // RTC 사용을 위한 라이브러리
#include <Wire.h>                                                                // I2C 통신을 위한 라이브러리
#include <U8g2lib.h>                                                             // OLED 디스플레이를 제어하기 위한 라이브러리

#define PUMP1_PIN 2                                                              // 워터펌프 릴레이 1번
#define PUMP2_PIN 3                                                              // 워터펌프 릴레이 2번
#define FAN_PIN 4                                                                // 배기 팬 릴레이
#define SW1_PIN 8                                                                // 스위치 1번
#define SW2_PIN 9                                                                // 스위치 2번
#define SW3_PIN 10                                                               // 스위치 3번
#define GAS_PIN 12                                                               // 가스 센서
#define VOLT_PIN A0                                                              // 전압계 (30000 : 75000
#define WATER1_PIN A1                                                            // 수위 센서 1번
#define WATER2_PIN A2                                                            // 수위 센서 2번
#define IR_PIN A3                                                                // IR 센서
#define SDA 20                                                                   // OLED SDA 핀
#define SCL 21                                                                   // OLED SCL 핀
#define CLK 51                                                                   // RTC CLK 핀
#define DAT 49                                                                   // RTC DAT 핀
#define RST 47                                                                   // RTC RST 핀

#define MAIN_FIRST_PAGE 0                                                        // 첫번째 메인 페이지
#define MAIN_LAST_PAGE 1                                                         // 마지막 메인 페이지
#define DISCHARGE_FIRST_PAGE 2                                                   // 배터리를 통 안에 넣어주세요 페이지
#define DISCHARGE_LAST_PAGE 2                                                    // 배터리를 통 안에 넣어주세요 페이지
#define PUTBET_FIRST_PAGE 3                                                      // 배터리 타입 선택 페이지
#define PUTBET_LAST_PAGE 5                                                       // 배터리 타입 선택 페이지
#define CELLCEHCK_FIRST_PAGE 6                                                   // 셀 갯수 선택 페이지
#define CELLCEHCK_LAST_PAGE 6                                                    // 셀 갯수 선택 페이지
#define CELLOVER_FIRST_PAGE 7                                                    // 셀 경고 페이지
#define CELLOVER_LAST_PAGE 7                                                     // 셀 경고 페이지
#define MAIN2_FIRST_PAGE 8                                                       // 워터펌프 급수 페이지
#define MAIN2_LAST_PAGE 8                                                        // 워터펌프 급수 페이지
#define MAIN3_FIRST_PAGE 9                                                       // 방전 메인 페이지
#define MAIN3_LAST_PAGE 9                                                        // 방전 메인 페이지

#define LIION_CELL_VOLT 4.2                                                      // 리튬 이온 배터리 최대 전압
#define LIPO_CELL_VOLT 4.2                                                       // 리튬 폴리머 배터리 최대 전압
#define NIMH_CELL_VOLT 1.45                                                      // 니켈 수소 배터리 최대 전압
#define MAX_VOLT 30                                                              // 전압계 최대 전압

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // 128x32 해상도의 SSD1306 OLED 디스플레이를 I2C 인터페이스로 초기화
DS1302 rtc(RST, DAT, CLK);                                                       // RTC 객체 생성
Time now_time;                                                                   // RTC 현재 시간 구조체 생성
Time start_time;                                                                 // RTC 시작 시간 구조체 생성

void Sensor_refresh();                                                           // 센서 업데이트 함수
void Sensor_log();                                                               // Serial로 센서 값 로그

int sw1_value = 0;                                                               // 스위치 1번 값
int sw2_value = 0;                                                               // 스위치 2번 값
int sw3_value = 0;                                                               // 스위치 3번 값

int gas_value = 0;                                                               // 가스 센서 값
int ir_value = 0;                                                                // IR 센서 값

int water1_value = 0;                                                            // 수위 센서 1번 값
int water2_value = 0;                                                            // 수위 센서 2번 값

int volt_value = 0;                                                              // 전압 센서 값
float Bat_voltage = 0.0;                                                         // 배터리 전압 값
int cell_count = 1;                                                              // 셀 갯수
float cell_voltage = 0.0;                                                        // 셀당 전압
float before_cell_voltage = 0.0;                                                 // 이전 셀당 전압
int battery_type = 0;                                                            // 배터리 타입 / 1 = Li-ion | 2 = Li-po | 3= Ni-MH
int percent_volt = 0;                                                            // 전압 맵핑 0~100
int display_volt = 0;                                                            // 전압 맵핑 0~24

int page = 0;                                                                    // 현재 페이지

bool sw1_flag = false;                                                           // 스위치 반응을 한번만 하기 위한 flag
bool sw2_flag = false;                                                           // 스위치 반응을 한번만 하기 위한 flag
bool sw3_flag = false;                                                           // 스위치 반응을 한번만 하기 위한 flag

bool main_page = true;                                                           // 시작할때 main page에서 시작
bool discharge_page = false;                                                     // page 플래그
bool setup_page = false;                                                         // page 플래그
bool putbet_page = false;                                                        // page 플래그
bool select_page = false;                                                        // page 플래그
bool cellcheck_page = false;                                                     // page 플래그
bool cellover_page = false;                                                      // page 플래그
bool pump_page = false;                                                          // page 플래그
bool Discharging_page = false;                                                   // page 플래그

int page_blink = 0;                                                              // 깜박이는걸 구현하기 위한 시간 기록 변수

int timer_sec = 0;                                                               // 화면 표시용 초
int timer_min = 0;                                                               // 화면 표시용 분
int timer_hour = 0;                                                              // 화면 표시용 시간
int timer_flag1 = 0;                                                             // 타이머 플래그
int timer_flag2 = 0;                                                             // 타이머 플래그

void setup() {
  rtc.halt(false);                                                               // RTC가 시간이 멈추지 않게 설정
  rtc.writeProtect(false);                                                       // RTC 레지스터 접근 권한 허용
  u8g2.begin();                                                                  // 화면 I2C 통신 시작
  pinMode(FAN_PIN, OUTPUT);                                                      // 핀 셋업
  pinMode(PUMP1_PIN, OUTPUT);                                                    // 핀 셋업
  pinMode(PUMP2_PIN, OUTPUT);                                                    // 핀 셋업
  pinMode(SW1_PIN, INPUT);                                                       // 핀 셋업
  pinMode(SW2_PIN, INPUT);                                                       // 핀 셋업
  pinMode(SW3_PIN, INPUT);                                                       // 핀 셋업
  pinMode(GAS_PIN, INPUT);                                                       // 핀 셋업
  Serial.begin(9600);                                                            // 시리얼 통신 시작
  StartPage();                                                                   // Boot 페이지 시작
}

void loop() {
  MainPage();                                                                    // 메인 페이지로 전환
  SensorRefresh();                                                               // 센서값 업데이트
  SensorLog();                                                                   // 센서값 로그에 저장
  if(sw1_value == LOW && sw1_flag == false)                                      // 스위치가 눌렸다면
  {
    sw1_flag = true;                                                             // 스위치 flag True

    if(page < MAIN_LAST_PAGE && main_page) page++;                               // 메인 페이지의 최대값보다 작다면 page 증가
    if(page < DISCHARGE_LAST_PAGE && discharge_page) page++;                     // 배터리를 통안에 넣어주세요 페이지의 최대값보다 작다면 page 증가
    if(page < PUTBET_LAST_PAGE && putbet_page) page++;                           // 배터리 타입 선택 페이지의 최대값보다 작다면 page 증가
    if(select_page) cell_count++;                                                // 셀 선택 페이지라면 셀 갯수 증가
  }
  else sw1_flag = false;                                                         // 스위치가 안눌렸다면 flag false

  if(sw3_value == LOW && sw3_flag == false)                                      // 스위치가 눌렸다면
  {
    sw3_flag = true;                                                             // 스위치 flag True
    if(page > MAIN_FIRST_PAGE && main_page) page--;                              // 메인 페이지의 최소값보다 크다면 page 감소
    if(page > DISCHARGE_FIRST_PAGE && discharge_page) page--;                    // 배터리를 통안에 넣어주세요 페이지의 최소값보다 크다면 page 감소
    if(page > PUTBET_FIRST_PAGE && putbet_page) page--;                          // 배터리 타입 선택 페이지의 최소값보다 크다면 page 감소
    if(cell_count > 1 && select_page) cell_count--;                              // 셀 갯수가 1보다 크다면 셀 갯수 감소 
  }
  else sw3_flag = false;                                                         // 스위치가 안눌렸다면 flag false

  if(sw2_value == LOW && sw2_flag == false)                                      // 스위치가 눌렸다면
  {
    sw2_flag = true;                                                             // 스위치 flag True
    switch(page)                                                                 // page 값에 따라 변경
    {
      case 0: discharge_page = true; main_page = false; page = 2; break;         // page가 0이라면 2번 페이지로 변경
      case 1: setup_page = true; main_page = false; page = 99; break;            // page가 1이라면 setup 페이지로 변경(아직 없음)
      case 2: putbet_page = true; discharge_page = false; page = 3; break;       // page가 2라면 3번 페이지로 변경
      case 3: battery_type = 1; select_page = true; putbet_page = false; page = 6; break;   // page가 3이라면 6번 페이지로 이동
      case 4: battery_type = 2; select_page = true; putbet_page = false; page = 6; break;   // page가 4이라면 6번 페이지로 이동
      case 5: battery_type = 3; select_page = true; putbet_page = false; page = 6; break;   // page가 5이라면 6번 페이지로 이동
      case 6:                                                                               // page가 6이라면
        select_page = false;                                                                // 페이지 이동
        switch(battery_type)                                                                // 배터리 타입에 따라 변경
        {
          case 1:                                                                           // Li-ion
            if(MAX_VOLT < cell_count * LIION_CELL_VOLT)                                     // 최대 전압보다 전압이 크지 않은지 검사
            {
              cellover_page = true;                                                         // 크다면 셀 경고 페이지로 이동
              page = CELLOVER_FIRST_PAGE;
            }
            else
            {
              start_time = now_time;                                                        // 아니라면 방전 메인페이지로 이동
              cellcheck_page = true;
              page = MAIN2_FIRST_PAGE;
            }
            break; 
          case 2:                                                                           // Li-Po
            if(MAX_VOLT < cell_count * LIPO_CELL_VOLT)                                      // 최대 전압보다 전압이 크지 않은지 검사
            {
              cellover_page = true;                                                         // 크다면 셀 경고 페이지로 이동
              page = CELLOVER_FIRST_PAGE;
            }
            else
            {
              start_time = now_time;                                                        // 아니라면 방전 메인페이지로 이동
              cellcheck_page = true;
              page = MAIN2_FIRST_PAGE;
            }
            break;
          case 3:                                                                           // Ni-MH
            if(MAX_VOLT < cell_count * NIMH_CELL_VOLT)                                      // 최대 전압보다 전압이 크지 않은지 검사
            {
              cellover_page = true;                                                         // 크다면 셀 경고 페이지로 이동
              page = CELLOVER_FIRST_PAGE;
            }
            else
            {
              start_time = now_time;                                                        // 아니라면 방전 메인페이지로 이동
              cellcheck_page = true;
              page = MAIN2_FIRST_PAGE;
            }
            break;
        }
      case 10:
        page = 0;                                                                           // page 초기화
        pump_page = true;
        cellcheck_page = false;
        break
    }
  }
  else sw2_flag = false;                                                                    // 스위치가 눌리지 않았다면 page 초기화
  before_cell_voltage = cell_voltage;                                                       // 배터리 셀당 전압 기록
}

void SensorRefresh()                                                                        // 센서 업데이트
{
  now_time = rtc.getTime();                                                                 // RTC 시간 받아오기
  sw1_value = digitalRead(SW1_PIN);                                                         // 스위치 값 읽기
  sw2_value = digitalRead(SW2_PIN);                                                         // 스위치 값 읽기
  sw3_value = digitalRead(SW3_PIN);                                                         // 스위치 값 읽기
  gas_value = digitalRead(GAS_PIN);                                                         // 가스센서 값 읽기
  ir_value = analogRead(IR_PIN);                                                            // IR 값 읽기
  water1_value = analogRead(WATER1_PIN);                                                    // 수위센서 값 읽기
  water2_value = analogRead(WATER2_PIN);                                                    // 수위센서 값 읽기
  volt_value = analogRead(VOLT_PIN);                                                        // 전압 아날로그 값
  Bat_voltage = ((volt_value * 5.0) / 1024.0) * 5.0;                                        // 전압 아날로그 값을 실 변환 값으로 변환, 저항 비 = 30000 : 7500
  cell_voltage = Bat_voltage / cell_count;                                                  // 셀 전압 계산
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


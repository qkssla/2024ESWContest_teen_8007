void StartPage()
{
  for(int i = 0; i < 3; i++)
  {
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
        u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
        u8g2.drawStr(30,20,"Boot.");
    } while (u8g2.nextPage());
    delay(300);
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
        u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
        u8g2.drawStr(30,20,"Boot..");
    } while (u8g2.nextPage());
    delay(300);
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
        u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
        u8g2.drawStr(30,20,"Boot...");
    } while (u8g2.nextPage()); 
    delay(300);
  }
  delay(1000);
}

void MainPage()
{
  page_blink++;
  if(page_blink < 3 || page == 2 || page == 7 || page == 8 || page == 9)
  {
    switch (page) {
      case 0:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(5,20,"Discharge start");
        } while (u8g2.nextPage());
        break;
      case 1:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(40,20,"Setup");
        } while (u8g2.nextPage());
        break;
      case 2:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(32,16,"Please");
            u8g2.drawStr(10,28,"put in battery");
        } while (u8g2.nextPage());
        break;
      case 3:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(10,16,"select battery");
            u8g2.drawStr(36,28,"Li-ion");
        } while (u8g2.nextPage());
        break;
      case 4:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(10,16,"select battery");
            u8g2.drawStr(36,28,"Li-Po");
        } while (u8g2.nextPage());
        break;
      case 5:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(10,16,"select battery");
            u8g2.drawStr(36,28,"NI-MH");
        } while (u8g2.nextPage());
        break;
      case 6:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(10,16,"battery cell");
            char buffer[16];                                     
            sprintf(buffer, "%02d", cell_count);    
            u8g2.drawStr(50,28,buffer);
        } while (u8g2.nextPage());
        break;
      case 7:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(10,16,"battery VOLT");
            u8g2.drawStr(10,28,"too high...");
        } while (u8g2.nextPage());
        delay(3000);
        cellover_page = false;
        main_page = true;
        page = 0;
        cell_count = 1;
        break;
      case 8:
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(36,16,"Filling");
            u8g2.drawStr(24,30, "salt water");
        } while (u8g2.nextPage());
        if(water1_value >= 512) 
        {
          digitalWrite(PUMP1_PIN, LOW);
          do {
              u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
              u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
              u8g2.drawStr(5,20,"Completion");
          } while (u8g2.nextPage());
          delay(500);
          page = 9;
          pump_page = true;
          cellcheck_page = false;
        }
        else digitalWrite(PUMP1_PIN, HIGH);
        break;
      case 9:
        if(gas_value == 0)
        {
          u8g2.firstPage();
          do {
              u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
              u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
              u8g2.drawStr(36,16,"hydrogen detection");
              u8g2.drawStr(24,30, "Return Main Menu");
          } while (u8g2.nextPage());
          digitalWrite(FAN_PIN, LOW);
          delay(3000);
          Discharging_page = true;
          pump_page = false;
          page = 10;
          cell_count = 1;
        }
        digitalWrite(FAN_PIN, HIGH);
        u8g2.firstPage();
        do {
          u8g2.drawFrame(0,0,128,32);
          u8g2.drawLine(0,8,128,8);
          u8g2.setFont(u8g2_font_5x7_tf);
          if(battery_type == 1) u8g2.drawStr(5,8,"Li-ion/Discharging");
          if(battery_type == 2) u8g2.drawStr(5,8,"Li-Po/Discharging");
          if(battery_type == 3) u8g2.drawStr(5,8,"Ni-MH/Discharging");
          u8g2.drawLine(64,9,64,32);
          u8g2.setFont(u8g2_font_unifont_t_symbols);
          if (now_time.sec < start_time.sec) timer_sec = now_time.sec + 60 - start_time.sec;
          else timer_sec = now_time.sec - start_time.sec;
          
          if(timer_sec == 59 && timer_flag1 == 0) timer_flag1 = 1;
          if(timer_sec == 0 && timer_flag1 == 1) 
          {
            timer_flag1 = 0; 
            timer_min++;
          }

          if(timer_min == 59 && timer_flag2 == 0) timer_flag2 = 1;
          if(timer_min == 0 && timer_flag2 == 1) 
          {
            timer_min = 0;
            timer_flag2 = 0; 
            timer_hour++;
          }
          char buffer[16];                                            // 충분한 크기의 버퍼를 정의합니다
          sprintf(buffer, "%02d", timer_hour);
          u8g2.drawStr(8,30,buffer);
          u8g2.drawStr(22,30,":");
          sprintf(buffer, "%02d", timer_min);
          u8g2.drawStr(28,30,buffer);
          u8g2.drawStr(42,30,":");
          sprintf(buffer, "%02d", timer_sec);
          u8g2.drawStr(48,30,buffer);
          u8g2.drawFrame(0, 8, 10, 10);
          u8g2.drawStr(0,18,"+");
          u8g2.drawFrame(118, 8, 128, 32-8);
          u8g2.setCursor(70, 30);                                      // 왼쪽 하단에 커서 위치 설정
          u8g2.print(cell_voltage);
          u8g2.drawFrame(64, 8, 10, 10);
          u8g2.drawStr(64,18,"-");
          if(battery_type == 1) percent_volt = map(cell_voltage, 0, LIION_CELL_VOLT, 0, 100);
          if(battery_type == 2) percent_volt = map(cell_voltage, 0, LIPO_CELL_VOLT, 0, 100);
          if(battery_type == 3) percent_volt = map(cell_voltage, 0, NIMH_CELL_VOLT, 0, 100);
          display_volt = map(percent_volt, 0, 100, 0, 24);
          u8g2.drawBox(118,32-display_volt,10,display_volt);
        } while(u8g2.nextPage());
        if (cell_voltage < 0.1) {page = 10; pump_page = false; Discharging_page = true;}
        break;
      case 10:
        digitalWrite(FAN_PIN, LOW);
        u8g2.firstPage();
        do {
            u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
            u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
            u8g2.drawStr(36,16,"drain");
            u8g2.drawStr(24,30, "salt water");
        } while (u8g2.nextPage());
        if(water2_value >= 512) 
        {
          digitalWrite(PUMP2_PIN, LOW);
          do {
              u8g2.setFont(u8g2_font_unifont_t_symbols);                  // 폰트 설정
              u8g2.drawFrame(0, 0, 128, 32);                              // 전체 화면 테두리
              u8g2.drawStr(5,20,"Please empty");
              u8g2.drawStr(24,30, "the water");
          } while (u8g2.nextPage());
        }
        else digitalWrite(PUMP2_PIN, HIGH);
        break;
    }
  }
  else if(page_blink < 4 && page <= PUTBET_LAST_PAGE && page >= PUTBET_FIRST_PAGE)
  {
    u8g2.firstPage();
    do {
        u8g2.drawFrame(0, 0, 128, 32);                                    // 전체 화면 테두리
        u8g2.drawStr(10,16,"select battery");
    } while (u8g2.nextPage());
  }
  else if(page_blink < 4 && page == 6)
  {
    u8g2.firstPage();
    do {
        u8g2.drawFrame(0, 0, 128, 32);                                    // 전체 화면 테두리
        u8g2.drawStr(10,16,"battery cell");
    } while (u8g2.nextPage());
  }
  else if(page_blink < 4)
  {
    u8g2.firstPage();
    do {
        u8g2.drawFrame(0, 0, 128, 32);                                    // 전체 화면 테두리
    } while (u8g2.nextPage());
  }
  else
  {
    page_blink = 0;
  }
}
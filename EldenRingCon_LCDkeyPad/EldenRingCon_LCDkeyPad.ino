/*  ArduinoとLCD keypad Shield端末を使用して実行するプログラム
 *  PlayStation 5にReaSnowS1を介してジョイスティックとして機能するようボタン設定
 *
 *  Elden Ring  Ver.1.2.3
 *  プログラムセット Ver.1.2
 *
 *  SELECTボタンでモードの切り替え
 *  モード0  モーグウィン王朝でのルーン稼ぎマクロ
 *          プロロマクロ
 *  モード1-7  ReaSnowS1にArduinoコントローラーを登録するためのプログラム
 *
 *
 */

#include <LiquidCrystal.h>
#include <NSControlLibrary.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/* ============================================================== */
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5
#define ERRORR 6

int read_LCD_buttons(int adc_key_in) {
  if (adc_key_in < 50) return btnRIGHT;    // 0   , 戻り値0, 0V
  if (adc_key_in < 250) return btnUP;      // 131 , 戻り値1, 0.64V
  if (adc_key_in < 450) return btnDOWN;    // 307 , 戻り値2, 1.50V
  if (adc_key_in < 650) return btnLEFT;    // 479 , 戻り値3, 2.34V
  if (adc_key_in < 850) return btnSELECT;  // 721 , 戻り値4, 3.52V
  if (adc_key_in > 1000) return btnNONE;   // 1023, 戻り値5, 5.00V
  return btnNONE;                          //
}
/* ============================================================== */

unsigned char modeNow;
unsigned char modeOld;
unsigned char keys;
unsigned char keysOld;
int lap = 0;
int prg = 0;
bool steyModeUP;
bool steyMode0;
int toggle;
bool modeChanged = true;  // モード切り替えフラグ
unsigned long previousModeMillis = 0;
const long modeInterval = 3000;  // モード切り替えの間隔（ミリ秒）
bool isUpper = true;             // 表示が "U=U D=D" か "L=L R=R" かを判定するフラグ

/* ============================================================== */
/* 決定Aボタンと×ボタンの決定・キャンセルのキーマッピング */
// Nintendo Switchは"0"、PlayStation 5は"1"
bool consoleType = 1;  // Aボタンと×ボタンの決定、機種固有の違い
unsigned char confirmButton;
unsigned char cancelButton;

/* ============================================================== */

// triangle
byte triangleChar[] = {
  B00000,
  B00100,
  B01010,
  B01010,
  B10001,
  B11111,
  B00000,
  B00000
};
// square
byte squareChar[] = {
  B00000,
  B11111,
  B10001,
  B10001,
  B10001,
  B11111,
  B00000,
  B00000
};
// circle
byte circleChar[] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000
};
// cross
byte crossChar[] = {
  B00000,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
  B00000
};
// up
byte upChar[] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000,
  B00000,
};
// down
byte downChar[] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
  B00000,
};
// left
byte leftChar[] = {
  B00000,
  B00100,
  B01100,
  B11111,
  B01100,
  B00100,
  B00000,
  B00000,
};
// right
byte rightChar[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B00110,
  B00100,
  B00000,
  B00000,
};

/* ============================================================== */

void setup() {
  /* コントローラー接続 */
  pushButton(Button::HOME, 100, 600);

  switch (consoleType) {
    case 1:
      confirmButton = Button::B;
      cancelButton = Button::A;
      break;
  }

  lcd.begin(16, 2);
  lcd.createChar(0, triangleChar);  // triangle 0
  lcd.createChar(1, squareChar);    // square 1
  lcd.createChar(2, circleChar);    // circle 2
  lcd.createChar(3, crossChar);     // cross  3
  lcd.createChar(4, upChar);        // up 4
  lcd.createChar(5, downChar);      // down 5
  lcd.createChar(6, leftChar);      // left 6
  lcd.createChar(7, rightChar);     // right 7
}

/* ============================================================== */

void lcdCount(int count) {
  char text[5];
  lcd.setCursor(11, 0);
  lcd.print("C");
  sprintf(text, "%4d", count);
  lcd.print(text);
}

void mode0() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP && keysOld == btnNONE) {
    steyModeUP = !steyModeUP;
  }
  if (steyModeUP && prg == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RUNES");
    lcd.setCursor(0, 1);
    lcd.print("STOP :");
    lcd.write(byte(4));  // up 4
    lcdCount(lap);
    lcd.setCursor(5, 0);
    lcd.blink();
    leftStickTilt(0, 100, 2000);
    prg++;
  } else if (steyModeUP && prg == 1) {
    leftStickTilt(-25, 100, 2000);
    prg++;
  } else if (steyModeUP && prg == 2) {
    leftStickTilt(0, 100, 1000);
    prg++;
  } else if (steyModeUP && prg == 3) {
    pushButton(Button::ZL, 40, 3000, 2);
    leftStickNeutral();
    delay(3000);
    prg++;
  } else if (steyModeUP && prg == 4) {
    pushButton(Button::MINUS, 50, 500);
    pushButton(Button::X, 50, 500);
    pushButton(confirmButton, 50, 500, 2);
    prg++;
  } else if (steyModeUP && prg == 5) {
    lap++;
    delay(4500);
    prg = 0;
  }
  if (keys == btnDOWN && !steyModeUP) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.blink();
    lcd.print("PROLO");
    pushButton(Button::PLUS, 100);
    pushHat(Hat::UP, 100);
    pushButton(confirmButton, 100);
    pushButton(Button::L, 100);
    pushButton(confirmButton, 100);
    pushHat(Hat::LEFT, 200);
    pushButton(confirmButton, 100);
    lcd.noBlink();
  } else if (!steyModeUP) {
    lcd.setCursor(0, 0);
    lcd.print("RUNES:");
    lcd.write(byte(4));  // up 4
    lcd.setCursor(0, 1);
    lcd.print("PROLO:");
    lcd.write(byte(5));  // down 5
    leftStickNeutral();
    lcd.noBlink();
    prg = 0;
  }
  keysOld = keys;
}
void mode1() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP) {
    leftStickTilt(0, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnDOWN) {
    leftStickTilt(180, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnLEFT) {
    leftStickTilt(270, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnRIGHT) {
    leftStickTilt(90, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    leftStickNeutral();
    lcd.noCursor();
  }
}
void mode2() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP) {
    rightStickTilt(0, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnDOWN) {
    rightStickTilt(180, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnLEFT) {
    rightStickTilt(270, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnRIGHT) {
    rightStickTilt(90, 100);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    rightStickNeutral();
    lcd.noCursor();
  }
}
void mode3() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP) {
    holdButton(Button::L);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnDOWN) {
    holdButton(Button::ZR);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnLEFT) {
    holdButton(Button::ZL);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnRIGHT) {
    holdButton(Button::R);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    releaseButton(Button::L);
    releaseButton(Button::ZR);
    releaseButton(Button::ZL);
    releaseButton(Button::R);
    lcd.noCursor();
  }
}
void mode4() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP) {
    holdHat(Hat::UP);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnDOWN) {
    holdHat(Hat::DOWN);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnLEFT) {
    holdHat(Hat::LEFT);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnRIGHT) {
    holdHat(Hat::RIGHT);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    releaseHat();
    lcd.noCursor();
  }
}
void mode5() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP) {
    holdButton(Button::MINUS);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnDOWN) {
    holdButton(Button::HOME);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnRIGHT) {
    holdButton(Button::PLUS);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnLEFT) {
    holdButton(Button::CAPTURE);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    releaseButton(Button::MINUS);
    releaseButton(Button::HOME);
    releaseButton(Button::PLUS);
    releaseButton(Button::CAPTURE);
    lcd.noCursor();
  }
}
void mode6() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnUP) {
    holdButton(Button::X);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  }
  if (keys == btnDOWN) {
    holdButton(Button::B);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  }
  if (keys == btnLEFT) {
    holdButton(Button::Y);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  }
  if (keys == btnRIGHT) {
    holdButton(Button::A);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    releaseButton(Button::X);
    releaseButton(Button::B);
    releaseButton(Button::Y);
    releaseButton(Button::A);
    lcd.noCursor();
  }
}
void mode7() {
  keys = read_LCD_buttons(analogRead(0));
  if (keys == btnLEFT) {
    holdButton(Button::LCLICK);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else if (keys == btnRIGHT) {
    holdButton(Button::RCLICK);
    lcd.setCursor(2, 0);
    lcd.cursor();
    delay(250);
  } else {
    releaseButton(Button::LCLICK);
    releaseButton(Button::RCLICK);
    lcd.noCursor();
  }
}

void screenDisplay() {
  if (!steyModeUP) {
    switch (toggle) {
      case 1:
      case 2:
        lcd.clear();
        if (isUpper) {
          lcd.setCursor(0, 1);
          lcd.write(byte(4));  // up 4
          lcd.print("FRONT");
          lcd.setCursor(8, 1);
          lcd.write(byte(5));  // down 5
          lcd.print("BACK");
        } else {
          lcd.setCursor(0, 1);
          lcd.write(byte(6));  // left 6
          lcd.print("LEFT");
          lcd.setCursor(8, 1);
          lcd.write(byte(7));  // right 7
          lcd.print("RIGHT");
        }
        break;
      case 3:
        lcd.clear();
        if (isUpper) {
          lcd.setCursor(0, 1);
          lcd.write(byte(4));  // up 4
          lcd.print("L(1)");
          lcd.setCursor(8, 1);
          lcd.write(byte(5));  // down 5
          lcd.print("ZR(R2)");
        } else {
          lcd.setCursor(0, 1);
          lcd.write(byte(6));  // left 6
          lcd.print("ZL(L2)");
          lcd.setCursor(8, 1);
          lcd.write(byte(7));  // right 7
          lcd.print("R(1)");
        }
        break;
      case 4:
        lcd.clear();
        if (isUpper) {
          lcd.setCursor(0, 1);
          lcd.write(byte(4));  // up 4
          lcd.print("UP");
          lcd.setCursor(8, 1);
          lcd.write(byte(5));  // down 5
          lcd.print("DOWN");
        } else {
          lcd.setCursor(0, 1);
          lcd.write(byte(6));  // left 6
          lcd.print("LEFT");
          lcd.setCursor(8, 1);
          lcd.write(byte(7));  // right 7
          lcd.print("RIGHT");
        }
        break;
      case 5:
        lcd.clear();
        if (isUpper) {
          lcd.setCursor(0, 1);
          lcd.write(byte(4));  // up 4
          lcd.print("TOUCHPAD-");
          lcd.setCursor(12, 1);
          lcd.write(byte(5));  // down 5
          lcd.print("PS+");
        } else {
          lcd.setCursor(0, 1);
          lcd.write(byte(6));  // left 6
          lcd.print("CREATE");
          lcd.setCursor(9, 1);
          lcd.write(byte(7));  // right 7
          lcd.print("OPTION");
        }
        break;
      case 6:
        lcd.clear();
        if (isUpper) {
          // lcd.print("U=TRIAN' D=CROSS");
          lcd.setCursor(0, 1);
          lcd.write(byte(4));  // up 4
          lcd.print("X(");
          lcd.write(byte(0));  // triangle 0
          lcd.print(")");
          lcd.setCursor(8, 1);
          lcd.write(byte(5));  // down 5
          lcd.print("B(");
          lcd.write(byte(3));  // cross  3
          lcd.print(")");
        } else {
          // lcd.print("L=SQUAR R=CIRCLE");
          lcd.setCursor(0, 1);
          lcd.write(byte(6));  // left 6
          lcd.print("Y(");
          lcd.write(byte(1));  // square 1
          lcd.print(")");
          lcd.setCursor(8, 1);
          lcd.write(byte(7));  // right 7
          lcd.print("A(");
          lcd.write(byte(2));  // circle 2
          lcd.print(")");
        }
        break;
      case 7:
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.write(byte(6));  // left 6
        lcd.print("L3");
        lcd.setCursor(8, 1);
        lcd.write(byte(7));  // right 7
        lcd.print("R3");
        break;
    }
    isUpper = !isUpper;
  }
}

/* ============================================================== */

void loop() {
  modeNow = read_LCD_buttons(analogRead(0));
  //SELECTでモード切り替え
  if (!steyModeUP && (modeNow == btnSELECT && modeOld == btnNONE)) {
    steyMode0 = !steyMode0;
    toggle++;
    if (toggle > 7) {
      toggle = 0;
    }
    lcd.clear();
    previousModeMillis = millis();  // モード切り替え時刻を記録
    modeChanged = true;             // モードが切り替わったことをフラグで示す
    delay(100);
  }
  modeOld = modeNow;

  switch (toggle) {
    case 0:
      mode0();
      break;
    case 1:
      mode1();
      lcd.setCursor(0, 0);
      lcd.print("M1 ");
      lcd.print("L AnalogStick");
      break;
    case 2:
      mode2();
      lcd.setCursor(0, 0);
      lcd.print("M2 ");
      lcd.print("R AnalogStick");
      break;
    case 3:
      mode3();
      lcd.setCursor(0, 0);
      lcd.print("M3 ");
      lcd.print("Side Button  ");
      break;
    case 4:
      mode4();
      lcd.setCursor(0, 0);
      lcd.print("M4 ");
      lcd.print("Hat Button   ");
      break;
    case 5:
      mode5();
      lcd.setCursor(0, 0);
      lcd.print("M5 ");
      lcd.print("Select Button");
      break;
    case 6:
      mode6();
      lcd.setCursor(0, 0);
      lcd.print("M6 ");
      lcd.print("Action Button");
      break;
    case 7:
      mode7();
      lcd.setCursor(0, 0);
      lcd.print("M7 ");
      lcd.print("Stick Button");
      break;
  }

  // 2列目LCD表示切り替え
  unsigned long currentMillis = millis();
  if (currentMillis - previousModeMillis >= modeInterval || modeChanged) {
    previousModeMillis = currentMillis;
    screenDisplay();
    modeChanged = false;
  }
}
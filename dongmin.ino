#include <LiquidCrystal_I2C.h>

#define BUTTON1_PIN 2  // 첫 번째 버튼 핀
#define BUTTON2_PIN 3  // 두 번째 버튼 핀
#define BUTTON5_PIN 5  // 5번 핀 (입력 버튼 핀)
#define BUZZER_PIN 4   // 부저 핀 (핀 4에 부저 연결)

LiquidCrystal_I2C lcd(0x27, 16, 2);

int count = 0;                // 현재 세트의 카운트
int setCount[3] = {0, 0, 0};  // 3개의 세트 카운트를 저장하는 배열
int setIndex = 0;             // 현재 세트 번호
unsigned long startTime = 0;  // 카운팅 시작 시간
unsigned long restStartTime = 0;  // 휴식 시작 시간
bool counting = false;        // 카운팅 상태
bool resting = false;         // 휴식 상태
unsigned long countLimit = 3000; // 20초 동안 카운팅
unsigned long restLimit = 1000;  // 10초 동안 휴식

void setup() {
  lcd.init();
  pinMode(BUTTON1_PIN, INPUT_PULLUP);  // 버튼 1 핀
  pinMode(BUTTON2_PIN, INPUT_PULLUP);  // 버튼 2 핀
  pinMode(BUTTON5_PIN, INPUT_PULLUP);  // 5번 핀
  pinMode(BUZZER_PIN, OUTPUT);        // 부저 핀 설정 (출력)

  Serial.begin(9600);
  lcd.clear();
  lcd.backlight();
  lcd.print("Ready!");
}

void loop() {
  unsigned long currentMillis = millis();  // 현재 시간

  // 두 버튼이 동시에 눌리면 카운팅 시작
  if (digitalRead(BUTTON1_PIN) == LOW && digitalRead(BUTTON2_PIN) == LOW) {
    if (!counting && !resting) {
      counting = true;
      count = 0;
      setCount[setIndex] = 0;  // 현재 세트의 카운트 초기화
      startTime = currentMillis;  // 카운팅 시작 시간 기록
      lcd.clear();
      lcd.print("Set ");
      lcd.print(setIndex + 1);
      lcd.print(" Start!");  // 첫 번째 세트 시작
      delay(1000);  // 1초 딜레이
    }
  }

  // 두 버튼이 떼어지면 카운팅 중지
  if ((digitalRead(BUTTON1_PIN) == HIGH || digitalRead(BUTTON2_PIN) == HIGH) && counting) {
    counting = false;
    lcd.clear();
    lcd.print("Counting Stopped");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(setCount[0]);
    lcd.print(" / ");
    lcd.print(setCount[1]);
    lcd.print(" / ");
    lcd.print(setCount[2]);// 5초 동안 표시 후
    lcd.setCursor(0,1);
    lcd.print("Total Count: ");
    lcd.print(setCount[0] + setCount[1] + setCount[2]);
    delay(5000);  // 5초 동안 총 카운트 표시
    setIndex = 0;  // 세트 카운트 초기화
    lcd.clear();
    lcd.print("Ready!");
  }

  // 카운팅 중일 때
  if (counting) {
    // 5번 핀이 LOW일 때 카운트 증가
    if (digitalRead(BUTTON5_PIN) == LOW) {
      count++;
      setCount[setIndex] = count;  // 현재 세트의 카운트 업데이트
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Count: ");
      lcd.setCursor(0, 1);
      lcd.print(count);
      Serial.print("Button 5 pressed, count: ");
      Serial.println(count);

      // 부저 소리 내기 (간단한 삐 소리)
      tone(BUZZER_PIN, 1000, 200);  // 1000Hz, 200ms 동안 소리
      delay(500);  // 너무 빠르게 카운트가 증가하지 않도록 잠시 대기
    }

    // 카운팅 시간이 20초가 되면 휴식
    unsigned long timeElapsed = currentMillis - startTime;
    if (timeElapsed < countLimit) {
      unsigned long timeRemaining = countLimit - timeElapsed;
      lcd.setCursor(0, 1);
      lcd.print("Time Left: ");
      lcd.print(timeRemaining / 1000);  // 남은 카운팅 시간 (초 단위)
    } else {
      counting = false;
      resting = true;
      restStartTime = currentMillis;  // 휴식 시작 시간 기록
      lcd.clear();
      lcd.print("Resting...");
      delay(1000);  // 1초 딜레이
    }
  }

  // 휴식 중일 때
  if (resting) {
    unsigned long restElapsed = currentMillis - restStartTime;
    if (restElapsed < restLimit) {
      unsigned long restTimeRemaining = restLimit - restElapsed;

      // LCD에 휴식 시간 표시
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Resting...");
      lcd.setCursor(0, 1);
      lcd.print("Time Left: ");
      lcd.print(restTimeRemaining / 1000);  // 남은 휴식 시간 (초 단위)
    } else {
      resting = false;  // 휴식 종료
      setIndex++;       // 다음 세트로 이동

      // 세트 번호를 1, 2, 3 순으로 설정
      if (setIndex < 3) {
        lcd.clear();
        lcd.print("Set ");
        lcd.print(setIndex + 1);
        lcd.print(" Start!");  // 다음 세트 시작
        delay(1000);  // 1초 딜레이
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(setCount[0]);
        lcd.print(" / ");
        lcd.print(setCount[1]);
        lcd.print(" / ");
        lcd.print(setCount[2]);// 5초 동안 표시 후
        lcd.setCursor(0,1);
        lcd.print("Total Count: ");
        lcd.print(setCount[0] + setCount[1] + setCount[2]);
        delay(5000);  // 5초 동안 총 카운트 표시
        setIndex = 0;  // 세트 카운트 초기화
        lcd.clear();
        lcd.print("Ready!");
        delay(1000);  // 잠시 대기
      }
    }
  }

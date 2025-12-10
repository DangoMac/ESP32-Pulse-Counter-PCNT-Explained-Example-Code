#include <Arduino.h>
#include <driver/pcnt.h>

#define BTN_UP_PIN    4   // ปุ่มเพิ่ม
#define BTN_DOWN_PIN  5   // ปุ่มลด

#define PCNT_UNIT_ID  PCNT_UNIT_0

int16_t lastCount = 0;

// กำหนดค่า filter (จำนวนรอบ APB clock)
// 1000 ~ 12.5 µs บน ESP32 (APB = 80 MHz)
const uint16_t PCNT_FILTER_VAL = 1000;

void setup_pcnt_ch0_ch1() {
  // ---------- CH0: นับเพิ่ม ----------
  pcnt_config_t ch0 = {};
  ch0.pulse_gpio_num = BTN_UP_PIN;        // sig_ch0
  ch0.ctrl_gpio_num  = PCNT_PIN_NOT_USED; // ไม่ใช้ control
  ch0.unit           = PCNT_UNIT_ID;
  ch0.channel        = PCNT_CHANNEL_0;

  ch0.pos_mode   = PCNT_COUNT_INC;        // ขาขึ้น → +1
  ch0.neg_mode   = PCNT_COUNT_DIS;        // ขาลง → ไม่ทำอะไร
  ch0.lctrl_mode = PCNT_MODE_KEEP;
  ch0.hctrl_mode = PCNT_MODE_KEEP;

  ch0.counter_h_lim = 32767;
  ch0.counter_l_lim = -32768;

  pcnt_unit_config(&ch0);

  // ---------- CH1: นับลด ----------
  pcnt_config_t ch1 = ch0;                // copy config เดิม
  ch1.pulse_gpio_num = BTN_DOWN_PIN;      // sig_ch1
  ch1.channel        = PCNT_CHANNEL_1;

  ch1.pos_mode = PCNT_COUNT_DEC;          // ขาขึ้น → -1
  ch1.neg_mode = PCNT_COUNT_DIS;

  pcnt_unit_config(&ch1);

  // เคลียร์และเริ่มนับ
  pcnt_counter_pause(PCNT_UNIT_ID);
  pcnt_counter_clear(PCNT_UNIT_ID);

  // ★★ เปิดใช้ glitch filter ★★
  pcnt_set_filter_value(PCNT_UNIT_ID, PCNT_FILTER_VAL);
  pcnt_filter_enable(PCNT_UNIT_ID);

  pcnt_counter_resume(PCNT_UNIT_ID);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Example 5.5: PCNT ch0(+1) / ch1(-1) in same unit + filter");

  pinMode(BTN_UP_PIN,   INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);

  setup_pcnt_ch0_ch1();
}

void loop() {
  int16_t count = 0;
  pcnt_get_counter_value(PCNT_UNIT_ID, &count);

  if (count != lastCount) {
    Serial.print("PCNT Count = ");
    Serial.println(count);
    lastCount = count;
  }

  delay(20);
}

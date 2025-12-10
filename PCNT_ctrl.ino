#include <Arduino.h>
#include <driver/pcnt.h>

#define BUTTON_PIN   4     // ปุ่มกดนับ
#define MODE_PIN     18    // สวิตช์เลือกโหมด (up/down)

#define PCNT_UNIT_ID PCNT_UNIT_0
#define PCNT_CH      PCNT_CHANNEL_0

// ใช้สำหรับแสดงเฉย ๆ
int16_t lastCount = 0;

// ค่าฟิลเตอร์ของ PCNT (จำนวนรอบ APB clock)
// APB_CLK = 80 MHz → 1000 รอบ ≈ 12.5 ไมโครวินาที
const uint16_t PCNT_FILTER_VAL = 1000;

void setup_pcnt_with_ctrl() {
  pcnt_config_t cfg = {};
  cfg.pulse_gpio_num = BUTTON_PIN;      // sig_ch0 → รับ edge จากปุ่ม
  cfg.ctrl_gpio_num  = MODE_PIN;        // ctrl_ch0 → รับระดับจากสวิตช์โหมด
  cfg.unit           = PCNT_UNIT_ID;
  cfg.channel        = PCNT_CH;

  // นับจาก "ขาขึ้น" ของปุ่ม (ปล่อยปุ่มจาก LOW -> HIGH)
  cfg.pos_mode   = PCNT_COUNT_INC;      // base action = +1
  cfg.neg_mode   = PCNT_COUNT_DIS;      // ไม่สนใจขาลง

  // ตอน ctrl = LOW  (สวิตช์ ON, ต่อกราวด์) → กลับทิศ (+1 -> -1) = นับลง
  cfg.lctrl_mode = PCNT_MODE_REVERSE;

  // ตอน ctrl = HIGH (สวิตช์ปล่อย, โดน PULLUP) → KEEP = นับขึ้นตามปกติ
  cfg.hctrl_mode = PCNT_MODE_KEEP;

  cfg.counter_h_lim = 32767;
  cfg.counter_l_lim = -32768;

  // apply config ให้ unit นี้
  pcnt_unit_config(&cfg);

  // เตรียม counter
  pcnt_counter_pause(PCNT_UNIT_ID);
  pcnt_counter_clear(PCNT_UNIT_ID);

  // ★ เปิด glitch filter ★
  pcnt_set_filter_value(PCNT_UNIT_ID, PCNT_FILTER_VAL);
  pcnt_filter_enable(PCNT_UNIT_ID);

  pcnt_counter_resume(PCNT_UNIT_ID);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Example 5.6: PCNT + control pin (self-lock switch for up/down) + filter");

  // ปุ่ม & สวิตช์ ใช้ Active-Low ทั้งคู่
  pinMode(BUTTON_PIN, INPUT_PULLUP); // ปกติ HIGH, กดแล้วเป็น LOW
  pinMode(MODE_PIN,   INPUT_PULLUP); // ปกติ HIGH = โหมด UP, กดล็อกลง GND = โหมด DOWN

  setup_pcnt_with_ctrl();
}

void loop() {
  int16_t count = 0;
  pcnt_get_counter_value(PCNT_UNIT_ID, &count);

  int modeState = digitalRead(MODE_PIN); // 1 = ปล่อย, 0 = กด/ON

  if (count != lastCount) {
    Serial.print("Count = ");
    Serial.print(count);
    Serial.print(" | MODE = ");
    Serial.println(modeState == LOW ? "DOWN (invert, นับลง)" : "UP (normal, นับขึ้น)");
    lastCount = count;
  }

  delay(20);
}

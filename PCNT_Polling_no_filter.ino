#include <Arduino.h>
#include <driver/pcnt.h>

#define BUTTON_PIN   4
#define PCNT_UNIT    PCNT_UNIT_0
#define PCNT_CHANNEL PCNT_CHANNEL_0

int16_t lastCount = 0;

void setup_pcnt_basic() {
  pcnt_config_t pcnt_config = {};

  pcnt_config.pulse_gpio_num = BUTTON_PIN;        // sig_ch0
  pcnt_config.ctrl_gpio_num  = PCNT_PIN_NOT_USED; // ยังไม่ใช้ control
  pcnt_config.unit           = PCNT_UNIT;
  pcnt_config.channel        = PCNT_CHANNEL;

  // นับเฉพาะขาขึ้น (ปล่อยปุ่ม)
  pcnt_config.pos_mode = PCNT_COUNT_INC;  // POS_MODE = 1 → นับเพิ่ม
  pcnt_config.neg_mode = PCNT_COUNT_DIS;  // NEG_MODE = 0 → ไม่ทำอะไรตอนขาลง

  // control ไม่ได้ใช้ → KEEP
  pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;

  // ตั้ง limit กว้าง ๆ
  pcnt_config.counter_h_lim = 32767;
  pcnt_config.counter_l_lim = -32768;

  // Apply config
  pcnt_unit_config(&pcnt_config);

  // เคลียร์ + เริ่มนับ
  pcnt_counter_pause(PCNT_UNIT);
  pcnt_counter_clear(PCNT_UNIT);
  pcnt_filter_disable(PCNT_UNIT);   // ยังไม่ใช้ filter
  pcnt_counter_resume(PCNT_UNIT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Example 2: PCNT without filter (see bouncing)");

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // ปุ่ม Active-Low
  setup_pcnt_basic();
}

void loop() {
  int16_t count = 0;
  pcnt_get_counter_value(PCNT_UNIT, &count);

  if (count != lastCount) {
    Serial.print("Count = ");
    Serial.println(count);
    lastCount = count;
  }

  delay(20); // อ่านช้า ๆ พอ
}

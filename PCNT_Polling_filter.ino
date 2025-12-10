#include <Arduino.h>
#include <driver/pcnt.h>

#define BUTTON_PIN   4
#define PCNT_UNIT    PCNT_UNIT_0
#define PCNT_CHANNEL PCNT_CHANNEL_0

int16_t lastCount = 0;

// กำหนด Threshold ของ Filter (จำนวนรอบ APB_CLK)
// ค่านี้ปรับได้ตามการทดลองจริง
const uint16_t PCNT_FILTER_VAL = 1000;

void setup_pcnt_with_filter() {
  pcnt_config_t pcnt_config = {};
  pcnt_config.pulse_gpio_num = BUTTON_PIN;
  pcnt_config.ctrl_gpio_num  = PCNT_PIN_NOT_USED;
  pcnt_config.unit           = PCNT_UNIT;
  pcnt_config.channel        = PCNT_CHANNEL;

  pcnt_config.pos_mode = PCNT_COUNT_INC;   // นับเพิ่มที่ขาขึ้น
  pcnt_config.neg_mode = PCNT_COUNT_DIS;   // ไม่สนใจขาลง
  pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;

  pcnt_config.counter_h_lim = 32767;
  pcnt_config.counter_l_lim = -32768;

  pcnt_unit_config(&pcnt_config);

  pcnt_counter_pause(PCNT_UNIT);
  pcnt_counter_clear(PCNT_UNIT);

  // ตั้งค่าและเปิด Filter
  pcnt_set_filter_value(PCNT_UNIT, PCNT_FILTER_VAL);
  pcnt_filter_enable(PCNT_UNIT);

  pcnt_counter_resume(PCNT_UNIT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Example 3: PCNT with glitch filter");

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setup_pcnt_with_filter();
}

void loop() {
  int16_t count = 0;
  pcnt_get_counter_value(PCNT_UNIT, &count);

  if (count != lastCount) {
    Serial.print("Count (filtered) = ");
    Serial.println(count);
    lastCount = count;
  }

  delay(20);
}

#include <Arduino.h>
#include <driver/pcnt.h>

#define BUTTON1_PIN   4
#define BUTTON2_PIN   5

#define UNIT1         PCNT_UNIT_0
#define UNIT2         PCNT_UNIT_1

int16_t lastCount1 = 0;
int16_t lastCount2 = 0;

// กำหนดค่าฟิลเตอร์ (จำนวนรอบ APB_CLK ที่สั้นกว่านี้จะถูกมองว่าเป็น glitch)
const uint16_t PCNT_FILTER_VAL = 1000;  // ลองเริ่มที่ 1000 ก่อน แล้วค่อยปรับเอาจากผลจริง

void setup_pcnt_unit(pcnt_unit_t unit, gpio_num_t pin) {
  pcnt_config_t pcnt_config = {};
  pcnt_config.pulse_gpio_num = pin;
  pcnt_config.ctrl_gpio_num  = PCNT_PIN_NOT_USED;
  pcnt_config.unit           = unit;
  pcnt_config.channel        = PCNT_CHANNEL_0;

  // นับเฉพาะขาขึ้น
  pcnt_config.pos_mode   = PCNT_COUNT_INC;   // rising edge → +1
  pcnt_config.neg_mode   = PCNT_COUNT_DIS;   // falling edge → ไม่ทำอะไร
  pcnt_config.lctrl_mode = PCNT_MODE_KEEP;
  pcnt_config.hctrl_mode = PCNT_MODE_KEEP;

  pcnt_config.counter_h_lim = 32767;
  pcnt_config.counter_l_lim = -32768;

  // apply config
  pcnt_unit_config(&pcnt_config);

  pcnt_counter_pause(unit);
  pcnt_counter_clear(unit);

  // ★★ เปิดใช้งาน glitch filter ★★
  pcnt_set_filter_value(unit, PCNT_FILTER_VAL);  // ตั้ง threshold
  pcnt_filter_enable(unit);                      // เปิดฟิลเตอร์

  pcnt_counter_resume(unit);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Example: PCNT with 2 units + filter (2 buttons)");

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  setup_pcnt_unit(UNIT1, (gpio_num_t)BUTTON1_PIN);
  setup_pcnt_unit(UNIT2, (gpio_num_t)BUTTON2_PIN);
}

void loop() {
  int16_t c1 = 0, c2 = 0;
  pcnt_get_counter_value(UNIT1, &c1);
  pcnt_get_counter_value(UNIT2, &c2);

  if (c1 != lastCount1 || c2 != lastCount2) {
    Serial.print("Button1 Count = ");
    Serial.print(c1);
    Serial.print(" | Button2 Count = ");
    Serial.println(c2);

    lastCount1 = c1;
    lastCount2 = c2;
  }

  delay(20);
}

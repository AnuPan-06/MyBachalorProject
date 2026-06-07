// Only supports SX1276/SX1278
// 仅支持 SX1276/SX1278 无线电模块,SX1280,SX1262等其他无线电模块请使用RadioLibExamples目录的示例
#include <LoRa.h>
#include "LoRaBoards.h"

#ifndef CONFIG_RADIO_FREQ
#define CONFIG_RADIO_FREQ           868.0
#endif
#ifndef CONFIG_RADIO_OUTPUT_POWER
#define CONFIG_RADIO_OUTPUT_POWER   17
#endif
#ifndef CONFIG_RADIO_BW
#define CONFIG_RADIO_BW             125.0
#endif


#if !defined(USING_SX1276) && !defined(USING_SX1278)
#error "LoRa example is only allowed to run SX1276/78. For other RF models, please run examples/RadioLibExamples
// 仅支持 SX1276/SX1278 无线电模块,SX1280,SX1262等其他无线电模块请使用RadioLibExamples目录的示例
#endif

int counter = 0;
const int SensorPin = 36;

const int dryValue = 4095; // ค่าเมื่อดินแห้ง (ปรับตามการสอบเทียบ)
const int wetValue = 964; // ค่าเมื่อดินชื้น 
const int moistureThreshold = 500; // ค่า threshold สำหรับชื้น/แห้ง

void setup() {

    setupBoards();
    // When the power is turned on, a delay is required.
    delay(1500);

#ifdef  RADIO_TCXO_ENABLE
    pinMode(RADIO_TCXO_ENABLE, OUTPUT);
    digitalWrite(RADIO_TCXO_ENABLE, HIGH);
#endif

#ifdef RADIO_CTRL
    Serial.println("Turn off LAN,Trun on PA, Enter Tx mode.");
    /*
    * BPF LoRa LAN Control ,set Low turn off LAN , TX Mode
    * */
    digitalWrite(RADIO_CTRL, LOW);
#endif /*RADIO_CTRL*/

    Serial.println("LoRa Sender");
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(CONFIG_RADIO_FREQ * 1000000)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }

    LoRa.setTxPower(CONFIG_RADIO_OUTPUT_POWER);
    LoRa.setSignalBandwidth(CONFIG_RADIO_BW * 1000);
    LoRa.setSpreadingFactor(10);
    LoRa.setPreambleLength(16);
    LoRa.setSyncWord(0xAB);
    LoRa.disableCrc();
    LoRa.disableInvertIQ();
    LoRa.setCodingRate4(7);

    
}

void loop() {

    String Soil_type = "";
    int MoistureVal = analogRead(SensorPin);
    int MoisturePercent = map(MoistureVal, dryValue, wetValue, 0, 100);
    MoisturePercent = constrain(MoisturePercent, 0, 100);

    Serial.print("Sending packet: ");
    Serial.println(counter);

    Serial.print("Raw: ");
    Serial.println(MoistureVal);
    Serial.print("Moisture: ");
    Serial.print(MoisturePercent);
    Serial.println("%");

    if (MoisturePercent <= 15) {
    Soil_type += "Dry";
    Serial.println("ดินแห้ง/อากาศ");
  } 
   else if (MoisturePercent > 15 && MoisturePercent <= 45 ) {
    Soil_type += "Less wet";
    Serial.println("ดินชื้นเล็กน้อย");
  }
   else if (MoisturePercent > 45 && MoisturePercent <= 70 ) {
    Soil_type += "Wet";
    Serial.println("ดินชื้น");
  }
  else if (MoisturePercent > 70 ) {
    Soil_type += "Very wet";
    Serial.println("ดินชื้นมาก");
  }
    Serial.println("--------------------");

    // send packet
    LoRa.beginPacket();

    LoRa.print(Soil_type);
    LoRa.print(": ");
    LoRa.print(MoisturePercent);
    LoRa.print("%");

    LoRa.endPacket();

    if (u8g2) {
        char buf[256];
        u8g2->clearBuffer();
        u8g2->drawStr(0, 12, "Transmitting: OK!");
        snprintf(buf, sizeof(buf), "Sending: %d", counter);
        u8g2->drawStr(0, 30, buf);
        u8g2->sendBuffer();
    }
    counter++;
    delay(5000);
}

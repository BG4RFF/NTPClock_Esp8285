#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <TM1650.h>
#include "air_kiss_connect.h"

const char *ssid     = "XXXXXXXXX";
const char *password = "xxxxxxxxxxxxxx";

WiFiUDP ntpUDP;

// NTP服务器地址和同步间隔时间
NTPClient timeClient(ntpUDP, "192.168.11.20", 8 * 60 * 60);

// 驱动数码管的TM1650的I2C总线端口
#define TM1650_SDA  9
#define TM1650_SCL  10

// 数码管小数点和冒号的数组下标
// 使用4位共阴带冒号的时钟数码管
// 不同型号的数码管点位不同，控制方式也会有所不同
#define TM1650_DOT_POS        0
#define TM1650_SEMICOLON_POS  1

// 数码管对象实例
TM1650  NixieTube;

void I2C_init(uint8_t sda_pin, uint8_t scl_pin) {
  // TM1650 I2C总线设置为上拉
  pinMode(sda_pin, INPUT_PULLUP);
  pinMode(scl_pin, INPUT_PULLUP);
  
  // 初始化并启动I2C总线
  Wire.begin(sda_pin, scl_pin);
}

// 数码管显示剩余时间
void show_timer(NTPClient &ntp_time) {
  // 显示缓存
  char str[5];
  // 打印显示字符串
  sprintf(str, "%02d%02d", ntp_time.getHours(), ntp_time.getMinutes());
  // 根据秒数的奇偶来切换":"显示
  
  str[TM1650_SEMICOLON_POS] |= (ntp_time.getSeconds()%2)?0x80:0x00;
  // 显示
  NixieTube.displayString(str);
}

void setup(){
  Serial.begin(115200);
  
  // 自动配置WiFi
  air_kiss_connect();

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // 启动NTP连接获取同步时间
  timeClient.begin();
  
  // 初始化I2C总线
  I2C_init(TM1650_SDA, TM1650_SCL);
  
  // 初始化数码管
  NixieTube.init();
  // 设置数码管亮度
  NixieTube.setBrightness(3);

}

void clock_loop() {
  static uint32_t time_out = 0; // 刷新时间控制
  uint32_t now_mill = millis(); // 当前的系统时间
  
  // 如果系统计时器溢出
  if(now_mill >= time_out {
    // 设定下次刷新的时间点（1s以后再次刷新）
    time_out = now_mill + 1000;
    // 更新TNP时间
    timeClient.update();

    Serial.println(timeClient.getFormattedTime());
    // 显示时间
    show_timer(timeClient);
  }
}

void loop() {
  clock_loop();
}

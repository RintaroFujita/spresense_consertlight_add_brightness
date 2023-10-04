#include <Wire.h>
#include <SpresenseNeoPixel.h>

volatile uint8_t data[14]; // センサからのデータ格納用配列
volatile int16_t az = 0;  // 出力データ(生値)
int16_t previousAz = 0;    // 前回の加速度センサー値
int16_t threshold = 8000;  // 降るかどうかの閾値
int16_t fallCount = 0;     // 降った回数をカウント
const uint16_t PIN = 1;
const uint16_t NUM_PIXELS = 1;
int brightness = 0; // NeoPixelの明るさ

SpresenseNeoPixel<PIN, NUM_PIXELS> neopixel;

void setup() {
  Serial.begin(115200); // シリアル通信を開始する
  Wire.begin();         // I2C通信を開始する
  Wire.beginTransmission(0x68); // 送信処理を開始する(0x68がセンサーのアドレス)
  Wire.write(0x6b);             // レジスタ「0x6b」(動作状変数)を指定
  Wire.write(0x00);             // 0x00を指定(ON)
  Wire.endTransmission();
  neopixel.framerate(60);
}

void loop() {
  MPU_DATAGET();
  Serial.print("az: ");
  Serial.println(az);

  int16_t azDelta = az - previousAz;

  if (azDelta > 1000) {
    // 加速度の変動が大きいときにNeoPixelの明るさを増加させる
    brightness += 10;
  } else {
    // 加速度の変動が小さいときにNeoPixelの明るさを減少させる
    brightness -= 5;
  }

  if (brightness > 255) {
    brightness = 255;
  } else if (brightness < 0) {
    brightness = 0;
  }

  neopixel.set(brightness, 0, 0); // NeoPixelの色を赤く設定
  neopixel.show();

  previousAz = az;
  delay(100);
}

void MPU_DATAGET() {
  Wire.beginTransmission(0x68); // 送信処理を開始する
  Wire.write(0x3b);            // (取得値の先頭を指定)
  Wire.endTransmission();       // 送信を終了する
  Wire.requestFrom(0x68, 14);   // データを要求する(0x3bから14バイトが6軸の値)

  uint8_t i = 0;
  while (Wire.available()) {
    data[i++] = Wire.read();
  }

  az = (data[0] << 8) | data[1];
}

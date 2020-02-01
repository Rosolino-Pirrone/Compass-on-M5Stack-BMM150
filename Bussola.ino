#include <Arduino.h>
#include <Wire.h>
#include <M5Stack.h>
// libraries
#include "bmm150.h"
#include "bmm150_defs.h"

BMM150 bmm = BMM150();
bmm150_mag_data value_offset;
float lineX = 0;
float lineY = 0;
float media_valori_dir_[50];
float dir_media_ = 0;

int count = 0;
float headingDegrees = 0;

void setup()
{
  Serial.begin(115200);
  // Initialize the M5Stack object
  M5.begin();
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();



  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setTextSize(2);

  if (bmm.initialize() == BMM150_E_ID_NOT_CONFORM) {
    Serial.println("Chip ID can not read!");
    while (1);
  } else {
    Serial.println("Initialize done!");
  }

  Serial.println("Start figure-8 calibration after 3 seconds.");
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.printf("Start figure-8 calibration after 3 seconds.");
  delay(3000);
  calibrate(10000);
  Serial.print("\n\rCalibrate done..");
  M5.Lcd.clearDisplay();
  M5.Lcd.fillCircle(160, 120, 80, TFT_WHITE);


}

/**
   @brief Do figure-8 calibration for a limited time to get offset values of x/y/z axis.
   @param timeout - seconds of calibration period.
*/
void calibrate(uint32_t timeout)
{
  int16_t value_x_min = 0;
  int16_t value_x_max = 0;
  int16_t value_y_min = 0;
  int16_t value_y_max = 0;
  int16_t value_z_min = 0;
  int16_t value_z_max = 0;
  uint32_t timeStart = 0;

  bmm.read_mag_data();
  value_x_min = bmm.raw_mag_data.raw_datax;
  value_x_max = bmm.raw_mag_data.raw_datax;
  value_y_min = bmm.raw_mag_data.raw_datay;
  value_y_max = bmm.raw_mag_data.raw_datay;
  value_z_min = bmm.raw_mag_data.raw_dataz;
  value_z_max = bmm.raw_mag_data.raw_dataz;
  delay(100);

  timeStart = millis();

  while ((millis() - timeStart) < timeout)
  {
    bmm.read_mag_data();

    /* Update x-Axis max/min value */
    if (value_x_min > bmm.raw_mag_data.raw_datax)
    {
      value_x_min = bmm.raw_mag_data.raw_datax;
      // Serial.print("Update value_x_min: ");
      // Serial.println(value_x_min);

    }
    else if (value_x_max < bmm.raw_mag_data.raw_datax)
    {
      value_x_max = bmm.raw_mag_data.raw_datax;
      // Serial.print("update value_x_max: ");
      // Serial.println(value_x_max);
    }

    /* Update y-Axis max/min value */
    if (value_y_min > bmm.raw_mag_data.raw_datay)
    {
      value_y_min = bmm.raw_mag_data.raw_datay;
      // Serial.print("Update value_y_min: ");
      // Serial.println(value_y_min);

    }
    else if (value_y_max < bmm.raw_mag_data.raw_datay)
    {
      value_y_max = bmm.raw_mag_data.raw_datay;
      // Serial.print("update value_y_max: ");
      // Serial.println(value_y_max);
    }

    /* Update z-Axis max/min value */
    if (value_z_min > bmm.raw_mag_data.raw_dataz)
    {
      value_z_min = bmm.raw_mag_data.raw_dataz;
      // Serial.print("Update value_z_min: ");
      // Serial.println(value_z_min);

    }
    else if (value_z_max < bmm.raw_mag_data.raw_dataz)
    {
      value_z_max = bmm.raw_mag_data.raw_dataz;
      // Serial.print("update value_z_max: ");
      // Serial.println(value_z_max);
    }

    Serial.print(".");
    M5.Lcd.printf(".");
    delay(100);


  }

  value_offset.x = value_x_min + (value_x_max - value_x_min) / 2;
  value_offset.y = value_y_min + (value_y_max - value_y_min) / 2;
  value_offset.z = value_z_min + (value_z_max - value_z_min) / 2;


}

void loop()
{ //M5.Lcd.fillRect(0, 0, 320, 240, TFT_WHITE);


  if (headingDegrees > 0 && headingDegrees < 90) {
    M5.Lcd.fillTriangle(160 - (80 * sin(headingDegrees * 0.0174532925)), 120 - (80 * cos(headingDegrees * 0.0174532925)), 160 - (66.68 * sin((headingDegrees + 13) * 0.0174532925)), 120 - (66.68 * cos((headingDegrees + 13) * 0.0174532925)), 160 - (66.68 * sin((headingDegrees - 13) * 0.0174532925)), 120 - (66.68 * cos((headingDegrees - 13) * 0.0174532925)), TFT_WHITE);
    /*
      M5.Lcd.drawLine(160 - (70 * sin(headingDegrees * 0.0174532925)) - 1, 120 - (70 * cos(headingDegrees * 0.0174532925)), 160-1, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 - (70 * sin(headingDegrees * 0.0174532925)), 120 - (70 * cos(headingDegrees * 0.0174532925)), 160, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 - (70 * sin(headingDegrees * 0.0174532925)) + 1, 120 - (70 * cos(headingDegrees * 0.0174532925)), 160+1, 120, TFT_WHITE);
    */
  }

  if (headingDegrees > 90 && headingDegrees < 180) {
    M5.Lcd.fillTriangle(160 - (80 * sin((180 - headingDegrees) * 0.0174532925)), 120 + (80 * cos((180 - headingDegrees) * 0.0174532925)), 160 - (66.68 * sin((180 - (headingDegrees + 13)) * 0.0174532925)), 120 + (66.68 * cos((180 - (headingDegrees + 13)) * 0.0174532925)), 160 - (66.68 * sin((180 - (headingDegrees - 13)) * 0.0174532925)), 120 + (66.68 * cos((180 - (headingDegrees - 13)) * 0.0174532925)), TFT_WHITE);
    /*
      M5.Lcd.drawLine(160 - (70 * sin((180 - headingDegrees) * 0.0174532925)) - 1, 120 + (70 * cos((180 - headingDegrees) * 0.0174532925)), 160-1, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 - (70 * sin((180 - headingDegrees) * 0.0174532925)), 120 + (70 * cos((180 - headingDegrees) * 0.0174532925)), 160, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 - (70 * sin((180 - headingDegrees) * 0.0174532925)) + 1, 120 + (70 * cos((180 - headingDegrees) * 0.0174532925)), 160+1, 120, TFT_WHITE);
    */
  }

  if (headingDegrees > 180 && headingDegrees < 270) {
    M5.Lcd.fillTriangle(160 + (80 * sin((headingDegrees - 180) * 0.0174532925)), 120 + (80 * cos((headingDegrees - 180) * 0.0174532925)), 160 + (66.68 * sin(((headingDegrees + 13) - 180) * 0.0174532925)), 120 + (66.68 * cos(((headingDegrees + 13) - 180) * 0.0174532925)), 160 + (66.68 * sin(((headingDegrees - 13) - 180) * 0.0174532925)), 120 + (66.68 * cos(((headingDegrees - 13) - 180) * 0.0174532925)), TFT_WHITE);
    /*
      M5.Lcd.drawLine(160 + (70 * sin((headingDegrees - 180) * 0.0174532925)) - 1, 120 + (70 * cos((headingDegrees - 180) * 0.0174532925)), 160-1, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 + (70 * sin((headingDegrees - 180) * 0.0174532925)), 120 + (70 * cos((headingDegrees - 180) * 0.0174532925)), 160, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 + (70 * sin((headingDegrees - 180) * 0.0174532925)) + 1, 120 + (70 * cos((headingDegrees - 180) * 0.0174532925)), 160+1, 120, TFT_WHITE);
    */
  }

  if (headingDegrees > 270 && headingDegrees < 360) {
    M5.Lcd.fillTriangle(160 + (80 * sin((360 - headingDegrees) * 0.0174532925)), 120 - (80 * cos((360 - headingDegrees) * 0.0174532925)), 160 + (66.68 * sin((360 - (headingDegrees + 13)) * 0.0174532925)), 120 - (66.68 * cos((360 - (headingDegrees + 13)) * 0.0174532925)), 160 + (66.68 * sin((360 - (headingDegrees - 13)) * 0.0174532925)), 120 - (66.68 * cos((360 - (headingDegrees - 13)) * 0.0174532925)), TFT_WHITE);
    /*
      M5.Lcd.drawLine(160 + (70 * sin((360 - headingDegrees) * 0.0174532925)) - 1, 120 - (70 * cos((360 - headingDegrees) * 0.0174532925)), 160-1, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 + (70 * sin((360 - headingDegrees) * 0.0174532925)), 120 - (70 * cos((360 - headingDegrees) * 0.0174532925)), 160, 120, TFT_WHITE);
      M5.Lcd.drawLine(160 + (70 * sin((360 - headingDegrees) * 0.0174532925)) + 1, 120 - (70 * cos((360 - headingDegrees) * 0.0174532925)), 160+1, 120, TFT_WHITE);
    */
  }

  bmm150_mag_data value;
  bmm.read_mag_data();

  value.x = bmm.raw_mag_data.raw_datax - value_offset.x;
  value.y = bmm.raw_mag_data.raw_datay - value_offset.y;
  value.z = bmm.raw_mag_data.raw_dataz - value_offset.z;


  lineX = dir_media_;
  lineY = value.y;
  float xyHeading = atan2(value.x, value.y);
  float zxHeading = atan2(value.z, value.x);
  float heading = xyHeading;

  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  headingDegrees = heading * 180 / M_PI;
  float xyHeadingDegrees = xyHeading * 180 / M_PI;
  float zxHeadingDegrees = zxHeading * 180 / M_PI;


  Serial.print("Heading: ");
  Serial.println(headingDegrees);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setCursor(0, 15);
  M5.Lcd.printf("Heading: ");
  M5.Lcd.print(headingDegrees);
  M5.Lcd.fillTriangle(160, 112, 145, 127, 175, 127, TFT_RED);
  M5.Lcd.setTextColor(TFT_RED);
  M5.Lcd.setCursor(156, 40);
  M5.Lcd.printf("N");
  M5.Lcd.setCursor(156, 187);
  M5.Lcd.printf("S");
  M5.Lcd.setCursor(80, 115);
  M5.Lcd.printf("W");
  M5.Lcd.setCursor(231, 115);
  M5.Lcd.printf("E");


  if (headingDegrees > 0 && headingDegrees < 90) {
    M5.Lcd.fillTriangle(160 - (80 * sin(headingDegrees * 0.0174532925)), 120 - (80 * cos(headingDegrees * 0.0174532925)), 160 - (66.68 * sin((headingDegrees + 13) * 0.0174532925)), 120 - (66.68 * cos((headingDegrees + 13) * 0.0174532925)), 160 - (66.68 * sin((headingDegrees - 13) * 0.0174532925)), 120 - (66.68 * cos((headingDegrees - 13) * 0.0174532925)), TFT_RED);
    /*
      M5.Lcd.drawLine(160 - (70 * sin(headingDegrees * 0.0174532925)) - 1, 120 - (70 * cos(headingDegrees * 0.0174532925)), 160-1, 120, TFT_RED);
      M5.Lcd.drawLine(160 - (70 * sin(headingDegrees * 0.0174532925)), 120 - (70 * cos(headingDegrees * 0.0174532925)), 160, 120, TFT_RED);
      M5.Lcd.drawLine(160 - (70 * sin(headingDegrees * 0.0174532925)) + 1, 120 - (70 * cos(headingDegrees * 0.0174532925)), 160+1, 120, TFT_RED);
    */
  }

  if (headingDegrees > 90 && headingDegrees < 180) {
    M5.Lcd.fillTriangle(160 - (80 * sin((180 - headingDegrees) * 0.0174532925)), 120 + (80 * cos((180 - headingDegrees) * 0.0174532925)), 160 - (66.68 * sin((180 - (headingDegrees + 13)) * 0.0174532925)), 120 + (66.68 * cos((180 - (headingDegrees + 13)) * 0.0174532925)), 160 - (66.68 * sin((180 - (headingDegrees - 13)) * 0.0174532925)), 120 + (66.68 * cos((180 - (headingDegrees - 13)) * 0.0174532925)), TFT_RED);
    /*
      M5.Lcd.drawLine(160 - (70 * sin((180 - headingDegrees) * 0.0174532925)) - 1, 120 + (70 * cos((180 - headingDegrees) * 0.0174532925)), 160-1, 120, TFT_RED);
      M5.Lcd.drawLine(160 - (70 * sin((180 - headingDegrees) * 0.0174532925)), 120 + (70 * cos((180 - headingDegrees) * 0.0174532925)), 160, 120, TFT_RED);
      M5.Lcd.drawLine(160 - (70 * sin((180 - headingDegrees) * 0.0174532925)) + 1, 120 + (70 * cos((180 - headingDegrees) * 0.0174532925)), 160+1, 120, TFT_RED);
    */
  }

  if (headingDegrees > 180 && headingDegrees < 270) {
    M5.Lcd.fillTriangle(160 + (80 * sin((headingDegrees - 180) * 0.0174532925)), 120 + (80 * cos((headingDegrees - 180) * 0.0174532925)), 160 + (66.68 * sin(((headingDegrees + 13) - 180) * 0.0174532925)), 120 + (66.68 * cos(((headingDegrees + 13) - 180) * 0.0174532925)), 160 + (66.68 * sin(((headingDegrees - 13) - 180) * 0.0174532925)), 120 + (66.68 * cos(((headingDegrees - 13) - 180) * 0.0174532925)), TFT_RED);
    /*
      M5.Lcd.drawLine(160 + (70 * sin((headingDegrees - 180) * 0.0174532925)) - 1, 120 + (70 * cos((headingDegrees - 180) * 0.0174532925)), 160-1, 120, TFT_RED);
      M5.Lcd.drawLine(160 + (70 * sin((headingDegrees - 180) * 0.0174532925)), 120 + (70 * cos((headingDegrees - 180) * 0.0174532925)), 160, 120, TFT_RED);
      M5.Lcd.drawLine(160 + (70 * sin((headingDegrees - 180) * 0.0174532925)) + 1, 120 + (70 * cos((headingDegrees - 180) * 0.0174532925)), 160+1, 120, TFT_RED);
    */
  }

  if (headingDegrees > 270 && headingDegrees < 360) {
    M5.Lcd.fillTriangle(160 + (80 * sin((360 - headingDegrees) * 0.0174532925)), 120 - (80 * cos((360 - headingDegrees) * 0.0174532925)), 160 + (66.68 * sin((360 - (headingDegrees + 13)) * 0.0174532925)), 120 - (66.68 * cos((360 - (headingDegrees + 13)) * 0.0174532925)), 160 + (66.68 * sin((360 - (headingDegrees - 13)) * 0.0174532925)), 120 - (66.68 * cos((360 - (headingDegrees - 13)) * 0.0174532925)), TFT_RED);
    /*
      M5.Lcd.drawLine(160 + (70 * sin((360 - headingDegrees) * 0.0174532925)) - 1, 120 - (70 * cos((360 - headingDegrees) * 0.0174532925)), 160-1, 120, TFT_RED);
      M5.Lcd.drawLine(160 + (70 * sin((360 - headingDegrees) * 0.0174532925)), 120 - (70 * cos((360 - headingDegrees) * 0.0174532925)), 160, 120, TFT_RED);
      M5.Lcd.drawLine(160 + (70 * sin((360 - headingDegrees) * 0.0174532925)) + 1, 120 - (70 * cos((360 - headingDegrees) * 0.0174532925)), 160+1, 120, TFT_RED);
    */
  }

  delay(100);

  count++;

  if (count >= 29) count = 29;
}

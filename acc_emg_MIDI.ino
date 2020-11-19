#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#include <Wire.h>  // Wire library - used for I2C communication
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs

float mapfloat(float val, float in_min, float in_max, float out_min, float out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int temp0;
int temp1;


void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(9600);
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
  // This code goes in the SETUP section
  // Off-set Calibration
  //X-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);  // X-axis offset register
  Wire.write(1);
  Wire.endTransmission();
  delay(10);
  
  //Y-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F); // Y-axis offset register
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);
  
  //Z-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20); // Z-axis offset register
  Wire.write(3);
  Wire.endTransmission();
  delay(10);
  pinMode(2, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}
void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = (X_out/256)+0.95; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = (Y_out/256)+1;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/256;

  if (Y_out < 0){
    Y_out = 0;
  }

  if (X_out < 0){
    X_out = 0;
  }
  digitalWrite(2, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);

  int val2 = analogRead(A2);
  int val3 = analogRead(A3);
  int val4 = analogRead(A1);
  bool check2 = false;
  bool check3 = false;
  bool check4 = false;

  if (val2 > 1000){
    check2 = true;
    delay(1000);
  }
  
  if (val3 > 1000){
    check3 = true;
    delay(1000);
  }
  
  if (val4 > 1000){
    check4 = true;
    delay(1000);
  }
  while(check2 == true){
    digitalWrite(2, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
    int val4 = analogRead(A1);
    MIDI.sendControlChange(1, 50, 1);
    delay(500);
    MIDI.sendControlChange(1, 51, 1);
    delay(500);
    if (val4 > 1000){
      check2 = false;
      digitalWrite(2, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      }
  }
  while(check3 == true){
    digitalWrite(2, LOW);
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
    int val4 = analogRead(A1);
    MIDI.sendControlChange(2, 50, 1);
    delay(500);
    MIDI.sendControlChange(2, 51, 1);
    delay(500);
    if (val4 > 1000){
      check3 = false;
      digitalWrite(2, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);
      }
  }

    
  int val0 = mapfloat(X_out, 0, 2, 0, 127);
  if (val0 > 127){
    val0 = 127;
  }
  if (val0 != temp0+1 && val0 != temp0-1 && val0 != temp0){
    MIDI.sendControlChange(1, val0, 1);
    }
  delay(20);
  
  temp0 = val0;


  int val1 = mapfloat(Y_out, 0, 2, 0, 127);
  if (val1 > 127){
    val1 = 127;
  }
  if (val1 != temp1+1 && val1 != temp1-1 && val1 != temp1){
    MIDI.sendControlChange(2, val1, 1);
    }
  delay(20);

  temp1 = val1;

  bool startStop = false;
  int val5 =analogRead(A5);
  if (val5 > 1015){
    digitalWrite(2, HIGH);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    delay(1500);
    startStop = true;
    while (startStop == true){
      delay(1500);
      val5 =analogRead(A5);
      if (val5 > 1000){
        startStop = false;
        delay(1500);
      }
    }
  }
  /*Serial.println(val2);*/
  
  /*Serial.print("Xa= ");
  Serial.print(val0);
  Serial.print("   Ya= ");
  Serial.print(val1);
  Serial.print("   Za= ");
  Serial.println(Z_out);*/
}

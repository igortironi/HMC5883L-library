#include "arduino.h"
#include "HMC5883L.h"

#include <Wire.h>


#define ADDR 0x1E


HMC5883L::HMC5883L(){
}


//configs the first 3 registers and checks for communication errors.
byte HMC5883L::begin(){

  if( i2c_send(0x1E, 0x00, 0x70) ) return 1; //8-average, 15 Hz default, normal measurement.
  if( i2c_send(0x1E, 0x01, 0xA0) ) return 1; //Gain=2.
  if( i2c_send(0x1E, 0x02, 0x00) ) return 1; //Continuous-measurement mode.

  return 0;

}


//Checks i2c buffers, reads sensor values, validates, perform autogain analysis if required
byte HMC5883L::read(int *x_value, int *y_value, int *z_value){

  byte state = 0;

  do{

    if(Wire.available()){
      while( Wire.available() ) Wire.read();                              //checks i2c buffers for old data.
    }

    Wire.beginTransmission(ADDR);
    Wire.write(0x03);                                                    //Sets first data register.
    if(Wire.endTransmission())return 1;

    Wire.requestFrom(ADDR, 0x06);                                        //request sensor data.

    unsigned long timeout = millis();
    while( (Wire.available() < 6 ) || ( (millis() - timeout) > 10) );    //waits for 6 bytes of data or timeout timer, whichever comes first.
    if(millis() - timeout > 10)return 1;

    *x_value = (Wire.read()<<8) | Wire.read();
    *z_value = (Wire.read()<<8) | Wire.read();                           //read data from buffer and stores it on the desired variables.
    *y_value = (Wire.read()<<8) | Wire.read();

    if(is_autogain_on){
      state = gain_analysis(*x_value, *y_value, *z_value);               //perform auto gain analysis if active.
      if(state) delay(between_data_delay);
    }

  }while(state);

  return 0;

}


//send a register configuration to the desired register, and check for communication errors.
byte HMC5883L::i2c_send(int8_t addr, int8_t regis, int8_t data){

  Wire.beginTransmission(addr);
  Wire.write(regis);
  Wire.write(data);
  return(Wire.endTransmission());

}


//analyses the sensor values and determine if gain is optimized. If gain it's not optimized, sets a new gain.
byte HMC5883L::gain_analysis(int x_value, int y_value, int z_value){

  byte read_again = 0;
  int resultant_force = sqrt( pow(x_value , 2) + pow(y_value , 2) + pow(z_value , 2) );    //calculates the resultant force, based on a diagonal of a box.

  if(resultant_force > maximum_autogain){
    if(gain > 0){
      gain--;
      set_gain(gain);
      read_again = 1;                                       //check if gain is optimal, and if it's not, perform a change.
    }
  }
  else if(resultant_force < minimum_autogain){
    if(gain < 7){
      gain++;
      set_gain(gain);
      read_again = 1;
    }
  }

  return read_again;

}


//turns on autogain
void HMC5883L::autogain(byte condition){

  is_autogain_on = condition;

}


//set autogain parameter. and checks if parameter is a valid value
void HMC5883L::set_autogain(int maximum, int minimum){

  if( maximum <= 2047 && minimum >= 0 && maximum > minimum ){
    maximum_autogain = maximum;
    minimum_autogain = minimum;
  }

}


//checks value and sets the sensor gain.
byte HMC5883L::set_gain(byte value){

  if(value >= 0 && value <= 7){
      gain = value;                                           //Check if value is in the correct range and inverts value.
      value = 7 - value;
      if( i2c_send(ADDR, 0x01, (value << 5)) ) return 1;      //apply the change and checks communication.
  }
  else return 1;

  return 0;

}


//check value and sets sample avarege.
byte HMC5883L::set_sample_average(byte number){

  switch (number){
    case 1:
      average_sample = 0;
    break;

    case 2:
      average_sample = 1;
    break;                                     //checks value and converts to register value.

    case 4:
      average_sample = 2;
    break;

    case 8:
      average_sample = 3;
    break;

    default:
      return 1;
  }

  byte value = (average_sample << 5) | (data_output << 2);       //construct register config.

  if( i2c_send(ADDR, 0x00, value) ) return 1;                           //send data and check for errors.

  return 0;

}


byte HMC5883L::set_data_output(float number){

   between_data_delay = (int) (1000.0/number);   //calculate new data delay.


   int value = (int)(number * 100);       //switch only accepts integers, so I'm multiplying for 100 to remove decimal places and transfer to integer.

   switch (value){
    case 75:
      data_output = 0;
    break;

    case 150:
      data_output = 1;
    break;                                     //checks value and converts to register value.

    case 300:
      data_output = 2;
    break;

    case 750:
      data_output = 3;
    break;

    case 1500:
      data_output = 4;
    break;

    case 3000:
      data_output = 5;
    break;

    case 7500:
      data_output = 6;
    break;

    default:
      return 1;
  }

  value = (average_sample << 5) | (data_output << 2);       //construct register config.

  if( i2c_send(ADDR, 0x00, value) ) return 1;                           //send data and check for errors.

  return 0;

}


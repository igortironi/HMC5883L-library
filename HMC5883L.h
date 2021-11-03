#ifndef HMC5883L_H
#define HMC5883L_H

#include "arduino.h"


class HMC5883L{
public:
    HMC5883L();
    byte begin();
    byte read(int *x_value, int *y_value, int *z_value);        //read the sensor values and stores it in x,y, z value variable.
    void autogain(byte condition);                                          //turns on/off autogain.
    void set_autogain(int maximum, int minimum);                            //sets criteria to autogain adjust. Value between 0 and 2047.
    byte set_gain(byte value);                                              //sets the sensor gain. Value between 0 and 7.
    byte set_sample_average(byte number);                                   //sets the number of samples to average. Values can be 1, 2, 4, 8.
    byte set_data_output(float number);                                     //sets data output frequency. Values can be 0.75, 1.5, 3, 7.5, 15, 30, 75.



private:
    byte i2c_send(int8_t addr, int8_t regis, int8_t data);                  //internal function, used for i2c communication.
    byte gain_analysis(int x_value, int y_value, int z_value);              //internal function, analyses the sensor value to choose a suitable gain.

    byte gain = 2;                                                          //stores current gain value.
    byte is_autogain_on = 0;                                                //stores if autogain is on or off.
    int maximum_autogain = 2000, minimum_autogain = 200;                    //stores criteria to autogain control.
    byte average_sample = 3;                                                //stores sample configuration.
    byte data_output = 4;                                                   //stores data output configuration.
    byte between_data_delay = 68;                                           //time between data renew. Calculated with data output value
};

#endif // HMC5883L_H

#include <Wire.h>
#include <HMC5883L.h>

HMC5883L bussola;

void setup() {
  Wire.begin();
  
  bussola.begin();
  bussola.autogain(true);
 
  
  Serial.begin(2000000);
  
}

void loop() {

  int x,y,z;
  bussola.read(&x,&y,&z);

  float angulo = degrees( atan2(x, y) );
  
  Serial.print("Eixo x: ");
  Serial.println(x);
  Serial.print("Eixo y: ");
  Serial.println(y);
  Serial.print("Eixo z: ");
  Serial.println(z);
   Serial.print("Angulo: ");
  Serial.println(angulo);
 
  Serial.println("");

  delay(66);

}

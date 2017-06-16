// Distributed with a free-will license.

// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.

// SI7020-A20

// This code is designed to work with the SI7020-A20_I2CS I2C Mini Module available from ControlEverything.com.

// https://www.controleverything.com/content/Humidity?sku=SI7020-A20_I2CS#tabs-0-product_tabset-2


//#include <Wire.h>
#include <math.h>
#include <stdbool.h>

// SI7020-A20 I2C address is 0x40(64)

#define Addr 0x40

void setup()

{

  // Initialise I2C communication as MASTER
  Wire.begin();

  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C transmission
  Wire.beginTransmission(Addr);

  // Stop I2C transmission
  Wire.endTransmission();

  delay(300);

}


void loop()

{

    unsigned int data[2];
  

    // Start I2C transmission
    Wire.beginTransmission(Addr);
    // Send humidity measurement command, NO HOLD MASTER
    Wire.write(0xF5);
    // Stop I2C transmission
    Wire.endTransmission();
    delay(500);

    // Request 2 bytes of data
    Wire.requestFrom(Addr, 2);

	bool bFirstTransmit = false;

    // Read 2 bytes of data
    // humidity msb, humidity lsb 
    if(Wire.available() == 2)
    {
		bFirstTransmit = true;
		data[0] = Wire.read();
		data[1] = Wire.read();
    }

    // Calculate Humidity Data
    int wholehumidity  = 0;
	if (bFirstTransmit) //Was first read successful
	{
		// Convert the data
		float humidity  = ((data[0] * 256.0) + data[1]);
		humidity = ((125 * humidity) / 65536.0) - 6;
   
		// Round the Humidity down to the nearest whole number 
		wholehumidity = floor(humidity + .5); 
	}


    // Start I2C transmission
    Wire.beginTransmission(Addr);

    // Send temperature measurement command, NO HOLD MASTER
    Wire.write(0xF3);

    // Stop I2C transmission
    Wire.endTransmission();

    delay(500);

    
    // Request 2 bytes of data
    Wire.requestFrom(Addr, 2);

	bool bSecondTransmit = false;
    // Read 2 bytes of data
    // temp msb, temp lsb
    if(Wire.available() == 2)
    {
		bSecondTransmit = true;
		data[0] = Wire.read();
		data[1] = Wire.read();
    }

    // Calculate Temperature Data
	int wholeTempF = 0;
	//int wholeTempC = 0;
	if(bSecondTransmit) //Was second read successful
	{
		// Convert the data
		float temp  = ((data[0] * 256.0) + data[1]);
		float ctemp = ((175.72 * temp) / 65536.0) - 46.85;
		float ftemp = ctemp * 1.8 + 32;
     
		// Round the Farenheit Tempature down to the nearest whole number 
		// and subtract 5 degrees as sensors sometimes run hot
		wholeTempF = floor((ftemp + .5)-5); 
		//wholeTempC = floor((ctemp + .5)-5); 
	}

	if(bFirstTransmit) //Was first read successful
	{

		Particle.publish("humidity", String(wholehumidity),60,PRIVATE);
	}
	else 
	{
		Particle.publish("humidity", "READ FAIL Humidity",60,PRIVATE);
	}
	
	if(bSecondTransmit) //Was second read successful
	{

		Particle.publish("temperature", String(wholeTempF),60,PRIVATE);
		//Particle.publish("temperature", String(wholeTempC),60,PRIVATE);

	}
	else 
	{
		Particle.publish("temperature", "READ FAIL Temp",60,PRIVATE);
	}

    delay(5000);

}

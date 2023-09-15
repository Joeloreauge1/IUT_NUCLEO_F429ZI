// Copyright (c) 2020 Antoine TRAN TAN
// Copyright (c) 2017 Sarah MARSH

#include "mbed.h"
#include "rtos.h"
#include "C12832.h"

// Using Arduino pin notation
C12832 lcd(D11, D13, D12, D7, D10);
I2C my_i2c(I2C_SDA, I2C_SCL);
UnbufferedSerial my_serial(USBTX,USBRX,115200);

int main()
{
    char data_mode_stby[2] = {0x07, 0x00};
    char data_SR[2] = {0x08, 0x21};
    char data_mode_act[2] = {0x07, 0x01};

    my_i2c.frequency(100000);

    my_i2c.write(0x98, data_mode_stby, 2, false);
    my_i2c.write(0x98, data_mode_act, 2, false);
    my_i2c.write(0x98, data_SR, 2, false);

    signed char xyz[3];
    char registre = 0x00;
    signed char rx_data[3];
    char buffer[100];
    bool alert = 0;


    my_serial.format(8,SerialBase::None,1);

    while (true)
    { // this is the third thread

        my_i2c.write(0x98, &registre, 1, true);
        my_i2c.read(0x99, (char*) rx_data, 3);
        lcd.cls();

        for (int i = 0; i < 3; i++)
        {
            if ((rx_data[i] & 0x20) == 0x20)
            {
                xyz[i] = (rx_data[i]) | 0xC0;
            }
            else // if ((rx_data[i] & 0x20) == 0x00)
            {
                xyz[i] = (rx_data[i]) & 0x3F;
            }
        }

        if((xyz[1]& 0x40)== 1)
        {
            alert = 1;
        }
        
        else if((xyz[2]& 0x40)== 1)
        {
            alert = 1;
        }
        
        if((xyz[3]& 0x40)== 1)
        {
            alert = 1;
        }




        lcd.locate(0, 0);
        lcd.printf("OUT_X : %.2f\n", static_cast<float>(xyz[0]) * (1.5 / 32.0));
        lcd.printf("OUT_Y : %.2f\n", static_cast<float>(xyz[1]) * (1.5 / 32.0));
        lcd.printf("OUT_Z : %.2f\n", static_cast<float>(xyz[2]) * (1.5 / 32.0));



        if(alert == 1)
        {
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Alert!!!!");


        }

        sprintf(buffer," %0.2f\n",static_cast<float>(xyz[1])* (1.5 / 32.0)*720);
        my_serial.write(buffer,strlen(buffer));
        ThisThread::sleep_for(100ms);
    }
}

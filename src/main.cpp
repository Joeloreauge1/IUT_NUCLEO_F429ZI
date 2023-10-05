// Copyright (c) 2020 Antoine TRAN TAN
// Copyright (c) 2017 Sarah MARSH

#include "mbed.h"
#include "rtos.h"
#include "C12832.h"

// Using Arduino pin notation
C12832 lcd(D11, D13, D12, D7, D10);
I2C my_i2c(I2C_SDA, I2C_SCL);

int main()
{

    float val;
    u_int16_t temp1;
    u_int16_t temp2;
    u_int16_t temp;

    char conf_tx[2] = {0x01, 0x00};
    char data_tx_temp[1] = {0x00};
    char data_rx[2];
    my_i2c.frequency(100000);
    my_i2c.write(0x90, conf_tx, 2, false);

    while (true)
    { // this is the third thread

        temp1 = static_cast<u_int16_t>(data_rx[0]) << 8;
        temp2 = static_cast<u_int16_t>(data_rx[1]);

        temp = (temp1 | temp2) >> 5;
        val = static_cast<float>(temp) ;
        my_i2c.write(0x90, data_tx_temp, 1, true);
        my_i2c.read(0x91, data_rx, 2);

        lcd.locate(0, 0);
        lcd.printf("temperature :  %f °C", val*0.125);
        ThisThread::sleep_for(10ms);
    }
}

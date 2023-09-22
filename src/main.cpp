// Copyright (c) 2020 Antoine TRAN TAN
// Copyright (c) 2017 Sarah MARSH

#include "mbed.h"
#include "rtos.h"
#include "C12832.h"
#include "c7x10g_font.h"
#include"thread"

// Using Arduino pin notation
C12832 lcd(D11, D13, D12, D7, D10);

SPI my_spi(PE_6, PE_5, PE_2, PE_4, use_gpio_ssel);

DigitalOut reset_ligne(PC_6, 0);
DigitalOut ligne_suivante(PF_8, 1);
int cpt = 0;
int orthercpt = 0;
int main()
{

    my_spi.select();
    my_spi.deselect();

    my_spi.frequency(25000000);
    my_spi.format(8, 0);

    char paquet_tx[7][2] =
        {
            {0x0E, 0x08},
            {0x11, 0x0C},
            {0x10, 0x0A},
            {0x08, 0x09},
            {0x04, 0x1F},
            {0x02, 0x08},
            {0x1F, 0x08},
        };

    while (1)
    {
        //int dizaine = orthercpt / 10;
        int unite = orthercpt % 10;
        reset_ligne = 1;
        wait_us(1);
        reset_ligne = 0;

        for (size_t i = 0; i < 7; i++)
        {
            my_spi.write(c7x10g_font[8 * (97 + unite) + i]);
            //my_spi.write(c7x10g_font[8 * (97+ dizaine) + i]);
            ligne_suivante = 0;
            ThisThread::sleep_for(1ms);
            ligne_suivante = 1;
        }

        cpt++;
        if (cpt == 100)
        {
            cpt = 0;
            orthercpt = (orthercpt+1)%100;
        }
        ThisThread::sleep_for(3ms);
    }
}

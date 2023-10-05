// Copyright (c) 2020 Antoine TRAN TAN
// Copyright (c) 2017 Sarah MARSH

#include "mbed.h"
#include "rtos.h"
#include "C12832.h"
#include "c7x10g_font.h"
#include "thread"
#include <my_header.h>

#define INT_STATUS 0x00;
#define MAIN_CONFIG_1 0x01;
#define MAIN_CONFIG_2 0x02;
#define SEQ_CONFIG_1 0x03;
#define SEQ_CONFIG_2 0x04;
#define AFE_CONFIG 0x05;
#define LED_CONFIG 0x06;
#define COLUMN_GAIN_2_1 0xA5;
#define COLUMN_GAIN_4_3 0xA6;
#define COLUMN_GAIN_6_5 0xA7;
#define COLUMN_GAIN_8_7 0xA8;
#define COLUMN_GAIN_10_9 0xA9;
#define LED_CTRL 0xC1;

// Using Arduino pin_ notation
C12832 lcd(D11, D13, D12, D7, D10);

SPI my_spi(PE_6, PE_5, PE_2, PE_4, use_gpio_ssel);
InterruptIn int_pin(PD_12);

char main_cofig_1_tx[3] = {0x01, 0x00, 0x04};
char main_cofig_2_tx[3] = {0x02, 0x00, 0x02};

char seq_config_1_tx[3] = {0x03, 0x00, 0x76};

char seq_config_2_tx[3] = {0x04, 0x00, 0x90};

char afe_config_tx[3] = {0x05, 0x00, 0x08};
char led_config_tx[3] = {0x06, 0x00, 0x0F};

char column_gain_2_1_tx[3] = {0xA5, 0x00, 0x88};
char column_gain_4_3_tx[3] = {0xA6, 0x00, 0x88};
char column_gain_6_5_tx[3] = {0xA7, 0x00, 0x88};
char column_gain_8_7_tx[3] = {0xA8, 0x00, 0x88};
char column_gain_10_9_tx[3] = {0xA9, 0x00, 0x88};

char led_ctrl_tx[3] = {0xC1, 0x00, 0x02};

char int_status[2] = {0x00, 0x80};
char int_status_rx[1];

char read_buffer[2] = {0x12, 0x80};
char buffer[120];

EventFlags eflags;

void fct_ISR(void);
UnbufferedSerial serial_port(USBTX, USBRX, 19200);
int16_t pixels[60];

int main()
{

    serial_port.format(8, SerialBase::None, 1);
    my_spi.select();
    my_spi.deselect();

    my_spi.frequency(100000);
    my_spi.format(8, 0);

    my_spi.write(main_cofig_1_tx, 3, nullptr, 0);
    my_spi.write(main_cofig_2_tx, 3, nullptr, 0);
    my_spi.write(seq_config_1_tx, 3, nullptr, 0);
    my_spi.write(seq_config_2_tx, 3, nullptr, 0);

    my_spi.write(afe_config_tx, 3, nullptr, 0);
    my_spi.write(led_config_tx, 3, nullptr, 0);

    my_spi.write(column_gain_2_1_tx, 3, nullptr, 0);
    my_spi.write(column_gain_4_3_tx, 3, nullptr, 0);
    my_spi.write(column_gain_6_5_tx, 3, nullptr, 0);
    my_spi.write(column_gain_8_7_tx, 3, nullptr, 0);
    my_spi.write(column_gain_10_9_tx, 3, nullptr, 0);
    my_spi.write(led_ctrl_tx, 3, nullptr, 0);

    ThisThread::sleep_for(100ms);

    my_spi.write(int_status, 2, int_status_rx, 1);
    int_pin.fall(&fct_ISR);

    // float min = 0, max = 0;

    char ipot, jpot;
    while (1)
    {
        eflags.wait_any(0x01);
        int k = 0;
        int cpt = 0;
        my_spi.write(read_buffer, 2, buffer, 120);

        for (int i = 0; i < 60; i++)
        {
            pixels[i] = (((uint16_t)(buffer[2 * i])) << 8) | (((uint16_t)buffer[2 * i + 1]) & 0x00FF);
        }
        // printf("st\n");
        // for (int i = 0; i < 60; i++)
        // {
        //     if (pixels[i] > 0)
        //     {
        //         printf("1.000\n");
        //     }
        //     else
        //     {
        //         printf("0.000\n");
        //     }
        // } exo Tp 1

        // exo bonus 2.4
        // for (int i = 0; i < 60; i++)
        // {
        //     if ((pixels[i]) > max)
        //     {
        //         max = (float)(pixels[i]);

        //     }
        //     else if ((pixels[i]) < min)
        //     {
        //         min = (float)(pixels[i]);

        //     }

        // }
        // for(int i = 0; i < 60 ; i++)
        //     {
        //         printf("%.3f\n" ,((float)(pixels[i]-min))/(max-min));
        //     }

        for (int j = 0; j < 6; j++)
        {
            for (int i = 0; i < 10; i++)
            {
                if (pixels[k] > 0)
                {

                    ipot += i;
                    jpot += j;
                    cpt++;
                }
                k++;
            }
        }
        if (cpt != 0)
        {
             ipot = (int)(ipot / cpt);
        jpot = (int)(jpot / cpt);

        for (int i = 0; i < 60; i++)
        {
            if (ipot == i && jpot == i)
            {
                printf("%.3f\n", 1.000);
            }

            else
            {
                printf("%.3f\n", 0.000);
            }

        }
           
        }
    }
}

void fct_ISR()
{
    eflags.set(0x01);
}

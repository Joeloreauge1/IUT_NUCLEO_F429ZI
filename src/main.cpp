// Copyright (c) 2020 Antoine TRAN TAN
// Copyright (c) 2017 Sarah MARSH

#include "mbed.h"
#include "rtos.h"
#include "C12832.h"
#define tx_message_jog 0x790

// Using Arduino pin notation
C12832 lcd(D11, D13, D12, D7, D10);
RawCAN busCan(PD_0, PD_1, 1000000);
CANMessage rx_message;


EventFlags eflag;

uint32_t read_flag;


Thread t1;
Thread t2;
void orthtermain(void);
void reception(void);

bool notifiaction_nouveau_message = false;

int main()
{
    char data[2];
 int n = 0; 
    t1.start(&orthtermain);
    t2.start(&reception);
    CANMessage tx_message_bar(0x7B0, data, 2, CANData, CANStandard);
    uint16_t motif = 0x0001;
    while (1)
    {

        read_flag = eflag.wait_any(0x01 | 0x10);
        if (read_flag == 0x10)
        {
            n++;
            if (n > 9)
            {
                n = 0;
            }
        }
        if (read_flag == 0x01)
        {
            n--;
            if (n < 0)
            {
                n = 9;
            }
        }
        motif = (0x0001 << n);
        tx_message_bar.data[0] = (uint8_t)(motif >> 8);
        tx_message_bar.data[1] = (uint8_t)(motif);

        busCan.write(tx_message_bar);

        ThisThread::sleep_for(100ms);
    }    
}

void orthtermain(void)
{

    while (1)
    {
        CANMessage requet(tx_message_jog, CANStandard);
        busCan.write(requet);
        ThisThread::sleep_for(200ms);
    }
}

void reception(void)
{

    busCan.attach(
        []()
        {
            busCan.read(rx_message);

            notifiaction_nouveau_message = true;
        });

    while (1)
    {
        if (notifiaction_nouveau_message)
        {

            if (rx_message.id == 0x791)
            {
                switch (rx_message.data[0])
                {
                case 0x08:
                    lcd.locate(0, 0);
                    lcd.cls();
                    lcd.printf("Haut");

                    break;
                case 0x01:
                    lcd.locate(0, 0);
                    lcd.cls();
                    lcd.printf("Bas");

                    break;
                case 0x10:
                    lcd.locate(0, 0);
                    lcd.cls();
                    lcd.printf("Droite");
                    eflag.set(0x01);
                    break;
                case 0x02:
                    lcd.locate(0, 0);
                    lcd.cls();
                    lcd.printf("Gauche");
                    eflag.set(0x10);
                    break;
                default:
                    break;
                }
            }
            notifiaction_nouveau_message = false;
        }
        ThisThread::sleep_for(200ms);
    }
}
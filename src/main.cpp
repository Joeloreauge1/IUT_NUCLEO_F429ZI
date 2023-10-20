// Copyright (c) 2020 Antoine TRAN TAN
// Copyright (c) 2017 Sarah MARSH

#include "mbed.h"
#include "rtos.h"
#include "C12832.h"
#define tx_message_jog 0x790
#define tx_message_cod 0x7A0

// Using Arduino pin notation
C12832 lcd(D11, D13, D12, D7, D10);
RawCAN busCan(PD_0, PD_1, 1000000);
CANMessage rx_message;
DigitalOut Rouge(D5);
DigitalOut Blue(D8);
DigitalOut Vert(D9);

EventFlags eflag;

uint32_t read_flag;

Thread t1;
Thread t2;

void trame(void);
void reception(void);

bool notification_nouveau_message = false;
bool notification_nouveau_trame = false;
uint8_t valeur_job = 0;
uint8_t valeur_codeur = 0;

int main()
{
    char data[2];    int n = 0;
    t1.start(&trame);
    t2.start(&reception);
    CANMessage tx_message_bar(0x7B0, data, 2, CANData, CANStandard);
    uint16_t motif = 0x0001;
    while (1)
    {

        read_flag = eflag.wait_any(0x80 | 0x08);
        if (read_flag == 0x80)
        {
            n++;
            if (n > 9)
            {
                n = 0;
            }
        }
        if (read_flag == 0x08)
        {
            n--;
            if (n < 0)
            {
                n = 9;
            }
        }
        
        motif = ((0x0001 << n)-1);
        tx_message_bar.data[0] = (uint8_t)(motif >> 8);
        tx_message_bar.data[1] = (uint8_t)(motif & 0x00FF);

        busCan.write(tx_message_bar);

        ThisThread::sleep_for(200ms);
    }
}

void trame(void)
{
    CANMessage requet_1(tx_message_jog, CANStandard);
    CANMessage requet_2(tx_message_cod, CANStandard);
    while (1)
    {
        if (notification_nouveau_trame)
        {

            busCan.write(requet_1);
        }
        else
        {

            busCan.write(requet_2);
        }
        ThisThread::sleep_for(100ms);
    }
}

void reception(void)
{
    busCan.filter(0x781, 0x7CF, CANStandard, 0);
    // busCan.filter(0x7A1, 0x7CF, CANStandard, 1);
    busCan.attach(
        []()
        {
            busCan.read(rx_message, 0);
            busCan.read(rx_message, 1);

            notification_nouveau_trame = true;
        });

    while (1)
    {
        if (notification_nouveau_trame)
        {
            notification_nouveau_trame = false;
            if (rx_message.id == 0x791)
            {
                lcd.cls();
                switch (valeur_job = rx_message.data[0])
                {
                case 0x08:
                    lcd.locate(0, 0);
                    // lcd.cls();
                    lcd.printf("Haut: %d", valeur_job);

                    break;
                case 0x01:
                    lcd.locate(0, 0);
                    // lcd.cls();
                    lcd.printf("Bas: %d", valeur_job);

                    break;
                case 0x10:
                    lcd.locate(0, 0);
                    // lcd.cls();
                    lcd.printf("Droite: %d", valeur_job);
                    
                    break;
                case 0x02:
                    lcd.locate(0, 0);
                    // lcd.cls();
                    lcd.printf("Gauche: %d", valeur_job);
                  
                    break;
                default:
                    break;
                }
            }

           else if (rx_message.id == 0x7A1)
            {
                //  exo 2.2 et 2.1
                
                // exo 2.3
                if(rx_message.data[0] > valeur_codeur)
                {
                    eflag.set(0x08);
                }

                 else if(rx_message.data[0] < valeur_codeur)
                {
                    eflag.set(0x80);
                }
                else
                {

                }
                valeur_codeur = rx_message.data[0];
                lcd.locate(0, 15);
                lcd.printf(" valeur coder %d", rx_message.data[0]);
            }
        }

    }
}
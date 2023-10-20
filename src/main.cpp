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
DigitalOut Bleu(D8);
DigitalOut Vert(D9);









bool notification_nouveau_message = false;
bool notification_nouveau_trame = false;


int main()
{

    
    Rouge = 1;
    Bleu = 1;
    Vert = 1;
   busCan.filter(0x240, 0x7F9, CANStandard, 0);
    busCan.attach(
        []()
        {
            busCan.read(rx_message, 0);
           

            notification_nouveau_trame = true;
        });
    while (1)
    {
 
 if(notification_nouveau_trame == true)
 {
    notification_nouveau_trame = false;
    
        if (rx_message.id == 0x240 && rx_message.data[0] == 0x01)
        {
            Rouge = 0;
            Bleu = 1;
            Vert = 1;
        }
        else if (rx_message.id == 0x242 && rx_message.data[0] == 0x01)
        {
            Bleu = 0;
            Rouge = 1;
            Vert = 1;
        }
        else if (rx_message.id == 0x244 && rx_message.data[0] == 0x01)
        {
            Rouge = 1;
            Bleu = 1;
            Vert = 0;
        }
        else if (rx_message.id == 0x246 && rx_message.data[0] == 0x01)
        {
            Rouge = 1;
            Bleu = 1;
            Vert = 1;
        }

        ThisThread::sleep_for(200ms);
 }
    }
}





// Easy bluetooth controlled scrolling text


// by Yoruk for Instructable

// 26 02 15 : first code 
// 03 03 15 : buffer updating
// 04 03 15 : faster version, both buffers
// 05 03 15 : cleaning and help


/*
WIRING :
 pin 12 - DataIn of the first led matrix module
 pin 11 - CLK of the first led matrix module
 pin 10 - CS of the first led matrix module
 
 pin 9 : RX pin of the bluetooth module
 pin 8 : TX pin of the bluetooth module
 
 5V : 5v of the matrix module, 5v of the bluetooth module
 GND : the two GND signals of the matrix modules and the bluetooth module
 
 WARNING : Use a 3v3 arduino card (nano...) or a level converter !!!
 
 Wire the matrix modules in serie. See the Instructable steps.
 
 */



//Librairie for the LEDs matrix
#include "LedControl.h"

// virtual serial lib, used with bluetooth
#include <SoftwareSerial.h>

//How many matrixes we have ?
int MatrixNumber =8;


//virtual serial port bluetooth configuration
SoftwareSerial BT(8, 9);   //pin 8 : virtual RX    pin 9 :  virtual TX


// the delay between each screen display (ms)
int SpeedDelay = 12;


//total string lenght. The first message is 25 characters long (including spaces)
int LongueurChaine=25;


char incomingByte; //incoming data from the BT link


// the two pixels arrays
char Buffer[200];   //contient tout les pixels de TOUTE LA PHRASE
char Buffer_inverse[200]; //contient tout les pixels de TOUTE LA PHRASE, mais symetrises pour affichage sur ecran inverses

// the message array
char Phrase[50];

//here we set up the led matrix
LedControl lc=LedControl(12,11,10,MatrixNumber); 


// character lib
int fontDefinitions[480] = {
    0x00,0x00,0x00,0x00,0x00,/*space*/ // is 32 in ASCII
    0x00,0xF6,0xF6,0x00,0x00,/*!*/
    0x00,0xE0,0x00,0xE0,0x00,/*"*/
    0x28,0xFE,0x28,0xFE,0x28,/*#*/
    0x00,0x64,0xD6,0x54,0x08,/*$*/
    0xC2,0xCC,0x10,0x26,0xC6,/*%*/
    0x4C,0xB2,0x92,0x6C,0x0A,/*&*/
    0x00,0x00,0xE0,0x00,0x00,/*'*/
    0x00,0x38,0x44,0x82,0x00,/*(*/
    0x00,0x82,0x44,0x38,0x00,/*)*/
    0x88,0x50,0xF8,0x50,0x88,/***/
    0x08,0x08,0x3E,0x08,0x08,/*+*/
    0x00,0x00,0x05,0x06,0x00,/*,*/
    0x08,0x08,0x08,0x08,0x08,/*-*/
    0x00,0x00,0x06,0x06,0x00,/*.*/
    0x02,0x0C,0x10,0x60,0x80,/*/*/
    0x7C,0x8A,0x92,0xA2,0x7C,/*0*/
    0x00,0x42,0xFE,0x02,0x00,/*1*/
    0x42,0x86,0x8A,0x92,0x62,/*2*/
    0x44,0x82,0x92,0x92,0x6C,/*3*/
    0x10,0x30,0x50,0xFE,0x10,/*4*/
    0xE4,0xA2,0xA2,0xA2,0x9C,/*5*/
    0x3C,0x52,0x92,0x92,0x0C,/*6*/
    0x80,0x86,0x98,0xE0,0x80,/*7*/
    0x6C,0x92,0x92,0x92,0x6C,/*8*/
    0x60,0x92,0x92,0x94,0x78,/*9*/
    0x00,0x00,0x36,0x36,0x00,/*:*/
    0x00,0x00,0x35,0x36,0x00,/*;*/
    0x10,0x28,0x44,0x82,0x00,/*<*/
    0x28,0x28,0x28,0x28,0x28,/*=*/
    0x00,0x82,0x44,0x28,0x10,/*>*/
    0x40,0x80,0x8A,0x90,0x60,/*?*/
    0x7C,0x82,0xBA,0xBA,0x62,/*@*/
    0x3E,0x48,0x88,0x48,0x3E,/*A*/
    0xFE,0x92,0x92,0x92,0x6C,/*B*/
    0x7C,0x82,0x82,0x82,0x44,/*C*/
    0xFE,0x82,0x82,0x82,0x7C,/*D*/
    0xFE,0x92,0x92,0x92,0x82,/*E*/
    0xFE,0x90,0x90,0x90,0x80,/*F*/
    0x7C,0x82,0x82,0x8A,0x4E,/*G*/
    0xFE,0x10,0x10,0x10,0xFE,/*H*/
    0x82,0x82,0xFE,0x82,0x82,/*I*/
    0x84,0x82,0xFC,0x80,0x80,/*J*/
    0xFE,0x10,0x28,0x44,0x82,/*K*/
    0xFE,0x02,0x02,0x02,0x02,/*L*/
    0xFE,0x40,0x20,0x40,0xFE,/*M*/
    0xFE,0x60,0x10,0x0C,0xFE,/*N*/
    0x7C,0x82,0x82,0x82,0x7C,/*O*/
    0xFE,0x90,0x90,0x90,0x60,/*P*/
    0x7C,0x82,0x82,0x86,0x7E,/*Q*/
    0xFE,0x90,0x98,0x94,0x62,/*R*/
    0x64,0x92,0x92,0x92,0x4C,/*S*/
    0x80,0x80,0xFE,0x80,0x80,/*T*/
    0xFC,0x02,0x02,0x02,0xFC,/*U*/
    0xF8,0x04,0x02,0x04,0xF8,/*V*/
    0xFC,0x02,0x0C,0x02,0xFC,/*W*/
    0xC6,0x28,0x10,0x28,0xC6,/*X*/
    0xC0,0x20,0x1E,0x20,0xC0,/*Y*/
    0x86,0x8A,0x92,0xA2,0xC2,/*Z*/
    0x00,0x00,0xFE,0x82,0x00,/*[*/
    0x00,0x00,0x00,0x00,0x00,/*this should be / */
    0x80,0x60,0x10,0x0C,0x02,/*]*/
    0x20,0x40,0x80,0x40,0x20,/*^*/
    0x01,0x01,0x01,0x01,0x01,/*_*/
    0x80,0x40,0x20,0x00,0x00,/*`*/
    0x04,0x2A,0x2A,0x2A,0x1E,/*a*/
    0xFE,0x12,0x22,0x22,0x1C,/*b*/
    0x1C,0x22,0x22,0x22,0x14,/*c*/
    0x1C,0x22,0x22,0x12,0xFE,/*d*/
    0x1C,0x2A,0x2A,0x2A,0x18,/*e*/
    0x10,0x7E,0x90,0x80,0x40,/*f*/
    0x18,0x25,0x25,0x25,0x1E,/*g*/
    0xFE,0x10,0x10,0x10,0x0E,/*h*/
    0x00,0x12,0x5E,0x02,0x00,/*i*/
    0x02,0x01,0x01,0x11,0x5E,/*j*/
    0xFE,0x08,0x08,0x14,0x22,/*k*/
    0x00,0x82,0xFE,0x02,0x00,/*l*/
    0x3E,0x20,0x1C,0x20,0x1E,/*m*/
    0x3E,0x20,0x20,0x20,0x1E,/*n*/
    0x1C,0x22,0x22,0x22,0x1C,/*o*/
    0x3F,0x24,0x24,0x24,0x18,/*p*/
    0x18,0x24,0x24,0x3F,0x01,/*q*/
    0x3E,0x10,0x20,0x20,0x10,/*r*/
    0x12,0x2A,0x2A,0x2A,0x04,/*s*/
    0x00,0x10,0x3C,0x12,0x04,/*t*/
    0x3C,0x02,0x02,0x02,0x3E,/*u*/
    0x30,0x0C,0x02,0x0C,0x30,/*v*/
    0x38,0x06,0x18,0x06,0x38,/*w*/
    0x22,0x14,0x08,0x14,0x22,/*x*/
    0x38,0x05,0x05,0x05,0x3E,/*y*/
    0x22,0x26,0x2A,0x32,0x22,/*z*/
    0x00,0x10,0x6C,0x82,0x82,/*{*/
    0x00,0x00,0xFF,0x00,0x00,/*|*/
    0x04,0x02,0xFF,0x02,0x04,/*|, arrow*/
    0x82,0x82,0x6C,0x10,0x00,/*}*/
    0x08,0x10,0x18,0x08,0x10/*~*/
};



void setup() {

    //matrix modules init
    for(int adresse=0;adresse<MatrixNumber;adresse++) {
        /*The MAX72XX is in power-saving mode on startup*/
        lc.shutdown(adresse,false);
        /* Set the brightness to a medium values */
        lc.setIntensity(adresse,4);
        /* and clear the display */
        lc.clearDisplay(adresse);
    }

//    BufferBuilding("Hi ! Sent me a message...", 25);   //first message on the led screen


    BufferBuilding("text", 4);   //first message on the led screen


    Serial.begin(9600); //we also set the normal serial link, for debug

    BT.begin(9600);      // bluetooth initialisation

    //we sent a message on the phone to tell the user that he can do someting...
    BT.println("Hi ! I'm waiting for a message. Type it and press SEND."); 

}



void loop() { 

    DrawText( LongueurChaine); //draw the message, in a loop


    //or, if we get something on the virtual bluetooth port...
    if (BT.available() > 0) {

        LongueurChaine = BT.available(); //incoming string lenght

        for  (int i = 0; i < LongueurChaine; i++) {

            // read the incoming byte:
            incomingByte = BT.read();

            Phrase[i] = incomingByte; ///store the character into the string array

        }

        //debug features, to check :
        Serial.print("Display: ");
        Serial.println(Phrase);
        Serial.print("Length : ");
        Serial.println(LongueurChaine);

        //buffers cleaning, to store the new message
        for(int k = 0; k < 200; k++) {
            Buffer[k]=0;
            Buffer_inverse[k]=0;

        }

        //we create the new buffers, so the new message will be drawn when leaving the sub
        BufferBuilding(Phrase, LongueurChaine);
    }

}



void BufferBuilding(char * stringToDisplay, byte stringLength)
{
    int i =0;

    //loop on every characters (on the string)
    for(int k = 0; k < stringLength; k++) {
        //here we call the characters library  
        int caract= k;   
        Buffer[i+0] =  fontDefinitions[((stringToDisplay[caract] - 32) * 5) + 0];  
        Buffer[i+1] =  fontDefinitions[((stringToDisplay[caract] - 32) * 5) + 1];  
        Buffer[i+2] =  fontDefinitions[((stringToDisplay[caract] - 32) * 5) + 2];  
        Buffer[i+3] =  fontDefinitions[((stringToDisplay[caract] - 32) * 5) + 3];  
        Buffer[i+4] =  fontDefinitions[((stringToDisplay[caract] - 32) * 5) + 4];  
        Buffer[i+5] =  B00000000;  //one blank column of leds between two characters
        i=i+6;
    }

    // here, we create a second buffer, to display the message on the switched matrixes
    for(int k = 0; k < stringLength*6; k++) {
        for(int i = 0; i < 8; i++) {
            bitWrite(  Buffer_inverse[k] ,i,  bitRead(Buffer[k] ,7-i) )   ;
        }
    }
}



void DrawText( byte stringLength) {

    for(int k = 0; k < stringLength*6; k++) { 

        delay(SpeedDelay);


//delay(10000);
        //k is the column

        //buffer drawing
        for (int m = 0; m < MatrixNumber; m++) {  //loop on the matrix                     
            if (m % 2 ==  0) //check is the module is upside-down or not...
            {
                //here, the matrix is flipped upside-down                
                for (int i = 0; i < 8; i++) {  //loop on the columns
     
                      lc.setRow(m,7-i,Buffer[i+k+8*m]);
                }
            }
            else
            {
                //classical   
                for (int i = 0; i < 8; i++) {  //loop on the columns

                    
                       lc.setRow(m,i,Buffer_inverse[i+k+8*m]);
                }
            }
        }
    }

}














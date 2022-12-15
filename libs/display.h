#ifndef DISPLAY_H
#define DISPLAY_H

#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>

// Pinos do Display LCD
#define RS  13
#define E   18
#define D4  21
#define D5  24
#define D6  26
#define D7  27

int display_lcd;


/**
 * Escreve em duas linhas do display LCD
 * @param linha1 - Primeira linha do display
 * @param linha2 - Segunda linha do display
*/
void write_textLCD(char *linha1, char *linha2) {
    lcdHome(display_lcd);
    lcdClear(display_lcd);
    
    // escreve na primeira linha
    lcdPosition(display_lcd, 0, 0);
    lcdPuts(display_lcd, linha1);

    // escreve na segunda linha
    lcdPosition(display_lcd, 0, 1);
    lcdPuts(display_lcd, linha2);
}

/**
 * Escreve em uma linha do display LCD
 * @param linha - Linha do display
 * @param texto - texto a ser escrito no display
*/
void write_textLCD_linha(int linha, char *texto) {
    lcdHome(display_lcd);
    
    lcdPosition(display_lcd, 0, linha);
    lcdPuts(display_lcd, texto);
}

/**
 * Realiza as rotinas de inicializacao do display
 */
void initDisplay(){
    display_lcd = lcdInit(2, 16, 4, RS, E, D4, D5, D6, D7, 0, 0, 0, 0) ;
    lcdHome(display_lcd);
    lcdClear(display_lcd);

}  
    

#endif
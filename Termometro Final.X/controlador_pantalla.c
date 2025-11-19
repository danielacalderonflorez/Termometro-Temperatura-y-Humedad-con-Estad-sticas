#include "controlador_pantalla.h"

void Pantalla_ConfigurarPuerto(char a) {
    if(a & 1) PANTALLA_D4 = 1; else PANTALLA_D4 = 0;
    if(a & 2) PANTALLA_D5 = 1; else PANTALLA_D5 = 0;
    if(a & 4) PANTALLA_D6 = 1; else PANTALLA_D6 = 0;
    if(a & 8) PANTALLA_D7 = 1; else PANTALLA_D7 = 0;
}

void Pantalla_EnviarComando(char a) {
    PANTALLA_RS = 0;
    Pantalla_ConfigurarPuerto(a);
    PANTALLA_EN = 1;
    __delay_ms(4);
    PANTALLA_EN = 0;
    __delay_ms(4);
}

void Pantalla_Limpiar(void) {
    Pantalla_EnviarComando(0);
    Pantalla_EnviarComando(1);
    __delay_ms(20);
}

void Pantalla_EstablecerPosicion(char a, char b) {
    char temp, z, y;
    if(a == 1) {
        temp = 0x80 + b - 1;
    } else {
        temp = 0xC0 + b - 1;
    }
    z = temp >> 4;
    y = temp & 0x0F;
    Pantalla_EnviarComando(z);
    Pantalla_EnviarComando(y);
}

void Pantalla_Inicializar(void) {
    PANTALLA_RS_DIR = 0;
    PANTALLA_EN_DIR = 0;
    PANTALLA_D4_DIR = 0;
    PANTALLA_D5_DIR = 0;
    PANTALLA_D6_DIR = 0;
    PANTALLA_D7_DIR = 0;
    
    __delay_ms(20);
    Pantalla_EnviarComando(0x03);
    __delay_ms(5);
    Pantalla_EnviarComando(0x03);
    __delay_ms(11);
    Pantalla_EnviarComando(0x03);
    __delay_ms(5);
    Pantalla_EnviarComando(0x02);
    __delay_ms(5);
    Pantalla_EnviarComando(0x02);
    Pantalla_EnviarComando(0x08);
    __delay_ms(5);
    Pantalla_EnviarComando(0x00);
    Pantalla_EnviarComando(0x0C);
    __delay_ms(5);
    Pantalla_EnviarComando(0x00);
    Pantalla_EnviarComando(0x01);
    __delay_ms(20);
}

void Pantalla_EscribirCaracter(char a) {
    char temp, y;
    temp = a & 0x0F;
    y = a & 0xF0;
    PANTALLA_RS = 1;
    Pantalla_ConfigurarPuerto(y >> 4);
    PANTALLA_EN = 1;
    __delay_us(40);
    PANTALLA_EN = 0;
    Pantalla_ConfigurarPuerto(temp);
    PANTALLA_EN = 1;
    __delay_us(40);
    PANTALLA_EN = 0;
    __delay_ms(2);
}

void Pantalla_EscribirTexto(const char *a) {
    int i;
    for(i = 0; a[i] != '\0'; i++)
        Pantalla_EscribirCaracter(a[i]);
}

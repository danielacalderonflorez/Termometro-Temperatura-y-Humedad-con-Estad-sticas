#ifndef CONTROLADOR_PANTALLA_H
#define CONTROLADOR_PANTALLA_H

#define _XTAL_FREQ 20000000

#include <xc.h>

#define PANTALLA_RS PORTDbits.RD2
#define PANTALLA_EN PORTDbits.RD3
#define PANTALLA_D4 PORTDbits.RD4
#define PANTALLA_D5 PORTDbits.RD5
#define PANTALLA_D6 PORTDbits.RD6
#define PANTALLA_D7 PORTDbits.RD7

#define PANTALLA_RS_DIR TRISDbits.TRISD2
#define PANTALLA_EN_DIR TRISDbits.TRISD3
#define PANTALLA_D4_DIR TRISDbits.TRISD4
#define PANTALLA_D5_DIR TRISDbits.TRISD5
#define PANTALLA_D6_DIR TRISDbits.TRISD6
#define PANTALLA_D7_DIR TRISDbits.TRISD7

void Pantalla_ConfigurarPuerto(char a);
void Pantalla_EnviarComando(char a);
void Pantalla_Limpiar(void);
void Pantalla_EstablecerPosicion(char a, char b);
void Pantalla_Inicializar(void);
void Pantalla_EscribirCaracter(char a);
void Pantalla_EscribirTexto(const char *a);

#endif
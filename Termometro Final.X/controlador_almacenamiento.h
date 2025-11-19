#ifndef CONTROLADOR_ALMACENAMIENTO_H
#define CONTROLADOR_ALMACENAMIENTO_H

#define _XTAL_FREQ 20000000
#include <xc.h>
#include <stdint.h>

#define ALMACENAMIENTO_CS_PIN    PORTBbits.RB0
#define ALMACENAMIENTO_CS_TRIS   TRISBbits.TRISB0
#define ALMACENAMIENTO_MOSI_PIN  PORTBbits.RB1
#define ALMACENAMIENTO_MOSI_TRIS TRISBbits.TRISB1
#define ALMACENAMIENTO_MISO_PIN  PORTBbits.RB2
#define ALMACENAMIENTO_MISO_TRIS TRISBbits.TRISB2
#define ALMACENAMIENTO_SCK_PIN   PORTBbits.RB3
#define ALMACENAMIENTO_SCK_TRIS  TRISBbits.TRISB3

#define ALMACENAMIENTO_CMD0    0x00
#define ALMACENAMIENTO_CMD8    0x08
#define ALMACENAMIENTO_CMD16   0x10
#define ALMACENAMIENTO_CMD17   0x11
#define ALMACENAMIENTO_CMD24   0x18
#define ALMACENAMIENTO_CMD41   0x29
#define ALMACENAMIENTO_CMD55   0x37

void Almacenamiento_IniciarSPI(void);
uint8_t Almacenamiento_TransferirSPI(uint8_t dato);
uint8_t Almacenamiento_Inicializar(void);
void Almacenamiento_LeerDatos(uint32_t sector, uint16_t desplazamiento, uint8_t *buffer, uint8_t longitud);
uint8_t Almacenamiento_LeerBloque(uint32_t sector, uint8_t *buffer);
uint8_t Almacenamiento_EscribirBloque(uint32_t sector, uint8_t *buffer);
uint8_t Almacenamiento_LeerByte(uint32_t sector, uint16_t desplazamiento);
void Almacenamiento_EscribirDatos(uint32_t sector, uint16_t desplazamiento, uint8_t *buffer, uint8_t longitud);

#endif
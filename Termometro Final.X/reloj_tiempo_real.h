#ifndef RELOJ_TIEMPO_REAL_H
#define RELOJ_TIEMPO_REAL_H

#define _XTAL_FREQ 20000000
#include <xc.h>
#include <stdint.h>

#define RELOJ_SCL_PIN  PORTCbits.RC3
#define RELOJ_SDA_PIN  PORTCbits.RC4
#define RELOJ_SCL_TRIS TRISCbits.TRISC3
#define RELOJ_SDA_TRIS TRISCbits.TRISC4

#define RELOJ_DIRECCION_I2C 0x68

#define RELOJ_REG_SEGUNDOS  0x00
#define RELOJ_REG_MINUTOS   0x01
#define RELOJ_REG_HORAS     0x02
#define RELOJ_REG_DIA       0x03
#define RELOJ_REG_FECHA     0x04
#define RELOJ_REG_MES       0x05
#define RELOJ_REG_ANIO      0x06

typedef struct {
    uint8_t segundos;
    uint8_t minutos;
    uint8_t horas;
    uint8_t dia;
    uint8_t fecha;
    uint8_t mes;
    uint8_t anio;
} Reloj_FechaHora;

void    Reloj_IniciarModulo(void);
uint8_t Reloj_VerificarConexion(void);
uint8_t Reloj_ObtenerFechaHora(Reloj_FechaHora *fechahora);
uint8_t Reloj_ConvertirBCD(uint8_t bcd);

#endif
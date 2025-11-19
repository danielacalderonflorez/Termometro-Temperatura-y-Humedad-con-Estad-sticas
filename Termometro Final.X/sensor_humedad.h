#ifndef SENSOR_HUMEDAD_H
#define SENSOR_HUMEDAD_H

#define _XTAL_FREQ 20000000

#include <xc.h>
#include <stdint.h>

#define SENSOR_TRIS TRISCbits.TRISC2
#define SENSOR_PORT PORTCbits.RC2

void SensorHumedad_Configurar(void);
uint8_t SensorHumedad_ObtenerDatos(float *temperatura, float *humedad);

#endif
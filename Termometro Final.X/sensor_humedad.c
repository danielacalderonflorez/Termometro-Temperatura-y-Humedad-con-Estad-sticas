#include "sensor_humedad.h"

void SensorHumedad_Configurar(void){
    SENSOR_TRIS = 0;
    SENSOR_PORT = 1;
    __delay_ms(100);
}

uint8_t SensorHumedad_ObtenerDatos(float *temperatura, float *humedad){
    uint8_t bits[5];
    uint8_t i,j=0;
     
    SENSOR_PORT = 0;
    __delay_ms(18);  
    
    SENSOR_PORT = 1;
    __delay_us(40);
    
    SENSOR_TRIS = 1;
    
    if(SENSOR_PORT){
        SENSOR_TRIS = 0;
        SENSOR_PORT = 1;
        return 0;
    }    
    __delay_us(80);
    
    if(!SENSOR_PORT){
        SENSOR_TRIS = 0;
        SENSOR_PORT = 1;
        return 0;
    }
    __delay_us(80);
    
    for (j=0; j<5; j++)
    {
        uint8_t resultado=0;
        for (i=0; i<8; i++)
        {
            while (!SENSOR_PORT);
            __delay_us(35);
            if (SENSOR_PORT)
                resultado |= (1<<(7-i));
                    
            while(SENSOR_PORT);
        }
        bits[j] = resultado;
    }
    
    SENSOR_TRIS = 0;
    SENSOR_PORT = 1;
    
    if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4])
    {
        uint16_t humedad_cruda = ((uint16_t)bits[0]<<8) | bits[1];
        uint16_t temperatura_cruda = ((uint16_t)bits[2]<<8) | bits[3];
        
        if (temperatura_cruda & 0x8000){
            *temperatura = (float)((temperatura_cruda & 0x7fff) / 10.0) * -1.0;
        }else{
            *temperatura = (float)(temperatura_cruda)/10.0;
        }
        *humedad = (float)(humedad_cruda)/10.0;
        return 1;  	
    }
    return 0;
}
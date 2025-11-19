#include "controlador_almacenamiento.h"

#define ALMACENAMIENTO_CS_BAJO()   ALMACENAMIENTO_CS_PIN = 0
#define ALMACENAMIENTO_CS_ALTO()  ALMACENAMIENTO_CS_PIN = 1

void Almacenamiento_IniciarSPI(void){
    ALMACENAMIENTO_CS_TRIS = 0;
    ALMACENAMIENTO_MOSI_TRIS = 0;
    ALMACENAMIENTO_MISO_TRIS = 1;
    ALMACENAMIENTO_SCK_TRIS = 0;
    ALMACENAMIENTO_CS_ALTO();
    ALMACENAMIENTO_SCK_PIN = 0;
    ALMACENAMIENTO_MOSI_PIN = 1;
}

uint8_t Almacenamiento_TransferirSPI(uint8_t dato){
    uint8_t i, resultado = 0;
    for(i = 0; i < 8; i++){
        ALMACENAMIENTO_MOSI_PIN = (dato & 0x80) ? 1 : 0;
        dato <<= 1;
        ALMACENAMIENTO_SCK_PIN = 1;
        __delay_us(1);
        resultado = (uint8_t)((resultado << 1) | (ALMACENAMIENTO_MISO_PIN ? 1 : 0));
        ALMACENAMIENTO_SCK_PIN = 0;
        __delay_us(1);
    }
    return resultado;
}

static uint8_t Almacenamiento_EnviarComando(uint8_t cmd, uint32_t arg){
    uint8_t r, i, crc = 0xFF;
    if(cmd == ALMACENAMIENTO_CMD0) crc = 0x95;
    if(cmd == ALMACENAMIENTO_CMD8) crc = 0x87;
    
    Almacenamiento_TransferirSPI(0xFF);
    
    Almacenamiento_TransferirSPI(cmd | 0x40);
    Almacenamiento_TransferirSPI((uint8_t)(arg >> 24));
    Almacenamiento_TransferirSPI((uint8_t)(arg >> 16));
    Almacenamiento_TransferirSPI((uint8_t)(arg >> 8));
    Almacenamiento_TransferirSPI((uint8_t)arg);
    Almacenamiento_TransferirSPI(crc);
    
    for(i = 0; i < 10; i++){
        r = Almacenamiento_TransferirSPI(0xFF);
        if(!(r & 0x80)) return r;
    }
    return 0xFF;
}

uint8_t Almacenamiento_Inicializar(void){
    uint8_t i, r;
    uint16_t reintentos;
    
    Almacenamiento_IniciarSPI();
    ALMACENAMIENTO_CS_ALTO();
    for(i = 0; i < 10; i++) Almacenamiento_TransferirSPI(0xFF);
    
    ALMACENAMIENTO_CS_BAJO();
    __delay_ms(1);
    
    for(reintentos = 0; reintentos < 200; reintentos++){
        if(Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD0, 0) == 0x01) break;
    }
    if(reintentos >= 200){ ALMACENAMIENTO_CS_ALTO(); return 0; }
    
    r = Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD8, 0x000001AA);
    if(r == 0x01){
        for(i = 0; i < 4; i++) Almacenamiento_TransferirSPI(0xFF);
    }
    
    for(reintentos = 0; reintentos < 500; reintentos++){
        Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD55, 0);
        if(Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD41, 0x40000000) == 0x00) break;
        __delay_ms(10);
    }
    if(reintentos >= 500){ ALMACENAMIENTO_CS_ALTO(); return 0; }
    
    if(Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD16, 512) != 0x00){ ALMACENAMIENTO_CS_ALTO(); return 0; }
    
    ALMACENAMIENTO_CS_ALTO();
    Almacenamiento_TransferirSPI(0xFF);
    return 1;
}

void Almacenamiento_LeerDatos(uint32_t sector, uint16_t desplazamiento, uint8_t *buffer, uint8_t longitud){
    uint8_t r;
    uint16_t i, reintentos;
    
    ALMACENAMIENTO_CS_BAJO();
    if(Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD17, sector) != 0x00){ ALMACENAMIENTO_CS_ALTO(); return; }
    
    for(reintentos = 0; reintentos < 5000; reintentos++){
        r = Almacenamiento_TransferirSPI(0xFF);
        if(r == 0xFE) break;
    }
    if(reintentos >= 5000){ ALMACENAMIENTO_CS_ALTO(); return; }
    
    for(i = 0; i < desplazamiento; i++) Almacenamiento_TransferirSPI(0xFF);
    for(i = 0; i < longitud; i++) buffer[i] = Almacenamiento_TransferirSPI(0xFF);
    for(i = desplazamiento + longitud; i < 512; i++) Almacenamiento_TransferirSPI(0xFF);
    
    Almacenamiento_TransferirSPI(0xFF);
    Almacenamiento_TransferirSPI(0xFF);
    ALMACENAMIENTO_CS_ALTO();
    Almacenamiento_TransferirSPI(0xFF);
}

void Almacenamiento_EscribirDatos(uint32_t sector, uint16_t desplazamiento, uint8_t *buffer, uint8_t longitud){
    uint16_t i;
    uint8_t respuesta, byte_dato;
    
    ALMACENAMIENTO_CS_ALTO();
    Almacenamiento_TransferirSPI(0xFF);
    __delay_ms(1);
    
    ALMACENAMIENTO_CS_BAJO();
    __delay_ms(1);
    
    respuesta = Almacenamiento_EnviarComando(ALMACENAMIENTO_CMD24, sector);
    if(respuesta != 0x00){ 
        ALMACENAMIENTO_CS_ALTO(); 
        Almacenamiento_TransferirSPI(0xFF);
        return; 
    }
    
    __delay_ms(1);
    
    Almacenamiento_TransferirSPI(0xFE);
    
    for(i = 0; i < 512; i++){
        if(i >= desplazamiento && i < (desplazamiento + longitud)){
            byte_dato = buffer[i - desplazamiento];
            Almacenamiento_TransferirSPI(byte_dato);
        } else {
            Almacenamiento_TransferirSPI(0xFF);
        }
    }
    
    Almacenamiento_TransferirSPI(0xFF);
    Almacenamiento_TransferirSPI(0xFF);
    
    respuesta = Almacenamiento_TransferirSPI(0xFF);
    
    if((respuesta & 0x1F) != 0x05){ 
        ALMACENAMIENTO_CS_ALTO(); 
        Almacenamiento_TransferirSPI(0xFF);
        return; 
    }
    
    i = 0;
    while(i < 60000){
        respuesta = Almacenamiento_TransferirSPI(0xFF);
        if(respuesta != 0x00){
            break;
        }
        i++;
    }
    
    ALMACENAMIENTO_CS_ALTO();
    Almacenamiento_TransferirSPI(0xFF);
    __delay_ms(20);
}
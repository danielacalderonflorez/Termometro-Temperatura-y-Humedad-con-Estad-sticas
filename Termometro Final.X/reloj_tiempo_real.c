#include "reloj_tiempo_real.h"

#define RETARDO_I2C() __delay_us(5)

static inline void SDA_Liberar(void){ RELOJ_SDA_TRIS = 1; }
static inline void SDA_ForzarBajo(void){ RELOJ_SDA_TRIS = 0; RELOJ_SDA_PIN = 0; }
static inline void SCL_Liberar(void){ RELOJ_SCL_TRIS = 1; }
static inline void SCL_ForzarBajo(void){ RELOJ_SCL_TRIS = 0; RELOJ_SCL_PIN = 0; }

static void I2C_IniciarBus(void){
    SDA_Liberar();
    SCL_Liberar();
    RETARDO_I2C();
}

static void I2C_CondicionInicio(void){
    SDA_Liberar();
    SCL_Liberar(); RETARDO_I2C();
    SDA_ForzarBajo();  RETARDO_I2C();
    SCL_ForzarBajo();
}

static void I2C_CondicionParo(void){
    SDA_ForzarBajo();  RETARDO_I2C();
    SCL_Liberar(); RETARDO_I2C();
    SDA_Liberar(); RETARDO_I2C();
}

static uint8_t I2C_EscribirByte(uint8_t dato){
    for(uint8_t i=0;i<8;i++){
        if(dato & 0x80) SDA_Liberar(); else SDA_ForzarBajo();
        SCL_Liberar(); RETARDO_I2C();
        SCL_ForzarBajo();
        dato <<= 1;
    }
    SDA_Liberar();
    SCL_Liberar(); RETARDO_I2C();
    uint8_t confirmacion = (RELOJ_SDA_PIN == 0);
    SCL_ForzarBajo();
    return confirmacion;
}

static uint8_t I2C_LeerByte(uint8_t confirmacion){
    uint8_t dato=0;
    SDA_Liberar();
    for(uint8_t i=0;i<8;i++){
        dato <<= 1;
        SCL_Liberar(); RETARDO_I2C();
        if(RELOJ_SDA_PIN) dato |= 1;
        SCL_ForzarBajo();
    }
    if(confirmacion) SDA_ForzarBajo(); else SDA_Liberar();
    SCL_Liberar(); RETARDO_I2C();
    SCL_ForzarBajo();  SDA_Liberar();
    return dato;
}

uint8_t Reloj_ConvertirBCD(uint8_t b){ 
    return ((b>>4)*10u) + (b & 0x0Fu); 
}

void Reloj_IniciarModulo(void){ 
    I2C_IniciarBus(); 
    __delay_ms(30); 
}

uint8_t Reloj_VerificarConexion(void){
    I2C_CondicionInicio();
    uint8_t ok = I2C_EscribirByte(RELOJ_DIRECCION_I2C<<1);
    I2C_CondicionParo();
    return ok;
}

static void LimpiarFechaHora(Reloj_FechaHora *t){
    t->segundos=t->minutos=t->horas=0;
    t->dia=t->fecha=t->mes=t->anio=0;
}

uint8_t Reloj_ObtenerFechaHora(Reloj_FechaHora *fechahora){
    uint8_t r;

    if(!Reloj_VerificarConexion()){ 
        LimpiarFechaHora(fechahora); 
        return 0; 
    }

    I2C_CondicionInicio();
    if(!I2C_EscribirByte(RELOJ_DIRECCION_I2C<<1)) { 
        I2C_CondicionParo(); 
        LimpiarFechaHora(fechahora); 
        return 0; 
    }
    if(!I2C_EscribirByte(RELOJ_REG_SEGUNDOS)) { 
        I2C_CondicionParo(); 
        LimpiarFechaHora(fechahora); 
        return 0; 
    }

    I2C_CondicionInicio();
    if(!I2C_EscribirByte((RELOJ_DIRECCION_I2C<<1)|1)) { 
        I2C_CondicionParo(); 
        LimpiarFechaHora(fechahora); 
        return 0; 
    }

    r = I2C_LeerByte(1); fechahora->segundos = Reloj_ConvertirBCD(r & 0x7F);
    r = I2C_LeerByte(1); fechahora->minutos = Reloj_ConvertirBCD(r & 0x7F);
    r = I2C_LeerByte(1); fechahora->horas   = Reloj_ConvertirBCD(r & 0x3F);
    r = I2C_LeerByte(1); fechahora->dia     = Reloj_ConvertirBCD(r & 0x07);
    r = I2C_LeerByte(1); fechahora->fecha    = Reloj_ConvertirBCD(r & 0x3F);
    r = I2C_LeerByte(1); fechahora->mes   = Reloj_ConvertirBCD(r & 0x1F);
    r = I2C_LeerByte(0); fechahora->anio    = Reloj_ConvertirBCD(r);

    I2C_CondicionParo();

    if(fechahora->segundos>59 || fechahora->minutos>59 || fechahora->horas>23 ||
       fechahora->fecha<1 || fechahora->fecha>31 || fechahora->mes<1 || fechahora->mes>12){
        LimpiarFechaHora(fechahora); 
        return 0;
    }
    return 1;
}

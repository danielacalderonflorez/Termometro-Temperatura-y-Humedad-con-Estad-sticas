#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF
#pragma config BOR4V = BOR40V
#pragma config WRT = OFF

#define _XTAL_FREQ 20000000

#include <xc.h>
#include "controlador_pantalla.h"
#include "sensor_humedad.h"
#include "reloj_tiempo_real.h"
#include "controlador_almacenamiento.h"
#include "gestor_archivos.h"

#define BOTON_CAMBIO PORTAbits.RA2
#define TAMANO_BUFFER 4

static char buffer_datos[35];
static float buffer_temp[TAMANO_BUFFER];
static uint8_t idx_buffer = 0;
static uint8_t buffer_lleno = 0;

typedef struct {
    uint8_t n;
    float min, max, media;
} Stats;

static Stats stats = {0, 0, 0, 0};

void ImprimirDigito2(uint8_t v) {
    Pantalla_EscribirCaracter('0' + (v / 10));
    Pantalla_EscribirCaracter('0' + (v % 10));
}

void ImprimirTemp(float f) {
    uint16_t v = (uint16_t)(f * 10.0 + 0.5);
    if(f < 0) { Pantalla_EscribirCaracter('-'); v = -v; }
    Pantalla_EscribirCaracter('0' + (v / 100));
    Pantalla_EscribirCaracter('0' + ((v / 10) % 10));
    Pantalla_EscribirCaracter('.');
    Pantalla_EscribirCaracter('0' + (v % 10));
}

void AgregarTemp(float t) {
    buffer_temp[idx_buffer] = t;
    idx_buffer = (idx_buffer + 1) % TAMANO_BUFFER;
    if(idx_buffer == 0) buffer_lleno = 1;
}

void CalcStats(void) {
    uint8_t n = buffer_lleno ? TAMANO_BUFFER : idx_buffer;
    uint8_t i;
    float suma = 0;
    
    if(n == 0) return;
    
    stats.n = n;
    stats.min = buffer_temp[0];
    stats.max = buffer_temp[0];
    
    for(i = 0; i < n; i++) {
        suma += buffer_temp[i];
        if(buffer_temp[i] < stats.min) stats.min = buffer_temp[i];
        if(buffer_temp[i] > stats.max) stats.max = buffer_temp[i];
    }
    stats.media = suma / n;
}

void MostrarPrincipal(Reloj_FechaHora *t, float temp, float hum) {
    Pantalla_EstablecerPosicion(1, 1);
    ImprimirDigito2(t->fecha); Pantalla_EscribirCaracter('/');
    ImprimirDigito2(t->mes); Pantalla_EscribirCaracter('/');
    ImprimirDigito2(t->anio); Pantalla_EscribirTexto(" ");
    ImprimirDigito2(t->horas); Pantalla_EscribirCaracter(':');
    ImprimirDigito2(t->minutos);
    
    Pantalla_EstablecerPosicion(2, 1);
    Pantalla_EscribirTexto("T:");
    ImprimirTemp(temp);
    Pantalla_EscribirTexto(" H:");
    ImprimirTemp(hum);
    Pantalla_EscribirTexto("%  ");
}

void MostrarStats(void) {
    Pantalla_EstablecerPosicion(1, 1);
    Pantalla_EscribirTexto("Min:");
    ImprimirTemp(stats.min);
    Pantalla_EscribirTexto(" Max:");
    ImprimirTemp(stats.max);
    
    Pantalla_EstablecerPosicion(2, 1);
    Pantalla_EscribirTexto("Media:");
    ImprimirTemp(stats.media);
    Pantalla_EscribirTexto(" n=");
    Pantalla_EscribirCaracter('0' + stats.n);
}

uint8_t LeerBoton(void) {
    static uint8_t prev = 1;
    uint8_t curr = BOTON_CAMBIO;
    
    if(curr == 0 && prev == 1) {
        __delay_ms(30);
        if(BOTON_CAMBIO == 0) {
            prev = 0;
            return 1;
        }
    }
    if(curr == 1) prev = 1;
    return 0;
}

void main(void) {
    Reloj_FechaHora fecha;
    float temp = 0, hum = 0;
    uint8_t sd_ok = 0, pos, cnt = 0, sensor_ok = 0;
    uint8_t modo = 0;
    uint16_t h10;
    int16_t t10;
    static uint8_t min_prev = 0xFF;
    
    ANSEL = 0x00;
    ANSELH = 0x00;
    TRISAbits.TRISA2 = 1;
    
    Pantalla_Inicializar();
    Pantalla_Limpiar();
    Pantalla_EstablecerPosicion(1, 1);
    Pantalla_EscribirTexto("Iniciando...");
    
    SensorHumedad_Configurar();
    Reloj_IniciarModulo();
    __delay_ms(100);
    
    Pantalla_EstablecerPosicion(2, 1);
    if(Almacenamiento_Inicializar() && GestorArchivos_Configurar()) {
        sd_ok = 1;
        Pantalla_EscribirTexto("SD OK!");
        GestorArchivos_GuardarLinea("DATA.TXT", "Fecha,Hora,Temp,Hum\r\n");
    } else {
        Pantalla_EscribirTexto("SD Error!");
    }
    
    __delay_ms(500);
    Pantalla_Limpiar();
    
    Reloj_ObtenerFechaHora(&fecha);
    sensor_ok = SensorHumedad_ObtenerDatos(&temp, &hum);
    AgregarTemp(temp);
    
    MostrarPrincipal(&fecha, temp, hum);
    __delay_ms(500);
    
    while(1) {
        if(LeerBoton()) {
            modo = (modo + 1) % 2;
            Pantalla_Limpiar();
            
            if(modo == 1) {
                CalcStats();
                MostrarStats();
            } else {
                MostrarPrincipal(&fecha, temp, hum);
            }
            __delay_ms(200);
        }
        
        Reloj_ObtenerFechaHora(&fecha);
        
        if(cnt == 0) {
            sensor_ok = SensorHumedad_ObtenerDatos(&temp, &hum);
            AgregarTemp(temp);
        }
        
        if(modo == 0) {
            MostrarPrincipal(&fecha, temp, hum);
        }
        
        cnt = (cnt + 1) % 5;
        
        if(modo == 0 && fecha.minutos % 5 == 0 && fecha.minutos != min_prev) {
            min_prev = fecha.minutos;
            
            if(sd_ok && sensor_ok && temp > -40.0 && temp < 80.0) {
                pos = 0;
                
                buffer_datos[pos++] = '0' + (fecha.fecha / 10);
                buffer_datos[pos++] = '0' + (fecha.fecha % 10);
                buffer_datos[pos++] = '/';
                buffer_datos[pos++] = '0' + (fecha.mes / 10);
                buffer_datos[pos++] = '0' + (fecha.mes % 10);
                buffer_datos[pos++] = '/';
                buffer_datos[pos++] = '0' + (fecha.anio / 10);
                buffer_datos[pos++] = '0' + (fecha.anio % 10);
                buffer_datos[pos++] = ',';
                
                buffer_datos[pos++] = '0' + (fecha.horas / 10);
                buffer_datos[pos++] = '0' + (fecha.horas % 10);
                buffer_datos[pos++] = ':';
                buffer_datos[pos++] = '0' + (fecha.minutos / 10);
                buffer_datos[pos++] = '0' + (fecha.minutos % 10);
                buffer_datos[pos++] = ',';
                
                t10 = (int16_t)(temp * 10.0 + 0.5);
                if(t10 < 0) {
                    buffer_datos[pos++] = '-';
                    t10 = -t10;
                }
                buffer_datos[pos++] = '0' + (t10 / 100);
                buffer_datos[pos++] = '0' + ((t10 / 10) % 10);
                buffer_datos[pos++] = '.';
                buffer_datos[pos++] = '0' + (t10 % 10);
                buffer_datos[pos++] = ',';
                
                h10 = (uint16_t)(hum * 10.0 + 0.5);
                buffer_datos[pos++] = '0' + (h10 / 100);
                buffer_datos[pos++] = '0' + ((h10 / 10) % 10);
                buffer_datos[pos++] = '.';
                buffer_datos[pos++] = '0' + (h10 % 10);
                buffer_datos[pos++] = '\r';
                buffer_datos[pos++] = '\n';
                buffer_datos[pos] = '\0';
                
                if(GestorArchivos_GuardarLinea("DATA.TXT", buffer_datos)) {
                    Pantalla_EstablecerPosicion(2, 16);
                    Pantalla_EscribirCaracter('S');
                    __delay_ms(300);
                } else {
                    sd_ok = 0;
                }
            }
        }
        
        __delay_ms(500);
    }
}
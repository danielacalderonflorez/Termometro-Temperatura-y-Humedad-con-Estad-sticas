#include "gestor_archivos.h"

SistemaArchivos_Configuracion sistema_archivos;
static uint32_t sector_actual = 1000;

uint8_t GestorArchivos_Configurar(void){
    sector_actual = 1000;
    return 1;
}

uint8_t GestorArchivos_GuardarLinea(const char *nombre_archivo, const char *linea){
    uint8_t longitud = 0;
    
    while(linea[longitud] && longitud < 50) longitud++;
    
    Almacenamiento_EscribirDatos(sector_actual, 0, (uint8_t*)linea, longitud);
    
    sector_actual++;
    
    return 1;
}

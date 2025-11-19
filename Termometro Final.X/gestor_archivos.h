#ifndef GESTOR_ARCHIVOS_H
#define GESTOR_ARCHIVOS_H

#include <stdint.h>
#include "controlador_almacenamiento.h"

typedef struct {
    uint32_t cluster_inicio_lba;
    uint32_t cluster_raiz;
} SistemaArchivos_Configuracion;

extern SistemaArchivos_Configuracion sistema_archivos;

uint8_t GestorArchivos_Configurar(void);
uint8_t GestorArchivos_GuardarLinea(const char *nombre_archivo, const char *linea);

#endif
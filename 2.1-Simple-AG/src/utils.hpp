#pragma once

#include <vector>

// Módulo utilitario: conversión binario → fenotipo

/** Convierte un vector binario a un entero sin signo.
 *  Formula de suma ponderada: entero = Σ (bit_i * 2^(L-1-i)).
 *  @param bits Vector de bits binarios (MSB en la posición 0).
 *  @return Valor entero entero. */
int binarioAEntero(const std::vector<int>& bits);

/** Mapea un entero codificado de L bits a la escala fenotípica real [0,63].
 *  Formula: x = entero * 63 / (2^L - 1).
 *  @param entero Valor entero codificado.
 *  @param L Longitud del cromosoma en bits.
 *  @param precision Cantidad de decimales para redondeo.
 *  @return Valor real escalado x ∈ [0,63]. */
double mapeoReal(int entero, int L, int precision);

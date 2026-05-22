#ifndef DETECTOR_BORDES_HPP
#define DETECTOR_BORDES_HPP

#include "automata2d.hpp"
#include <functional>
#include <string>
#include <vector>

/**
 * @brief Detector de bordes usando autómatas celulares
 * 
 * Implementa varias reglas de detección de bordes:
 * - silueta: Solo borde interior (1 píxel)
 * - borde_completo: Interior y exterior (2 píxeles)
 * - erosion: Erosión morfológica
 * - dilatacion: Dilatación morfológica
 */
class DetectorBordes {
public:
    DetectorBordes();
    
    /** @brief Regla de silueta/contorno (interior) */
    std::function<int(const Automata2D&, size_t, size_t)> reglaSilueta();
    
    /** @brief Regla de borde completo (interior + exterior) */
    std::function<int(const Automata2D&, size_t, size_t)> reglaBordeCompleto();
    
    /** @brief Regla de erosión morfológica */
    std::function<int(const Automata2D&, size_t, size_t)> reglaErosion();
    
    /** @brief Regla de dilatación morfológica */
    std::function<int(const Automata2D&, size_t, size_t)> reglaDilatacion();
    
    /**
     * @brief Obtiene regla por nombre
     * @param nombre Nombre de la regla
     * @return Función regla o nullptr si no existe
     */
    std::function<int(const Automata2D&, size_t, size_t)> obtenerRegla(const std::string& nombre);
    
    /**
     * @brief Procesa la matriz con una regla
     * @param matriz Matriz de entrada
     * @param nombreRegla Nombre de la regla
     * @return Matriz procesada
     */
    std::vector<std::vector<int>> procesar(const std::vector<std::vector<int>>& matriz, const std::string& nombreRegla);
};

#endif
#ifndef PROCESADOR_IMAGEN_HPP
#define PROCESADOR_IMAGEN_HPP

#include <vector>
#include <string>
#include <SFML/Graphics/Image.hpp>

/**
 * @brief Procesador de imágenes para conversión con autómata celular
 * 
 * Maneja la carga y guardado de imágenes PNG, y su conversión
 * a/from matrices binarias (0/1) para el autómata.
 */
class ProcesadorImagen {
public:
    ProcesadorImagen();
    
    /**
     * @brief Carga una imagen desde archivo
     * @param ruta Ruta del archivo PNG
     * @return true si se cargó correctamente
     */
    bool cargarImagen(const std::string& ruta);
    
    /**
     * @brief Guarda la imagen actual a archivo
     * @param ruta Ruta de salida
     * @return true si se guardó correctamente
     */
    bool guardarImagen(const std::string& ruta) const;
    
    /**
     * @brief Convierte la imagen a matriz binaria
     * @param umbral Umbral para binarización (default 128)
     * @return Matriz de celdas (0=negro, 1=blanco)
     */
    std::vector<std::vector<int>> aMatrizBinaria(int umbral = 128) const;
    
    /**
     * @brief Crea imagen desde matriz binaria
     * @param matriz Matriz de celdas (0=negro, 1=blanco)
     */
    void deMatrizBinaria(const std::vector<std::vector<int>>& matriz);
    
    /** @brief Obtiene dimensiones de la imagen */
    sf::Vector2u getTamano() const;
    
private:
    sf::Image imagen;
};

#endif
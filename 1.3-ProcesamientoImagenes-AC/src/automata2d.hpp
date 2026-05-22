#ifndef AUTOMATA2D_HPP
#define AUTOMATA2D_HPP

#include <vector>
#include <cstddef>
#include <functional>

/**
 * @brief Autómata celular 2D para procesamiento de imágenes
 * 
 * Almacena el estado como matriz de celdas (0=fondo, 1=objeto)
 * y permite la evolución mediante reglas de transición.
 */
class Automata2D {
public:
    /**
     * @brief Constructor con dimensiones
     * @param rows Número de filas
     * @param cols Número de columnas
     */
    Automata2D(size_t rows, size_t cols);
    
    /**
     * @brief Constructor con estado inicial
     * @param initialState Matriz de celdas (0 o 1)
     */
    Automata2D(const std::vector<std::vector<int>>& initialState);
    
    /**
     * @brief Establece el valor de una celda
     * @param row Fila
     * @param col Columna
     * @param value Valor (0 o 1)
     */
    void setState(size_t row, size_t col, int value);
    
    /**
     * @brief Obtiene el valor de una celda
     * @param row Fila
     * @param col Columna
     * @return Valor de la celda (0 o 1)
     */
    int getState(size_t row, size_t col) const;
    
    /** @brief Obtiene número de filas */
    size_t getRows() const;
    
    /** @brief Obtiene número de columnas */
    size_t getCols() const;
    
    /**
     * @brief Cuenta vecinos vivos en vecindad de Moore (8 vecinos)
     * @param row Fila de la celda
     * @param col Columna de la celda
     * @return Número de vecinos vivos (máximo 8)
     */
    int countMooreNeighbors(size_t row, size_t col) const;
    
    /**
     * @brief Cuenta vecinos vivos en vecindad de Von Neumann (4 vecinos)
     * @param row Fila de la celda
     * @param col Columna de la celda
     * @return Número de vecinos vivos (máximo 4)
     */
    int countVonNeumannNeighbors(size_t row, size_t col) const;
    
    /**
     * @brief Aplica una regla de evolución (actualización síncrona)
     * @param rule Función que recibe automata y posición, retorna nuevo estado
     */
    void applyRule(std::function<int(const Automata2D&, size_t, size_t)> rule);
    
    /** @brief Obtiene copia de la matriz actual */
    std::vector<std::vector<int>> getMatrix() const;
    
private:
    std::vector<std::vector<int>> grid;
};

#endif
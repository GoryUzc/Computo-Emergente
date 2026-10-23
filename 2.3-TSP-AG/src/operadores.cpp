#include "operadores.hpp"
#include <algorithm>
#include <random>
#include <numeric>
#include <iostream>


std::vector<int> cruce_OX(const std::vector<int>& padre1,
                          const std::vector<int>& padre2,
                          std::mt19937& gen) {
    int n = padre1.size();
    std::vector<int> hijo(n, -1);
    std::vector<bool> usado(n, false);
    
    // 1. Seleccionar 2 puntos de cruce aleatorios
    std::uniform_int_distribution<int> dist(0, n - 1);
    int p1 = dist(gen);
    int p2 = dist(gen);
    
    // Asegurar que p1 < p2
    if (p1 > p2) {
        std::swap(p1, p2);
    }
    
    // 2. Copiar segmento [p1, p2] del padre1 al hijo
    for (int i = p1; i <= p2; ++i) {
        hijo[i] = padre1[i];
        usado[padre1[i]] = true;
    }
    
    // 3. Rellenar posiciones VACÍAS con orden del padre2
    // Empezar desde la posición después de p2 (circular)
    int pos_hijo = (p2 + 1) % n;  // Posición en el hijo a rellenar
    int pos_padre2 = (p2 + 1) % n;  // Posición en padre2 para leer
    
    // Contar cuántas posiciones vacías hay
    int posiciones_vacias = n - (p2 - p1 + 1);
    
    for (int i = 0; i < posiciones_vacias; ++i) {
        // Buscar la siguiente ciudad no usada en padre2
        while (usado[padre2[pos_padre2]]) {
            pos_padre2 = (pos_padre2 + 1) % n;
        }
        
        // Colocar la ciudad en la posición vacía del hijo
        hijo[pos_hijo] = padre2[pos_padre2];
        usado[padre2[pos_padre2]] = true;
        
        // Avanzar ambas posiciones
        pos_hijo = (pos_hijo + 1) % n;
        pos_padre2 = (pos_padre2 + 1) % n;
    }
    
    // 4. Verificación de seguridad (debug)
    #ifdef DEBUG_OX
    for (int i = 0; i < n; ++i) {
        if (hijo[i] == -1) {
            std::cerr << "[ERROR OX] Posición " << i << " no rellenada!\n";
        }
    }
    #endif
    
    return hijo;
}


std::vector<int> mutacion_swap(const std::vector<int>& ruta,
                               std::mt19937& gen) {
    std::vector<int> hijo = ruta;
    int n = ruta.size();
    
    if (n < 2) return hijo;  // No se puede mutar
    
    std::uniform_int_distribution<int> dist(0, n - 1);
    int i, j;
    do {
        i = dist(gen);
        j = dist(gen);
    } while (i == j);
    
    std::swap(hijo[i], hijo[j]);
    return hijo;
}


int seleccion_ruleta(const std::vector<double>& fitness,
                     std::mt19937& gen) {
    if (fitness.empty()) {
        return 0;
    }
    
    double fitness_total = std::accumulate(fitness.begin(), fitness.end(), 0.0);
    
    // Si todos tienen fitness 0, seleccionar aleatoriamente
    if (fitness_total <= 0.0) {
        std::uniform_int_distribution<size_t> dist(0, fitness.size() - 1);
        return dist(gen);
    }
    
    std::uniform_real_distribution<double> dist(0.0, fitness_total);
    double r = dist(gen);
    
    double suma_acumulada = 0.0;
    for (size_t i = 0; i < fitness.size(); ++i) {
        suma_acumulada += fitness[i];
        if (suma_acumulada >= r) {
            return static_cast<int>(i);
        }
    }
    
    return static_cast<int>(fitness.size() - 1);
}
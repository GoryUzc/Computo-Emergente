## SPEC00: Contexto del Algoritmo


##  1. Stack:
Lenguaje: C++17
Build: CMake 3.16+ con Visual Studio 2022
Sin dependencias externas (solo C++ estándar + <cmath> para logaritmos)
Sistema operativo: Windows 10/11 (x64)

**Objetivos**: Implementar seis medidas cuantitativas sobre autómatas celulares 1D con vecindad de radio 1, estados binarios {0,1}, y bordes circulares (toroidales).


## 2. Estructura de Carpetas a Crear


1.2-Medidas-Cuantitativas-AC/
├── CMakeLists.txt
├── ALGORITMO.md                    #Documentacion tecnica del algoritmo. 
├── src/                          
│   ├── main.cpp                  # Punto de entrada, orquesta análisis batch
│   ├── automata1d.hpp            # Clase del autómata celular 1D
│   ├── automata1d.cpp            # Implementación del autómata
│   ├── analizador.hpp            # Clase estática con las 6 medidas
│   └── analizador.cpp            # Implementación matemática de las medidas
├── docs/
│   └── medidas_matematicas.md    # Desarrollo formal (ya existe, referencia)
├── resultados/                   # Vacía, se llena con CSV al ejecutar
└── python/
    └── graficar.py               # Script Python para visualización (fuera de scope)

## 3. Medidad a implementar


**Fase 1** : Tres medidas Basicas:


**a.** Densidad (ρ):
Concepto: ¿Qué porcentaje del espacio está "vivo" (en estado 1)?
Matemática:
                                                    i=0
                                        ρ(t)= 1/N * ∑        s_i(t)
                                                    N−1
​Donde N  es el ancho y s_i(t)​ ∈ {0,1}.


                            ```cpp
                            // Agregar a Automata1D
                            double calcularDensidad() const {
                                int suma = 0;
                                for (int celda : estadoActual) suma += celda;
                                return static_cast<double>(suma) / estadoActual.size();
                            }

                            ```


**b** Distancia de Hamming (H):

Dado dos autómatas con la misma regla pero diferente estado inicial, ¿en cuántas celdas difieren?
Matemática:

                                                i=0
                                        H(A,B) = ∑   ∣a_i − b_i|
                                                N−1
​
​Es simplemente contar las posiciones donde difieren.

                        ```cpp
                        // Función libre o método estático
                        double distanciaHamming(const std::vector<int>& a, const std::vector<int>& b) {
                            if (a.size() != b.size()) return -1.0;
                            int diff = 0;
                            for (size_t i = 0; i < a.size(); ++i)
                                if (a[i] != b[i]) ++diff;
                            return static_cast<double>(diff) / a.size(); // Normalizada [0,1]
                        }
                        ```


**c** Parámetro de Langton (λ):

Inventado por Chris Langton. Mide la "vida" de una regla. Si λ ≈ 0, todo muere (Clase I). Si λ ≈ 1, todo explota en ruido. La complejidad (Clase IV) vive en el medio.
Matemática para el caso binario de vecindad r=1:
La regla es un número de 8 bits (256 reglas). Cada bit dice el resultado de un patrón.

                        λ = (numero de transiciones que producen 1) / 8
​

                        ```cpp
                        // Agregar a Automata1D o como utilidad
                        double calcularLambda() const {
                            int unos = 0;
                            for (int patron = 0; patron < 8; ++patron) {
                                if (aplicarRegla(patron) == 1) ++unos;
                            }
                            return static_cast<double>(unos) / 8.0;
                        }
```


                    | λ         | Comportamiento típico                                           |
                    | --------- | --------------------------------------------------------------- |
                    | 0.0 - 0.2 | Orden, muerte homogénea (Clase I)                               |
                    | 0.2 - 0.4 | Estructuras periódicas (Clase II)                               |
                    | ~0.5      | **Zona compleja** (Clase IV, ej: Regla 110 tiene λ = 4/8 = 0.5) |
                    | 0.6 - 1.0 | Caos expansivo (Clase III)                                      |

Nota: λ es una medida estática (propiedad de la regla, no del estado). No predice perfectamente la clase de Wolfram (ej: 30 y 110 tienen λ similares pero comportamientos distintos), pero da una primera aproximación.



**Fase 2**: 


**d** Entropía Espacial (Shannon):

¿Qué tan "desordenada" está una fila? Si todas las celdas son 0, entropía = 0 (máximo orden). Si hay 50% 0s y 50% 1s, entropía = 1 (máximo desorden para binario).
Matemática (Shannon):


                                                1
                                H_esp(t) =  −   ∑  p_k * log_2(p_k)
                                                k=0
​
Donde: p_1 = ρ , p_ 0= 1−ρ 

                        ```cpp
                        #include <cmath>

                        double calcularEntropiaEspacial() const {
                            double p1 = calcularDensidad();
                            if (p1 <= 0.0 || p1 >= 1.0) return 0.0; // Límite: 0*log(0) = 0

                            double p0 = 1.0 - p1;
                            return -(p0 * std::log2(p0) + p1 * std::log2(p1));
                        }
                        ```
Rango: [0,1]  para binario. Máximo en p=0.5 

                |            Situación           |   ρ  |  H\_esp |
                | :----------------------------: | :--: | :-----: |
                |          Todas muertas         |  0.0 |   0.0   |
                |     75% muertas, 25% vivas     | 0.25 |  0.811  |
                | Equiprobable (máximo desorden) |  0.5 | **1.0** |
                |     25% muertas, 75% vivas     | 0.75 |  0.811  |
                |           Todas vivas          |  1.0 |   0.0   |



**e** Entropía Temporal:

Fija una celda i  y mira su historia a través del tiempo. ¿Qué tan impredecible es?
Matematica:

                                                            T−1
                                            p_1^(i) = 1/T * ∑   s_i^(t)
                                                            t=0

​Luego aplicamos Shannon **d**.

Para el análisis cuantitativo es mejor tenerlo en Automata1D o pasar la matriz de historial a un analizador.

            ```cpp 
            // historial es vector<vector<int>> de T filas x N columnas
            double calcularEntropiaTemporal(const std::vector<std::vector<int>>& historial, int columna) {
                int T = historial.size();
                if (T == 0) return 0.0;

                int unos = 0;
                for (const auto& fila : historial)
                    unos += fila[columna];
                
                double p1 = static_cast<double>(unos) / T;
                if (p1 <= 0.0 || p1 >= 1.0) return 0.0;

                double p0 = 1.0 - p1;
                return -(p0 * std::log2(p0) + p1 * std::log2(p1));
            }

            ```


                |  H̄\_temp   |          Significado                                         |
                | :--------: | :----------------------------------------------------------- |
                |     ≈ 0    | Todas las celdas son predecibles (siempre 0 o siempre 1)     |
                |     ≈ 1    | Todas las celdas son impredecibles (alternan aleatoriamente) |
                | Intermedio | Mix: algunas celdas estables, otras caóticas                 |

**Fase 3**: 


**f**: Exponente de Lyapunov - AC 1D

En sistemas dinámicos, mide la "sensibilidad a condiciones iniciales" (efecto mariposa). Para autómatas celulares, la definición es más sutil que en ecuaciones diferenciales, pero existe una aproximación práctica:

1. Crea dos autómatas A y B con la misma regla R .
2. Estado inicial de A: un 1 en el centro (o aleatorio).
3. Estado inicial de B: idéntico a A, pero cambias 1 celda (cambias 0→1 o 1→0 en una posición).
4. Evolucionas ambos T  pasos.
5. En cada paso t , calculas la distancia de Hamming normalizada h(t) .
6. El exponente se aproxima como:


                                                    T
                                    λ_Lyap ≈ 1/T  *  ∑  ln( h(t) / h(t−1) )
​                                                    t=1
En la práctica, como los AC son discretos, muchos autores usan simplemente:

                                    λ_Lyap ≈ ln(h(T)) − ln(h(0)) / T

                ```cpp
                double calcularLyapunov(int regla, int ancho, int T) {
                    Automata1D A(ancho, regla);
                    Automata1D B(ancho, regla);
                    
                    A.reiniciarCentro();
                    B.reiniciarCentro();
                    // Flippear una celda en B (ej: la posición 40)
                    auto estadoB = B.getEstado(); // Necesitarás un setter o modificar antes de evolucionar
                    // ... modificar estadoB[40] ^= 1;
                    
                    std::vector<double> hammingHistory;
                    for (int t = 0; t < T; ++t) {
                        hammingHistory.push_back(distanciaHamming(A.getEstado(), B.getEstado()));
                        A.evolucionar();
                        B.evolucionar();
                    }
                    
                    // Regresión lineal sobre ln(hamming) vs t
                    // O aproximación simple:
                    double h0 = hammingHistory[0];
                    double hT = hammingHistory.back();
                    if (h0 < 1e-10) h0 = 1e-10; // Evitar ln(0)
                    if (hT < 1e-10) hT = 1e-10;
                    
                    return (std::log(hT) - std::log(h0)) / T;
                }

                ```
​Nota: Para AC 1D binarios, el "exponente de Lyapunov" no es tan estándar como en caos continuo. Algunos artículos usan definiciones alternativas basadas en "defectos" o "frentes de perturbación". La aproximación basada en divergencia de Hamming.

**Manejo de casos degenerados**:
Si h(t)=0  en algún paso (perturbaciones se aniquilaron), usar 
epsilon=10^−10 para evitar ln(0) . la celda perturbada debe ser diferente del centro para que h_0 > 0. Recomendado: ancho / (2 + 1).

|   λ\_L   | Comportamiento                                                                                                     |
| :------: | :----------------------------------------------------------------------------------------------------------------- |
| λ\_L > 0 | **Sensibilidad a condiciones iniciales**: perturbaciones crecen. Indicador de caos (Clase III) o complejidad (IV). |
| λ\_L ≈ 0 | Perturbaciones se propagan linealmente o se estabilizan. Típico de Clase II.                                       |
| λ\_L < 0 | Perturbaciones decaen. Convergencia a atractor            (ClaseI).                                                          |

## Clasificacion de Wolfram (referencia)

| Clase   | Descripción                               | Reglas de ejemplo               |
| ------- | ----------------------------------------- | ------------------------------- |
| **I**   | Evoluciona hacia estado homogéneo (fijo)  | 0, 32, 128, 160, 192, 232       |
| **II**  | Estructuras periódicas o estables locales | 4, 56, 90, 108, 184             |
| **III** | Caos aparente, pseudoaleatorio            | 30, 60, 126, 150                |
| **IV**  | Complejidad (emergencia, universales)     | **110** (universalmente Turing) |

## REFERENCIAS 

1. Wolfram, S. (2002). A New Kind of Science. Wolfram Media.
→ Fundamento teórico de las reglas elementales y la clasificación en 4 clases.
2. Langton, C. G. (1990). "Computation at the edge of chaos." Physica D: Nonlinear Phenomena, 42(1-3), 12-37.
→ Introducción del parámetro λ como medida de complejidad en AC.
3. Cook, M. (2004). "Universality in Elementary Cellular Automata." Complex Systems, 15(1), 1-40.
→ Demostración formal de que la Regla 110 es universalmente Turing-completa.
4. Shannon, C. E. (1948). "A Mathematical Theory of Communication." Bell System Technical Journal, 27(3), 379-423.
→ Fundamento de la entropía de Shannon usada en las medidas espacial y temporal.
5. Wolfram, S. (1984). "Universality and complexity in cellular automata." Physica D, 10(1-2), 1-35.
→ Artículo original que establece la clasificación de 4 clases de comportamiento.
6. Wuensche, A. (1999). "Classifying Cellular Automata Automatically." Complexity, 4(3), 47-66.
→ Métodos automáticos para clasificar AC, incluyendo medidas de entropía y Lyapunov.
7. Shalizi, C. R. (2001). Causal Architecture, Complexity and Self-Organization in Time Series and Cellular Automata. PhD Thesis, University of Wisconsin-Madison.
→ Tratamiento riguroso de entropías espacial y temporal en AC.
8. SFML Documentation. Simple and Fast Multimedia Library. https://www.sfml-dev.org/documentation/3.0.0/
→ Referencia técnica para la parte gráfica (si se extiende el proyecto).



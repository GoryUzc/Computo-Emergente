#include "procesador_imagen.hpp"
#include "detector_bordes.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string rutaEntrada = "assets/test_binario.png";
    std::string rutaSalida = "resultados/bordes_detectados.png";
    std::string nombreRegla = "silueta";
    
    if (argc >= 2) rutaEntrada = argv[1];
    if (argc >= 3) rutaSalida = argv[2];
    if (argc >= 4) nombreRegla = argv[3];
    
    std::cout << "[1/5] Cargando: " << rutaEntrada << std::endl;
    
    ProcesadorImagen proc;
    if (!proc.cargarImagen(rutaEntrada)) {
        std::cerr << "Error: No se pudo cargar la imagen " << rutaEntrada << std::endl;
        return 1;
    }
    
    auto tam = proc.getTamano();
    std::cout << "[2/5] Automata: " << tam.x << "x" << tam.y << std::endl;
    
    auto matriz = proc.aMatrizBinaria();
    
    std::cout << "[3/5] Regla: " << nombreRegla << " (1 iter)" << std::endl;
    
    DetectorBordes detector;
    if (detector.obtenerRegla(nombreRegla) == nullptr) {
        std::cerr << "Error: Regla desconocida: " << nombreRegla << std::endl;
        return 1;
    }
    
    auto resultado = detector.procesar(matriz, nombreRegla);
    
    proc.deMatrizBinaria(resultado);
    
    std::cout << "[4/5] Guardando: " << rutaSalida << std::endl;
    
    if (!proc.guardarImagen(rutaSalida)) {
        std::cerr << "Error: No se pudo guardar la imagen " << rutaSalida << std::endl;
        return 1;
    }
    
    int pixelesBlancos = 0;
    for (const auto& fila : resultado) {
        for (int val : fila) pixelesBlancos += val;
    }
    std::cout << "[5/5] ✅ Completado - Pixeles en borde: " << pixelesBlancos << std::endl;
    
    return 0;
}
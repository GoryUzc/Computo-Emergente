#include "analizador.hpp"

// ── 1. Parámetro de Langton ──────────────────────────────────────────────────
double AnalizadorAC::parametroLangton(int regla) {
    int unos = 0;
    for (int patron = 0; patron < 8; ++patron) {
        if (((regla >> patron) & 1) != 0) ++unos;
    }
    return static_cast<double>(unos) / 8.0;
}

// ── 2. Densidad ─────────────────────────────────────────────────────────────
double AnalizadorAC::densidad(const std::vector<int>& estado) {
    if (estado.empty()) return 0.0;
    int suma = 0;
    for (int celda : estado) suma += celda;
    return static_cast<double>(suma) / estado.size();
}

// ── 3. Distancia de Hamming ─────────────────────────────────────────────────
int AnalizadorAC::hammingCrudo(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.size() != b.size()) return -1;
    int diff = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) ++diff;
    }
    return diff;
}

double AnalizadorAC::hamming(const std::vector<int>& a, const std::vector<int>& b) {
    int crudo = hammingCrudo(a, b);
    if (crudo < 0) return -1.0;
    return static_cast<double>(crudo) / static_cast<double>(a.size());
}

// ── 4. Entropía espacial de Shannon ─────────────────────────────────────────
double AnalizadorAC::entropiaShannon(const std::vector<int>& estado) {
    double p1 = densidad(estado);
    if (p1 <= 0.0 || p1 >= 1.0) return 0.0;

    double p0 = 1.0 - p1;
    return -(p0 * std::log2(p0) + p1 * std::log2(p1));
}

// ── 5. Entropía temporal de Shannon ─────────────────────────────────────────
double AnalizadorAC::entropiaTemporal(const std::vector<std::vector<int>>& historial, int columna) {
    int T = static_cast<int>(historial.size());
    if (T == 0) return 0.0;

    int unos = 0;
    for (const auto& fila : historial) {
        if (columna >= 0 && columna < static_cast<int>(fila.size())) {
            unos += fila[columna];
        }
    }

    double p1 = static_cast<double>(unos) / static_cast<double>(T);
    if (p1 <= 0.0 || p1 >= 1.0) return 0.0;

    double p0 = 1.0 - p1;
    return -(p0 * std::log2(p0) + p1 * std::log2(p1));
}

double AnalizadorAC::entropiaTemporalPromedio(const std::vector<std::vector<int>>& historial) {
    if (historial.empty()) return 0.0;

    int N = static_cast<int>(historial[0].size());
    double suma = 0.0;
    for (int i = 0; i < N; ++i) {
        suma += entropiaTemporal(historial, i);
    }
    return suma / static_cast<double>(N);
}

// ── 6. Exponente de Lyapunov ─────────────────────────────────────────────────
double AnalizadorAC::lyapunovAproximado(int regla, int ancho, int T, int celdaPerturbada) {
    Automata1D A(ancho, regla, false);
    Automata1D B(ancho, regla, false);

    A.reiniciarCentro();
    B.reiniciarCentro();
    B.flipCelda(celdaPerturbada);

    double h0 = hamming(A.getEstado(), B.getEstado());
    const double EPSILON = 1e-10;
    if (h0 < EPSILON) h0 = EPSILON;

    for (int t = 0; t < T; ++t) {
        A.evolucionar();
        B.evolucionar();
    }

    double hT = hamming(A.getEstado(), B.getEstado());
    if (hT < EPSILON) hT = EPSILON;

    return (std::log(hT) - std::log(h0)) / static_cast<double>(T);
}

// ── Utilidades ──────────────────────────────────────────────────────────────
void AnalizadorAC::exportarCSV(const std::string& nombreArchivo,
                               const std::vector<MetricasGeneracion>& datos) {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo: " << nombreArchivo << std::endl;
        return;
    }

    archivo << "generacion,densidad,entropia_espacial,hamming_vs_inicial\n";

    for (const auto& m : datos) {
        archivo << m.generacion << ','
                << std::fixed << std::setprecision(6) << m.densidad << ','
                << std::fixed << std::setprecision(6) << m.entropiaEspacial << ','
                << std::fixed << std::setprecision(6) << m.hammingVsInicial << '\n';
    }

    archivo.close();
    std::cout << "[OK] CSV exportado: " << nombreArchivo << " (" << datos.size() << " filas)" << std::endl;
}

std::string AnalizadorAC::obtenerClaseWolfram(int regla) {
    // Prioridad: IV > III > II > I
    const int claseIV[]  = {18, 26, 54, 58, 62, 90, 94, 102, 110, 122, 126, 146, 150, 154, 158, 166, 182, 186, 210, 222};
    const int claseIII[] = {15, 18, 22, 26, 30, 42, 46, 50, 54, 58, 62, 94, 102, 110, 122, 126, 146, 150, 154, 158, 182, 186, 210, 214};
    const int claseII[]  = {4, 12, 40, 56, 60, 72, 90, 104, 108, 152, 164, 180, 184, 200, 232};
    const int claseI[]   = {0, 32, 128, 136, 160, 162, 168, 192, 200, 204, 232};

    auto pertenece = [](int valor, const int* arr, size_t n) {
        for (size_t i = 0; i < n; ++i) if (arr[i] == valor) return true;
        return false;
    };

    if (regla == 110) return "IV (Complejo - Universal)";
    if (pertenece(regla, claseIV,  20)) return "IV (Complejidad)";
    if (pertenece(regla, claseIII, 24)) return "III (Caos)";
    if (pertenece(regla, claseII,  15)) return "II (Periodica)";
    if (pertenece(regla, claseI,   11)) return "I (Orden)";

    return "Desconocida";
}

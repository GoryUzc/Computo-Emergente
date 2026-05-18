"""
Generador de Gráficas - Medidas Cuantitativas AC
Lee los CSV generados por el programa C++ y produce:
  1. comparativa_dashboard.png  — 3 subplots apilados
  2. langton_por_regla.png      — Gráfico de barras de λ
"""

import glob
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

# ── Configuración global ────────────────────────────────────────────────────
COLORES = {
    0:   "#888888",  # Gris        — Clase I
    30:  "#E74C3C",  # Rojo        — Clase III
    90:  "#3498DB",  # Azul        — Clase III
    110: "#2ECC71",  # Verde       — Clase IV
    184: "#F39C12",  # Naranja     — Clase II
}

CLASES_WOLFRAM = {
    0:   "I",
    30:  "III",
    90:  "III",
    110: "IV",
    184: "II",
}

DATOS_LANGTON = {
    0:   0.000,
    30:  0.500,
    90:  0.500,
    110: 0.625,
    184: 0.500,
}

REGLA_ORDEN = [0, 30, 90, 110, 184]


# ── Carga de datos ──────────────────────────────────────────────────────────
def cargar_datos() -> dict[int, pd.DataFrame]:
    archivos = sorted(glob.glob("resultados/regla_*_metricas.csv"))

    if not archivos:
        print("[ERROR] No se encontraron CSV en resultados/")
        print("Ejecuta primero: .\\build\\Release\\MedidasCuantitativasAC.exe")
        raise SystemExit(1)

    datos: dict[int, pd.DataFrame] = {}
    for archivo in archivos:
        nombre = os.path.basename(archivo)          # "regla_30_metricas.csv"
        partes = nombre.split("_")                   # ["regla", "30", "metricas.csv"]
        regla = int(partes[1])
        datos[regla] = pd.read_csv(archivo)

    return datos


# ── Gráfica 1: Dashboard combinado ─────────────────────────────────────────
def graficar_dashboard(datos: dict[int, pd.DataFrame]) -> None:
    fig, axes = plt.subplots(3, 1, figsize=(12, 14), sharex=True)
    fig.suptitle(
        "Medidas Cuantitativas - Autómatas Celulares 1D",
        fontsize=16, fontweight="bold",
    )

    # ── Subplot 0 — Densidad ────────────────────────────────────────────────
    for regla in REGLA_ORDEN:
        df = datos[regla]
        axes[0].plot(
            df["generacion"], df["densidad"],
            color=COLORES[regla],
            label=f"Regla {regla} (Clase {CLASES_WOLFRAM[regla]})",
            linewidth=1.5,
        )
    axes[0].set_ylabel("Densidad ρ(t)")
    axes[0].set_ylim(0, 1)
    axes[0].grid(True, alpha=0.3)
    axes[0].legend(loc="upper right", fontsize=9)
    axes[0].set_title("Evolución de la Densidad")

    # ── Subplot 1 — Entropía Espacial ───────────────────────────────────────
    for regla in REGLA_ORDEN:
        df = datos[regla]
        axes[1].plot(
            df["generacion"], df["entropia_espacial"],
            color=COLORES[regla], linewidth=1.5,
        )
    axes[1].axhline(
        y=1.0, color="black", linestyle="--", alpha=0.4,
        label="Máximo teórico (1 bit)",
    )
    axes[1].set_ylabel("Entropía (bits)")
    axes[1].set_ylim(0, 1.1)
    axes[1].grid(True, alpha=0.3)
    axes[1].legend(loc="lower right", fontsize=9)
    axes[1].set_title("Entropía Espacial de Shannon")

    # ── Subplot 2 — Hamming ─────────────────────────────────────────────────
    for regla in REGLA_ORDEN:
        df = datos[regla]
        axes[2].plot(
            df["generacion"], df["hamming_vs_inicial"],
            color=COLORES[regla], linewidth=1.5,
        )
    axes[2].set_ylabel("Hamming normalizado")
    axes[2].set_xlabel("Generación")
    axes[2].set_ylim(0, 1)
    axes[2].grid(True, alpha=0.3)
    axes[2].set_title("Divergencia del Estado Inicial")

    plt.tight_layout()
    plt.savefig("resultados/comparativa_dashboard.png", dpi=150, bbox_inches="tight")
    plt.close(fig)
    print("[OK] Dashboard guardado: resultados/comparativa_dashboard.png")


# ── Gráfica 2: Parámetro de Langton ────────────────────────────────────────
def graficar_langton() -> None:
    fig, ax = plt.subplots(figsize=(10, 6))

    etiquetas = [f"Regla {r}\n(Clase {CLASES_WOLFRAM[r]})" for r in REGLA_ORDEN]
    lambdas   = [DATOS_LANGTON[r] for r in REGLA_ORDEN]
    colores   = [COLORES[r] for r in REGLA_ORDEN]

    barras = ax.bar(etiquetas, lambdas, color=colores, edgecolor="black", alpha=0.85)

    # Línea de referencia λ = 0.5
    ax.axhline(
        y=0.5, color="red", linestyle="--", linewidth=2,
        label="Zona de complejidad (λ = 0.5)",
    )

    # Anotaciones sobre cada barra
    for barra, valor in zip(barras, lambdas):
        ax.text(
            barra.get_x() + barra.get_width() / 2,
            barra.get_height() + 0.03,
            f"{valor:.3f}",
            ha="center", fontsize=11, fontweight="bold",
        )

    ax.set_title("Parámetro de Langton (λ) por Regla", fontsize=14, fontweight="bold")
    ax.set_ylabel("λ", fontsize=12)
    ax.set_ylim(0, 1)
    ax.grid(True, axis="y", alpha=0.3)
    ax.legend(fontsize=10)

    plt.tight_layout()
    plt.savefig("resultados/langton_por_regla.png", dpi=150, bbox_inches="tight")
    plt.close(fig)
    print("[OK] Gráfico Langton guardado: resultados/langton_por_regla.png")


# ── Main ───────────────────────────────────────────────────────────────────
def main() -> None:
    print("=== Generador de Gráficas - Medidas Cuantitativas AC ===")

    datos = cargar_datos()
    print(f"Cargados {len(datos)} archivos CSV")

    graficar_dashboard(datos)
    graficar_langton()

    print("\n=== Todas las gráficas generadas correctamente ===")
    print("Revisa la carpeta resultados/")


if __name__ == "__main__":
    main()

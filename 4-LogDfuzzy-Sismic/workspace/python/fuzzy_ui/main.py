from fuzzy_ui.bridge import FuzzySystemBridge
from fuzzy_ui.cli import CLI
from fuzzy_ui.plotter import Plotter
from fuzzy_ui.exporter import Exporter


def main():
    """Inicializa y ejecuta el sistema."""
    print("Inicializando sistema difuso sísmico...")
    
    # Construir bridge y sistema
    bridge = FuzzySystemBridge()
    try:
        bridge.build_default_seismic_system()
        print("✓ Sistema construido exitosamente")
    except Exception as e:
        print(f"Error al construir el sistema: {e}")
        return
    
    # Inicializar componentes
    plotter = Plotter()
    exporter = Exporter()
    
    # Ejecutar REPL
    cli = CLI(bridge, plotter, exporter)
    cli.run()


if __name__ == "__main__":
    main()

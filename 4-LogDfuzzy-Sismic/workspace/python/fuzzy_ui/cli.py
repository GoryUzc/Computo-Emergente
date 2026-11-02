from typing import Dict, Optional, List
from fuzzy_ui.bridge import FuzzySystemBridge
from fuzzy_ui.plotter import Plotter
from fuzzy_ui.exporter import Exporter


class CLI:
    """Interfaz de línea de comandos interactiva."""
    
    def __init__(self, bridge: FuzzySystemBridge, plotter: Plotter, exporter: Exporter):
        self.bridge = bridge
        self.plotter = plotter
        self.exporter = exporter
        self.history: List[Dict] = []
    
    def run(self) -> None:
        """Bucle principal del REPL."""
        print("=" * 70)
        print("SISTEMA DIFUSO PARA EVALUACIÓN DE SEVERIDAD SÍSMICA")
        print("=" * 70)
        print("Comandos disponibles:")
        print("  calcular      - Ejecutar inferencia con nuevas entradas")
        print("  sensibilidad  - Comparar dos configuraciones de MF")
        print("  exportar      - Exportar historial a CSV")
        print("  reglas        - Mostrar base de reglas")
        print("  historial     - Mostrar resultados anteriores")
        print("  salir         - Terminar el programa")
        print("=" * 70)
        
        while True:
            try:
                command = input("\n> ").strip().lower()
                
                if command == "calcular":
                    self._handle_calcular()
                elif command == "sensibilidad":
                    self._handle_sensibilidad()
                elif command == "exportar":
                    self._handle_exportar()
                elif command == "reglas":
                    self._handle_reglas()
                elif command == "historial":
                    self._handle_historial()
                elif command == "salir":
                    print("Hasta luego!")
                    break
                elif command == "":
                    continue
                else:
                    print("Comando no reconocido. Escribe 'salir' para terminar.")
            except KeyboardInterrupt:
                print("\nUse 'salir' para terminar el programa.")
            except EOFError:
                print("\nHasta luego!")
                break
    
    def _handle_calcular(self) -> None:
        """Lee entradas, ejecuta inferencia y grafica resultados."""
        inputs = self._read_inputs()
        if inputs is None:
            return
        
        try:
            result = self.bridge.compute(inputs)
            self.history.append({"inputs": inputs, "result": result})
            
            print(f"\n{'='*70}")
            print(f"RESULTADO DE INFERENCIA")
            print(f"{'='*70}")
            print(f"Intensidad Sísmica Estimada: {result['crisp_output']:.2f} (Mercalli)")
            print(f"{'='*70}")
            print("\nGenerando gráficas...")
            
            self.plotter.plot_all(
                inputs=inputs,
                result=result,
                bridge=self.bridge
            )
        except Exception as e:
            print(f"\nError durante la inferencia: {e}")
    
    def _handle_sensibilidad(self) -> None:
        """Compara dos configuraciones de funciones de pertenencia."""
        print("\nAnálisis de Sensibilidad")
        print("-" * 70)
        print("Esta funcionalidad permite comparar cómo cambian los resultados")
        print("al modificar las funciones de pertenencia.")
        print("\nPara un análisis completo, modifique los parámetros en config.py")
        print("y ejecute el mismo caso de prueba con ambas configuraciones.")
        print("\nEjemplo: variar la anchura de las MF triangulares ±20%")
        print("y observar el cambio en la intensidad estimada.")
    
    def _handle_exportar(self) -> None:
        """Exporta el historial a CSV."""
        if not self.history:
            print("No hay resultados para exportar.")
            return
        
        path = input("Ruta del archivo CSV (default: resultados.csv): ").strip()
        if not path:
            path = "resultados.csv"
        
        try:
            self.exporter.export_to_csv(self.history, path)
            print(f"✓ Resultados exportados a {path}")
        except Exception as e:
            print(f"Error al exportar: {e}")
    
    def _handle_reglas(self) -> None:
        """Muestra la base de reglas."""
        print("\nBASE DE REGLAS DIFUSAS")
        print("-" * 70)
        rules = self.bridge.get_rules()
        config = self.bridge._config
        
        for rule_config in config["rules"]:
            rule_id = rule_config["id"]
            antecedents = rule_config["antecedents"]
            consequent = rule_config["consequent"]
            
            ant_str = " Y ".join([f"{var} es {term}" for var, term in antecedents])
            cons_var, cons_term = consequent
            
            print(f"R{rule_id:2d}: SI {ant_str}")
            print(f"       ENTONCES {cons_var} es {cons_term}")
            print()
    
    def _handle_historial(self) -> None:
        """Muestra los resultados anteriores."""
        if not self.history:
            print("No hay resultados en el historial.")
            return
        
        print("\nHISTORIAL DE RESULTADOS")
        print("-" * 70)
        for i, entry in enumerate(self.history, 1):
            inputs = entry["inputs"]
            result = entry["result"]
            print(f"{i}. M={inputs['Magnitud']:.2f}, h={inputs['Profundidad']:.1f}km, "
                  f"d={inputs['Distancia']:.1f}km → I={result['crisp_output']:.2f}")
    
    def _read_inputs(self) -> Optional[Dict[str, float]]:
        """Lee y valida las entradas del usuario."""
        print("\nIngrese los valores (o 'cancelar' para volver):")
        
        magnitud = self._read_float("Magnitud (3.0-9.0): ", 3.0, 9.0)
        if magnitud is None:
            return None
        
        profundidad = self._read_float("Profundidad (1-300 km): ", 1.0, 300.0)
        if profundidad is None:
            return None
        
        distancia = self._read_float("Distancia (1-500 km): ", 1.0, 500.0)
        if distancia is None:
            return None
        
        return {
            "Magnitud": magnitud,
            "Profundidad": profundidad,
            "Distancia": distancia
        }
    
    def _read_float(self, prompt: str, min_val: float, max_val: float) -> Optional[float]:
        """Lee un float y valida que esté en el rango especificado."""
        while True:
            value_str = input(prompt).strip()
            if value_str.lower() == "cancelar":
                return None
            
            try:
                value = float(value_str)
                if min_val <= value <= max_val:
                    return value
                else:
                    print(f"  Error: debe estar entre {min_val} y {max_val}")
            except ValueError:
                print("  Error: ingrese un número válido")

# Fase 4 — Implementación del Front-end Python (`fuzzy_ui`)

> Estado: el bridge importa el módulo nativo desde el paquete `fuzzy_ui`, crea la configuración predeterminada y permite calcular la intensidad. Los trapezoides de hombro de la configuración ya son admitidos por el motor. El plotter muestra cada visualización en una ventana independiente y secuencial. La sensibilidad comparativa sigue pendiente.

## 1. `workspace/python/fuzzy_ui/config.py`

```python
"""
config.py — Configuración del sistema difuso sísmico.
"""

from typing import Dict, Any, List, Tuple


SEISMIC_CONFIG: Dict[str, Any] = {
    "variables": {
        "Magnitud": {
            "universe": (3.0, 9.0),
            "terms": [
                {"name": "Baja", "type": "trapezoidal", "params": (3.0, 3.0, 4.0, 5.0)},
                {"name": "Moderada", "type": "triangular", "params": (4.0, 5.5, 7.0)},
                {"name": "Alta", "type": "triangular", "params": (6.0, 7.0, 8.0)},
                {"name": "Muy Alta", "type": "triangular", "params": (7.5, 8.25, 9.0)},
                {"name": "Extrema", "type": "trapezoidal", "params": (8.5, 9.0, 9.0, 9.0)}
            ]
        },
        "Profundidad": {
            "universe": (1.0, 300.0),
            "terms": [
                {"name": "Superficial", "type": "trapezoidal", "params": (1.0, 1.0, 30.0, 70.0)},
                {"name": "Intermedia", "type": "triangular", "params": (50.0, 150.0, 250.0)},
                {"name": "Profunda", "type": "trapezoidal", "params": (200.0, 270.0, 300.0, 300.0)}
            ]
        },
        "Distancia": {
            "universe": (1.0, 500.0),
            "terms": [
                {"name": "Muy Cercana", "type": "trapezoidal", "params": (1.0, 1.0, 30.0, 80.0)},
                {"name": "Cercana", "type": "triangular", "params": (50.0, 120.0, 200.0)},
                {"name": "Media", "type": "triangular", "params": (150.0, 275.0, 400.0)},
                {"name": "Lejana", "type": "trapezoidal", "params": (350.0, 450.0, 500.0, 500.0)}
            ]
        },
        "Intensidad": {
            "universe": (1.0, 12.0),
            "terms": [
                {"name": "I-II", "type": "trapezoidal", "params": (1.0, 1.0, 2.0, 3.0)},
                {"name": "III-IV", "type": "triangular", "params": (2.5, 3.5, 4.5)},
                {"name": "V-VI", "type": "triangular", "params": (4.0, 5.5, 7.0)},
                {"name": "VII-VIII", "type": "triangular", "params": (6.5, 7.5, 8.5)},
                {"name": "IX-X", "type": "triangular", "params": (8.0, 9.0, 10.0)},
                {"name": "XI-XII", "type": "trapezoidal", "params": (9.5, 11.0, 12.0, 12.0)}
            ]
        }
    },
    "rules": [
        {"id": 1, "antecedents": [("Magnitud", "Baja")], "consequent": ("Intensidad", "I-II")},
        {"id": 2, "antecedents": [("Magnitud", "Moderada"), ("Profundidad", "Superficial"), ("Distancia", "Muy Cercana")], "consequent": ("Intensidad", "V-VI")},
        {"id": 3, "antecedents": [("Magnitud", "Moderada"), ("Profundidad", "Superficial"), ("Distancia", "Cercana")], "consequent": ("Intensidad", "V-VI")},
        {"id": 4, "antecedents": [("Magnitud", "Moderada"), ("Profundidad", "Superficial"), ("Distancia", "Media")], "consequent": ("Intensidad", "III-IV")},
        {"id": 5, "antecedents": [("Magnitud", "Moderada"), ("Profundidad", "Intermedia")], "consequent": ("Intensidad", "III-IV")},
        {"id": 6, "antecedents": [("Magnitud", "Moderada"), ("Profundidad", "Profunda")], "consequent": ("Intensidad", "I-II")},
        {"id": 7, "antecedents": [("Magnitud", "Alta"), ("Profundidad", "Superficial"), ("Distancia", "Muy Cercana")], "consequent": ("Intensidad", "VII-VIII")},
        {"id": 8, "antecedents": [("Magnitud", "Alta"), ("Profundidad", "Superficial"), ("Distancia", "Cercana")], "consequent": ("Intensidad", "VII-VIII")},
        {"id": 9, "antecedents": [("Magnitud", "Alta"), ("Profundidad", "Superficial"), ("Distancia", "Media")], "consequent": ("Intensidad", "V-VI")},
        {"id": 10, "antecedents": [("Magnitud", "Alta"), ("Profundidad", "Intermedia"), ("Distancia", "Cercana")], "consequent": ("Intensidad", "V-VI")},
        {"id": 11, "antecedents": [("Magnitud", "Alta"), ("Profundidad", "Intermedia"), ("Distancia", "Media")], "consequent": ("Intensidad", "III-IV")},
        {"id": 12, "antecedents": [("Magnitud", "Alta"), ("Profundidad", "Profunda")], "consequent": ("Intensidad", "III-IV")},
        {"id": 13, "antecedents": [("Magnitud", "Muy Alta"), ("Profundidad", "Superficial"), ("Distancia", "Muy Cercana")], "consequent": ("Intensidad", "IX-X")},
        {"id": 14, "antecedents": [("Magnitud", "Muy Alta"), ("Profundidad", "Superficial"), ("Distancia", "Cercana")], "consequent": ("Intensidad", "IX-X")},
        {"id": 15, "antecedents": [("Magnitud", "Muy Alta"), ("Profundidad", "Intermedia"), ("Distancia", "Cercana")], "consequent": ("Intensidad", "VII-VIII")},
        {"id": 16, "antecedents": [("Magnitud", "Muy Alta"), ("Profundidad", "Profunda")], "consequent": ("Intensidad", "V-VI")},
        {"id": 17, "antecedents": [("Magnitud", "Extrema")], "consequent": ("Intensidad", "XI-XII")},
        {"id": 18, "antecedents": [("Magnitud", "Baja"), ("Profundidad", "Profunda"), ("Distancia", "Lejana")], "consequent": ("Intensidad", "I-II")}
    ]
}


def get_seismic_config() -> Dict[str, Any]:
    """Retorna la configuración del sistema sísmico."""
    return SEISMIC_CONFIG
```

---

## 2. `workspace/python/fuzzy_ui/bridge.py`

```python
"""
bridge.py — Wrapper de pybind11 que expone el motor C++ a Python.
"""

from typing import Dict, List, Any
import numpy as np
from .fuzzy_core import (
    FuzzyResult,
    FuzzyRule,
    FuzzySystem,
    LinguisticVariable,
    TrapezoidalMF,
    TriangularMF,
)
from fuzzy_ui.config import get_seismic_config


class FuzzySystemBridge:
    """Wrapper que facilita el uso del motor C++ desde Python."""
    
    def __init__(self):
        self._system: FuzzySystem = None
        self._config: Dict[str, Any] = None
    
    def build_default_seismic_system(self) -> None:
        """Construye el sistema difuso sísmico por defecto con 18 reglas."""
        self._config = get_seismic_config()
        
        # Crear variable de salida
        output_config = self._config["variables"]["Intensidad"]
        output = LinguisticVariable("Intensidad", output_config["universe"])
        for term in output_config["terms"]:
            mf = self._create_mf(term)
            output.add_term(mf)
        
        # Crear sistema
        self._system = FuzzySystem(output, n_points=1000)
        
        # Crear variables de entrada
        for var_name, var_config in self._config["variables"].items():
            if var_name == "Intensidad":
                continue
            
            var = LinguisticVariable(var_name, var_config["universe"])
            for term in var_config["terms"]:
                mf = self._create_mf(term)
                var.add_term(mf)
            self._system.add_input(var)
        
        # Crear reglas
        for rule_config in self._config["rules"]:
            rule = FuzzyRule(rule_config["id"])
            for var_name, term_name in rule_config["antecedents"]:
                rule.add_antecedent(var_name, term_name)
            cons_var, cons_term = rule_config["consequent"]
            rule.set_consequent(cons_var, cons_term)
            self._system.add_rule(rule)
    
    def _create_mf(self, term_config: Dict[str, Any]):
        """Crea una función de pertenencia según la configuración."""
        name = term_config["name"]
        mf_type = term_config["type"]
        params = term_config["params"]
        
        if mf_type == "triangular":
            return TriangularMF(name, *params)
        elif mf_type == "trapezoidal":
            return TrapezoidalMF(name, *params)
        else:
            raise ValueError(f"Tipo de MF no soportado: {mf_type}")
    
    def compute(self, inputs: Dict[str, float]) -> Dict[str, Any]:
        """Ejecuta el pipeline de inferencia."""
        if self._system is None:
            raise RuntimeError("Sistema no construido. Llame a build_default_seismic_system() primero.")
        
        result: FuzzyResult = self._system.compute(inputs)
        return {
            "crisp_output": result.crisp_output,
            "fuzzified_inputs": dict(result.fuzzified_inputs),
            "rule_degrees": list(result.rule_degrees),
            "aggregated_mu": np.array(result.aggregated_mu),
            "discretized_output": np.array(result.discretized_output)
        }
    
    def get_membership_functions(self, var_name: str) -> List[Dict[str, Any]]:
        """Obtiene las funciones de pertenencia de una variable."""
        if var_name == "Intensidad":
            var = self._system.get_output()
        else:
            # Buscar en inputs
            for input_name in self._system.get_input_names():
                if input_name == var_name:
                    # Necesitamos acceder a la variable; el binding no expone esto directamente
                    # Usamos la configuración en su lugar
                    break
            var_config = self._config["variables"].get(var_name)
            if not var_config:
                return []
            
            mfs = []
            for term in var_config["terms"]:
                mf_obj = self._create_mf(term)
                mfs.append({
                    "name": term["name"],
                    "type": term["type"],
                    "params": term["params"],
                    "support": mf_obj.get_support(),
                    "evaluate": mf_obj.evaluate,
                    "evaluate": mf_obj.evaluate
                })
            return mfs
        
        # Para la variable de salida
        var_config = self._config["variables"]["Intensidad"]
        mfs = []
        for term in var_config["terms"]:
            mf_obj = self._create_mf(term)
            mfs.append({
                "name": term["name"],
                "type": term["type"],
                "params": term["params"],
                "support": mf_obj.get_support(),
                "evaluate": mf_obj.evaluate,
                "evaluate": mf_obj.evaluate
            })
        return mfs
    
    def get_rules(self) -> List[Dict[str, Any]]:
        """Obtiene todas las reglas del sistema."""
        rules_cpp = self._system.get_rules()
        rules = []
        for rule in rules_cpp:
            rules.append({
                "id": rule.get_id(),
                "weight": rule.get_weight()
            })
        return rules
    
    def get_input_names(self) -> List[str]:
        """Retorna los nombres de las variables de entrada."""
        return self._system.get_input_names()
    
    def get_output_name(self) -> str:
        """Retorna el nombre de la variable de salida."""
        return "Intensidad"
```

---

## 3. `workspace/python/fuzzy_ui/exporter.py`

```python
"""
exporter.py — Exportación de resultados a CSV con pandas.
"""

from typing import List, Dict, Any
import pandas as pd


class Exporter:
    """Exporta resultados del sistema difuso a formatos estructurados."""
    
    def export_to_csv(self, history: List[Dict[str, Any]], path: str) -> None:
        """Exporta el historial de resultados a CSV."""
        rows = []
        for entry in history:
            inputs = entry["inputs"]
            result = entry["result"]
            
            row = {
                "Magnitud": inputs["Magnitud"],
                "Profundidad": inputs["Profundidad"],
                "Distancia": inputs["Distancia"],
                "Intensidad": result["crisp_output"]
            }
            rows.append(row)
        
        df = pd.DataFrame(rows)
        df.to_csv(path, index=False, encoding='utf-8')
    
    def export_detailed_csv(self, history: List[Dict[str, Any]], path: str) -> None:
        """Exporta resultados detallados (incluyendo grados de reglas) a CSV."""
        rows = []
        for entry in history:
            inputs = entry["inputs"]
            result = entry["result"]
            
            row = {
                "Magnitud": inputs["Magnitud"],
                "Profundidad": inputs["Profundidad"],
                "Distancia": inputs["Distancia"],
                "Intensidad": result["crisp_output"]
            }
            
            # Añadir grados de reglas
            for i, degree in enumerate(result["rule_degrees"]):
                row[f"R{i+1}_grado"] = degree
            
            rows.append(row)
        
        df = pd.DataFrame(rows)
        df.to_csv(path, index=False, encoding='utf-8')
```

---

## 4. `workspace/python/fuzzy_ui/plotter.py`

El módulo genera cuatro visualizaciones conservando el diseño de cada gráfica, pero ya no las coloca en una cuadrícula de subgráficas. `plot_all` crea una figura Matplotlib independiente para cada visualización, con tamaño de 10 × 10 pulgadas:

1. **Funciones de pertenencia** de las variables de entrada, incluyendo los valores ingresados.
2. **Reglas activadas**, con el grado de activación de cada regla activa.
3. **Conjunto difuso agregado y centroide** de la salida.
4. **Superficie de decisión 3D** de magnitud y profundidad frente a intensidad, calculada con una distancia fija de 50 km.

Las ventanas se muestran en ese orden y de forma bloqueante (`plt.show(block=True)`). Cada ventana tiene un título numerado (`1/4` a `4/4`); al cerrarla se libera su figura y se muestra la siguiente. Al cerrar la cuarta ventana, la ejecución vuelve al REPL. La barra de color de la superficie 3D forma parte de esa misma figura y no representa una visualización adicional.

Cada figura aplica `tight_layout(pad=3)` para separar el título de las etiquetas y subtítulos de los ejes. La superficie conserva una barra de color etiquetada como intensidad.

---

## 5. `workspace/python/fuzzy_ui/cli.py`

```python
"""
cli.py — REPL interactivo por consola.
"""

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
                    print("Comando no reconocido. Escriba 'salir' para terminar.")
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
```

---

## 6. `workspace/python/fuzzy_ui/main.py`

```python
"""
main.py — Punto de entrada del sistema.
"""

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
```

---

## 7. `workspace/python/fuzzy_ui/__init__.py`

```python
"""
fuzzy_ui — Front-end interactivo para el sistema difuso sísmico.
"""

__version__ = "1.0.0"
```

---

## 8. Configuración y ejecución (Windows)

Desde la raíz del proyecto, configura CMake indicando el toolchain de vcpkg (o una instalación de Eigen3 que exporte `Eigen3Config.cmake`):

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="C:\ruta\a\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

El proyecto utiliza C++17 y CMake 3.20 o posterior. Eigen3 se resuelve con `find_package(Eigen3 CONFIG REQUIRED)`; pybind11 y GoogleTest se obtienen mediante FetchContent. El target `fuzzy_core_lib` propaga `Eigen3::Eigen` al binding y a las pruebas.

El binding se copia automáticamente a `workspace/python/fuzzy_ui/`. Instala las dependencias Python del frontend en el mismo entorno que se usó para configurar pybind11:

```powershell
python -m pip install numpy matplotlib pandas
cd workspace\python
python -m fuzzy_ui.main
```

No hay actualmente `requirements.txt` ni `pyproject.toml`. El build C++ y los siete tests CTest (cuatro de integración y tres de MF trapezoidales) se verificaron correctamente en Windows. El comando de sensibilidad todavía no compara configuraciones; además, el plotter de superficie captura excepciones de forma amplia.

---

## 9. Ejemplo de Uso

```
> calcular

Ingrese los valores (o 'cancelar' para volver):
Magnitud (3.0-9.0): 6.5
Profundidad (1-300 km): 30
Distancia (1-500 km): 80

======================================================================
RESULTADO DE INFERENCIA
======================================================================
Intensidad Sísmica Estimada: 6.87 (Mercalli)
======================================================================

Generando gráficas...
[Se abre "1/4 - Funciones de pertenencia"]
[Al cerrar, se muestra "2/4 - Reglas activadas"]
[Al cerrar, se muestra "3/4 - Conjunto agregado"]
[Al cerrar, se muestra "4/4 - Superficie de decisión"]
[Al cerrar la cuarta ventana, vuelve al REPL]

> reglas

BASE DE REGLAS DIFUSAS
----------------------------------------------------------------------
R 1: SI Magnitud es Baja
       ENTONCES Intensidad es I-II

R 2: SI Magnitud es Moderada Y Profundidad es Superficial Y Distancia es Muy Cercana
       ENTONCES Intensidad es V-VI

...

> exportar
Ruta del archivo CSV (default: resultados.csv): 
✓ Resultados exportados a resultados.csv

> salir
Hasta luego!
```

---

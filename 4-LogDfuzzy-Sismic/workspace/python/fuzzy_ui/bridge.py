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
            "fuzzified_inputs": {k: dict(zip(range(len(v)), v)) for k, v in result.fuzzified_inputs.items()},
            "rule_degrees": list(result.rule_degrees),
            "aggregated_mu": np.array(result.aggregated_mu),
            "discretized_output": np.array(result.discretized_output)
        }
    
    def get_membership_functions(self, var_name: str) -> List[Dict[str, Any]]:
        """Obtiene las funciones de pertenencia de una variable."""
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
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
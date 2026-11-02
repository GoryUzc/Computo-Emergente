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
from typing import Dict, Any
import numpy as np
import matplotlib.pyplot as plt
from fuzzy_ui.bridge import FuzzySystemBridge


class Plotter:
    
    def plot_all(self, inputs: Dict[str, float], result: Dict[str, Any], bridge: FuzzySystemBridge) -> None:
        """Muestra cada gráfica en una figura independiente y secuencial."""
        fig = plt.figure(figsize=(10, 10))
        self._plot_membership_functions(fig, inputs, bridge)
        fig.tight_layout(pad=3)
        self._show_figure(fig, "1/4 - Funciones de pertenencia")

        fig = plt.figure(figsize=(10, 10))
        self._plot_activated_rules(fig, result, bridge)
        fig.tight_layout(pad=3)
        self._show_figure(fig, "2/4 - Reglas activadas")

        fig = plt.figure(figsize=(10, 10))
        self._plot_aggregated_set(fig, result, bridge)
        fig.tight_layout(pad=3)
        self._show_figure(fig, "3/4 - Conjunto agregado")

        fig = plt.figure(figsize=(10, 10))
        self._plot_decision_surface(fig, bridge)
        fig.tight_layout(pad=3)
        self._show_figure(fig, "4/4 - Superficie de decisión")

    @staticmethod
    def _show_figure(fig: plt.Figure, window_title: str) -> None:
        """Espera el cierre de una ventana y libera su figura antes de continuar."""
        fig.canvas.manager.set_window_title(window_title)
        try:
            plt.show(block=True)
        finally:
            plt.close(fig)
    
    def _plot_membership_functions(self, fig, inputs: Dict[str, float], bridge: FuzzySystemBridge) -> None:
        """Grafica las funciones de pertenencia de las entradas y marca los valores de entrada."""
        ax = fig.add_subplot(1, 1, 1)
        
        input_names = bridge.get_input_names()
        colors = ['blue', 'green', 'red']
        
        for idx, var_name in enumerate(input_names):
            mfs = bridge.get_membership_functions(var_name)
            if not mfs:
                continue
            
            # Obtener rango del universo
            x_min = min(mf["support"][0] for mf in mfs)
            x_max = max(mf["support"][1] for mf in mfs)
            x = np.linspace(x_min, x_max, 500)
            
            for mf in mfs:
                y = [mf["evaluate"](xi) for xi in x]
                ax.plot(x, y, label=mf["name"], color=colors[idx % len(colors)], alpha=1
                        )
            
            # Marcar el valor de entrada
            ax.axvline(inputs[var_name], color=colors[idx], linestyle='--', linewidth=2, 
                    label=f'{var_name}={inputs[var_name]:.2f}')
        
        ax.set_xlabel("Valor")
        ax.set_ylabel("Grado de Pertenencia μ(x)")
        ax.set_title("Funciones de Pertenencia de Entradas")
        ax.legend(fontsize=7, loc='upper left', bbox_to_anchor=(1.02, 1), framealpha=0.9)
        ax.tick_params(axis='both', pad=4)
        ax.grid(True, alpha=1)
        ax.set_ylim(-0.05, 1.05)
    
    def _plot_activated_rules(self, fig, result: Dict[str, Any], bridge: FuzzySystemBridge) -> None:
        """Grafica las reglas activadas con sus grados."""
        ax = fig.add_subplot(1, 1, 1)
        
        rules = bridge.get_rules()
        degrees = result["rule_degrees"]
        
        # Filtrar reglas con grado > 0.01
        active_rules = [(r, d) for r, d in zip(rules, degrees) if d > 0.01]
        
        if not active_rules:
            ax.text(0.5, 0.5, "No hay reglas activadas", ha='center', va='center', 
                transform=ax.transAxes, fontsize=9)
            ax.set_title("Reglas Activadas")
            return
        
        rule_ids = [f"R{r['id']}" for r, d in active_rules]
        rule_degrees = [d for r, d in active_rules]
        
        bars = ax.barh(rule_ids, rule_degrees, color='steelblue', alpha=0.7)
        ax.set_xlabel("Grado de Activación α")
        ax.set_title("Reglas Activadas")
        ax.set_xlim(0, 1)
        ax.grid(True, alpha=1, axis='x')
        
        # Añadir valores en las barras
        for bar, degree in zip(bars, rule_degrees):
            ax.text(bar.get_width() + 0.02, bar.get_y() + bar.get_height()/2, 
                   f'{degree:.3f}', va='center', fontsize=9)
    
    def _plot_aggregated_set(self, fig, result: Dict[str, Any], bridge: FuzzySystemBridge) -> None:
        """Grafica el conjunto difuso agregado y el centroide."""
        ax = fig.add_subplot(1, 1, 1)
        
        universe = result["discretized_output"]
        mu = result["aggregated_mu"]
        y_star = result["crisp_output"]
        
        ax.plot(universe, mu, 'b-', linewidth=2, label='Conjunto Agregado μ(y)')
        ax.axvline(y_star, color='red', linestyle='--', linewidth=2, label=f'Centroide y*={y_star:.2f}')
        ax.fill_between(universe, mu, alpha=0.5, color='blue')
        
        ax.set_xlabel("Intensidad Sísmica (Mercalli)")
        ax.set_ylabel("Grado de Pertenencia μ(y)")
        ax.set_title("Conjunto Difuso Agregado y Desfusificación")
        ax.legend()
        ax.grid(True, alpha=1)
        ax.set_ylim(-0.05, 1.05)
    
    def _plot_decision_surface(self, fig, bridge: FuzzySystemBridge) -> None:
        """Grafica la superficie de decisión 3D (Magnitud vs Profundidad → Intensidad)."""
        ax = fig.add_subplot(1, 1, 1, projection='3d')
        
        # Generar grid de entradas
        magnitudes = np.linspace(3.0, 9.0, 25)
        profundidades = np.linspace(1, 300, 25)
        M, H = np.meshgrid(magnitudes, profundidades)
        I = np.zeros_like(M)
        
        # Calcular intensidad para cada punto (distancia fija = 50 km)
        distancia_fija = 50.0
        for i in range(M.shape[0]):
            for j in range(M.shape[1]):
                try:
                    result = bridge.compute({
                        "Magnitud": M[i, j],
                        "Profundidad": H[i, j],
                        "Distancia": distancia_fija
                    })
                    I[i, j] = result["crisp_output"]
                except:
                    I[i, j] = np.nan
        
        surf = ax.plot_surface(M, H, I, cmap='viridis', edgecolor='none', alpha=0.8)
        ax.set_xlabel('Magnitud (M)', labelpad=10)
        ax.set_ylabel('Profundidad (km)', labelpad=10)
        ax.set_zlabel('Intensidad (I)', labelpad=10)
        ax.set_title(f'Superficie de Decisión\n(Distancia={distancia_fija} km)')
        fig.colorbar(surf, ax=ax, shrink=0.5, aspect=5, label='Intensidad')
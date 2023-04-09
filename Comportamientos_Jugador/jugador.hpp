#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;

struct state{
  int fil;
  int col;
  Orientacion brujula;
};

struct mapas_auxiliares{
  vector<vector<int>> frecuencias;
  vector<vector<unsigned char>> terreno;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de estado
      current_state.fil = current_state.col = 99;
      current_state.brujula = norte;
      last_action = actIDLE;
      bikini = zapatillas = cargando = bien_situado = frecuencia_analizada = en_camino = false;
      girar_derecha = rodeando_muro = 0;
      inicializarMapaAux();
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}
    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void inicializarMapaAux();
    void reiniciarMapaTerreno();
    bool esAgua(int n);
    bool esBosque(int n);
    bool esMuro(int n);
    bool esPrecipicio(int n);
    bool esBateria(int n);
    bool esZapatillas(int n);
    bool esBikini(int n);
    bool esPosicion(int n);
    bool esViablePasar(int n);
    bool esViableSeguirCargando();
    bool esTransitable(int n);
    bool enModoAhorroEnergia();
    void gestionarCasillasEspeciales();
    int cuantasVecesHaPasado(int fil, int col);
    Action analizarVision();
    Action analizarFrecuencias();
    

  private:
  
  // Declarar aquí las variables de estado
  state current_state;
  Action last_action;
  int girar_derecha, rodeando_muro;
  mapas_auxiliares map_aux;
  bool bikini, zapatillas, cargando, bien_situado, frecuencia_analizada, en_camino;
  Sensores sensor;
};

#endif

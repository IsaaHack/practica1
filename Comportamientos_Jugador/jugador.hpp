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
      bikini = zapatillas = false;
      girar_derecha = bien_situado = false;
      inicializarMapaAux();
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}
    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void inicializarMapaAux();
    void reiniciarMapaTerreno();

  private:
  
  // Declarar aquí las variables de estado
  state current_state;
  Action last_action;
  bool girar_derecha, bien_situado;
  mapas_auxiliares map_aux;
  bool bikini, zapatillas;
};

#endif

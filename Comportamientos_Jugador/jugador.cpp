#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

void ponerTerrenoEnMatriz(const vector<unsigned char> &terreno, const state &st, 
							vector<vector<unsigned char>> &matriz);

Action ComportamientoJugador::think(Sensores sensores){
	Action accion = actIDLE;
	int a;

	switch (last_action){
		case actFORWARD:
			// Actualización en caso de avanzar
			switch (current_state.brujula){
				case norte: current_state.fil--; break;
				case noreste: current_state.fil--; current_state.col++; break;
				case este: current_state.col++; break;
				case sureste: current_state.fil++; current_state.col++;break;
				case sur: current_state.fil++; break;
				case suroeste: current_state.fil++; current_state.col--;break;
				case oeste: current_state.col--; break;
				case noroeste: current_state.fil--; current_state.col--;break;
			}
			break;
		case actTURN_SL:
			// Actualización en caso de girar 45º a la izquierda
			a = current_state.brujula;
			a = (a+7)%8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_SR:
			// Actualización en caso de girar 45º a la derecha
			a = current_state.brujula;
			a = (a+1)%8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_BL:
			// Actualización en caso de girar 135º a la izquierda
			a = current_state.brujula;
			a = (a+5)%8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
		case actTURN_BR:
			// Actualización en caso de girar 135º a la derecha
			a = current_state.brujula;
			a = (a+3)%8;
			current_state.brujula = static_cast<Orientacion>(a);
			break;
	}

	if(sensores.reset){
		bikini = zapatillas = bien_situado = false;
		current_state.fil = current_state.col = 99;
    	current_state.brujula = norte;
		reiniciarMapaTerreno();
	}

	if(!bien_situado)
		ponerTerrenoEnMatriz(sensores.terreno, current_state, map_aux.terreno);

	if (sensores.posF != -1 and !bien_situado){
		state last_state = current_state;
		current_state.fil = sensores.posF;
		current_state.col= sensores.posC;
		current_state.brujula = sensores.sentido;
		transladarMatriz(map_aux.terreno, mapaResultado, last_state, current_state);
		bien_situado = true;
	}else{
		ponerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado);
		map_aux.frecuencias[current_state.fil][current_state.col]++;
	}

	//Decidir la nueva acción
	if ((sensores.terreno[2]=='T' or sensores.terreno[2]=='S' or sensores.terreno[2]=='G') and
		sensores.superficie[2]=='_'){
		accion = actFORWARD;
	} else if (!girar_derecha){
		accion = actTURN_SL;
		girar_derecha = (rand()%2 ==0);
	} else{
		accion = actTURN_SR;
		girar_derecha = (rand()%2 ==0);
	}

	// Determinar el efecto de la ultima accion enviada
	//Recordar la ultima accion
	last_action = accion;
	return accion;
}

void ponerTerrenoEnMatriz(const vector<unsigned char> &terreno, const state &st, 
							vector<vector<unsigned char>> &matriz){
	matriz [st.fil][st.col] = terreno[0];
	//horizontal: =1 derecha =0 no =-1 izquierda
	int horizontal;
	//vertical: =1 abajo =0 no =-1 arriba
	int vertical;

	switch (st.brujula)
	{
	case norte:
		horizontal = 0;
		vertical = -1;
		break;

	case sur:
		horizontal = 0;
		vertical = 1;
		break;

	case este:
		horizontal = 1;
		vertical = 0;
		break;

	case oeste:
		horizontal = -1;
		vertical = 0;
		break;

	case noreste:
		horizontal = 1;
		vertical = -1;
		break;

	case sureste:
		horizontal = 1;
		vertical = 1;
		break;

	case suroeste:
		horizontal = -1;
		vertical = 1;
		break;

	case noroeste:
		horizontal = -1;
		vertical = -1;
		break;
	}

	int fil = st.fil, col = st.col;


	for (int i = 1; i < 4; ++i){
		if(horizontal == 0 || vertical == 0){
			fil += vertical - horizontal;
			col += vertical + horizontal;
		}else{
			switch (st.brujula)
			{
			case noreste:
				fil = st.fil + vertical*i;
				col = st.col;
				break;

			case sureste:
				fil = st.fil;
				col = st.col + horizontal*i;
				break;

			case suroeste:
				fil = st.fil + vertical*i;
				col = st.col;
				break;

			case noroeste:
				fil = st.fil;
				col = st.col + horizontal*i;
				break;
			}
		}

		for(int j = 0; j < 1+2*i; ++j){
			matriz[fil][col] = terreno[i*i + j];

			if(horizontal != 0 && vertical != 0){
				if(i*i + j < i*(i + 1)){
					switch (st.brujula)
					{
					case noreste:
						col++;
						break;

					case sureste:
						fil++;
						break;

					case suroeste:
						col--;
						break;

					case noroeste:
						fil--;
						break;
					}
				}else{
					switch (st.brujula)
					{
					case noreste:
						fil++;
						break;

					case sureste:
						col--;
						break;

					case suroeste:
						fil--;
						break;

					case noroeste:
						col++;
						break;
					}
				}

				
			}else if(horizontal == 0) col -= vertical;
			else if(vertical == 0) fil += horizontal;
			
		}

		if(horizontal == 0 || vertical == 0){
			col = col + vertical*(2*i + 1);
			fil = fil - horizontal*(2*i + 1);
		}
	}
}

void ComportamientoJugador::inicializarMapaAux(){
	for(int i = 0; i < 200; ++i)
        for(int j = 0; j < 200; ++j){
			map_aux.frecuencias[i][j] = 0;
			map_aux.terreno[i][j] = '?';
		}
}

void ComportamientoJugador::reiniciarMapaTerreno(){
	for(int i = 0; i < 200; ++i)
        for(int j = 0; j < 200; ++j){
			map_aux.terreno[i][j] = '?';
		}
}

void transladarMatriz(vector<vector <unsigned char>> fuente , vector<vector <unsigned char>> destino, state estado_anterior, state estado){
	int dif_filas = estado_anterior.fil - estado.fil;
	int dif_columnas = estado_anterior.col - estado.col;

	for(int i = 0; i < destino.size(); ++i)
		for(int j = 0; j < destino[i].size(); ++j)
			if(destino[i][j] == '?') destino[i][j] = fuente[dif_filas + i][dif_columnas + j];
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}

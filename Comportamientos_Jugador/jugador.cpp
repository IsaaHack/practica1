#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

void ponerTerrenoEnMatriz(const vector<unsigned char> &terreno, const state &st, 
							vector<vector<unsigned char>> &matriz);

void transladarMatriz(vector<vector <unsigned char>> &fuente , vector<vector <unsigned char>> &destino, state &estado_anterior, state &estado);

Action ComportamientoJugador::think(Sensores sensores){
	Action accion = actIDLE;
	int a;
	sensor = sensores;

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

	if(sensores.nivel == 0){
		current_state.fil = sensores.posF;
		current_state.col= sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	} 

	if(sensores.reset){
		bikini = zapatillas = bien_situado = false;
		current_state.fil = current_state.col = 99;
    	current_state.brujula = norte;
		frecuencia_analizada = en_camino = false;
		veces_forward = girar_derecha = 0;
		cargando = false;
		reiniciarMapaTerreno();
	}

	gestionarCasillasEspeciales();

	if(last_action == actFORWARD) veces_forward++;
	else veces_forward = 0;

	if(!bien_situado)
		ponerTerrenoEnMatriz(sensores.terreno, current_state, map_aux.terreno);

	if (sensores.posF != -1 and !bien_situado){
		state last_state = current_state;
		current_state.fil = sensores.posF;
		current_state.col= sensores.posC;
		current_state.brujula = sensores.sentido;
		transladarMatriz(map_aux.terreno, mapaResultado, last_state, current_state);
		bien_situado = true;
	}else if (bien_situado){
		ponerTerrenoEnMatriz(sensores.terreno, current_state, mapaResultado);
		if(!esBateria(0) || !esBikini(0) || !esZapatillas(0) || !esPosicion(0) || last_action == actFORWARD)
			map_aux.frecuencias[current_state.fil][current_state.col]++;

		//if(rodeando_muro) map_aux.frecuencias[current_state.fil][current_state.col]--;

		//if(esAgua(0) || esBosque(0)) map_aux.frecuencias[current_state.fil][current_state.col]++;
	}
	//(sensores.terreno[2]=='T' or sensores.terreno[2]=='S' or sensores.terreno[2]=='G')
	//Decidir la nueva acción
	if(girar_derecha){
		accion = actTURN_BL;
		girar_derecha++;
		if(girar_derecha > 2) girar_derecha = 0;
	}else if (esViablePasar(2) && sensores.superficie[2]!='a' && (veces_forward < 6 || en_camino)){
		cargando = false;
		if (rodeando_muro && !esMuro(rodeando_muro)){
			accion = actFORWARD;
			if(rodeando_muro == 1 && esMuro(5)) accion = actTURN_SL;
			else if(rodeando_muro == 3 && esMuro(7)) accion = actTURN_SR;
			rodeando_muro = 0;
		}else accion = analizarVision();
			if(accion == actFORWARD && bien_situado && !frecuencia_analizada && !en_camino){
				accion = analizarFrecuencias();
			}else if(frecuencia_analizada){
				frecuencia_analizada = false;
			}
		
	} else if (cargando){
		accion = actIDLE;
		if(!esViableSeguirCargando()){
			accion = analizarVision();
			if(accion == actFORWARD && bien_situado && !frecuencia_analizada && !en_camino) {
				accion = analizarFrecuencias();
			}else if(frecuencia_analizada){
				frecuencia_analizada = false;
			}
			cargando = false;
		} 
	}else{
		accion = analizarVision();
		if(accion == actFORWARD && bien_situado && !frecuencia_analizada && !en_camino) {
			accion = analizarFrecuencias();
		}else if(frecuencia_analizada){
			frecuencia_analizada = false;
		}

		if((accion == actFORWARD)){
			int random = (rand()%2);
			switch (random)
				{
				case 0:
					accion = actTURN_SL;
					break;
				
				case 1:
					accion = actTURN_SR;
					break;
				}
		}
			
			//if(last_action == actTURN_SL) accion = actTURN_BR;
			//else if(last_action == actTURN_SR) accion = actTURN_BL;
		
	}
	
	/*if (!girar_derecha){
		accion = actTURN_SL;
		girar_derecha = (rand()%2 ==0);
	} else{
		accion = actTURN_SR;
		girar_derecha = (rand()%2 ==0);
	}
	*/

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
	for(int i = 0; i < 200; ++i){
		map_aux.frecuencias.push_back(vector<int>());
		map_aux.terreno.push_back(vector<unsigned char>());
        for(int j = 0; j < 200; ++j){
			map_aux.frecuencias[i].push_back(0);
			map_aux.terreno[i].push_back('?');
		}
	}
}

void ComportamientoJugador::reiniciarMapaTerreno(){
	for(int i = 0; i < 200; ++i)
        for(int j = 0; j < 200; ++j){
			map_aux.terreno[i][j] = '?';
		}
}

void transladarMatriz(vector<vector <unsigned char>> &fuente , vector<vector <unsigned char>> &destino, state &estado_anterior, state &estado){
	int dif_filas = estado_anterior.fil - estado.fil;
	int dif_columnas = estado_anterior.col - estado.col;

	for(int i = 0; i < destino.size(); ++i)
		for(int j = 0; j < destino[i].size(); ++j){
			if(destino[i][j] == '?') destino[i][j] = fuente[dif_filas + i][dif_columnas + j];
		}
			
}

bool ComportamientoJugador::esAgua(int n){
	if(sensor.terreno[n] == 'A') return true;
	return false;
}

bool ComportamientoJugador::esBosque(int n){
	if(sensor.terreno[n] == 'B') return true;
	return false;
}

bool ComportamientoJugador::esMuro(int n){
	if(sensor.terreno[n] == 'M') return true;
	return false;
}

bool ComportamientoJugador::esPrecipicio(int n){
	if(sensor.terreno[n] == 'P') return true;
	return false;
}

bool ComportamientoJugador::esZapatillas(int n){
	if(sensor.terreno[n] == 'D') return true;
	return false;
}

bool ComportamientoJugador::esBikini(int n){
	if(sensor.terreno[n] == 'K') return true;
	return false;
}

bool ComportamientoJugador::esPosicion(int n){
	if(sensor.terreno[n] == 'G') return true;
	return false;
}

bool ComportamientoJugador::esBateria(int n){
	if(sensor.terreno[n] == 'X') return true;
	return false;
}

bool ComportamientoJugador::esViablePasar(int n){
	if(!esTransitable(n)) return false;
	if(esBateria(0) && esViableSeguirCargando()) return false;
	if(esAgua(0) && !bikini) return true;
	if(esBosque(0) && !zapatillas) return true;
	if((esBateria(0) || esPosicion(0) || esZapatillas(0)) && esAgua(1) && esAgua(2) && esAgua(3)) return true;
	if((esBateria(0) || esPosicion(0) || esBikini(0)) && esBosque(1) && esBosque(2) && esBosque(3)) return true;
	if(esAgua(n) && !bikini) return false;
	if(esBosque(n) && !zapatillas) return false;
	//if((esAgua(n) && !bikini) && enModoAhorroEnergia()) return false;
	//if((esBosque(n) && !zapatillas) && enModoAhorroEnergia()) return false;
	
	return true;
	

}

void ComportamientoJugador::gestionarCasillasEspeciales(){
	if(!esTransitable(1) && !esTransitable(2) && !esTransitable(3)){
		girar_derecha = 1;
	}
	if(esMuro(1) && !girar_derecha){
		rodeando_muro = 1;
	}
	if(esMuro(3) && !girar_derecha){
		rodeando_muro = 3;
	}
	if(esPosicion(0)){
		en_camino = false;
		return;
	}
	if(esZapatillas(0)){
		zapatillas = true;
		en_camino = false;
		return;
	}
	if(esBikini(0)){
		bikini = true;
		en_camino = false;
		return;
	}
	if(esBateria(0)){
		en_camino = false;
		cargando = true;
		return;
	}

}

bool ComportamientoJugador::esViableSeguirCargando(){
	if(sensor.vida < ((5000 - sensor.bateria)/10) || sensor.bateria >= 4800) return false;
	return true;
}

bool ComportamientoJugador::enModoAhorroEnergia(){
	if(sensor.bateria < 3000) return true;
	return false;
}

bool ComportamientoJugador::esTransitable(int n){
	if(esPrecipicio(n) || esMuro(n)) return false;
	return true;
}

Action ComportamientoJugador::analizarVision(){
	bool esViable1 = esViablePasar(1), esViable4 = esViablePasar(4),
	 	 esViable3 = esViablePasar(3), esViable8 = esViablePasar(8);
	en_camino = true;
	if(!bien_situado){
		if(esPosicion(2) || esPosicion(6) || esPosicion(12)) return actFORWARD;
		if(esPosicion(1)) return actTURN_SL;
		if(esPosicion(3)) return actTURN_SR;
		if(esPosicion(4) && esViable1) return actTURN_SL;
		if(esPosicion(8) && esViable3) return actTURN_SR;
		if(esPosicion(9) && esViable1 && esViable4) return actTURN_SL;
		if(esPosicion(15) && esViable3 && esViable8) return actTURN_SR;
	}
	if(!bikini){
		if(esBikini(2) || esBikini(6) || esBikini(12)) return actFORWARD;
		if(esBikini(1)) return actTURN_SL;
		if(esBikini(3)) return actTURN_SR;
		if(esBikini(4) && esViable1) return actTURN_SL;
		if(esBikini(8) && esViable3) return actTURN_SR;
		if(esBikini(9) && esViable1 && esViable4) return actTURN_SL;
		if(esBikini(15) && esViable3 && esViable8) return actTURN_SR;
	}
	if(!zapatillas){
		if(esZapatillas(2) || esZapatillas(6) || esZapatillas(12)) return actFORWARD;
		if(esZapatillas(1)) return actTURN_SL;
		if(esZapatillas(3)) return actTURN_SR;
		if(esZapatillas(4) && esViable1) return actTURN_SL;
		if(esZapatillas(8) && esViable3) return actTURN_SR;
		if(esZapatillas(9) && esViable1 && esViable4) return actTURN_SL;
		if(esZapatillas(15) && esViable3 && esViable8) return actTURN_SR;
	}
	if(enModoAhorroEnergia()){
		if(esBateria(2) || esBateria(6) || esBateria(12)) return actFORWARD;
		if(esBateria(1)) return actTURN_SL;
		if(esBateria(3)) return actTURN_SR;
		if(esBateria(4) && esViable1) return actTURN_SL;
		if(esBateria(8) && esViable3) return actTURN_SR;
		if(esBateria(9) && esViable1 && esViable4) return actTURN_SL;
		if(esBateria(15) && esViable3 && esViable8) return actTURN_SR;
	}
	if(esBosque(0) && !zapatillas){
		Action accion;
		if(!esBosque(2) || !esBosque(6) || !esBosque(12)) accion = actFORWARD;
		else if(!esBosque(1)) accion = actTURN_SL;
		else if(!esBosque(3)) accion = actTURN_SR;
		else if(!esBosque(4) && esViable1) accion = actTURN_SL;
		else if(!esBosque(8) && esViable3) accion = actTURN_SR;
		else if(!esBosque(9) && esViable1 && esViable4) accion = actTURN_SL;
		else if(!esBosque(15) && esViable3 && esViable8) accion = actTURN_SR;

		en_camino = false;
		return accion;
	}

	en_camino = false;
	return actFORWARD;
}

Action ComportamientoJugador::analizarFrecuencias(){
	Action accion;
	int frec0, frec1, frec2, frec3;
	frec0 = cuantasVecesHaPasado(current_state.fil, current_state.col);

	switch (current_state.brujula)
	{
	case norte:
		frec1 = cuantasVecesHaPasado(current_state.fil-1, current_state.col-1);
		frec2 = cuantasVecesHaPasado(current_state.fil-1, current_state.col);
		frec3 = cuantasVecesHaPasado(current_state.fil-1, current_state.col+1);
		break;

	case sur:
		frec1 = cuantasVecesHaPasado(current_state.fil+1, current_state.col+1);
		frec2 = cuantasVecesHaPasado(current_state.fil+1, current_state.col);
		frec3 = cuantasVecesHaPasado(current_state.fil+1, current_state.col-1);
		break;

	case este:
		frec1 = cuantasVecesHaPasado(current_state.fil-1, current_state.col+1);
		frec2 = cuantasVecesHaPasado(current_state.fil, current_state.col+1);
		frec3 = cuantasVecesHaPasado(current_state.fil+1, current_state.col+1);
		break;

	case oeste:
		frec1 = cuantasVecesHaPasado(current_state.fil+1, current_state.col-1);
		frec2 = cuantasVecesHaPasado(current_state.fil, current_state.col-1);
		frec3 = cuantasVecesHaPasado(current_state.fil-1, current_state.col-1);
		break;

	case noreste:
		frec1 = cuantasVecesHaPasado(current_state.fil-1, current_state.col);
		frec2 = cuantasVecesHaPasado(current_state.fil-1, current_state.col+1);
		frec3 = cuantasVecesHaPasado(current_state.fil, current_state.col+1);
		break;

	case sureste:
		frec1 = cuantasVecesHaPasado(current_state.fil, current_state.col+1);
		frec2 = cuantasVecesHaPasado(current_state.fil+1, current_state.col+1);
		frec3 = cuantasVecesHaPasado(current_state.fil+1, current_state.col);
		break;

	case suroeste:
		frec1 = cuantasVecesHaPasado(current_state.fil+1, current_state.col);
		frec2 = cuantasVecesHaPasado(current_state.fil+1, current_state.col-1);
		frec3 = cuantasVecesHaPasado(current_state.fil, current_state.col-1);
		break;

	case noroeste:
		frec1 = cuantasVecesHaPasado(current_state.fil, current_state.col-1);
		frec2 = cuantasVecesHaPasado(current_state.fil-1, current_state.col-1);
		frec3 = cuantasVecesHaPasado(current_state.fil-1, current_state.col);
		break;
	}

	frecuencia_analizada = true;

	if(frec0 - 1 > frec1 && frec0 - 1 > frec2 && frec0 - 1 > frec3) return actFORWARD;
	if(frec1 == frec2 && frec2 == frec3) return actFORWARD;
	if(frec1 <= frec2 && frec1 <= frec3 && !esAgua(0) && esViablePasar(1)) return actTURN_SL;
	if(frec1 <= frec2 && frec1 <= frec3 && esAgua(0) && esViablePasar(1)) return actTURN_BL;
	if(frec3 <= frec2 && frec3 <= frec1 && !esAgua(0) && esViablePasar(3)) return actTURN_SR;
	if(frec3 <= frec2 && frec3 <= frec1 && esAgua(0) && esViablePasar(3)) return actTURN_BR;
	
	return actFORWARD;
}

int ComportamientoJugador::cuantasVecesHaPasado(int fil, int col){
	return map_aux.frecuencias[fil][col];
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}

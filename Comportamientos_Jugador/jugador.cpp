#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;



Action ComportamientoJugador::think(Sensores sensores){

	Action accion = actIDLE;
	int a;

	cout << "Posicion: fila " << sensores.posF << " columna " << sensores.posC << " ";
	switch(sensores.sentido){
		case 0: cout << "Norte" << endl; break;
		case 1: cout << "Noreste" << endl; break;
		case 2: cout << "Este" << endl; break;
		case 3: cout << "Sureste" << endl; break;
		case 4: cout << "Sur " << endl; break;
		case 5: cout << "Suroeste" << endl; break;
		case 6: cout << "Oeste" << endl; break;
		case 7: cout << "Noroeste" << endl; break;
	}
	cout << "Terreno: ";
	for (int i=0; i<sensores.terreno.size(); i++)
		cout << sensores.terreno[i];
	cout << endl;

	cout << "Superficie: ";
	for (int i=0; i<sensores.superficie.size(); i++)
		cout << sensores.superficie[i];
	cout << endl;

	cout << "Colisión: " << sensores.colision << endl;
	cout << "Reset: " << sensores.reset << endl;
	cout << "Vida: " << sensores.vida << endl;
	cout << endl;

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

	if (sensores.posF != -1 and !bien_situado){
		current_state.fil = sensores.posF;
		current_state.col= sensores.posC;
		current_state.brujula = sensores.sentido;
		bien_situado = true;
	}

	if (bien_situado){
		
		//mapaResultado[current_state.fil][current_state.col] = sensores.terreno[0];
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
	
	default:
		break;
	}

	int fil = st.fil, col = st.col;


	for (int i = 1; i < 4; ++i){
		fil += vertical - horizontal;
		col += vertical + horizontal;

		for(int j = 0; j < 1+2*i; ++j){
			if(horizontal == 0) col -= vertical;
			else if(vertical == 0) fil += horizontal;

			matriz[fil][col] = terreno[i*i + j];
		}

		col = col + vertical*(2*i);
		fil = fil - horizontal*(2*i);
	}

	matriz [st.fil-1][st.col-1] = terreno[1];
	matriz [st.fil-1][st.col-1] = terreno[2];
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}

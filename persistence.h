#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include <string>
#include <vector>

using namespace std;

// Función para mostrar todos los usuarios registrados
void displayUsers();

// Función para guardar un nuevo usuario, evitando duplicados (no distingue mayúsculas/minúsculas y elimina espacios)
void saveUser(const string& name);

// Función para actualizar la puntuación de un jugador en el ranking (agrega una victoria a ranking.txt)
void updateRanking(const string& name);

// Función para mostrar los 5 mejores jugadores del ranking
void displayRanking();

// Función para borrar todas las entradas del archivo de ranking
void resetRanking();

#endif // PERSISTENCE_H

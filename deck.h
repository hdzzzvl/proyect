#ifndef DECK_H
#define DECK_H

#include <vector>
#include <string>
#include <algorithm> // Para std::shuffle
#include <random>    // Para std::mt19937 y std::random_device

using namespace std;

// Estructura Card para almacenar palo, valor y puntos
struct Card {
    string suit;
    string rank;
    int points;
};

// Clase Deck para manejar una baraja de cartas
class Deck {
private:
    vector<Card> cards; // Almacena todas las cartas en la baraja

public:
    // Constructor: Inicializa una baraja estándar de 52 cartas
    Deck();

    // Mezcla las cartas en la baraja
    void shuffle();

    // Reparte una sola carta de la parte superior de la baraja
    Card deal();
};

#endif // DECK_H

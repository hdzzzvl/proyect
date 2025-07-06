#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <string>
#include "deck.h" // Incluir deck.h para la estructura Card

using namespace std;

// Clase Player para gestionar la mano del jugador y la lógica del juego
class Player {
public:
    string name;
    vector<Card> hand; // Almacena las cartas en la mano del jugador

    // Constructor: Inicializa un jugador con un nombre dado
    Player(string n);

    // Recibe una carta y la añade a la mano del jugador
    void receiveCard(Card c);

    // Calcula el total de puntos de la mano del jugador
    // Marcado como const porque no modifica el estado del objeto Player
    int calculatePoints() const;

    // Muestra las cartas en la mano del jugador
    void displayHand() const; // Marcado como const ya que solo muestra
};

#endif // PLAYER_H

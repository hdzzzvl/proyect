#include "player.h"
#include <iostream> 
#include <vector>   

using namespace std;

// Constructor: Inicializa el nombre del jugador
Player::Player(string n) : name(n) {}

// Recibe una carta y la agrega a la mano del jugador
void Player::receiveCard(Card c) {
    hand.push_back(c);
}

// Calcula el total de puntos de la mano del jugador
// Maneja el valor del As (A) (1 o 11) dinámicamente
int Player::calculatePoints() const {
    int sum = 0;
    int aces = 0; // Contador de Ases para gestionar su valor (1 o 11)

    // Suma los puntos de todas las cartas en la mano
    for (const auto& c : hand) {
        sum += c.points;
        if (c.rank == "A") {
            aces++;
        }
    }

    // Ajusta el valor de los Ases si la suma supera 21
    // Cada As puede cambiarse de 11 a 1 para evitar pasarse
    while (sum > 21 && aces > 0) {
        sum -= 10; // Cambia el valor del As de 11 a 1
        aces--;
    }
    return sum;
}

// Muestra las cartas en la mano del jugador
void Player::displayHand() const {
    cout << name << " tiene: ";
    for (size_t i = 0; i < hand.size(); ++i) {
        cout << hand[i].rank << " de " << hand[i].suit;
        // Agrega coma solo si no es la última carta
        if (i < hand.size() - 1) {
            cout << ", ";
        }
    }
    cout << endl; // No se muestran los puntos aquí según lo solicitado
}

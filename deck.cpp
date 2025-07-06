#include "deck.h"
#include <algorithm> // Para shuffle
#include <random>    // Para mt19937 y random_device
#include <ctime>     // Para time (semilla para random)
#include <stdexcept> // Para runtime_error
#include <string>    // Para stoi (si el rango es numérico)

using namespace std;

// Constructor: Inicializa una baraja estándar de 52 cartas
Deck::Deck() {
    vector<string> suits = {"Hearts", "Diamonds", "Clubs", "Spades"};
    vector<string> ranks = {"A", "2", "3", "4", "5", "6", "7",
                                       "8", "9", "10", "J", "Q", "K"};

    // Crea las 52 cartas
    for (const auto& suit : suits) {
        for (const auto& rank : ranks) {
            int points = 0;
            if (rank == "A") {
                points = 11; // El As cuenta inicialmente como 11
            } else if (rank == "J" || rank == "Q" || rank == "K") {
                points = 10; // Las figuras cuentan como 10
            } else {
                points = stoi(rank); // Las cartas numéricas cuentan su valor
            }
            cards.push_back({suit, rank, points});
        }
    }
}

// Mezcla las cartas de la baraja usando un generador de números aleatorios
void Deck::shuffle() {
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

// Reparte una sola carta de la parte superior de la baraja
Card Deck::deal() {
    if (cards.empty()) {
        // Maneja el caso donde la baraja está vacía (por ejemplo, barajar de nuevo o terminar el juego)
        throw runtime_error("La baraja está vacía. No se pueden repartir más cartas.");
    }
    Card c = cards.back(); // Obtiene la carta superior
    cards.pop_back();      // La elimina de la baraja
    return c;
}

#include <iostream>
#include <vector>
#include <string>
#include <limits>   // Para std::numeric_limits
#include <fstream>  // Para operaciones de archivos (principalmente manejado por persistence.h)
#include <algorithm> // Para std::sort (principalmente manejado por persistence.h)
#include <random>   // Para std::mt19937 y std::random_device
#include <ctime>    // Para std::time

using namespace std; // Se agregó using namespace std;

// Incluir cabeceras del proyecto
#include "deck.h"       // Clase Deck y struct Card
#include "player.h"     // Clase Player
#include "persistence.h" // Funciones de persistencia de usuarios y ranking
#include "utils.h"      // Funciones utilitarias (mostrar mensajes, colores, etc.)

// Declaraciones anticipadas de funciones del juego
void playerTurn(Player& player, Deck& deck);
void dealerTurn(Player& dealer, Deck& deck, int playerPoints);
void playAgainstDealer(mt19937& rng); // Modificado para aceptar RNG
void playMultiplayer(mt19937& rng);   // Modificado para aceptar RNG
void displayMainMenu();


//Funciones de lógica del juego
// Maneja el turno de un solo jugador (humano o dealer)
void playerTurn(Player& player, Deck& deck) {
    char choice;
    while (true) {
        displayMessage("\n--- Turno de " + player.name + " ---", YELLOW_COLOR);
        player.displayHand();
        displayMessage("Puntos: " + to_string(player.calculatePoints()), CYAN_COLOR);

        // Verifica Blackjack inicial (21 con 2 cartas)
        if (player.calculatePoints() == 21 && player.hand.size() == 2) {
            displayAchievement("BLACKJACK!");
            break; // El turno termina con Blackjack
        }
        // Verifica si el jugador se pasó
        if (player.calculatePoints() > 21) {
            displayMessage(player.name + " se pasó (más de 21).", RED_COLOR);
            displayAchievement("¡TE PASASTE!"); // Logro por pasarse
            break;
        }
        // Verifica 5 cartas sin pasarse
        if (player.hand.size() == 5 && player.calculatePoints() <= 21) {
            displayAchievement("¡5 CARTAS SIN PASARSE!");
            break; // El turno termina después de tomar 5 cartas sin pasarse
        }

        displayMessage("¿Quieres otra carta? (y/n): ", GREEN_COLOR);
        cin >> choice;

        // Validación de entrada para 'y' o 'n'
        while (cin.fail() || (choice != 'y' && choice != 'n')) {
            displayMessage("Entrada inválida. Por favor ingresa 'y' o 'n'.", RED_COLOR);
            cin.clear(); // Limpia las banderas de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta entrada inválida
            cin >> choice;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia el buffer después de entrada válida

        if (choice == 'y') {
            Card newCard = deck.deal();
            player.receiveCard(newCard);
            displayMessage("Recibiste: ", BLUE_COLOR);
            displayCardText(newCard.rank, newCard.suit);
        } else {
            break; // El jugador decide plantarse
        }
    }
}

// Maneja el turno del dealer
void dealerTurn(Player& dealer, Deck& deck, int playerPoints) {
    displayMessage("\n--- Turno del Dealer ---", YELLOW_COLOR);
    displayMessage("Mano del Dealer: ", YELLOW_COLOR);
    dealer.displayHand(); // Ahora muestra la mano completa
    displayMessage("Puntos del Dealer: " + to_string(dealer.calculatePoints()), GREEN_COLOR);

    while (true) {
        int points = dealer.calculatePoints();
        // El dealer toma cartas hasta tener 17 o más puntos, o se pasa, o tiene más puntos que el jugador (si el jugador no se pasó)
        if ((points >= 17 && points >= playerPoints) || points > 21) {
            if (points > 21) {
                displayMessage("El Dealer se pasó (más de 21).", RED_COLOR);
                displayAchievement("¡DEALER SE PASÓ!"); // Logro por dealer pasarse
            } else {
                displayMessage("El Dealer se planta con " + to_string(points) + " puntos.", CYAN_COLOR);
            }
            break;
        }
        displayMessage("El Dealer toma una carta...", YELLOW_COLOR);
        Card c = deck.deal();
        dealer.receiveCard(c);
        displayCardText(c.rank, c.suit);
        dealer.displayHand(); // Muestra la mano actualizada después de tomar una carta
        displayMessage("Puntos del Dealer: " + to_string(dealer.calculatePoints()), GREEN_COLOR);
    }
}

// Juega una partida de Blackjack contra el dealer
void playAgainstDealer(mt19937& rng) { // Se agregó mt19937& rng
    string name;
    displayMessage("Ingresa tu nombre: ", GREEN_COLOR);
    getline(cin, name); // Usa getline para permitir espacios en el nombre
    saveUser(name); // Guarda el usuario (maneja duplicados)

    Deck deck;
    deck.shuffle(); // Baraja el mazo

    Player player(name);
    Player dealer("Dealer");

    // Reparto inicial: 2 cartas cada uno
    player.receiveCard(deck.deal());
    dealer.receiveCard(deck.deal()); // Primera carta del dealer (visible)
    player.receiveCard(deck.deal());
    dealer.receiveCard(deck.deal()); // Segunda carta del dealer (oculta hasta que termine el turno del jugador)

    displayMessage("\n--- Manos Iniciales ---", YELLOW_COLOR);
    player.displayHand();
    displayMessage("El Dealer muestra: ", YELLOW_COLOR);
    displayCardText(dealer.hand[0].rank, dealer.hand[0].suit);
    displayMessage("La segunda carta del Dealer está boca abajo.", YELLOW_COLOR);

    playerTurn(player, deck);

    // Si el jugador se pasa, el dealer no necesita jugar
    if (player.calculatePoints() > 21) {
        displayMessage("\n--- Resultados Finales ---", YELLOW_COLOR);
        displayMessage("Puntos de " + player.name + ": " + to_string(player.calculatePoints()), RED_COLOR);
        displayMessage("Puntos del Dealer: " + to_string(dealer.calculatePoints()), GREEN_COLOR); // Igual muestra los puntos del dealer
        displayMessage(player.name + " se pasó. ¡Gana el Dealer!", LOSER_COLOR);
        beepSound();
        updateRanking(dealer.name); // Gana el dealer
        return;
    }

    dealerTurn(dealer, deck, player.calculatePoints());

    int playerPoints = player.calculatePoints();
    int dealerPoints = dealer.calculatePoints();

    displayMessage("\n--- Resultados Finales ---", YELLOW_COLOR);
    displayMessage("Puntos de " + player.name + ": " + to_string(playerPoints), GREEN_COLOR);
    displayMessage("Puntos del Dealer: " + to_string(dealerPoints), RED_COLOR);

    // Determina el ganador
    if (playerPoints <= 21 && (playerPoints > dealerPoints || dealerPoints > 21)) {
        displayMessage("¡Ganaste!", WINNER_COLOR);
        beepSound();
        updateRanking(player.name);
    } else if (dealerPoints <= 21 && (dealerPoints > playerPoints || playerPoints > 21)) {
        displayMessage("¡Gana el Dealer!", LOSER_COLOR);
        beepSound();
        updateRanking(dealer.name);
    } else {
        displayMessage("¡Empate!", YELLOW_COLOR);
    }
}

// Juega una partida multijugador de Blackjack
void playMultiplayer(mt19937& rng) { // Modificado para aceptar RNG
    int numPlayers;
    displayMessage("Ingresa el número de jugadores (2-7): ", GREEN_COLOR);
    cin >> numPlayers;

    // Validación de entrada para el número de jugadores
    while (cin.fail() || numPlayers < 2 || numPlayers > 7) {
        displayMessage("Número de jugadores inválido. Ingresa un número entre 2 y 7.", RED_COLOR);
        cin.clear(); // Limpia las banderas de error
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta entrada inválida
        cout << "Ingresa el número de jugadores (2-7): ";
        cin >> numPlayers;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia el buffer

    vector<Player> players;
    for (int i = 0; i < numPlayers; ++i) {
        string name;
        displayMessage("Ingresa el nombre del Jugador " + to_string(i + 1) + ": ", GREEN_COLOR);
        getline(cin, name); // Usa getline para permitir espacios en el nombre
        saveUser(name); // Guarda el usuario (maneja duplicados)
        players.push_back(Player(name));
    }

    Deck deck;
    deck.shuffle(); // Baraja el mazo

    // Reparto inicial: 2 cartas para cada jugador
    for (int i = 0; i < 2; ++i) {
        for (Player& p : players) {
            p.receiveCard(deck.deal());
        }
    }

    displayMessage("\n--- Manos Iniciales ---", YELLOW_COLOR);
    for (Player& p : players) {
        displayMessage("Mano de " + p.name + ":", CYAN_COLOR);
        p.displayHand();
    }

    // Cada jugador toma su turno
    for (Player& p : players) {
        playerTurn(p, deck);
    }

    displayMessage("\n--- Resultados del Juego Multijugador ---", YELLOW_COLOR);
    int maxPoints = 0;
    // Encuentra la puntuación más alta entre los jugadores que no se pasaron
    for (const Player& p : players) {
        int points = p.calculatePoints();
        displayMessage(p.name + " terminó con " + to_string(points) + " puntos.", CYAN_COLOR);
        if (points <= 21 && points > maxPoints) {
            maxPoints = points;
        }
    }

    vector<string> winners;
    if (maxPoints == 0) { // Todos se pasaron
        displayMessage("Todos los jugadores se pasaron. No hay ganador claro.", YELLOW_COLOR);
    } else {
        // Reúne a todos los jugadores que lograron la máxima puntuación sin pasarse
        for (const Player& p : players) {
            if (p.calculatePoints() == maxPoints) {
                winners.push_back(p.name);
            }
        }

        if (winners.size() == 1) {
            displayMessage("El ganador es: " + winners[0] + "!", WINNER_COLOR);
            beepSound();
            updateRanking(winners[0]);
        } else {
            displayMessage("Empate entre los siguientes jugadores:", YELLOW_COLOR);
            for (const string& winnerName : winners) {
                displayMessage("- " + winnerName, YELLOW_COLOR);
                updateRanking(winnerName); // Actualiza ranking para todos los ganadores empatados
            }
            beepSound();
        }
    }
}

// Muestra el menú principal del juego
void displayMainMenu() {
    cout << "\n" << YELLOW_COLOR << "=========================" << RESET_COLOR << endl;
    cout << YELLOW_COLOR << "       BLACKJACK PRO       " << RESET_COLOR << endl;
    cout << YELLOW_COLOR << "=========================" << RESET_COLOR << endl;
    cout << "1. Jugar contra el Dealer" << endl;
    cout << "2. Jugar Multijugador" << endl;
    cout << "3. Ver Usuarios" << endl;
    cout << "4. Ver Ranking" << endl;
    cout << "5. Reiniciar Ranking" << endl; // Nueva opción
    cout << "6. Salir" << endl;
    cout << "\nIngresa tu opción: ";
}

// Bucle principal del juego
int main() {
    // Limpia la consola al inicio para una mejor apariencia (dependiente del sistema)
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    // Inicializa el generador de números aleatorios UNA SOLA VEZ al inicio del programa
    random_device rd;
    mt19937 rng(rd()); // Motor Mersenne Twister inicializado por random_device

    // Semilla alternativa si random_device falla (por ejemplo, retorna 0)
    if (rng == mt19937(0)) {
        rng.seed(static_cast<unsigned>(time(nullptr)));
    }

    int option;
    do {
        displayMainMenu();
        cin >> option;

        // Validación de entrada para la opción del menú
        while (cin.fail() || option < 1 || option > 6) { // Rango ajustado 1-6
            displayMessage("Opción inválida. Ingresa un número entre 1 y 6.", RED_COLOR);
            cin.clear(); // Limpia las banderas de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta entrada inválida
            displayMainMenu();
            cin >> option;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia el buffer después de entrada válida

        switch (option) {
            case 1: playAgainstDealer(rng); break; // Pasa rng
            case 2: playMultiplayer(rng); break;   // Pasa rng
            case 3: displayUsers(); break;
            case 4: displayRanking(); break;
            case 5: resetRanking(); break; // Nuevo caso para reiniciar ranking
            case 6: displayMessage("Gracias por jugar. ¡Hasta luego!", CYAN_COLOR); break;
            default: displayMessage("Opción inválida.", RED_COLOR); // No debería alcanzarse por el while
        }
        
        // Pausa antes de mostrar el menú de nuevo, a menos que salga
        if (option != 6) {
            displayMessage("\nPresiona Enter para continuar...", YELLOW_COLOR);
            cin.get(); // Espera a que el usuario presione enter
        }
    } while (option != 6); // Bucle hasta que el usuario elija salir

    return 0;
}
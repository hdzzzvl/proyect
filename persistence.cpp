#include "persistence.h"
#include <iostream>  
#include <fstream>   
#include <vector>    
#include <string>    
#include <algorithm> 
#include <cctype>    
#include <map>       

using namespace std;

// Función auxiliar para convertir una cadena a minúsculas y eliminar espacios en blanco al inicio/final
string toLowerAndTrim(const string& str) {
    string s = str;
    // Eliminar espacios en blanco al inicio
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    // Eliminar espacios en blanco al final
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
    // Convertir a minúsculas
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// Muestra todos los usuarios registrados desde "users.txt"
void displayUsers() {
    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << "No se encontraron usuarios registrados." << endl;
        return;
    }
    string user;
    cout << "--- Usuarios Registrados ---" << endl;
    while (getline(file, user)) {
        cout << "- " << user << endl;
    }
    file.close();
}

// Guarda un nuevo usuario en "users.txt", evitando duplicados (insensible a mayúsculas y espacios)
void saveUser(const string& name) {
    ifstream inputFile("users.txt");
    vector<string> processedExistingUsers;
    string line;

    // Leer todos los usuarios existentes y procesar sus nombres para comparación
    if (inputFile.is_open()) {
        while (getline(inputFile, line)) {
            processedExistingUsers.push_back(toLowerAndTrim(line));
        }
        inputFile.close();
    }

    // Procesar el nombre del nuevo usuario para comparación
    string processedName = toLowerAndTrim(name);

    // Verificar si el nombre procesado ya existe
    bool userExists = false;
    if (find(processedExistingUsers.begin(), processedExistingUsers.end(), processedName) != processedExistingUsers.end()) {
        userExists = true;
    }

    // Si el usuario no existe, agregar el nombre original al archivo
    if (!userExists) {
        ofstream outputFile("users.txt", ios::app); // Abrir en modo adjuntar
        if (outputFile.is_open()) {
            outputFile << name << endl; // Guardar el nombre original
            outputFile.close();
            cout << "Usuario '" << name << "' registrado." << endl;
        } else {
            cerr << "Error: No se pudo abrir el archivo de usuarios para escribir." << endl;
        }
    } else {
        cout << "El usuario '" << name << "' ya está registrado." << endl;
    }
}

// Actualiza la puntuación de un jugador en el archivo de ranking ("ranking.txt")
// Esta función simplemente agrega el nombre del ganador al archivo.
void updateRanking(const string& name) {
    ofstream file("ranking.txt", ios::app); // Abrir en modo adjuntar
    if (file.is_open()) {
        file << name << endl;
        file.close();
    } else {
        cerr << "Error: No se pudo abrir el archivo de ranking para escribir." << endl;
    }
}

// Muestra los 5 mejores jugadores del ranking según victorias
void displayRanking() {
    ifstream file("ranking.txt");
    if (!file.is_open()) {
        cout << "No hay partidas registradas en el ranking." << endl;
        return;
    }

    map<string, int> playerWins; // Mapa para contar victorias de cada jugador
    string name;
    while (getline(file, name)) {
        playerWins[name]++; // Incrementar el conteo de victorias por jugador
    }
    file.close();

    // Convertir el mapa a un vector de pares para ordenar
    vector<pair<string, int>> rankingList;
    for (const auto& entry : playerWins) {
        rankingList.push_back(entry);
    }

    // Ordenar la lista de ranking por victorias en orden descendente
    sort(rankingList.begin(), rankingList.end(), [](const auto &a, const auto &b) {
        return b.second < a.second; // Ordenar por victorias (segundo elemento) descendente
    });

    cout << "--- TOP 5 JUGADORES ---" << endl;
    for (size_t i = 0; i < rankingList.size() && i < 5; ++i) {
        cout << i + 1 << ". " << rankingList[i].first << " - " << rankingList[i].second << " victorias" << endl;
    }
}

// Reinicia (limpia) todas las entradas del archivo de ranking
void resetRanking() {
    ofstream file("ranking.txt", ios::trunc); // Abrir en modo truncar para limpiar contenido
    if (file.is_open()) {
        file.close();
        cout << "El ranking ha sido reiniciado." << endl;
    } else {
        cerr << "Error: No se pudo abrir el archivo de ranking para reiniciar." << endl;
    }
}

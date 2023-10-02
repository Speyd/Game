#include "Creation_Launch.h"
#include "Player.h"
#include "InputInfo.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include<fstream>
#include <chrono>
#include <thread>
using namespace std;
extern int graphic;
void ClearLineScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
//ClearLineScreen - Очистка экрана
int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));
    ClearLineScreen();
    Creation_Launch();//Создание/Загружение мира
    ClearLineScreen();
    if (graphic != 2) {
        Player();//Взаимодейстивя игрока
        InputInfo();//Схранение инфрмации
    }
    return 0;
}
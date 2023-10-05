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
#include <SFML/Audio.hpp>
using namespace std;
#pragma region StructWorld

typedef struct {
    char name;//Имя структуры которою добываем
    vector<int> amount;//Количество предметов
}InfoItem;//Структура котрая хранит дроп дерева, камня и т.д.
InfoItem drop;//Дроп который падает при добычи дерева, камня и т.д.

typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    double hp;//Время изчезновения
    vector<string> tempdrop;//То, что хрнит в себе временный сундук
}InfoTempItem;//Иформация про временный сундук
vector<InfoTempItem> timedrop;//Временный сундук

typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    string drop[12];//Ячейки под предметы в судуке
}InfoChestItem;//Структура котрая хранит информацию об сундуках
vector<InfoChestItem> dropchest;//Массив котрый хранит информацию об сундуках

typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    double hp;//Время роста
}InfoGrowthTree;//Иформация про рост деревьев
vector<InfoGrowthTree> tree;//Иформация про рост деревьев

typedef struct {
    char name;//Имя структуры(дерево, камень и т.д.)
    double hp;//Прочность
    int location[4];//Координаты места в мире
}InfoStucture;//Информацию об всех структурах мира
vector<InfoStucture> structure; //Динамический массив который хранит информацию об всех структурах мира
#pragma endregion
extern int setting_music;
extern void ClearLineScreen();//Очистка Экрана
extern vector<vector<int>> research_map;//Миникарта
extern vector<vector<vector<vector<char>>>> world;//Динамический массив который хранит мир
double timerespawne, save_time_delay = 0, time_world = 390;
//timerespawne - Переменная которая хранит время до респавна камня на чанках
//save_time_delay - Переменная которая хранит в себе время на колдаун действий
//time_world - Переменная которая хранит время мира(день-ночь)
char choiceswap;// перменная которая отвечает за выбор в смене-переносе ячеек в инвентаре-сундуке-временном сундуке-крафте
string inventory[20] = { "","","","","","","","","","","","","","","","","","","","" }, ejectitem[3] = { "","","" }, craft_aftermath = "", craft_aftermath_input = "";
//indentory - Инвентарь игрока,ejectitem - массив котрорый показывает удаление/добавление предметов с/в инвентарь и т.д., craft_aftermath - предмет который скрафтил ирок в инвентаре/верстаке, craft_aftermath_input - переменная котроая показывает что может скрафтить игрок
int error = 0, error_input = 0, item[2], hand = 0, empty_cell = 0, line_of_sight = 0, input_chest = 0, index_input_tempdrop, index_input_chestdrop,day = 1;
//error - переменная которая хранит в себе номер ошибки мира, error_input - переменная которая хранит в себе номер ошибки инвентаря, item - хранит в себе индекса ячейки инвентаря - сундука и т.д.(применяеться в MoveItem)
//amount_structure - возможно нужно добавить| empty_cell - Переменная которая обозначает количество пусых ячеек в инвентаре и т.д., hand - //Предмет который помещен в руку
//line_of_sight - Линия взгляда игрока (верх,низ,право,леов), input_chest - Вариант взаимодейсвия(1 - временный сундук, 2 - сундук, 3 - Верстак)
//index_input_tempdrop - индекс который вписывает игрок для обмена предметами в временном сундуке, index_input_chestdrop - индекс который вписывает игрок для обмена предметами в сундуке
//day - количесвто прожитых дней
bool check_fatigue = false;//Эта переменная отвечает за то, рубил/бил/ломал ли грок что-то
vector<vector<int>> torch;
vector<string> craft;//Область крафта в инвентаре(2х2 ячейки) и в верстаке(3х3 ячейки)
#pragma region DeleteItem
void FindIndexChest(int loc[4]);//Поиск сундука в массиве structure
void DeleteLineOfSight(int line_of_sight, int loc[4]) {
    if (line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == '^') {
        world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = ' ';
    }
    else if (line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'v') {
        world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = ' ';
    }
    else if (line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == '>') {
        world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = ' ';
    }
    else if (line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == '<') {
        world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = ' ';
    }
}
//DeleteLineOfSight - Удаление стрелочкек направления игрока(показывает куда смотрит игрок)
void DeleteToolInventory(sf::Sound& break_tool) {
    string temp_num[2] = { "","" };
    string g;
    for (int i = inventory[hand].find(' ') + 1; i != inventory[hand].find('n'); i++) {
        if (i < inventory[hand].find('('))temp_num[0] += inventory[hand][i];
        if (i > inventory[hand].find('('))temp_num[1] += inventory[hand][i];
    }
    if (stoi(temp_num[1]) == 1) { error = 16; break_tool.play(); inventory[hand] = ""; }
    else inventory[hand] = "x1 " + temp_num[0] + "(" + std::to_string(stoi(temp_num[1]) - 1) + "n)";
}
//DeleteToolInventory - Удаление инструментов с инвентаря
void DeleteIndexChest(int num) {
    for (int l = 0; l != dropchest.size(); l++) {
        if (num == dropchest[l].loc_in_structure) {
            num = l;
            break;
        }
    }
    for (int l = 0; l != dropchest.size(); l++) {
        if (structure.size() - 1 == dropchest[l].loc_in_structure) {
            dropchest[l].loc_in_structure = dropchest[num].loc_in_structure;
            break;
        }
    }
    for (int l = 0; l != timedrop.size(); l++) {
        if (structure.size() - 1 == timedrop[l].loc_in_structure) {
            timedrop[l].loc_in_structure = dropchest[num].loc_in_structure;
            break;
        }
    }
    for (int l = 0; l != tree.size(); l++) {
        if (structure.size() - 1 == tree[l].loc_in_structure) {
            tree[l].loc_in_structure = dropchest[num].loc_in_structure;
            break;
        }
    }
    swap(dropchest[num], dropchest[dropchest.size() - 1]);
    dropchest.resize(dropchest.size() - 1);
}
//DeleteIndexChest - Удаление сундука из массива structure
void DeleteTimeDrop(double timePlayer) {
    if (timedrop.size() != 0) {
        int num = 1;
        while (num != 0) {
            if (timedrop.size() == 0)break;
            for (int i = 0; i != timedrop.size(); i++) {
                timedrop[i].hp += timePlayer;
                if (timedrop[i].hp >= 120) {
                    for (int l = 0; l != timedrop.size(); l++) {
                        if (structure.size() - 1 == timedrop[l].loc_in_structure) {
                            timedrop[l].loc_in_structure = timedrop[i].loc_in_structure;
                            break;
                        }
                    }
                    for (int l = 0; l != tree.size(); l++) {
                        if (structure.size() - 1 == tree[l].loc_in_structure) {
                            tree[l].loc_in_structure = timedrop[i].loc_in_structure;
                            break;
                        }
                    }
                    for (int l = 0; l != dropchest.size(); l++) {
                        if (structure.size() - 1 == dropchest[l].loc_in_structure) {
                            dropchest[l].loc_in_structure = timedrop[i].loc_in_structure;
                            break;
                        }
                    }
                    swap(structure[timedrop[i].loc_in_structure], structure[structure.size() - 1]);
                    world[structure[structure.size() - 1].location[0]][structure[structure.size() - 1].location[1]][structure[structure.size() - 1].location[2]][structure[structure.size() - 1].location[3]] = ' ';
                    structure.resize(structure.size() - 1);
                    swap(timedrop[i], timedrop[timedrop.size() - 1]);
                    timedrop.resize(timedrop.size() - 1);
                    num = 1;
                    break;
                }
                else num = 0;
            }
        }
    }
}
//DeleteTimeDrop - Удаление временного сундука из массива structure
#pragma endregion
#pragma region ActionsPlayer
void ChekPlayer(vector<vector<vector<vector<char>>>>& world, int loc[4]) {
    for (int i = 0; i != world.size(); i++) {
        for (int l = 0; l != world[i].size(); l++) {
            for (int j = 0; j != world[i][l].size(); j++) {
                for (int f = 0; f != world[i][l][j].size(); f++) {
                    if (world[i][l][j][f] == '0') {
                        loc[0] = i; loc[1] = l; loc[2] = j; loc[3] = f;
                        break;
                    }
                }
            }
        }
    }
}
//ChekPlayer - Проверка где находиться игрок на карте
void InputDropInventory(int& invent_info, int loc[4], int line_of_sight, vector<InfoStucture>& structure, vector<InfoTempItem>& timedrop,sf::Sound& take) {
    srand(time(0));
    bool index_switch = false;//переменная которая отвечает за считывания drop выше 0 индекса
    for (int j = 0; j != drop.amount.size(); j++) {
        for (int i = 0; i != 20; i++) {
            string temp_text = "";
            int num = 0;
            bool wrong_cell = true;
            if (inventory[i].empty() == false) {
                if (inventory[i].find("топор") - 1 == inventory[i].find('-') || inventory[i].find("кирка") - 1 == inventory[i].find('-'))wrong_cell = true;
                else if (inventory[i].find("дерева") != string::npos && drop.name == 'Y' && index_switch == false)wrong_cell = false;
                else if (inventory[i].find("саженца") != string::npos && drop.name == 'y' || inventory[i].find("саженца") != string::npos && drop.name == 'Y' && index_switch == true)wrong_cell = false;
                else if (inventory[i].find("камня") != string::npos && drop.name == 'S' && index_switch == false)wrong_cell = false;
                else if (inventory[i].find("уголь") != string::npos && drop.name == 'S' && index_switch == true)wrong_cell = false;
                else if (inventory[i].find("сундук") != string::npos && drop.name == 'C')wrong_cell = false;
                else if (inventory[i].find("верстак") != string::npos && drop.name == 'W')wrong_cell = false;
                else if (inventory[i].find("факел") != string::npos && drop.name == 'i')wrong_cell = false;
                if (wrong_cell == false) {
                    for (int l = inventory[i].find('x') + 1; l != inventory[i].find(' '); l++) {
                        temp_text += inventory[i][l];
                    }
                    num = stoi(temp_text);
                    if (num == 12) continue;
                    else if (12 - num >= drop.amount[j]) {
                        if (j == 0) {
                            if (drop.name == 'Y') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " дерева"; index_switch = true; drop.amount[j] = 0; break; }
                            else if (drop.name == 'S') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " камня"; index_switch = true; drop.amount[j] = 0; break; }
                            else if (drop.name == 'C') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " сундук"; drop.amount[j] = 0; break; }
                            else if (drop.name == 'W') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " верстак"; drop.amount[j] = 0; break; }
                            else if (drop.name == 'i') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " факел"; drop.amount[j] = 0; break; }
                        }
                        if (j == 1) {
                            if (drop.name == 'y' || drop.name == 'Y') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " саженца"; drop.amount[j] = 0; break; }
                            else if (drop.name == 'S') { inventory[i] = "x" + std::to_string(drop.amount[j] + num) + " уголь"; drop.amount[j] = 0; break; }
                        }
                    }
                    else if (12 - num < drop.amount[j]) {
                        if(j == 0){
                            if (drop.name == 'Y') { inventory[i] = "x12 дерева"; drop.amount[j] -= (12 - num); }
                            else if (drop.name = 'S') { inventory[i] = "x12 камня"; drop.amount[j] -= (12 - num); }
                            else if (drop.name == 'C') { inventory[i] = "x12 сундук"; drop.amount[j] -= (12 - num); }
                            else if (drop.name == 'W') { inventory[i] = "x12 верстак"; drop.amount[j] -= (12 - num); }
                            else if (drop.name == 'i') { inventory[i] = "x12 факел"; drop.amount[j] -= (12 - num); }
                        }
                        if (j == 1) {
                            if (drop.name == 'y' || drop.name == 'Y') { inventory[i] = "x12 саженца"; drop.amount[j] -= (12 - num); }
                            else if (drop.name == 'S') { inventory[i] = "x12 уголь"; drop.amount[j] -= (12 - num); break; }
                        }
                    }
                }
            }
            else empty_cell++;
        }
        if (drop.amount.size() > 1 && drop.amount[0] != 0 && j == 0) index_switch = true;
    }
    if (empty_cell > 0) {
        for (int i = 0; i != 20; i++) {
            if (inventory[i].empty()) {
                if ( drop.amount[0] != 0) {
                    if (drop.name == 'Y') { inventory[i] = "x" + std::to_string(drop.amount[0]) + " дерева"; drop.amount[0] = 0; index_switch == true; }
                    else if (drop.name == 'S' || drop.name == 's') { inventory[i] = "x" + std::to_string(drop.amount[0]) + " камня"; drop.amount[0] = 0; index_switch == true; }
                    else if (drop.name == 'C') { inventory[i] = "x" + std::to_string(drop.amount[0]) + " сундук"; drop.amount[0] = 0; break; }
                    else if (drop.name == 'W') { inventory[i] = "x" + std::to_string(drop.amount[0]) + " верстак"; drop.amount[0] = 0; break; }
                    else if (drop.name == 'i') { inventory[i] = "x" + std::to_string(drop.amount[0]) + " факел"; drop.amount[0] = 0; break; }
                }
                else if (drop.amount.size() == 2 && drop.amount[1] != 0) {
                    if (drop.name == 'y' || drop.name == 'Y' && index_switch == true) { inventory[i] = "x" + std::to_string(drop.amount[1]) + " саженца"; drop.amount[1] = 0; break; }
                    else if (drop.name == 'S' && index_switch == true) { inventory[i] = "x" + std::to_string(drop.amount[1]) + " уголь"; drop.amount[1] = 0; break; }
                }
                else break;
            }
        }
    }
    for (int i = 0, amount = 0; i != drop.amount.size(); i++) {
        if (drop.amount[i] == 0)amount++;
        if(i == drop.amount.size()-1 && amount>0)take.play();
    }
    if (empty_cell == 0 && drop.amount[0] != 0 || drop.amount.size() > 1 && empty_cell == 0 && drop.amount[1] != 0) {
        timedrop.resize(timedrop.size() + 1);
        structure.resize(structure.size() + 1);
        structure[structure.size() - 1].name = 'T';
        if (line_of_sight == 1) {
            world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = 'T';
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2] - 1; structure[structure.size() - 1].location[3] = loc[3];
        }
        else if (line_of_sight == 2) {
            world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'T';
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2] + 1; structure[structure.size() - 1].location[3] = loc[3];
        }
        else if (line_of_sight == 3) {
            world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = 'T';
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2]; structure[structure.size() - 1].location[3] = loc[3] + 1;
        }
        else if (line_of_sight == 4) {
            world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = 'T';
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2]; structure[structure.size() - 1].location[3] = loc[3] - 1;
        }
        timedrop[timedrop.size() - 1].loc_in_structure = structure.size() - 1;
        if (drop.amount.size() > 1 && drop.amount[0] != 0 && drop.amount[1] != 0) {
            timedrop[timedrop.size() - 1].tempdrop.resize(2);
            if (invent_info == 2 || invent_info == 1) {
                if (drop.name == 'Y') {
                    timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " дерева";
                    timedrop[timedrop.size() - 1].tempdrop[1] = "x" + std::to_string(drop.amount[1]) + " саженца";
                }
                else if (drop.name == 'S') {
                    timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " камня";
                    timedrop[timedrop.size() - 1].tempdrop[1] = "x" + std::to_string(drop.amount[1]) + " уголь";
                }
            }
        }
        else if (drop.amount[0] != 0) {
            timedrop[timedrop.size() - 1].tempdrop.resize(1);
            if (drop.name == 'Y')timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " дерева";
            else if (drop.name == 'S')timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " камня";
            else if (drop.name == 'C')timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " сундук";
            else if (drop.name == 'W')timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " верстак";
            else if (drop.name == 'i')timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[0]) + " факел";
        }
        else if (drop.amount.size() > 1 && drop.amount[1] != 0) {
            timedrop[timedrop.size() - 1].tempdrop.resize(1);
            if (drop.name == 'y' || drop.name == 'Y' && index_switch == true)timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[1]) + " саженца";
            else if (drop.name == 'S' && index_switch == true) timedrop[timedrop.size() - 1].tempdrop[0] = "x" + std::to_string(drop.amount[1]) + " уголь";
        }
        drop.name = ' ';
    }
    empty_cell = 0;
}
//InputDropInventory - Доавление в инвентарь добытых ресурсов
void Menu(char& choice, sf::Music& day_sound, sf::Music& night_sound) {
    if (setting_music == 1) {
        if (time_world >= 390 && time_world < 1110 && day_sound.getStatus() == 2) day_sound.pause();
        else if (time_world < 390 && time_world >= 0 && night_sound.getStatus() == 2 || time_world >= 1110 && night_sound.getStatus() == 2)night_sound.pause();
    }
    do {
        ClearLineScreen();
        cout << "\t\t\tМеню\n[1] - Продолжить игру\n[2] - Настройки\n[3] - Сохранить\n[4] - Сохранить и выйти\n";
        if (error == 0) { cout << "Ваш выбор: "; cin >> choice; }
        if (choice == '1' || choice == '2' || choice == '3' || choice == '4')error = 0;
        else { cout << "Нету такого выбора\nВведите ваш выбор снова: "; cin >> choice; error = 7; }
        if (choice == '2') {
            char choice_setting = '1';
            do {
                ClearLineScreen(); cout << "\t\tНастройки\n[1] - Музыка\n";
                if (choice_setting != '1') { cout << "Нету такого выбора!\nВведите ваш выбор снова(0 для выхода): "; cin >> choice_setting; }
                else { cout << "Выш выбор(0 для выхода): "; cin >> choice_setting; }
                if (choice_setting == '1')break;
            } while (true);
            int temp_setting_music = setting_music,choice_sound;
            if (choice_setting == '1'){
                do {
                    ClearLineScreen(); cout << "\t\tМузыка\n";
                    if (setting_music == 1) { cout << "[1] - Вкл. музыка   <---- Ваш выбор\n[2] - Выл. музыка\n"; }
                    else if (setting_music == 2) { cout << "[1] - Вкл. музыка\n[2] - Выл. музыка   <---- Ваш выбор\n"; }
                    else cout << "[1] - Вкл. музыка\n[2] - Выл. музыка\n";
                    if (setting_music < 0 || setting_music>2) { cout << "Нету такого выбора!\nВведите ваш выбор снова(0 для выхода/изменения настроек): "; cin >> setting_music; }
                    else { cout << "Выш выбор(0 для выхода/изменения настроек): "; cin >> setting_music; }
                    if (setting_music == 0) break;
                    choice_sound = setting_music;
                } while (true);
                setting_music = choice_sound;
                if (setting_music == 0 && temp_setting_music == 1) {
                    if (time_world >= 390 && time_world < 1110 && day_sound.getStatus() == 0) day_sound.stop();
                    else if (time_world < 390 && time_world >= 0 && night_sound.getStatus() == 0 || time_world >= 1110 && night_sound.getStatus() == 0)night_sound.stop();
                }
            }
        }
    } while (error != 0);
    ClearLineScreen();
    if (choice == '2' || choice == '3') InputInfo();
    if (setting_music == 1) {
        if (time_world >= 390 && time_world < 1110 && day_sound.getStatus() != 2) day_sound.play();
        else if (time_world < 390 && time_world >= 0 && night_sound.getStatus() != 2 || time_world >= 1110 && night_sound.getStatus() != 2)night_sound.play();

    }
}
//Меню
void ProductionDelay(double timeDelay) {
    if (check_fatigue == false) {
        if (inventory[hand].find("кирка") != string::npos)save_time_delay = 1;
        else if (inventory[hand].find("топор") != string::npos)save_time_delay = 1;
        else save_time_delay = 1.5;
    }
    else {
        save_time_delay -= timeDelay;
        if (save_time_delay <= 0)check_fatigue = false;
    }
}
//ProductionDelay - Усталость при добывании/ломании структур
void ChopBreak(vector<vector<vector<vector<char>>>>& world, int loc[4], int& line_of_sight, int chek_input[2], int& invent_info, vector<InfoStucture>& structure, vector<InfoTempItem>& timedrop,sf::Sound& break_struct, sf::Sound& break_tool) {
    srand(time(0));
    int amount_structmass = 0;
    string temp_num = "",f;
    if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
        for (int i = inventory[hand].find('(') + 1; i != inventory[hand].find(')'); i++)temp_num += inventory[hand][i];
    }
    if (line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'Y' || line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'y' || line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'S' || line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'C' || line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'W' || line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'i') {
        if (chek_input[0] == 1 || chek_input[1] == 1) {
            if (structure.size() == 0) {
                structure.resize(1);
                if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'Y') {
                    structure[0].name = 'Y';
                    if (inventory[hand].find("топор") != string::npos) {
                        if (inventory[hand].find('W') != string::npos)structure[0].hp = 3;
                        else if (inventory[hand].find('S') != string::npos)structure[0].hp = 2.5;
                        DeleteToolInventory(break_tool);
                    }
                    else structure[0].hp = 3.5;
                }
                else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'S') {
                    structure[0].name = 'S';
                    if (inventory[hand].find('W') != string::npos)structure[0].hp = 5.5;
                    else if (inventory[hand].find('S') != string::npos)structure[0].hp = 5;
                    DeleteToolInventory(break_tool);
                }
                structure[0].location[0] = loc[0]; structure[0].location[1] = loc[1];
                structure[0].location[2] = loc[2] - 1; structure[0].location[3] = loc[3];
            }
            else {
                for (int i = 0; i != structure.size(); i++) {
                    if (structure[i].location[0] == loc[0] && structure[i].location[1] == loc[1] && structure[i].location[2] == loc[2] - 1 && structure[i].location[3] == loc[3]) {
                        if (structure[i].name == 'C') {
                            FindIndexChest(loc);
                            for (int l = 0; l != 12; l++) {
                                if (dropchest[index_input_chestdrop].drop[i].empty() == false) { error = 11; break; }
                            }
                        }
                        else if (structure[i].name == 'W') {
                            if (craft_aftermath != "")error = 15;
                            else if (craft.size() == 4) {
                                for (int i = 0; i != 4; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                            else if (craft.size() == 9) {
                                for (int i = 0; i != 9; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                        }
                        if (error != 15 && error != 11) {
                             if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                                 if (inventory[hand].find('W') != string::npos) {
                                     if(structure[i].name!='S')structure[i].hp -= 1;
                                     else structure[i].hp -= 0.5;
                                 }
                                 else if (inventory[hand].find('S') != string::npos) {
                                     if (structure[i].name != 'S')structure[i].hp -= 1.5;
                                     else structure[i].hp -= 1;
                                 }
                                DeleteToolInventory(break_tool);
                             }
                             else structure[i].hp -= 0.5;
                        }
                        if (structure[i].hp <= 0) {
                            save_time_delay = 0; check_fatigue = false;
                            if (structure[i].name == 'Y') { drop.amount.resize(2); drop.amount[0] = rand() % (5 - 3 + 1) + 3; drop.amount[1] = rand() % (2 - 1 + 1) + 1; drop.name = 'Y'; }
                            else if (structure[i].name == 'y') { drop.amount.resize(2); drop.amount[1] = 1; drop.name = 'y'; }
                            else if (structure[i].name == 'S') { drop.amount.resize(2); drop.amount[0] = rand() % (3 - 2 + 1) + 2; if(rand()%(100-1+1)+1 <=20)drop.amount[1] = rand() % (3 - 1 + 1) + 1; drop.name = 'S'; }
                            else if (structure[i].name == 'C') { break_struct.play(); drop.amount.resize(1); DeleteIndexChest(i); invent_info = 4; drop.amount[0] = 1; drop.name = 'C'; }
                            else if (structure[i].name == 'W') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'W'; }
                            else if (structure[i].name == 'i') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'i'; }
                            world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = ' ';
                            for (int j = 0; j != timedrop.size(); j++) {
                                if (structure.size() - 1 == timedrop[j].loc_in_structure) {
                                    timedrop[j].loc_in_structure = i; break;
                                }
                            }
                            swap(structure[i], structure[structure.size() - 1]);
                            structure.resize(structure.size() - 1);
                            amount_structmass++;
                            break;
                        }
                    }
                    else amount_structmass++;
                }
                if (amount_structmass == structure.size() && structure.size() != 0) {
                    structure.resize(structure.size() + 1);
                    if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'Y') { structure[structure.size() - 1].name = 'Y'; structure[structure.size() - 1].hp = 4; }
                    else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'S') { structure[structure.size() - 1].name = 'S'; structure[structure.size() - 1].hp = 6; }
                    structure[structure.size() - 1].location[0] = loc[0];
                    structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2] - 1;
                    structure[structure.size() - 1].location[3] = loc[3];
                    if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                        if (inventory[hand].find('W') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1;
                            else structure[structure.size() - 1].hp -= 0.5;
                        }
                        else if (inventory[hand].find('S') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1.5;
                            else structure[structure.size() - 1].hp -= 1;
                        }
                        DeleteToolInventory(break_tool);
                    }
                    else structure[structure.size() - 1].hp -= 0.5;
                }
            }
        }
        else {
            if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'Y' && chek_input[1] == 1 || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'y' && chek_input[1] == 1)error = 3;
            else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'C' && chek_input[1] == 1)error = 12;
            else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'W' && chek_input[1] == 1)error = 14;
            else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'S' && chek_input[0] == 1)error = 4;
        }
    }
    else if (line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'Y' || line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'y' || line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'S' || line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'C' || line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'W' || line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'i') {
        if (chek_input[0] == 1 ||  chek_input[1] == 1) {
            if (structure.size() <= 0) {
                structure.resize(1);
                if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'Y') {
                    structure[0].name = 'Y';
                    if (inventory[hand].find('-') + 1 == inventory[hand].find("топор")) {
                        if (inventory[hand].find('W') != string::npos)structure[0].hp = 3;
                        else if (inventory[hand].find('S') != string::npos)structure[0].hp = 2.5;
                        DeleteToolInventory(break_tool);
                    }
                    else structure[0].hp = 3.5;
                }
                else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'S') {
                    structure[0].name = 'S';
                    if (inventory[hand].find('W') != string::npos)structure[0].hp = 5.5;
                    else if (inventory[hand].find('S') != string::npos)structure[0].hp = 5;
                    DeleteToolInventory(break_tool);
                }
                structure[0].location[0] = loc[0]; structure[0].location[1] = loc[1];
                structure[0].location[2] = loc[2] + 1; structure[0].location[3] = loc[3];
            }
            else {
                for (int i = 0; i != structure.size(); i++) {
                    if (structure[i].location[0] == loc[0] && structure[i].location[1] == loc[1] && structure[i].location[2] == loc[2] + 1 && structure[i].location[3] == loc[3]) {
                        if (structure[i].name == 'C') {
                            FindIndexChest(loc);
                            for (int l = 0; l != 12; l++) {
                                if (dropchest[index_input_chestdrop].drop[i].empty() == false) { error = 11; break; }
                            }
                        }
                        else if (structure[i].name == 'W') {
                            if (craft_aftermath != "")error = 15;
                            else if (craft.size() == 4) {
                                for (int i = 0; i != 4; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                            else if (craft.size() == 9) {
                                for (int i = 0; i != 9; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                        }
                        if (error != 15 && error != 11) {
                            if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                                if (inventory[hand].find('W') != string::npos) {
                                    if (structure[i].name != 'S')structure[i].hp -= 1;
                                    else structure[i].hp -= 0.5;
                                }
                                else if (inventory[hand].find('S') != string::npos) {
                                    if (structure[i].name != 'S')structure[i].hp -= 1.5;
                                    else structure[i].hp -= 1;
                                }
                                DeleteToolInventory(break_tool);
                            }
                            else structure[i].hp -= 0.5;
                        }
                        if (structure[i].hp <= 0) {
                            save_time_delay = 0; check_fatigue = false;
                            if (structure[i].name == 'Y') { drop.amount.resize(2); drop.amount[0] = rand() % (5 - 3 + 1) + 3; drop.amount[1] = rand() % (2 - 1 + 1) + 1; drop.name = 'Y'; }
                            else if (structure[i].name == 'y') { drop.amount.resize(2); drop.amount[1] = 1; drop.name = 'y'; }
                            else if (structure[i].name == 'S') { drop.amount.resize(2); drop.amount[0] = rand() % (3 - 2 + 1) + 2; if(rand()%(100-1+1)+1 <=20)drop.amount[1] = rand() % (3 - 1 + 1) + 1; drop.name = 'S'; }
                            else if (structure[i].name == 'C') { break_struct.play(); drop.amount.resize(1); DeleteIndexChest(i); invent_info = 4; drop.amount[0] = 1; drop.name = 'C'; }
                            else if (structure[i].name == 'W') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'W'; }
                            else if (structure[i].name == 'i') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'i'; }
                            world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = ' ';
                            for (int j = 0; j != timedrop.size(); j++) {
                                if (structure.size() - 1 == timedrop[j].loc_in_structure) {
                                    timedrop[j].loc_in_structure = i; break;
                                }
                            }
                            swap(structure[i], structure[structure.size() - 1]);
                            structure.resize(structure.size() - 1);
                            amount_structmass++;
                            break;
                        }
                    }
                    else amount_structmass++;
                }
                if (amount_structmass == structure.size() && structure.size() != 0) {
                    structure.resize(structure.size() + 1);
                    if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'Y') { structure[structure.size() - 1].name = 'Y'; structure[structure.size() - 1].hp = 4; }
                    else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'S') { structure[structure.size() - 1].name = 'S'; structure[structure.size() - 1].hp = 6; }
                    structure[structure.size() - 1].location[0] = loc[0];
                    structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2] + 1;
                    structure[structure.size() - 1].location[3] = loc[3];
                    if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                        if (inventory[hand].find('W') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1;
                            else structure[structure.size() - 1].hp -= 0.5;
                        }
                        else if (inventory[hand].find('S') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1.5;
                            else structure[structure.size() - 1].hp -= 1;
                        }
                        DeleteToolInventory(break_tool);
                    }
                    else structure[structure.size() - 1].hp -= 0.5;
                }
            }
        }
        else {
            if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'Y' && chek_input[1] == 1 || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'y' && chek_input[1] == 1)error = 3;
            else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'C' && chek_input[1] == 1)error = 12;
            else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'W' && chek_input[1] == 1)error = 14;
            else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'S' && chek_input[0] == 1)error = 4;
        }
    }
    else if (line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'Y' || line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'y' || line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'S' || line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'C' || line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'W' || line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'i') {
        if (chek_input[0] == 1 || chek_input[1] == 1) {
            if (structure.size() == 0) {
                structure.resize(1);
                if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'Y') {
                    structure[0].name = 'Y';
                    if (inventory[hand].find("топор") != string::npos) {
                        if (inventory[hand].find('W') != string::npos)structure[0].hp = 3;
                        else if (inventory[hand].find('S') != string::npos)structure[0].hp = 2.5;
                        DeleteToolInventory(break_tool);
                    }
                    else structure[0].hp = 3.5;
                }
                else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'S') {
                    structure[0].name = 'S';
                    if (inventory[hand].find('W') != string::npos)structure[0].hp = 5.5;
                    else if (inventory[hand].find('S') != string::npos)structure[0].hp = 5;
                    DeleteToolInventory(break_tool);
                }
                structure[0].location[0] = loc[0]; structure[0].location[1] = loc[1];
                structure[0].location[2] = loc[2]; structure[0].location[3] = loc[3] + 1;
            }
            else {
                for (int i = 0; i != structure.size(); i++) {
                    if (structure[i].location[0] == loc[0] && structure[i].location[1] == loc[1] && structure[i].location[2] == loc[2] && structure[i].location[3] == loc[3] + 1) {
                        if (structure[i].name == 'C') {
                            FindIndexChest(loc);
                            for (int l = 0; l != 12; l++) {
                                if (dropchest[index_input_chestdrop].drop[i].empty() == false) { error = 11; break; }
                            }
                        }
                        else if (structure[i].name == 'W') {
                            if (craft_aftermath != "")error = 15;
                            else if (craft.size() == 4) {
                                for (int i = 0; i != 4; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                            else if (craft.size() == 9) {
                                for (int i = 0; i != 9; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                        }
                        if (error != 15 && error != 11) {
                            if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                                if (inventory[hand].find('W') != string::npos) {
                                    if (structure[i].name != 'S')structure[i].hp -= 1;
                                    else structure[i].hp -= 0.5;
                                }
                                else if (inventory[hand].find('S') != string::npos) {
                                    if (structure[i].name != 'S')structure[i].hp -= 1.5;
                                    else structure[i].hp -= 1;
                                }
                                DeleteToolInventory(break_tool);
                            }
                            else structure[i].hp -= 0.5;
                        }
                        if (structure[i].hp <= 0) {
                            save_time_delay = 0; check_fatigue = false;
                            if (structure[i].name == 'Y') { drop.amount.resize(2); drop.amount[0] = rand() % (5 - 3 + 1) + 3; drop.amount[1] = rand() % (2 - 1 + 1) + 1; drop.name = 'Y'; }
                            else if (structure[i].name == 'y') { drop.amount.resize(2); drop.amount[1] = 1; drop.name = 'y'; }
                            else if (structure[i].name == 'S') { drop.amount.resize(2); drop.amount[0] = rand() % (3 - 2 + 1) + 2; if (rand() % (100 - 1 + 1) + 1 <= 20)drop.amount[1] = rand() % (3 - 1 + 1) + 1; drop.name = 'S'; }
                            else if (structure[i].name == 'C') { break_struct.play(); drop.amount.resize(1); DeleteIndexChest(i); invent_info = 4; drop.amount[0] = 1; drop.name = 'C'; }
                            else if (structure[i].name == 'W') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'W'; }
                            else if (structure[i].name == 'i') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'i'; }
                            world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = ' ';
                            for (int j = 0; j != timedrop.size(); j++) {
                                if (structure.size() - 1 == timedrop[j].loc_in_structure) {
                                    timedrop[j].loc_in_structure = i; break;
                                }
                            }
                            swap(structure[i], structure[structure.size() - 1]);
                            structure.resize(structure.size() - 1);
                            amount_structmass++;
                            break;
                        }
                    }
                    else amount_structmass++;
                }
                if (amount_structmass == structure.size() && structure.size() != 0) {
                    structure.resize(structure.size() + 1);
                    if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'Y') { structure[structure.size() - 1].name = 'Y'; structure[structure.size() - 1].hp = 4; }
                    else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'S') { structure[structure.size() - 1].name = 'S'; structure[structure.size() - 1].hp = 6; }
                    structure[structure.size() - 1].location[0] = loc[0];
                    structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2];
                    structure[structure.size() - 1].location[3] = loc[3] + 1;
                    if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                        if (inventory[hand].find('W') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1;
                            else structure[structure.size() - 1].hp -= 0.5;
                        }
                        else if (inventory[hand].find('S') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1.5;
                            else structure[structure.size() - 1].hp -= 1;
                        }
                        DeleteToolInventory(break_tool);
                    }
                    else structure[structure.size() - 1].hp -= 0.5;
                }
            }
        }
        else {
            if (chek_input[1] == 1 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'Y' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'y' && chek_input[1] == 1)error = 3;
            else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'C' && chek_input[1] == 1)error = 12;
            else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'W' && chek_input[1] == 1)error = 14;
            else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'S' && chek_input[0] == 1)error = 4;
        }
    }
    else if (line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'Y' || line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'y' || line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'S' || line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'C' || line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'W' || line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'i') {
        if (chek_input[0] == 1 || chek_input[1] == 1) {
            if (structure.size() == 0) {
                structure.resize(1);
                if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'Y') {
                    structure[0].name = 'Y';
                    if (inventory[hand].find("топор") != string::npos) {
                        if (inventory[hand].find('W') != string::npos)structure[0].hp = 3;
                        else if (inventory[hand].find('S') != string::npos)structure[0].hp = 2.5;
                        DeleteToolInventory(break_tool);
                    }
                    else structure[0].hp = 3.5;
                }
                else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'S') {
                    structure[0].name = 'S';
                    if (inventory[hand].find('W') != string::npos)structure[0].hp = 5.5;
                    else if (inventory[hand].find('S') != string::npos)structure[0].hp = 5;
                    DeleteToolInventory(break_tool);
                }
                structure[0].location[0] = loc[0]; structure[0].location[1] = loc[1];
                structure[0].location[2] = loc[2]; structure[0].location[3] = loc[3] - 1;
            }
            else {
                for (int i = 0; i != structure.size(); i++) {
                    if (structure[i].location[0] == loc[0] && structure[i].location[1] == loc[1] && structure[i].location[2] == loc[2] && structure[i].location[3] == loc[3] - 1) {
                        if (structure[i].name == 'C') {
                            FindIndexChest(loc);
                            for (int l = 0; l != 12; l++) {
                                if (dropchest[index_input_chestdrop].drop[i].empty() == false) { error = 11; break; }
                            }
                        }
                        else if (structure[i].name == 'W') {
                            if (craft_aftermath != "")error = 15;
                            else if (craft.size() == 4) {
                                for (int i = 0; i != 4; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                            else if (craft.size() == 9) {
                                for (int i = 0; i != 9; i++) {
                                    if (craft[i].empty() == false) { error = 15; break; }
                                }
                            }
                        }
                        if (error != 15 && error != 11) {
                            if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                                if (inventory[hand].find('W') != string::npos) {
                                    if (structure[i].name != 'S')structure[i].hp -= 1;
                                    else structure[i].hp -= 0.5;
                                }
                                else if (inventory[hand].find('S') != string::npos) {
                                    if (structure[i].name != 'S')structure[i].hp -= 1.5;
                                    else structure[i].hp -= 1;
                                }
                                DeleteToolInventory(break_tool);
                            }
                            else structure[i].hp -= 0.5;
                        }
                        if (structure[i].hp <= 0) {
                            save_time_delay = 0; check_fatigue = false;
                            if (structure[i].name == 'Y') { drop.amount.resize(2); drop.amount[0] = rand() % (5 - 3 + 1) + 3; drop.amount[1] = rand() % (2 - 1 + 1) + 1; drop.name = 'Y'; }
                            else if (structure[i].name == 'y') { drop.amount.resize(2); drop.amount[1] = 1; drop.name = 'y'; }
                            else if (structure[i].name == 'S') { drop.amount.resize(2); drop.amount[0] = rand() % (3 - 2 + 1) + 2; if (rand() % (100 - 1 + 1) + 1 <= 20)drop.amount[1] = rand() % (3 - 1 + 1) + 1; drop.name = 'S'; }
                            else if (structure[i].name == 'C') { break_struct.play(); drop.amount.resize(1); DeleteIndexChest(i); invent_info = 4; drop.amount[0] = 1; drop.name = 'C'; }
                            else if (structure[i].name == 'W') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'W'; }
                            else if (structure[i].name == 'i') { break_struct.play(); drop.amount.resize(1); drop.amount[0] = 1; drop.name = 'i'; }
                            world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = ' ';
                            for (int j = 0; j != timedrop.size(); j++) {
                                if (structure.size() - 1 == timedrop[j].loc_in_structure) {
                                    timedrop[j].loc_in_structure = i; break;
                                }
                            }
                            swap(structure[i], structure[structure.size() - 1]);
                            structure.resize(structure.size() - 1);
                            amount_structmass++;
                            break;
                        }
                    }
                    else amount_structmass++;
                }
                if (amount_structmass == structure.size() && structure.size() != 0) {
                    structure.resize(structure.size() + 1);
                    if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'Y') { structure[structure.size() - 1].name = 'Y'; structure[structure.size() - 1].hp = 4; }
                    else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'S') { structure[structure.size() - 1].name = 'S'; structure[structure.size() - 1].hp = 6; }
                    structure[structure.size() - 1].location[0] = loc[0];
                    structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2];
                    structure[structure.size() - 1].location[3] = loc[3] - 1;
                    if (inventory[hand].find("топор") != string::npos || inventory[hand].find("кирка") != string::npos) {
                        if (inventory[hand].find('W') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1;
                            else structure[structure.size() - 1].hp -= 0.5;
                        }
                        else if (inventory[hand].find('S') != string::npos) {
                            if (structure[structure.size() - 1].name != 'S')structure[structure.size() - 1].hp -= 1.5;
                            else structure[structure.size() - 1].hp -= 1;
                        }
                        DeleteToolInventory(break_tool);
                    }
                    else structure[structure.size() - 1].hp -= 0.5;
                }
            }
        }
        else {
            if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'Y' && chek_input[1] == 1 || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'y' && chek_input[1] == 1)error = 3;
            else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'C' && chek_input[1] == 1)error = 12;
            else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'W' && chek_input[1] == 1)error = 14;
            else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'S' && chek_input[0] == 1)error = 4;
        }
    }
    else {
        check_fatigue = false;
        error = 2;
    }
}
//ChopBreak - Рубка/Розбивание/Ломание
void Map(vector<vector<vector<vector<char>>>>& world, int loc[4]) {
    string enter = " ";
    ClearLineScreen();
    for (int i = 0; i != world.size(); i++) {
        for (int f = 0; f != 5; f++) {
            if (f == 0 || f == 4) {
                int map_num = 0;
                for (int l = 0; l != world[i].size(); l++) {
                    if (research_map[i][l] == true && map_num == 0) {
                        cout << "---------"; map_num += 1;
                    }
                    else if(research_map[i][l] == true && map_num != 0)cout << "--------";
                    else cout << "\t";
                }
                cout << endl;
            }
            else {
                for (int l = 0; l != world[i].size(); l++) {
                    for (int j = 0; j != 8; j++) {
                        if (j == 0) {
                            if (l != 0 && research_map[i][l - 1] == true || research_map[i][l] == true)cout << "|";
                            else cout << " ";
                        }
                        else if (l == world[i].size() - 1 && j == 7) {
                            if (research_map[i][l] == true)cout << " |";
                            else cout << " ";
                        }
                        else if (j == 4 && f == 2 && loc[0] == i && loc[1] == l)cout << "0";
                        else cout << " ";
                    }
                }
                cout << endl;
            }
        }
    }
    cin.ignore();
    while (enter.empty() == false) {
        cout << "Введите enter для продолжения игры: "; getline(cin, enter);
    }
    ClearLineScreen();
}
//Map - Мини карта
#pragma endregion
#pragma region MovePlayer
void StepW(vector<vector<vector<vector<char>>>>& world, int loc[4], int& line_of_sight,sf::Sound& record_map) {
    line_of_sight = 1;
    if (loc[2] - 1 != 0) {
        if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == ' ') { swap(world[loc[0]][loc[1]][loc[2] - 1][loc[3]], world[loc[0]][loc[1]][loc[2]][loc[3]]); loc[2] -= 1; }
        else error = 1;
    }
    else {
        if (world[loc[0] - 1][loc[1]][world[loc[0] - 1][loc[1]].size() - 2][loc[3]] != ' ')error = 1;
        else {
            swap(world[loc[0] - 1][loc[1]][world[loc[0] - 1][loc[1]].size() - 2][loc[3]], world[loc[0]][loc[1]][loc[2]][loc[3]]);
            loc[2] = world[loc[0] - 1][loc[1]].size() - 2;
            loc[0] -= 1;
            if (research_map[loc[0]][loc[1]] == false) { 
                record_map.play();
                error = 21;
                research_map[loc[0]][loc[1]] = true;
            }
        }
    }
}
//StepW - Шаг вперед
void StepS(vector<vector<vector<vector<char>>>>& world, int loc[4], int& line_of_sight, sf::Sound& record_map) {
    line_of_sight = 2;
    if (loc[2] + 1 != world[loc[0]][loc[1]].size() - 1) {
        if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == ' ') { swap(world[loc[0]][loc[1]][loc[2] + 1][loc[3]], world[loc[0]][loc[1]][loc[2]][loc[3]]); loc[2] += 1; }
        else error = 1;
    }
    else {
        if (world[loc[0] + 1][loc[1]][1][loc[3]] != ' ')error = 1;
        else {
            swap(world[loc[0] + 1][loc[1]][1][loc[3]], world[loc[0]][loc[1]][loc[2]][loc[3]]);
            loc[0] += 1; loc[2] = 1;
            if (research_map[loc[0]][loc[1]] == false) {
                record_map.play();
                error = 21;
                research_map[loc[0]][loc[1]] = true;
            }
        }
    }
}
//StepS - Шаг назад
void StepD(vector<vector<vector<vector<char>>>>& world, int loc[4], int& line_of_sight, sf::Sound& record_map) {
    line_of_sight = 3;
    if (loc[3] + 1 != world[loc[0]][loc[1]][loc[2]].size() - 1) {
        if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == ' ') { swap(world[loc[0]][loc[1]][loc[2]][loc[3] + 1], world[loc[0]][loc[1]][loc[2]][loc[3]]); loc[3] += 1; }
        else error = 1;
    }
    else {
        if (world[loc[0]][loc[1] + 1][loc[2]][1] != ' ')error = 1;
        else {
            swap(world[loc[0]][loc[1] + 1][loc[2]][1], world[loc[0]][loc[1]][loc[2]][loc[3]]);
            loc[1] += 1; loc[3] = 1;
            if (research_map[loc[0]][loc[1]] == false) {
                record_map.play();
                error = 21;
                research_map[loc[0]][loc[1]] = true;
            }
        }
    }
}
//StepD - Шаг вправо 
void StepA(vector<vector<vector<vector<char>>>>& world, int loc[4], int& line_of_sight, sf::Sound& record_map) {
    line_of_sight = 4;
    if (loc[3] - 1 != 0) {
        if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == ' ') { swap(world[loc[0]][loc[1]][loc[2]][loc[3] - 1], world[loc[0]][loc[1]][loc[2]][loc[3]]); loc[3] -= 1; }
        else error = 1;
    }
    else {
        if (world[loc[0]][loc[1] - 1][loc[2]][world[loc[0]][loc[1]][loc[2]].size() - 2] != ' ')error = 1;
        else {
            swap(world[loc[0]][loc[1] - 1][loc[2]][world[loc[0]][loc[1]][loc[2]].size() - 2], world[loc[0]][loc[1]][loc[2]][loc[3]]);
            loc[3] = world[loc[0]][loc[1]][loc[2]].size() - 2; loc[1] -= 1;
            if (research_map[loc[0]][loc[1]] == false) {
                record_map.play();
                error = 21;
                research_map[loc[0]][loc[1]] = true;
            }
        }
    }
}
//StepA - Шаг влево
#pragma endregion
#pragma region InputInforamation
void InputLineOfSight(int line_of_sight,int loc[4]) {
    if (line_of_sight == 1 && world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == ' ') {
        world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = '^';
    }else if (line_of_sight == 2 && world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == ' ') {
        world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'v';
    }else if (line_of_sight == 3 && world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == ' ') {
        world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = '>';
    }else if (line_of_sight == 4 && world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == ' ') {
        world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = '<';
    }
}
//Вывод стрелочки направления игрока(показывает куда смотрит игрок)
void AmountTorch(int loc[4]) {
    for (int i = 0; i != world[loc[0]][loc[1]].size(); i++) {
        for (int l = 0; l != world[loc[0]][loc[1]][i].size(); l++) {
            if (world[loc[0]][loc[1]][i][l] == 'i') {
                torch.resize(torch.size()+1);
                torch[torch.size()-1].resize(2);
                torch[torch.size() - 1][0] = i;
                torch[torch.size() - 1][1] = l;
            }
        }
    }
}
//AmountTorch - Количество факелов в чанке
void InputWorldConsole(vector<vector<vector<vector<char>>>>& world, int loc[4], int& invent_info, int line_of_sight,sf::Sound& take, sf::Music& day_sound,sf::Music& night_sound) {
    if (time_world >= 1440) { time_world = 0; day++; }
    if (setting_music == 1) {
        if (time_world >= 390 && time_world < 1110 && day_sound.getStatus() == 0) {
            day_sound.play();
            night_sound.stop();
        }
        else if (time_world < 390 && time_world >= 0 && night_sound.getStatus() == 0 || time_world >= 1110 && night_sound.getStatus() == 0) {
            night_sound.play();
            day_sound.stop();
        }
    }
    int cell_output = 0,num_time_line,num_time_column;//num_time_line,num_time_column - Эти переменная отвечает за поиск ячеек котрые игрок может выдеть
    if (time_world >= 1080 && time_world < 1140 || time_world >= 270 && time_world < 390 || time_world >= 1140 && time_world < 1440 || time_world >= 0 && time_world < 270) {
        AmountTorch(loc);
        if (time_world >= 1140 && time_world < 1440 || time_world >= 0 && time_world < 270) {
            num_time_line = 5;
            num_time_column = 9;
        }
        else if (time_world >= 1080 && time_world < 1140 || time_world >= 270 && time_world < 390) {
            num_time_line = 9;
            num_time_column = 17;
        }
    }
    for (int i = 0; i != world[loc[0]][loc[1]].size(); i++) {
        for (int l = 0; l != world[loc[0]][loc[1]][i].size(); l++) {
            if (time_world >= 1080 && time_world < 1140 || time_world >= 270 && time_world < 390 || time_world >= 1140 && time_world < 1440 || time_world >= 0 && time_world < 270 ) {
                if (i == 0 || i == world[0][0].size() - 1 || l == 0 || l == world[0][0][0].size() - 1)cell_output = 1;
                else {
                    for (int j = num_time_line; j > 0; j--) {
                        for (int f = num_time_column; f > 0; f--) {
                            if (j > num_time_line / 2 + 1) {
                                if (f > num_time_column / 2 + 1 && loc[2] == i - (j - (num_time_line / 2 + 1)) && loc[3] == l - (f - (num_time_column  / 2 + 1))) { cell_output = 1; j == 0; break; }
                                else if (f <= num_time_column / 2 + 1 && loc[2] == i - (j -( num_time_line / 2 + 1)) && loc[3] == l + f - 1) { cell_output = 1; j == 0; break; }
                            }
                            else if (j <= num_time_line / 2 + 1) {
                                if (f > num_time_column / 2 + 1 && loc[2] == i + j - 1 && loc[3] == l - (f - (num_time_column / 2 + 1))) { cell_output = 1; j == 0; break; }
                                else if (f <= num_time_column / 2 + 1 && loc[2] == i + j - 1 && loc[3] == l + f - 1) { cell_output = 1; j == 0; break; }
                            }
                        }
                    }
                }
                if (torch.size() != 0 && cell_output == 0) {
                    for (int j = 0; j < torch.size(); j++) {
                        for (int f = num_time_line+2; f > 0; f--) {
                            for (int k = num_time_column + 2; k > 0; k--) {
                                if (f > (num_time_line + 2) / 2 + 1) {
                                    if (k > (num_time_column + 2) / 2 + 1 && torch[j][0] == i - (f - ((num_time_line + 2) / 2 + 1)) && torch[j][1] == l - (k - ((num_time_column + 2) / 2 + 1))) { cell_output = 1; f = 0; j = torch.size(); break; }
                                    else if (k <= (num_time_column + 2) / 2 + 1 && torch[j][0] == i - (f - 4) && torch[j][1] == l + k - 1) { cell_output = 1; f = 0; j = torch.size(); break; }
                                }
                                else if (f <= (num_time_line + 2) / 2 + 1) {
                                    if (k > (num_time_column + 2) / 2 + 1 && torch[j][0] == i + f - 1 && torch[j][1] == l - (k - ((num_time_column + 2) / 2 + 1))) { cell_output = 1; f = 0; j = torch.size(); break; }
                                    else if (k <= (num_time_column + 2) / 2 + 1 && torch[j][0] == i + f - 1 && torch[j][1] == l + k - 1) { cell_output = 1; f = 0; j = torch.size(); break; }
                                }
                            }
                        }
                    }
                }
                if(cell_output == 0) cout << ' ';
                else cout << world[loc[0]][loc[1]][i][l];
                cell_output = 0;
            }else cout << world[loc[0]][loc[1]][i][l];
        }
        if (i == world[loc[0]][loc[1]].size() - 1) {
            cout << "  Время: ";
            cout << day << "-ый день/";
            if (int(time_world / 60) < 10)cout << '0' << int(time_world / 60) << ':';
            else cout << int(time_world / 60) << ':';
            if(int(time_world) % 60 < 10)cout << '0' << int(time_world) % 60;
            else cout << int(time_world) % 60;
            if (time_world >= 270 && time_world < 390)cout << "(рассвет)";
            else if(time_world >= 390 && time_world < 720)cout << "(утро)";
            else if (time_world >= 720 && time_world < 1080)cout << "(день)";
            else if (time_world >= 1080 && time_world < 1140)cout << "(закат)";
            else if (time_world >= 1140 && time_world < 1440)cout << "(вечер)";
            else if (time_world >= 0 || time_world < 270)cout << "(ночь)";
        }
        if (i == 1 && error == 1)cout << "    Вы не можете пройти туда,там есть препятствие!";
        else if (i == 1 && error == 2)cout << "    Вы не повернуты к структуре!";
        else if (i == 1 && error == 3)cout << "    Вы не можете разбить дерево!";
        else if (i == 1 && error == 4)cout << "    Вы не можете рубить камень!";
        else if (i == 1 && error == 5)cout << "    Вам нечего рубить!";
        else if (i == 1 && error == 6)cout << "     Вам нечего разбивать!";
        else if (i == 1 && error == 7)cout << "     Нету такого выбора!";
        else if (i == 1 && error == 8)cout << "    Вам не с чем взаимодействовать!";
        else if (i == 1 && error == 9)cout << "    Вам нечего посадить/поставить!";
        else if (i == 1 && error == 10)cout << "    Вы не можете посадить/поставить предмет, там есть препятствие!";
        else if (i == 1 && error == 11)cout << "    Вы не можете ломать сундук, в нем есть предметы!";
        else if (i == 1 && error == 12)cout << "    Вы не можете разбить сундук!";
        else if (i == 1 && error == 13)cout << "    Прежде чем крафтить, освободите инвентарь!";
        else if (i == 1 && error == 14)cout << "    Вы не можете разбить верстак!";
        else if (i == 1 && error == 15)cout << "    Прежде чем ломать верстак, освободите инвентарь!";
        else if (i == 1 && error == 17)cout << "    Вы не можете рубить эту структуру киркой!";
        else if (i == 1 && error == 18)cout << "    Вы не можете разбить эту структуру топором!";
        else if (i == 1 && error == 19)cout << "    Вы не можете разбить эту структуру рукой!";
        else if (i == 1 && error == 21)cout << "    На карте зделана заметка!";
        else if (drop.name == 'Y') {
            if (i == 1)cout << "    Добыто x" << drop.amount[0] << " дерева!";
            if (i == 2) { cout << "    Добыто x" << drop.amount[1] << " саженца!"; InputDropInventory(invent_info, loc, line_of_sight, structure, timedrop, take); }
        }
        else if (i == 1 && drop.name == 'y') { cout << "    Добыто x" << drop.amount[1] << " саженца!"; InputDropInventory(invent_info, loc, line_of_sight, structure, timedrop, take); }
        else if (drop.name == 'S') {
            if (i == 1)cout << "    Добыто x" << drop.amount[0] << " камня!";
            else if (i == 2) {
                if (drop.amount.size() > 1 && drop.amount[1] != 0) cout << "    Добыто x" << drop.amount[1] << " угля!";
                InputDropInventory(invent_info, loc, line_of_sight, structure, timedrop, take);
            }
        }
        else if (i == 1 && drop.name == 'С') { cout << "    Добыто x1 сундук!"; InputDropInventory(invent_info, loc, line_of_sight, structure, timedrop, take); }
        else if (i == 1 && drop.name == 'W') { cout << "    Добыто x1 верстак!"; InputDropInventory(invent_info, loc, line_of_sight, structure, timedrop, take); }
        else if (i == 1 && drop.name == 'i') { cout << "    Добыто x1 факел!"; InputDropInventory(invent_info, loc, line_of_sight, structure, timedrop, take); }
        if (i == 2 && error == 16)cout << "    Инструмент сломалься!";
        if (i == 3 && error == 20) {
            cout.precision(1);
            cout << "    Вы устали! Попытайтесь попробывать снова через " << save_time_delay << " секуд";
        }
        cout << endl;
    }
    if (error != 7)error = 0;
    drop.name = ' ';
    //cin >> error;
    invent_info = 0; torch.resize(0);
}
//InputWorldConsole - Вывод мира в консоль
void InputConsoleInvent() {
    int amount_dash = 0; int index[4], num = 0;
    cout << "\t\t\t   Инвентарь";
    for (int i = 0; i != 4; i++) {
        if (inventory[i].empty() == false || inventory[i + 4].empty() == false || inventory[i + 8].empty() == false || inventory[i + 12].empty() == false || inventory[i + 16].empty() == false) {
            if (inventory[i].size() >= inventory[i + 4].size() && inventory[i].size() >= inventory[i + 8].size() && inventory[i].size() >= inventory[i + 12].size() && inventory[i].size() >= inventory[i + 16].size())index[i] = i;
            else  if (inventory[i + 4].size() >= inventory[i].size() && inventory[i + 4].size() >= inventory[i + 8].size() && inventory[i + 4].size() >= inventory[i + 12].size() && inventory[i + 4].size() >= inventory[i + 16].size())index[i] = i + 4;
            else  if (inventory[i + 8].size() >= inventory[i + 4].size() && inventory[i + 8].size() >= inventory[i + 4].size() && inventory[i + 8].size() >= inventory[i + 12].size() && inventory[i + 8].size() >= inventory[i + 16].size())index[i] = i + 8;
            else  if (inventory[i + 12].size() >= inventory[i].size() && inventory[i + 12].size() >= inventory[i + 4].size() && inventory[i + 12].size() >= inventory[i + 8].size() && inventory[i + 12].size() >= inventory[i + 16].size())index[i] = i + 12;
            else  if (inventory[i + 16].size() >= inventory[i].size() && inventory[i + 16].size() >= inventory[i + 8].size() && inventory[i + 16].size() >= inventory[i + 12].size() && inventory[i + 16].size() >= inventory[i + 4].size())index[i] = i + 16;
            else  if (inventory[i + 16].size() == inventory[i].size() && inventory[i + 16].size() == inventory[i + 8].size() && inventory[i + 16].size() == inventory[i + 12].size() && inventory[i + 16].size() == inventory[i + 4].size())index[i] = i;
        }
        else if (inventory[i].empty() && inventory[i + 4].empty() && inventory[i + 8].empty() && inventory[i + 12].empty() && inventory[i + 16].empty())index[i] = 20;
    }
    for (int i = 0; i <= 20; i++) {
        if (i % 4 == 0) {
            num = 0;
            cout << endl;
            for (int l = 0; l != 4; l++) {
                cout << "-----";
                if (index[l] != 20) {
                    for (int j = 0; j != inventory[index[l]].size() + 3; j++)cout << "-";
                }
                else {
                    for (int j = 0; j != 10; j++)cout << "-";
                }
            }
            if (i < 20)cout << "\n|";
        }
        if (i < 20) {
            if (inventory[i].empty() == false) {
                if (hand == i)cout << "\033[1m\033[4m" << i + 1 << ": " << inventory[i];
                else cout << i + 1 << ": " << inventory[i];
                if (i <= 8)cout << " ";
                if (inventory[index[num]].size() > inventory[i].size()) {
                    for (int l = inventory[i].size(); l != inventory[index[num]].size(); l++)cout << " ";
                }
                if (hand == i)cout << "\033[0m";
                cout << "  | ";
            }
            else if (inventory[i].empty()) {
                if (hand == i)cout << "\033[1m\033[4m" << i + 1 << ": ";
                else cout << i + 1 << ": ";
                if (i <= 8)cout << " ";
                if (index[num] != 20) {
                    for (int l = 0; l != inventory[index[num]].size()+1; l++)cout << " ";
                    if (hand == i)cout << "\033[0m";
                    cout << " | ";
                }
                else {
                    for (int j = 0; j != 8; j++)cout << " ";
                    if (hand == i)cout << "\033[0m";
                    cout << " | ";
                }
            }
        }
        if (i == 3 && error_input == 1)cout << "  Нету такого выбора!";
        else if (i == 3 && error_input == 2) {
            cout << "    Вы ввели номера ячеек которые выходитят за рамки инвентаря!";
        }
        else if (i == 3 && error_input == 3) {
            cout << "    Вы ввели номер перемещенной ячейки которая выходит за рамки инвентаря!";
        }
        else if (i == 3 && error_input == 4) {
            cout << "    Вы ввели номер ячейки(в которую переносят) выходит за рамки инвентаря!";
        }
        else if (i == 3 && error_input == 5) {
            cout << "    В ячейках размещены разные предметы!";
        }
        else if (i == 3 && error_input == 6) {
            cout << "    В ячейке, в которую переносят максимальное количество предметов!";
        }
        else if (i == 3 && error_input == 7) {
            cout << "    Вы ввели одну и туже ячейку!";
        }
        else if (i == 3 && error_input == 8) {
            cout << "    Введенная ячейка выходит за рамки инвентаря!";
        }
        else if (i == 3 && error_input == 9) {
            cout << "    Вы не можете выкинуть пустоту!";
        }
        else if (i == 3 && error_input == 10) {
            cout << "    Вы не можете выкинуть туда предмет(-ы), там есть препятствие!";
        }
        else if (i == 3 && error_input == 12) {
            cout << "    Вы не можете выкинуть туда предмет(-ы), временный сундук переполнен!";
        }
        else if (i == 3 && ejectitem[1].empty() == false && ejectitem[2] == "1" || i == 3 && ejectitem[1].empty() == false && ejectitem[2] == "2") {
            if (ejectitem[2] != "2")ejectitem[2] = "";
            else swap(ejectitem[1][0], ejectitem[0][0]);
            cout << "    " << ejectitem[1]; ejectitem[1] = "";
        }
        else if (i == 7 && ejectitem[0].empty() == false && ejectitem[2] == "2") {
            cout << "    " << ejectitem[0]; ejectitem[0] = ""; ejectitem[1] = ""; ejectitem[2] = "";
        }
        else if (i == 3 && error_input == 13) {
            cout << "    Вы не можете переместить предметы с временного сундука, предметы разные!";
        }
        else if (i == 3 && error_input == 14) {
            cout << "    Вы не можете переместить еденицу пустой ячейки в  инентарь!";
        }
        else if (i == 3 && error_input == 15) {
            cout << "    Вы ввели номера ячеек которые выходитят за рамки инвентаря и крафта!";
        }
        else if (i == 3 && error_input == 16) {
            cout << "    Вы ввели номер ячейки которая выходит за рамки крафта!";
        }
        else if (i == 3 && error_input == 17) {
            cout << "    Вы ввели номер ячейки которая выходит за рамки инвентаря!";
        }
        else if (i == 3 && error_input == 18) {
            cout << "    Вы не можете добавить пустую ячейку!";
        }
        else if (i == 3 && error_input == 19) {
            cout << "    В ячейке, в которую добавляют максимальное количество предметов!";
        }
        else if (i == 3 && error_input == 20) {
            cout << "    В ячейке, в которую убирают максимальное количество предметов!";
        }
        else if (i == 3 && error_input == 21) {
            cout << "    Прежде чем крафтить, освободите инвентарь!";
        }
        else if (i == 3 && error_input == 22) {
            cout << "    Вы не можете добавить пустоту!";
        }
        else if (i == 3 && error_input == 23) {
            cout << "    Вы не можете убрать пустоту!";
        }
        else if (i == 3 && error_input == 24) {
            cout << "    В ячейках размещены не стакающиеся предметы!";
        }
        else if (i == 3 && error_input == 25) {
            cout.precision(1);
            cout << "    Вы устали! Попытайтесь попробывать снова через "<< save_time_delay<<" секуд";
        }
        num++;
    }
}
//InputConsoleInvent - Вывод инвентаря в консоль
void InputConsoleDropChest() {
    cout << "\t\t\t   Сундук";
    int index[4], num = 0;;
    for (int i = 0; i != 4; i++) {
        if (dropchest[index_input_chestdrop].drop[i].empty() == false || dropchest[index_input_chestdrop].drop[i + 4].empty() == false || dropchest[index_input_chestdrop].drop[i + 8].empty() == false) {
            if (dropchest[index_input_chestdrop].drop[i].size() >= dropchest[index_input_chestdrop].drop[i + 4].size() && dropchest[index_input_chestdrop].drop[i].size() >= dropchest[index_input_chestdrop].drop[i + 8].size())index[i] = i;
            else if (dropchest[index_input_chestdrop].drop[i + 4].size() >= dropchest[index_input_chestdrop].drop[i].size() && dropchest[index_input_chestdrop].drop[i + 4].size() >= dropchest[index_input_chestdrop].drop[i + 8].size())index[i] = i + 4;
            else if (dropchest[index_input_chestdrop].drop[i + 8].size() >= dropchest[index_input_chestdrop].drop[i].size() && dropchest[index_input_chestdrop].drop[i + 8].size() >= dropchest[index_input_chestdrop].drop[i + 4].size())index[i] = i + 8;
        }
        else if (dropchest[index_input_chestdrop].drop[i].empty() && dropchest[index_input_chestdrop].drop[i + 4].empty() && dropchest[index_input_chestdrop].drop[i + 8].empty())index[i] = 12;
    }
    for (int i = 0; i <= 12; i++) {
        if (i % 4 == 0) {
            num = 0;
            cout << endl;
            for (int l = 0; l != 4; l++) {
                cout << "-----";
                if (index[l] != 12) {
                    for (int j = 0; j != dropchest[index_input_chestdrop].drop[index[l]].size()+2; j++)cout << "-";
                }
                else {
                    for (int j = 0; j != 10; j++)cout << "-";
                }
            }
            if (i < 12)cout << "\n|";
        }
        if (i < 12) {
            if (dropchest[index_input_chestdrop].drop[i].empty() == false) {
                cout << i + 1 << ": " << dropchest[index_input_chestdrop].drop[i];
                if (i <= 8 )cout << " ";
                for (int l = dropchest[index_input_chestdrop].drop[i].size(); l != dropchest[index_input_chestdrop].drop[index[num]].size(); l++)cout << " ";
                cout << " | ";
            }
            else if (dropchest[index_input_chestdrop].drop[i].empty()) {
                cout << i + 1 << ": ";
                if (i <= 8 )cout << " ";
                if (index[num] != 12) {
                    for (int l = 0; l != dropchest[index_input_chestdrop].drop[index[num]].size(); l++)cout << " ";
                    cout << " | ";
                }
                else {
                    for (int j = 0; j != 8; j++)cout << " ";
                    cout << " | ";
                }
            }
        }
        if (i == 3 && ejectitem[0].empty() == false && ejectitem[2] == "1" || i == 3 && ejectitem[0].empty() == false && ejectitem[2] == "2") {
            cout << "    " << ejectitem[0];
            if (ejectitem[2] != "2")ejectitem[0] = "";
        }
        else if (i == 7 && ejectitem[1].empty() == false && ejectitem[2] == "2") {
            cout << "    " << ejectitem[1];
        }
        num++;
    }
    cout << endl;
}
//InputConsoleDropChest - Вывод сундука в консоль
void InputConsoleCraftTable() {
    if (input_chest == 0) {
        cout << "\t   Крафт";
        int index[2], num;
        for (int i = 0; i != 2; i++) {
            if (craft[i].empty() == false || craft[i + 2].empty() == false) {
                if (craft[i].size() > craft[i + 2].size())index[i] = i;
                else if (craft[i].size() < craft[i + 2].size())index[i] = i + 2;
                else if (craft[i].size() == craft[i + 2].size())index[i] = i;
            }
            else if (craft[i].empty() && craft[i + 2].empty())index[i] = 4;
        }
        for (int i = 0; i <= 4; i++) {
            if (i % 2 == 0) {
                num = 0;
                cout << endl;
                for (int l = 0; l != 2; l++) {
                    cout << "----";
                    if (index[l] != 4) {
                        for (int j = 0; j != craft[index[l]].size()+2; j++)cout << "-";
                    }
                    else {
                        for (int j = 0; j != 10; j++)cout << "-";
                    }
                }
                if (i == 2) {
                    cout << "     ----->  ";
                    if (craft_aftermath_input != "")cout << craft_aftermath_input;
                }
                if (i < 4)cout << "\n|";
            }
            if (i < 4) {
                if (craft[i].empty() == false) {
                    cout << i + 1 << ": " << craft[i];
                    if (craft[index[num]] != craft[i]) {
                        for (int l = craft[i].size(); l != craft[index[num]].size(); l++)cout << " ";
                    }
                    cout << " | ";
                }
                else if (craft[i].empty()) {
                    cout << i + 1 << ": ";
                    if (index[num] != 4) {
                        for (int l = 0; l != craft[index[num]].size(); l++)cout << " ";
                        cout << " | ";
                    }
                    else {
                        for (int j = 0; j != 8; j++)cout << " ";
                        cout << " | ";
                    }
                }
            }
            num++;
        }
    }
    else {
        cout << "\t\tВерстак";
        int index[3], num = 0;
        for (int i = 0; i != 3; i++) {
            if (craft[i].empty() == false || craft[i + 3].empty() == false || craft[i + 6].empty() == false) {
                if (craft[i].size() > craft[i + 3].size() && craft[i].size() > craft[i + 6].size())index[i] = i;
                else if (craft[i].size() < craft[i + 3].size() && craft[i + 6].size() < craft[i + 3].size())index[i] = i + 3;
                else if (craft[i + 6].size() > craft[i].size() && craft[i + 6].size() > craft[i + 3].size())index[i] = i + 6;
                else if (craft[i].size() == craft[i + 3].size() && craft[i + 6].size() < craft[i + 3].size())index[i] = i;
                else if (craft[i].size() == craft[i + 6].size() && craft[i + 6].size() > craft[i + 3].size())index[i] = i;
                else if (craft[i + 3].size() == craft[i + 6].size() && craft[i + 6].size() > craft[i].size())index[i] = i + 3;
                else if (craft[i + 3].size() == craft[i + 6].size() && craft[i + 6].size() == craft[i].size())index[i] = i;
            }
            else if (craft[i].empty() && craft[i + 3].empty() && craft[i + 6].empty())index[i] = 10;
        }
        for (int i = 0; i <= 9; i++) {
            if (i % 3 == 0) {
                num = 0;
                cout << endl;
                for (int l = 0; l != 3; l++) {
                    cout << "----";
                    if (index[l] != 10) {
                        for (int j = 0; j != craft[index[l]].size() + 2; j++)cout << "-";
                    }
                    else {
                        for (int j = 0; j != 10; j++)cout << "-";
                    }
                }
                if (i < 9)cout << "\n|";
            }
            if (i < 9) {
                if (craft[i].empty() == false) {
                    cout << i + 1 << ": " << craft[i];
                    if (craft[index[num]] != craft[i]) {
                        for (int l = craft[i].size(); l != craft[index[num]].size(); l++)cout << " ";
                    }
                    cout << " | ";
                }
                else if (craft[i].empty()) {
                    cout << i + 1 << ": ";
                    if (index[num] != 10) {
                        for (int l = 0; l != craft[index[num]].size(); l++)cout << " ";
                        cout << " | ";
                    }
                    else {
                        for (int j = 0; j != 8; j++)cout << " ";
                        cout << " | ";
                    }
                }
            }
            if (i == 5) {
                cout << "     ----->  ";
                if (craft_aftermath_input != "")cout << craft_aftermath_input;
            }
            num++;
        }
    }
    cout << endl;
}
//InputConsoleCraftTable - Вывод крафта в консоль
void InputConsoleTimeDrop(int loc[4]) {
    cout << "\t\t\t Временный сундук";
    int num = -1;
    for (int i = 0; i != timedrop[index_input_tempdrop].tempdrop.size(); i++) {
        if (i % 4 == 0) {
            cout << endl;
            for (int l = 0; l != 4; l++) {
                num++;
                for (int j = 0; j != timedrop[index_input_tempdrop].tempdrop[l + i].size() + 2; j++)cout << '-';
                if ((i + l) % 4 == 0)cout << "----";
                else cout << "---";
                if (timedrop[index_input_tempdrop].tempdrop.size() - 1 == l + i)break;
            }
        }
        if (i % 4 == 0 && timedrop[index_input_tempdrop].tempdrop.size() > 3)cout << "\n|";
        else if (timedrop[index_input_tempdrop].tempdrop.size() <= 3 && i == 0)cout << "\n|";
        cout << i + 1 << ": " << timedrop[index_input_tempdrop].tempdrop[i] << " |";
        if ((i + 1) % 4 == 0 && i != 0 || timedrop[index_input_tempdrop].tempdrop.size() - 1 == i) {
            cout << endl;
            for (int l = i - num; l <= i; l++) {
                for (int j = 0; j != timedrop[index_input_tempdrop].tempdrop[l].size() + 2; j++)cout << '-';
                if (i <= 3 && (l - num) % 4 == 0  || i > 3 && l % 4 == 0)cout << "----";
                else cout << "---";
                if (timedrop[index_input_tempdrop].tempdrop.size() - 1 == l)break;
            }
            if ((i + 1) % 4 != 0)cout << endl;
            else num = -1;
        }
    }
    cout << "\n\n";
}
//InputConsoleTimeDrop - Вывод временного сундука в консоль
#pragma endregion
#pragma region CraftTable
void CraftAftermathDelete() {
    empty_cell = 0; string temp_text[4] = { "","","","" };
    for (int i = craft_aftermath.find('x') + 1; i != craft_aftermath.size(); i++) {
        if (i > craft_aftermath.find(' '))temp_text[0] += craft_aftermath[i];
        else if (i < craft_aftermath.find(' '))temp_text[2] += craft_aftermath[i];
    }
    if (craft_aftermath.find("топор") == string::npos && craft_aftermath.find("кирка") == string::npos) {
        for (int i = 0; i != 20; i++) {
            if (inventory[i].empty() == false && inventory[i].find("топор") == string::npos && inventory[i].empty() == false && inventory[i].find("кирка") == string::npos) {
                for (int l = inventory[i].find('x') + 1; l != inventory[i].size(); l++) {
                    if (l > inventory[i].find(' '))temp_text[1] += inventory[i][l];
                    else if (l < inventory[i].find(' '))temp_text[3] += inventory[i][l];
                }
                if (temp_text[0] == temp_text[1]) {
                    if (stoi(temp_text[3]) != 12) {
                        if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                            craft_aftermath = "";
                            inventory[i] = "x12 " + temp_text[0];
                            temp_text[2] = ""; break;
                        }
                        else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                            temp_text[2] = std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3])));
                            craft_aftermath = "x" + temp_text[2] + " " + temp_text[0];
                            inventory[i] = "x12 " + temp_text[1];
                        }
                        else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                            craft_aftermath = "";
                            inventory[i] = "x" + std::to_string(stoi(temp_text[3]) + stoi(temp_text[2])) + " " + temp_text[0];
                            temp_text[2] = ""; break;
                        }
                    }
                }
                temp_text[1] = ""; temp_text[3] = "";
            }
            else empty_cell++;
        }
    }else empty_cell++;
    if (empty_cell != 0 && temp_text[2] != "") {
        for (int i = 0; i != 20; i++) {
            if (inventory[i].empty()) {
                if (stoi(temp_text[2]) > 12) {
                    inventory[i] = "x12 " + temp_text[0];
                    temp_text[2] = std::to_string(stoi(temp_text[2]) - 12);
                    craft_aftermath = "x" + temp_text[2] + " " + temp_text[0];
                }
                else if (stoi(temp_text[2]) <= 12) {
                    if (craft_aftermath.find("топор") != string::npos || craft_aftermath.find("кирка") != string::npos) {
                        if (stoi(temp_text[2]) != 0) {
                            temp_text[2] = std::to_string(stoi(temp_text[2]) - 1);
                            if (stoi(temp_text[2]) != 0)inventory[i] = "x" + temp_text[2] + " " + temp_text[0];
                            else inventory[i] = "x" + std::to_string(stoi(temp_text[2]) + 1) + " " + temp_text[0];
                        }
                        else {
                            craft_aftermath = ""; break;
                        }
                    }
                    else {
                        craft_aftermath = "";
                        inventory[i] = "x" + temp_text[2] + " " + temp_text[0];
                    }
                }
            }
            if (craft_aftermath == "")break;
        }
    }
}
//CraftAftermathDelete - Функция которая добавляет скрафченные предмет в инвентарь
bool craft_turn = false;//Переменная котрая дает возможность скрафтить предмет 
void ItemCraftAdd() {
    empty_cell = 0; string temp_text[4] = { "","","","" };
    for (int i = 0; i != craft.size(); i++) {
        if (craft[i].empty() == false) {
            for (int l = craft[i].find('x') + 1; l != craft[i].size(); l++) {
                if (l > craft[i].find(' '))temp_text[0] += craft[i][l];
                else if (l < craft[i].find(' '))temp_text[2] += craft[i][l];
            }
            for (int l = 0; l != 20; l++) {
                if (inventory[l].empty() == false) {
                    if (inventory[l].find('-') + 1 == inventory[l].find("топор") || inventory[l].find('-') + 1 == inventory[l].find("кирка")) {
                        empty_cell = 1; break;
                    }
                    for (int j = inventory[l].find('x') + 1; j != inventory[l].size(); j++) {
                        if (j > inventory[l].find(' '))temp_text[1] += inventory[l][j];
                        else if (j < inventory[l].find(' '))temp_text[3] += inventory[l][j];
                    }
                    if (temp_text[0] == temp_text[1]) {
                        if (stoi(temp_text[3]) != 12) {
                            if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                                craft[i] = "";
                                inventory[l] = "x12 " + temp_text[1];
                                temp_text[2] = ""; break;
                            }
                            else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                                temp_text[2] = std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3])));
                                craft[i] = "x" + temp_text[2] + " " + temp_text[0];
                                inventory[l] = "x12 " + temp_text[1];
                            }
                            else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                                craft[i] = "";
                                inventory[l] = "x" + std::to_string(stoi(temp_text[3]) + stoi(temp_text[2])) + " " + temp_text[0];
                                temp_text[2] = ""; break;
                            }
                        }
                    }
                    temp_text[1] = ""; temp_text[3] = "";
                }
                else empty_cell++;
            }
            if (empty_cell != 0 && temp_text[2] != "") {
                for (int l = 0; l != 20; l++) {
                    if (inventory[l].empty()) {
                        craft[i] = "";
                        empty_cell = 0;
                        inventory[l] = "x" + temp_text[2] + " " + temp_text[0]; break;
                    }
                }
            }
        }
        temp_text[0] = ""; temp_text[1] = ""; temp_text[2] = ""; temp_text[3] = "";
    }
    empty_cell = 0;
    for (int i = 0; i != craft.size(); i++) {
        if (craft[i].empty() == false)empty_cell++;
    }
    if (empty_cell >= 1) { empty_cell = 0; error_input = 21; }
    else error_input = 0;
}
//ItemCraftAdd - Функция которая при выходе из крафта добавляет оставшиеся предметы в крафте
void CraftingRecipes(char choicecraft) {
    int chek_amountitem = 0, index = 4;
    vector<string> temp_num; vector<string>temp_text; vector<int> num;
    temp_num.resize(4); temp_text.resize(4); num.resize(4);
    if (input_chest != 0) { temp_num.resize(9); temp_text.resize(9); num.resize(9); index = 9; }
    for (int i = 0; i != index; i++) {
        if (craft[i].empty() == false) {
            for (int l = craft[i].find(' ') + 1; l != craft[i].size(); l++) { temp_text[chek_amountitem] += craft[i][l]; }
            for (int l = craft[i].find('x') + 1; l != craft[i].find(' '); l++) { temp_num[chek_amountitem] += craft[i][l]; }
            num[chek_amountitem] = i;
            chek_amountitem += 1;
        }
    }
    if (temp_text[0] == "дерева" && chek_amountitem == 1) {
        craft_turn = true;
        craft_aftermath_input = "x" + std::to_string(stoi(temp_num[0]) * 2) + " доски";
        if (choicecraft == 'c' || choicecraft == 'C') {
            for (int i = 0; i != index; i++)craft[i] = "";
            craft_aftermath = "x" + std::to_string(stoi(temp_num[0]) * 2) + " доски";
            craft_aftermath_input = "";
            craft_turn = false;
        }
    }
    else if (temp_text[0] == "уголь" && temp_text[1] == "палки" &&  chek_amountitem == 2 ) {
        if (num[1] - 2 == num[0] && craft.size() == 4 || num[1] - 3 == num[0] && craft.size() == 9) {
            craft_turn = true;
            if (stoi(temp_num[0]) > stoi(temp_num[1]))craft_aftermath_input = "x" + temp_num[0] + " факел";
            else if (stoi(temp_num[1]) == stoi(temp_num[0]))craft_aftermath_input = "x" + temp_num[0] + " факел";
            else craft_aftermath_input = "x" + temp_num[1] + " факел";
            if (choicecraft == 'c' || choicecraft == 'C') {
                if (stoi(temp_num[0]) > stoi(temp_num[1])) {
                    craft[num[0]] = "x" + std::to_string(stoi(temp_num[0]) - stoi(temp_num[1])) + " " + temp_text[0];
                    craft[num[1]] = "";
                    craft_aftermath = "x" + std::to_string(stoi(temp_num[1]) * 2) + " факел";
                }
                else if (stoi(temp_num[0]) < stoi(temp_num[1])) {
                    craft[num[1]] = "x" + std::to_string(stoi(temp_num[1]) - stoi(temp_num[0])) + " " + temp_text[1];
                    craft[num[0]] = "";
                    craft_aftermath = "x" + std::to_string(stoi(temp_num[0]) * 2) + " факел";
                }
                else if (stoi(temp_num[0]) == stoi(temp_num[1])) { 
                    craft[num[0]] = "";
                    craft[num[1]] = "";
                    craft_aftermath = "x" + std::to_string(stoi(temp_num[0]) * 2) + " факел";
                }
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
    }
    else if (temp_text[0] == "доски" && temp_text[1] == "доски" && chek_amountitem == 2) {
        if (num[1] - 2 == num[0] && craft.size() == 4 || num[1] - 3 == num[0] && craft.size() == 9) {
            craft_turn = true;
            if (stoi(temp_num[0]) < stoi(temp_num[1])) {
                craft_aftermath_input = "x" + std::to_string(stoi(temp_num[0]) * 2) + " палки";
                if (choicecraft == 'c' || choicecraft == 'C') {
                    craft[num[1]] = "x" + std::to_string(stoi(temp_num[1]) - stoi(temp_num[0])) + " " + temp_text[0]; craft[num[0]] = "";
                    craft_aftermath = "x" + std::to_string(stoi(temp_num[0]) * 2) + " палки";
                    craft_aftermath_input = "";
                    craft_turn = false;
                }
            }
            else if (stoi(temp_num[0]) > stoi(temp_num[1])) {
                craft_aftermath_input = "x" + std::to_string(stoi(temp_num[1]) * 2) + " палки";
                if (choicecraft == 'c' || choicecraft == 'C') {
                    craft[num[0]] = "x" + std::to_string(stoi(temp_num[0]) - stoi(temp_num[1])) + " " + temp_text[0]; craft[num[1]] = "";
                    craft_aftermath = "x" + std::to_string(stoi(temp_num[1]) * 2) + " палки";
                    craft_aftermath_input = "";
                    craft_turn = false;
                }
            }
            else if (stoi(temp_num[0]) == stoi(temp_num[1])) {
                craft_aftermath_input = "x" + std::to_string(stoi(temp_num[1]) * 2) + " палки";
                if (choicecraft == 'c' || choicecraft == 'C') {
                    craft[num[0]] = ""; craft[num[1]] = "";
                    craft_aftermath = "x" + std::to_string(stoi(temp_num[1]) * 2) + " палки";
                }
            }
        }
        else { craft_aftermath_input = ""; craft_turn = false; }
    }
    else if (temp_text[0] == "доски" && temp_text[1] == "доски" && temp_text[2] == "доски" && temp_text[3] == "доски" && chek_amountitem == 4) {
        int min = 0;
        if (craft.size() == 4) {
            if (num[1] - 1 == num[0] && num[0] == num[2] - 2 && num[1] == num[3] - 2 && num[2] == num[3] - 1)min = 1;
        }
        else {
            if (num[1] - 1 == num[0] && num[0] == num[2] - 3 && num[1] == num[3] - 3 && num[2] == num[3] - 1)min = 1;
        }
        if (min == 1) {
            min = 0;
            craft_turn = true;
            for (int i = 0; i != index; i++) {
                if (temp_num[i].empty() == false) {
                    if (stoi(temp_num[min]) > stoi(temp_num[i]))min = i;
                }
            }
            craft_aftermath_input = "x" + temp_num[min] + " верстак";
            if (choicecraft == 'c' || choicecraft == 'C') {
                for (int i = 0; i != index; i++) {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[i]) > stoi(temp_num[min]))craft[i] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min])) + " " + temp_text[0];
                        else if (stoi(temp_num[i]) == stoi(temp_num[min]))craft[num[i]] = "";
                    }
                }
                craft_aftermath = "x" + temp_num[min] + " верстак";
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
        else { craft_aftermath_input = ""; craft_turn = false; }
    }
    else if (craft.size() == 9) {
        if (temp_text[0] == "доски" && temp_text[1] == "доски" && temp_text[2] == "доски" && temp_text[3] == "доски" && temp_text[4] == "доски" && temp_text[5] == "доски" && temp_text[6] == "доски" && temp_text[7] == "доски" && chek_amountitem == 8 && num[3] != 4) {
            int min = 0;
            craft_turn = true;
            for (int i = 0; i != index; i++) {
                if (temp_num[i].empty() == false) {
                    if (stoi(temp_num[min]) > stoi(temp_num[i]))min = i;
                }
            }
            craft_aftermath_input = "x" + temp_num[min] + " сундук";
            if (choicecraft == 'c' || choicecraft == 'C') {
                for (int i = 0; i != index; i++) {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[i]) > stoi(temp_num[min]))craft[i] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min])) + " " + temp_text[0];
                        if (stoi(temp_num[i]) == stoi(temp_num[min]))craft[num[i]] = "";
                    }
                }
                craft_aftermath = "x" + temp_num[min] + " сундук";
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
        else if (temp_text[0] == "доски" && temp_text[1] == "доски" && temp_text[2] == "доски" && temp_text[3] == "палки" && temp_text[4] == "палки" && chek_amountitem == 5 && num[0] == 0 && num[1] == 1 && num[2] == 2 && num[3] == 4 && num[4] == 7) {
            int min_w = 0, min_p = 3;
            craft_turn = true;
            for (int i = 0; i != 9; i++) {
                if (temp_text[i] == "доски") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_w]) > stoi(temp_num[i]))min_w = i;
                    }
                }
                else if (temp_text[i] == "палки") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_p]) > stoi(temp_num[i]))min_p = i;
                    }
                }
            }
            if (stoi(temp_num[min_p]) > stoi(temp_num[min_w]))craft_aftermath_input = "x" + temp_num[min_w] + " W-кирка(20n)";
            else if (stoi(temp_num[min_p]) == stoi(temp_num[min_w]))craft_aftermath_input = "x" + temp_num[min_w] + " W-кирка(20n)";
            else craft_aftermath_input = "x" + temp_num[min_p] + " W-кирка(20n)";
            if (choicecraft == 'c' || choicecraft == 'C') {
                for (int i = 0; i != 9; i++) {
                    if (temp_text[i] == "доски") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_w]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_w])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_w]) && stoi(temp_num[min_w]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_w]) - stoi(temp_num[min_p])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_w]))craft[num[i]] = "";
                        }
                    }
                    else if (temp_text[i] == "палки") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_p])) + " " + temp_text[3];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]) && stoi(temp_num[min_w]) < stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_p]) - stoi(temp_num[min_w])) + " " + temp_text[3];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]))craft[num[i]] = "";
                        }
                    }
                }
                if (stoi(temp_num[min_p]) > stoi(temp_num[min_w]))craft_aftermath = "x" + temp_num[min_w] + " W-кирка(20n)";
                else if (stoi(temp_num[min_p]) == stoi(temp_num[min_w]))craft_aftermath = "x" + temp_num[min_w] + " W-кирка(20n)";
                else craft_aftermath = "x" + temp_num[min_p] + " W-кирка(20n)";
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
        else if (temp_text[0] == "доски" && temp_text[1] == "доски" && temp_text[2] == "доски" && temp_text[3] == "палки" && temp_text[4] == "палки" && chek_amountitem == 5 && num[0] == 0 && num[1] == 1 && num[2] == 3 && num[3] == 4 && num[4] == 7 || temp_text[0] == "доски" && temp_text[1] == "доски" && temp_text[3] == "доски" && temp_text[2] == "палки" && temp_text[4] == "палки" && chek_amountitem == 5 && num[0] == 1 && num[1] == 2 && num[2] == 4 && num[3] == 5 && num[4] == 7) {
            int min_w = 0, min_p = 3;
            craft_turn = true;
            for (int i = 0; i != 9; i++) {
                if (temp_text[i] == "доски") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_w]) > stoi(temp_num[i]))min_w = i;
                    }
                }
                else if (temp_text[i] == "палки") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_p]) > stoi(temp_num[i]))min_p = i;
                    }
                }
            }
            if (stoi(temp_num[min_p]) > stoi(temp_num[min_w]))craft_aftermath_input = "x" + temp_num[min_w] + " W-топор(20n)";
            else if (stoi(temp_num[min_p]) == stoi(temp_num[min_w]))craft_aftermath_input = "x" + temp_num[min_w] + " W-топор(20n)";
            else craft_aftermath_input = "x" + temp_num[min_p] + " W-топор(20n)";
            if (choicecraft == 'c' || choicecraft == 'C') {
                for (int i = 0; i != 9; i++) {
                    if (temp_text[i] == "доски") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_w]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_w])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_w]) && stoi(temp_num[min_w]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_w]) - stoi(temp_num[min_p])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_w]))craft[num[i]] = "";
                        }
                    }
                    else if (temp_text[i] == "палки") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_p])) + " " + temp_text[4];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]) && stoi(temp_num[min_w]) < stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_p]) - stoi(temp_num[min_w])) + " " + temp_text[4];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]))craft[num[i]] = "";
                        }
                    }
                }
                if (stoi(temp_num[min_p]) > stoi(temp_num[min_w]))craft_aftermath = "x" + temp_num[min_w] + " W-топор(20n)";
                else if (stoi(temp_num[min_p]) == stoi(temp_num[min_w]))craft_aftermath = "x" + temp_num[min_w] + " W-топор(20n)";
                else craft_aftermath = "x" + temp_num[min_p] + " W-топор(20n)";
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
        else if (temp_text[0] == "камня" && temp_text[1] == "камня" && temp_text[2] == "камня" && temp_text[3] == "палки" && temp_text[4] == "палки" && chek_amountitem == 5 && num[0] == 0 && num[1] == 1 && num[2] == 2 && num[3] == 4 && num[4] == 7) {
            int min_s = 0, min_p = 3;
            craft_turn = true;
            for (int i = 0; i != 9; i++) {
                if (temp_text[i] == "камня") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_s]) > stoi(temp_num[i]))min_s = i;
                    }
                }
                else if (temp_text[i] == "палки") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_p]) > stoi(temp_num[i]))min_p = i;
                    }
                }
            }
            if (stoi(temp_num[min_p]) > stoi(temp_num[min_s]))craft_aftermath_input = "x" + temp_num[min_s] + " S-кирка(30n)";
            else if (stoi(temp_num[min_p]) == stoi(temp_num[min_s]))craft_aftermath_input = "x" + temp_num[min_s] + " S-кирка(30n)";
            else craft_aftermath_input = "x" + temp_num[min_p] + " S-кирка(30n)";
            if (choicecraft == 'c' || choicecraft == 'C') {
                for (int i = 0; i != 9; i++) {
                    if (temp_text[i] == "камня") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_s]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_s])) + " " + temp_text[0];
                            else if(stoi(temp_num[i]) == stoi(temp_num[min_s]) && stoi(temp_num[min_s]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_s]) - stoi(temp_num[min_p])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_s]))craft[num[i]] = "";
                        }
                    }
                    else if (temp_text[i] == "палки") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_p])) + " " + temp_text[3];
                            else if(stoi(temp_num[i]) == stoi(temp_num[min_p]) && stoi(temp_num[min_s]) < stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_p]) - stoi(temp_num[min_s])) + " " + temp_text[3];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]))craft[num[i]] = "";
                        }
                    }
                }
                if (stoi(temp_num[min_p]) > stoi(temp_num[min_s]))craft_aftermath = "x" + temp_num[min_s] + " S-кирка(30n)";
                else if (stoi(temp_num[min_p]) == stoi(temp_num[min_s]))craft_aftermath = "x" + temp_num[min_s] + " S-кирка(30n)";
                else craft_aftermath = "x" + temp_num[min_p] + " S-кирка(30n)";
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
        else if (temp_text[0] == "камня" && temp_text[1] == "камня" && temp_text[2] == "камня" && temp_text[3] == "палки" && temp_text[4] == "палки" && chek_amountitem == 5 && num[0] == 0 && num[1] == 1 && num[2] == 3 && num[3] == 4 && num[4] == 7 || temp_text[0] == "камня" && temp_text[1] == "камня" && temp_text[3] == "камня" && temp_text[2] == "палки" && temp_text[4] == "палки" && chek_amountitem == 5 && num[0] == 1 && num[1] == 2 && num[2] == 4 && num[3] == 5 && num[4] == 7) {
            int min_s = 0, min_p = 3;
            craft_turn = true;
            for (int i = 0; i != 9; i++) {
                if (temp_text[i] == "камня") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_s]) > stoi(temp_num[i]))min_s = i;
                    }
                }
                else if (temp_text[i] == "палки") {
                    if (temp_num[i].empty() == false) {
                        if (stoi(temp_num[min_p]) > stoi(temp_num[i]))min_p = i;
                    }
                }
            }
            if (stoi(temp_num[min_p]) > stoi(temp_num[min_s]))craft_aftermath_input = "x" + temp_num[min_s] + " S-топор(30n)";
            else if (stoi(temp_num[min_p]) == stoi(temp_num[min_s]))craft_aftermath_input = "x" + temp_num[min_s] + " S-топор(30n)";
            else craft_aftermath_input = "x" + temp_num[min_p] + " S-топор(30n)";
            if (choicecraft == 'c' || choicecraft == 'C') {
                for (int i = 0; i != 9; i++) {
                    if (temp_text[i] == "камня") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_s]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_s])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_s]) && stoi(temp_num[min_s]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_s]) - stoi(temp_num[min_p])) + " " + temp_text[0];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_s]))craft[num[i]] = "";
                        }
                    }
                    else if (temp_text[i] == "палки") {
                        if (temp_num[i].empty() == false) {
                            if (stoi(temp_num[i]) > stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[i]) - stoi(temp_num[min_p])) + " " + temp_text[4];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]) && stoi(temp_num[min_s]) < stoi(temp_num[min_p]))craft[num[i]] = "x" + std::to_string(stoi(temp_num[min_p]) - stoi(temp_num[min_s])) + " " + temp_text[4];
                            else if (stoi(temp_num[i]) == stoi(temp_num[min_p]))craft[num[i]] = "";
                        }
                    }
                }
                if (stoi(temp_num[min_p]) > stoi(temp_num[min_s]))craft_aftermath = "x" + temp_num[min_s] + " S-топор(30n)";
                else if (stoi(temp_num[min_p]) == stoi(temp_num[min_s]))craft_aftermath = "x" + temp_num[min_s] + " S-топор(30n)";
                else craft_aftermath = "x" + temp_num[min_p] + " S-топор(30n)";
                craft_aftermath_input = "";
                craft_turn = false;
            }
        }
        else { craft_aftermath_input = ""; craft_turn = false; }
    }
    else { craft_aftermath_input = ""; craft_turn = false; }
}
//CraftingRecipes - Функция котррая хранит рецепты крафта
void Craft(sf::Sound& craft_sound) {
    static char choice; static char choicecraft; static int item[2]; static string temp_text[4] = { "","","","" };
    do {
        do {
            CraftingRecipes(choicecraft);
            ClearLineScreen(); InputConsoleCraftTable(); InputConsoleInvent();
            cout << "\nВыберете действие:\n[1] - Добавить предмет | [2] - Убрать предмет";
            if (craft_turn == true)cout << " | [c] - Скрафтить";
            cout << "\n[e] - Выйти из инвентаря\n";
            if (error_input == 0 || error_input >= 18 && error_input <= 20) { cout << "Ваш выбор: "; cin >> choicecraft; }
            else { cout << "Введите ваш выбор снова: "; cin >> choicecraft; }
            if (choicecraft == '1' || choicecraft == '2' || choicecraft == 'c' || choicecraft == 'C' || choicecraft == 'e' || choicecraft == 'E') {
                if (craft_turn == false && choicecraft == 'c' || craft_turn == false && choicecraft == 'C')error_input = 1;
                else if (craft_turn == true && choicecraft == 'c' || craft_turn == true && choicecraft == 'C')error_input = 0;
                else error_input = 0;
            }
            else error_input = 1;
        } while (error_input != 0);
        if (choicecraft == 'e' || choicecraft == 'E') { ItemCraftAdd(); break; }
        if (choicecraft != 'c' && choicecraft != 'C') {
            do {
                ClearLineScreen(); InputConsoleCraftTable(); InputConsoleInvent();
                cout << "\nВыберете действие:\n";
                if (choicecraft == '1')cout << "[1] - Добавить x1 предмет | [2] - Добавить все предмет в ячейке\n[e] - Выйти из инвентаря"<<endl;
                else if (choicecraft == '2')cout << "[1] - Убрать x1 предмет  | [2] - Убрать все предметы в ячейке\n[e] - Выйти из инвентаря"<<endl;
                if (error_input == 0) { cout << "Ваш выбор: "; cin >> choice; }
                else { cout << "Введите ваш выбор снова: "; cin >> choice; }
                if (choice == '1' || choice == '2' || choice == 'e' || choice == 'E')error_input = 0;
                else error_input = 1;
            } while (error_input != 0);
            if (choice == 'e' || choice == 'E') { ItemCraftAdd(); break; }
            else {
                if (choicecraft == '2') {
                    if (choice == '1')choice = '3';
                    else if (choice == '2')choice = '4';
                }
            }
            do {
                if (error_input == 0) {
                    ClearLineScreen(); InputConsoleCraftTable(); InputConsoleInvent();
                    if (choice == '1') {
                        cout << "\nВведите ячейку с которой хотите добавить x1 предмет(0 для выхода в инвентарь): "; cin >> item[0];
                        if (item[0] == 0) { error_input = 11; break; }
                        cout << "Введите ячейку в которой хотите добавить x1 предмет(0 для выхода в инвентарь): "; cin >> item[1];
                        if (item[1] == 0) { error_input = 11; break; }
                    }
                    else if (choice == '2') {
                        cout << "\nВведите ячейку с которой хотите добавить все предметы(0 для выхода в инвентарь): "; cin >> item[0];
                        if (item[0] == 0) { error_input = 11; break; }
                        cout << "Введите ячейку в которой хотите добавить все  предметы(0 для выхода в инвентарь): "; cin >> item[1];
                        if (item[1] == 0) { error_input = 11; break; }
                    }
                    else if (choice == '3') {
                        cout << "\nВведите ячейку с которой хотите убрать x1 предметы(0 для выхода в инвентарь): "; cin >> item[1];
                        if (item[1] == 0) { error_input = 11; break; }
                        cout << "Введите ячейку в которою хотите убрвть x1 предметы(0 для выхода в инвентарь): "; cin >> item[0];
                        if (item[0] == 0) { error_input = 11; break; }
                    }
                    else if (choice == '4') {
                        cout << "\nВведите ячейку с которой хотите убрать все предмет(0 для выхода в инвентарь): "; cin >> item[1];
                        if (item[1] == 0) { error_input = 11; break; }
                        cout << "Введите ячейку в которою хотите убрать все предметы(0 для выхода в инвентарь): "; cin >> item[0];
                        if (item[0] == 0) { error_input = 11; break; }
                    }
                }
                else {
                    ClearLineScreen(); InputConsoleCraftTable(); InputConsoleInvent();
                    if (error_input == 15 || error_input == 16 || error_input == 17) {
                        if (choice == '1') {
                            if (error_input != 16) { cout << "\nВведите ячейку с которой хотите добавить x1 предмет снова(0 для выхода в инвентарь): "; cin >> item[0]; }
                            if (item[0] == 0) { error_input = 11; break; }
                            if (error_input != 17) {
                                if (error_input != 15)cout << '\n';
                                cout << "Введите ячейку в которую хотите добавить x1 предмет снова(0 для выхода в инвентарь): "; cin >> item[1];
                            }
                            if (item[1] == 0) { error_input = 11; break; }
                        }
                        else if (choice == '2') {
                            if (error_input != 16) { cout << "\nВведите ячейку с которой хотите добавить все предметы снова(0 для выхода в инвентарь): "; cin >> item[0]; }
                            if (item[0] == 0) { error_input = 11; break; }
                            if (error_input != 17) {
                                if (error_input != 15)cout << '\n';
                                cout << "Введите ячейку в которую хотите добавить все  предметы снова(0 для выхода в инвентарь): "; cin >> item[1];
                            }
                            if (item[1] == 0) { error_input = 11; break; }
                        }
                        else if (choice == '3') {
                            if (error_input != 17) { cout << "\nВведите ячейку с которой хотите добавить x1 предметы снова(0 для выхода в инвентарь): "; cin >> item[1]; }
                            if (item[1] == 0) { error_input = 11; break; }
                            if (error_input != 16) {
                                if (error_input != 15)cout << '\n';
                                cout << "\nВведите ячейку в которою хотите добавить x1  предметы снова(0 для выхода в инвентарь): "; cin >> item[0];
                            }
                            if (item[0] == 0) { error_input = 11; break; }
                        }
                        else if (choice == '4') {
                            if (error_input != 17) { cout << "\nВведите ячейку с которой хотите убрать все предмет снова(0 для выхода в инвентарь): "; cin >> item[1]; }
                            if (item[1] == 0) { error_input = 11; break; }
                            if (error_input != 16) {
                                if (error_input != 15)cout << '\n';
                                cout << "\nВведите ячейку в которою хотите убрать все  предметы снова(0 для выхода в инвентарь): "; cin >> item[0];
                            }
                            if (item[0] == 0) { error_input = 11; break; }
                        }
                    }
                }
                if (input_chest == 0) {
                    if (item[1]<0 || item[1]>craft.size()) {
                        if (item[0] < 0 || item[0]>20)error_input = 15;
                        else if (item[0] > 0 && item[0] <= 20)error_input = 16;
                    }
                    else if (item[0] < 0 || item[0]>20) {
                        if (item[1]<0 || item[1]>craft.size())error_input = 15;
                        else if (item[1] > 0 && item[1] <= craft.size())error_input = 17;
                    }
                    else error_input = 0;
                }
                else {
                    if (item[1]<0 || item[1]>craft.size()) {
                        if (item[0] < 0 || item[0]>20) { error_input = 15; }
                        else if (item[0] > 0 && item[0] <= 20) { error_input = 16; }
                    }
                    else if (item[0] < 0 || item[0]>20) {
                        if (item[1]<0 || item[1]>craft.size()) { error_input = 15; }
                        else if (item[1] > 0 && item[1] <= craft.size()) { error_input = 17; }
                    }
                    else error_input = 0;
                }
            } while (error_input != 0);
            if (error_input != 11) {
                item[0]--; item[1]--;
                if (inventory[item[0]].empty() == false) {
                    for (int i = inventory[item[0]].find('x') + 1; i != inventory[item[0]].size(); i++) {
                        if (i > inventory[item[0]].find(' '))temp_text[0] += inventory[item[0]][i];
                        else if (i < inventory[item[0]].find(' '))temp_text[2] += inventory[item[0]][i];
                    }
                }
                if (craft[item[1]].empty() == false) {
                    for (int i = craft[item[1]].find('x') + 1; i != craft[item[1]].size(); i++) {
                        if (i > craft[item[1]].find(' '))temp_text[1] += craft[item[1]][i];
                        else if (i < craft[item[1]].find(' '))temp_text[3] += craft[item[1]][i];
                    }
                }
                if (craft[item[1]].empty() == false && inventory[item[0]].empty() == false) {
                    if (temp_text[0] != temp_text[1])error_input = 5;
                    else if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка"))error_input = 24;
                    else if (stoi(temp_text[3]) == 12 && choice == '1' || stoi(temp_text[3]) == 12 && choice == '2')error_input = 19;
                    else if (stoi(temp_text[2]) == 12 && choice == '3' || stoi(temp_text[3]) == 12 && choice == '4')error_input = 20;
                    else {
                        if (error_input != 18) {
                            if (choice == '1') {
                                if (stoi(temp_text[2]) == 1)inventory[item[0]] = "";
                                else inventory[item[0]] = "x" + std::to_string(stoi(temp_text[2]) - 1) + " " + temp_text[0];
                                craft[item[1]] = "x" + std::to_string(stoi(temp_text[3]) + 1) + " " + temp_text[1];
                            }
                            else if (choice == '2') {
                                if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                                    inventory[item[0]] = "";
                                    craft[item[1]] = "x12 " + temp_text[1];
                                }
                                else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                                    inventory[item[0]] = "x" + std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3]))) + " " + temp_text[0];
                                    craft[item[1]] = "x12 " + temp_text[1];
                                }
                                else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                                    inventory[item[0]] = "";
                                    craft[item[1]] = "x" + std::to_string(stoi(temp_text[3]) + stoi(temp_text[2])) + " " + temp_text[0];
                                }
                            }
                            else if (choice == '3') {
                                if (stoi(temp_text[3]) == 1)craft[item[1]] = "";
                                else craft[item[1]] = "x" + std::to_string(stoi(temp_text[3]) - 1) + " " + temp_text[1];
                                inventory[item[0]] = "x" + std::to_string(stoi(temp_text[2]) + 1) + " " + temp_text[0];
                            }
                            else if (choice == '4') {
                                if (12 - stoi(temp_text[2]) == stoi(temp_text[3])) {
                                    craft[item[1]] = "";
                                    inventory[item[0]] = "x12 " + temp_text[1];
                                }
                                else if (12 - stoi(temp_text[2]) < stoi(temp_text[3])) {
                                    craft[item[1]] = "x" + std::to_string(stoi(temp_text[3]) - (12 - stoi(temp_text[2]))) + " " + temp_text[1];
                                    inventory[item[0]] = "x12 " + temp_text[1];
                                }
                                else if (12 - stoi(temp_text[2]) > stoi(temp_text[3])) {
                                    craft[item[1]] = "";
                                    inventory[item[0]] = "x" + std::to_string(stoi(temp_text[3]) + stoi(temp_text[2])) + " " + temp_text[0];
                                }
                            }
                        }
                    }
                }
                else if (inventory[item[0]].empty() == false && craft[item[1]].empty() || inventory[item[0]].empty() && craft[item[1]].empty() == false || inventory[item[0]].empty() && craft[item[1]].empty()) {
                    if (temp_text[0].find("топор") != string :: npos || temp_text[0].find("кирка") != string::npos || temp_text[1].find("топор") != string::npos || temp_text[1].find("кирка") != string::npos)swap(craft[item[1]], inventory[item[0]]);
                    else if (inventory[item[0]].empty() == false && choice == '1' || inventory[item[0]].empty() == false && choice == '2') {
                        if (choice == '1') {
                            if (stoi(temp_text[2]) == 1)inventory[item[0]] = "";
                            else inventory[item[0]] = "x" + std::to_string(stoi(temp_text[2]) - 1) + " " + temp_text[0];
                            craft[item[1]] = "x1 " + temp_text[0];
                        }
                        else if (choice == '2') {
                            swap(craft[item[1]], inventory[item[0]]);
                        }
                    }
                    else if (inventory[item[0]].empty() && choice == '1' || inventory[item[0]].empty() && choice == '2') error_input = 22;
                    else if (craft[item[1]].empty() == false && choice == '3' || craft[item[1]].empty() == false && choice == '4') {
                        if (choice == '3') {
                            if (stoi(temp_text[3]) == 1)craft[item[1]] = "";
                            else craft[item[1]] = "x" + std::to_string(stoi(temp_text[3]) - 1) + " " + temp_text[1];
                            inventory[item[0]] = "x1 " + temp_text[1];
                        }
                        else if (choice == '4') {
                            inventory[item[0]] = "x" + temp_text[3] + " " + temp_text[1];
                            craft[item[1]] = "";
                        }
                    }
                    else if (craft[item[1]].empty() && choice == '3' || craft[item[1]].empty() && choice == '4')error_input = 23;
                }
            }
        }
        else {
            craft_sound.play();
            CraftingRecipes(choicecraft);
            CraftAftermathDelete();
            choicecraft = ' ';
            if (craft_aftermath != "") { error = 21; break; }
        }
        temp_text[0] = ""; temp_text[1] = ""; temp_text[2] = ""; temp_text[3] = "";
    } while (choice != 'e' && choice != 'E');
    craft_aftermath_input = ""; craft_turn = false;
}
//Крафт предметов
#pragma endregion
#pragma region Inventory-Inventory_Actions
void InventoryPlayer(int& error_input, int item[2], int line_of_sight, int loc[4], sf::Sound& craft_sound, sf::Sound& throw_away);//Инвентарь игрока
void EjectionOfObjects(int line_of_sight, int loc[4], vector<InfoStucture>& structure, vector<InfoTempItem>& timedrop, sf::Sound& throw_away) {
    ClearLineScreen();
    InputConsoleInvent();
    static int index_tempdrop, cell, check_empty = 0;
    static char choice = ' ';
    static string temp_text[4] = { "","","","" };
    do {
        if (error_input == 0) { cout << "\nВведите номер ячейки которую хотите выбросить(0 для выхода в инвентарь): "; cin >> cell; }
        else if (error_input == 9 || error_input == 8) { cout << "\nСнова введите номер ячейки которую хотите выбросить(0 для выхода в инвентарь): "; cin >> cell; }
        if (cell == 0) { error_input = 0; choice = 'e'; break; }
        else if (cell < 0 || cell>20) { error_input = 8; ClearLineScreen(); InputConsoleInvent(); }
        else if (inventory[cell - 1].empty()) { error_input = 9; ClearLineScreen(); InputConsoleInvent(); }
        else error_input = 0;
    } while (error_input != 0);
    if (choice != 'e' && choice != 'E') {
        ClearLineScreen();
        InputConsoleInvent();
        cout << "\nВыберете действие:\n[1] - Выбросить x1 предмет | [2] - Выбросить все предметы в ячейке | [e] - Выйти в инвентарь\nВаш выбор: "; cin >> choice;
        while (choice != '1' && choice != '2' && choice != 'e' && choice != 'E') {
            error_input = 1;
            ClearLineScreen();
            InputConsoleInvent();
            cout << "\nВыберете действие:\n[1] - Выбросить x1 предмет | [2] - Выбросить все предметы в ячейке | [e] - Выйти в инвентарь\n: ";
            if (error_input == 0) { cout << "Ваш выбор: "; cin >> choice; }
            else { cout << "Введите ваш выбор снова: "; cin >> choice; }
        }
        cell--; error_input = 0;
        if (choice == 'e' || choice == 'E')cout << "";
        else {
            for (int i = inventory[cell].find('x') + 1; i != inventory[cell].size(); i++) {
                if (i > inventory[cell].find(' '))temp_text[1] += inventory[cell][i];
                else if (i < inventory[cell].find(' '))temp_text[3] += inventory[cell][i];
            }
            if (choice == '1') { ejectitem[1] = "- x1 " + temp_text[1]; ejectitem[2] = "1"; }
            else { ejectitem[1] = "- x" + temp_text[3] + " " + temp_text[1]; ejectitem[2] = "1"; }
            if (line_of_sight == 1) {
                if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == ' ' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == '^') {
                    structure.resize(structure.size() + 1); timedrop.resize(timedrop.size() + 1);
                    world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = 'T'; structure[structure.size() - 1].name = 'T';
                    structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2] - 1; structure[structure.size() - 1].location[3] = loc[3];
                    timedrop[timedrop.size() - 1].loc_in_structure = structure.size() - 1;
                    timedrop[timedrop.size() - 1].tempdrop.resize(1);
                    index_tempdrop = timedrop.size() - 1;
                }
                else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'T') {
                    for (int i = 0; i != timedrop.size(); i++) {
                        if (structure[timedrop[i].loc_in_structure].location[0] == loc[0] && structure[timedrop[i].loc_in_structure].location[1] == loc[1] && structure[timedrop[i].loc_in_structure].location[2] == loc[2] - 1 && structure[timedrop[i].loc_in_structure].location[3] == loc[3]) {
                            index_tempdrop = i; break;
                        }
                    }
                    if (timedrop[index_tempdrop].tempdrop.size() >= 12)error_input = 12;
                }
                else error_input = 10;
            }
            else if (line_of_sight == 2) {
                if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == ' ' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'v') {
                    structure.resize(structure.size() + 1); timedrop.resize(timedrop.size() + 1);
                    world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'T'; structure[structure.size() - 1].name = 'T';
                    structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2] + 1; structure[structure.size() - 1].location[3] = loc[3];
                    timedrop[timedrop.size() - 1].loc_in_structure = structure.size() - 1;
                    timedrop[timedrop.size() - 1].tempdrop.resize(1);
                    index_tempdrop = timedrop.size() - 1;
                }
                else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'T') {
                    for (int i = 0; i != timedrop.size(); i++) {
                        if (structure[timedrop[i].loc_in_structure].location[0] == loc[0] && structure[timedrop[i].loc_in_structure].location[1] == loc[1] && structure[timedrop[i].loc_in_structure].location[2] == loc[2] + 1 && structure[timedrop[i].loc_in_structure].location[3] == loc[3]) {
                            index_tempdrop = i; break;
                        }
                    }
                    if (timedrop[index_tempdrop].tempdrop.size() >= 12)error_input = 12;
                }
                else error_input = 10;
            }
            else if (line_of_sight == 3) {
                if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == ' ' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == '>') {
                    structure.resize(structure.size() + 1); timedrop.resize(timedrop.size() + 1);
                    world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = 'T'; structure[structure.size() - 1].name = 'T';
                    structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2]; structure[structure.size() - 1].location[3] = loc[3] + 1;
                    timedrop[timedrop.size() - 1].loc_in_structure = structure.size() - 1;
                    timedrop[timedrop.size() - 1].tempdrop.resize(1);
                    index_tempdrop = timedrop.size() - 1;
                }
                else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'T') {
                    for (int i = 0; i != timedrop.size(); i++) {
                        if (structure[timedrop[i].loc_in_structure].location[0] == loc[0] && structure[timedrop[i].loc_in_structure].location[1] == loc[1] && structure[timedrop[i].loc_in_structure].location[2] == loc[2] && structure[timedrop[i].loc_in_structure].location[3] == loc[3] + 1) {
                            index_tempdrop = i; break;
                        }
                    }
                    if (timedrop[index_tempdrop].tempdrop.size() >= 12)error_input = 12;
                }
                else error_input = 10;
            }
            else if (line_of_sight == 4) {
                if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == ' ' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == '<') {
                    structure.resize(structure.size() + 1); timedrop.resize(timedrop.size() + 1);
                    world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = 'T'; structure[structure.size() - 1].name = 'T';
                    structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
                    structure[structure.size() - 1].location[2] = loc[2]; structure[structure.size() - 1].location[3] = loc[3] - 1;
                    timedrop[timedrop.size() - 1].loc_in_structure = structure.size() - 1;
                    timedrop[timedrop.size() - 1].tempdrop.resize(1);
                    index_tempdrop = timedrop.size() - 1;
                }
                else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'T') {
                    for (int i = 0; i != timedrop.size(); i++) {
                        if (structure[timedrop[i].loc_in_structure].location[0] == loc[0] && structure[timedrop[i].loc_in_structure].location[1] == loc[1] && structure[timedrop[i].loc_in_structure].location[2] == loc[2] && structure[timedrop[i].loc_in_structure].location[3] == loc[3] - 1) {
                            index_tempdrop = i; break;
                        }
                    }
                    if (timedrop[index_tempdrop].tempdrop.size() >= 12)error_input = 12;
                }
                else error_input = 10;
            }
            if (error_input != 12 && error_input != 10) {
                if (timedrop[index_tempdrop].tempdrop.size() >= 1 && timedrop[index_tempdrop].tempdrop[0].empty() == false) {
                    if (temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                        check_empty = 0;
                        timedrop[index_tempdrop].hp = 0;
                        timedrop[index_tempdrop].tempdrop.resize(timedrop[index_tempdrop].tempdrop.size() + 1);
                        timedrop[index_tempdrop].tempdrop[timedrop[index_tempdrop].tempdrop.size() - 1] = "x1 " + temp_text[1];
                        inventory[cell] = "";
                    }
                    else {
                        for (int i = 0; i != timedrop[index_tempdrop].tempdrop.size(); i++) {
                            for (int l = timedrop[index_tempdrop].tempdrop[i].find('x') + 1; l != timedrop[index_tempdrop].tempdrop[i].size(); l++) {
                                if (l > timedrop[index_tempdrop].tempdrop[i].find(' '))temp_text[0] += timedrop[index_tempdrop].tempdrop[i][l];
                                else if (l < timedrop[index_tempdrop].tempdrop[i].find(' '))temp_text[2] += timedrop[index_tempdrop].tempdrop[i][l];
                            }
                            if (temp_text[0] == temp_text[1]) {
                                if (choice == '1' || stoi(temp_text[3]) == 1) {
                                    if (stoi(temp_text[2]) != 12) {
                                        if (stoi(temp_text[3]) == 1)inventory[cell] = "";
                                        else inventory[cell] = std::to_string(stoi(temp_text[3]) - 1) + " " + temp_text[1];
                                        timedrop[index_tempdrop].hp = 0;
                                        timedrop[index_tempdrop].tempdrop[i] = "x" + std::to_string(stoi(temp_text[2]) + 1) + " " + temp_text[0];
                                        check_empty = 0;
                                        break;
                                    }
                                    else check_empty += 1;
                                }
                                else if (choice == '2') {
                                    if (stoi(temp_text[2]) != 12) {
                                        if (12 - stoi(temp_text[2]) > stoi(temp_text[3])) {
                                            temp_text[2] = std::to_string(stoi(temp_text[2]) + stoi(temp_text[3]));
                                            timedrop[index_tempdrop].hp = 0;
                                            timedrop[index_tempdrop].tempdrop[i] = "x" + temp_text[2] + " " + temp_text[0];
                                            inventory[cell] = "";
                                            check_empty = 0; break;
                                        }
                                        else if (12 - stoi(temp_text[2]) < stoi(temp_text[3])) {
                                            timedrop[index_tempdrop].hp = 0;
                                            timedrop[index_tempdrop].tempdrop[i] = "x12 " + temp_text[0];
                                            temp_text[3] = std::to_string(stoi(temp_text[3]) - (12 - stoi(temp_text[2])));
                                            check_empty += 1;
                                        }
                                        else if (12 - stoi(temp_text[2]) == stoi(temp_text[3])) {
                                            timedrop[index_tempdrop].hp = 0;
                                            timedrop[index_tempdrop].tempdrop[i] = "x12 " + temp_text[0];
                                            inventory[cell] = "";
                                            check_empty = 0; break;
                                        }
                                    }
                                    else check_empty += 1;
                                }
                            }
                            else check_empty += 1;
                            temp_text[0] = ""; temp_text[2] = "";
                        }
                        if (check_empty == timedrop[index_tempdrop].tempdrop.size()) {
                            check_empty = 0;
                            timedrop[index_tempdrop].hp = 0;
                            timedrop[index_tempdrop].tempdrop.resize(timedrop[index_tempdrop].tempdrop.size() + 1);
                            if (choice == '1' || stoi(temp_text[3]) == 1) {
                                timedrop[index_tempdrop].tempdrop[timedrop[index_tempdrop].tempdrop.size() - 1] = "x1 " + temp_text[1];
                                if (stoi(temp_text[3]) == 1)inventory[cell] = "";
                                else inventory[cell] = "x" + std::to_string(stoi(temp_text[3]) - 1) + " " + temp_text[1];
                            }
                            else if (choice == '2') {
                                timedrop[index_tempdrop].tempdrop[timedrop[index_tempdrop].tempdrop.size() - 1] = "x" + temp_text[3] + " " + temp_text[1];
                                inventory[cell] = "";
                            }
                            check_empty = 0;
                        }
                    }
                }
                else if (timedrop[index_tempdrop].tempdrop.size() == 1 && timedrop[index_tempdrop].tempdrop[0].empty()) {
                    timedrop[index_tempdrop].hp = 0;
                    if (temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                        check_empty = 0;
                        timedrop[index_tempdrop].tempdrop[timedrop[index_tempdrop].tempdrop.size() - 1] = "x1 " + temp_text[1];
                        inventory[cell] = "";
                    }
                    else if (choice == '1' || stoi(temp_text[3]) == 1) {
                        if (stoi(temp_text[3]) == 1)inventory[cell] = "";
                        else inventory[cell] = "x" + std::to_string(stoi(temp_text[3]) - 1) + " " + temp_text[1];
                        timedrop[index_tempdrop].tempdrop[0] = "x1 " + temp_text[1];
                    }
                    else if (choice == '2') {
                        timedrop[index_tempdrop].tempdrop[0] = "x" + std::to_string(stoi(temp_text[3])) + " " + temp_text[1];
                        inventory[cell] = "";
                    }
                    check_empty = 0;
                }
            }
        }
        ClearLineScreen();
        if (error_input == 0)throw_away.play();
    }
    temp_text[0] = ""; temp_text[1] = ""; temp_text[2] = ""; temp_text[3] = ""; choice = ' ';
}
//EjectionOfObjects - Выбрасывание предметов
void PutInHand(int line_of_sight, int loc[4]) {
    ClearLineScreen();
    InputConsoleInvent();
    do {
        if (error_input == 0) { cout << "\nВведите номер ячейки которую хотите поместить в руку(0 для выхода в инвентарь): "; cin >> hand; }
        else if (error_input == 8) { cout << "\nСнова введите номер ячейки которую хотите поместить в руку(0 для выхода в инвентарь): "; cin >> hand; }
        if (hand < 0 || hand>20) { error_input = 8; ClearLineScreen(); InputConsoleInvent(); }
        else if (hand == 0) { error_input = 0; break; }
    } while (error_input != 0);
    hand--;
}
//PutInHand - Взять предмет в руку
void MoveItem(int& error_input, int item[2], int line_of_sight, int loc[4]) {
    static string temp_text[4] = { "","","","" };
    static char choice = ' ';
    if (error_input == 5 || error_input == 6)item[0]++;
    if (input_chest == 0 || input_chest == 3)choice = '1';
    if (input_chest == 1 && error_input != 13 && error_input != 24) {
        do {
            ClearLineScreen(); InputConsoleTimeDrop(loc); InputConsoleInvent();
            cout << "\nВыберете действие:\n[1] - Перемещать предметы в инвентаре | [2] - Перемещать предметы с временного сундука в инвентарь\n[e] - Выйти в инвентарь\n";
            if (error_input == 0) { cout << "Ваш выбор: "; cin >> choice; }
            else { cout << "Введите ваш выбор снова: "; cin >> choice; }
            if (choice == '1' || choice == '2' || choice == 'e' || choice == 'E')error_input = 0;
            else error_input = 1;
        } while (error_input != 0);
        if (choice == 'e' || choice == 'E')input_chest = 0;
    }
    if (input_chest == 2) {
        do {
            ClearLineScreen(); InputConsoleDropChest(); InputConsoleInvent();
            cout << "\nВыберете действие:\n[1] - Перемещать предметы в инвентаре | [2] - Перемещать предметы в сундуке\n[3] - Перемещать предметы с сундука в инвентарь | [4] - Перемещать предметы с инвентаря в сундук\n[e] - Выйти в инвентарь\n";
            if (error_input == 0) { cout << "Ваш выбор: "; cin >> choice; }
            else { cout << "Введите ваш выбор снова: "; cin >> choice; }
            if (choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == 'e' || choice == 'E')error_input = 0;
            else error_input = 1;
        } while (error_input != 0);
    }
    if (choice != 'e' && choice != 'E') {
        do {
            ClearLineScreen();
            if (input_chest == 0)InputConsoleInvent();
            else if (input_chest == 1) { InputConsoleTimeDrop(loc); InputConsoleInvent(); }
            else if (input_chest == 2) { InputConsoleDropChest(); InputConsoleInvent(); }
            else if (input_chest == 3) { InputConsoleCraftTable(); InputConsoleInvent(); }
            if (error_input == 0) {
                if (choice == '1') { cout << "\nВведите номер ячейки которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 1 && choice == '2') { cout << "\nВведите номер ячейки(временный сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '2') { cout << "\nВведите номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '3') { cout << "\nВведите номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '4') { cout << "\nВведите номер ячейки(инвентарь) которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                if (item[0] == 0 && choice != '4' || input_chest == 2 && choice == '4' && item[1] == 0) { error_input = 11; choiceswap = 'e'; break; }
                if (choice == '1') { cout << "Введите номер ячейки в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 1 && choice == '2') { cout << "Введите номер ячейки(инвентарь) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '2') { cout << "Введите номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '3') { cout << "Введите номер ячейки(инвентарь) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '4') { cout << "Введите номер ячейки(сундук) в которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                if (item[1] == 0 && choice != '4' || input_chest == 2 && choice == '4' && item[0] == 0) { error_input = 11; choiceswap = 'e'; break; }
                if (choice == '1' || input_chest == 2 && choice == '2') {
                    if (item[0] == item[1])error_input = 7;
                    else error_input = 0;
                }
            }
            else if (error_input == 2 || error_input == 7 || error_input == 13 || error_input == 24) {
                if (choice == '1') { cout << "\nВведите снова номер ячейки которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 1 && choice == '2') { cout << "\nВведите снова номер ячейки(временный сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '2') { cout << "\nВведите снова номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '3') { cout << "\nВведите снова номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '4') { cout << "\nВведите снова номер ячейки(инвентарь) которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                if (item[0] == 0 && choice != '4' || input_chest == 2 && choice == '4' && item[1] == 0) { error_input = 11; choiceswap = 'e'; break; }
                if (choice == '1') { cout << "Введите снова номер ячейки в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 1 && choice == '2') { cout << "Введите снова номер ячейки(инвентарь) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '2') { cout << "Введите снова номер ячейки(сундук) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '3') { cout << "Введите снова номер ячейки(инвентарь) в которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '4') { cout << "Введите снова номер ячейки(сундук) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                if (item[1] == 0 && choice != '4' || input_chest == 2 && choice == '4' && item[1] == 0) { error_input = 11; choiceswap = 'e'; break; }
                if (choice == '1' || input_chest == 2 && choice == '2') {
                    if (item[0] == item[1] || input_chest == 2 && choice == '2')error_input = 7;
                    else error_input = 0;
                }
                if (error_input == 13)error_input = 0;
            }
            else if (error_input == 3) {
                if (choice == '1') { cout << "\nВведите снова номер ячейки которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 1 && choice == '2') { cout << "\nВведите снова номер ячейки(временный сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '2') { cout << "\nВведите снова номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '3') { cout << "\nВведите снова номер ячейки(сундук) которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                else if (input_chest == 2 && choice == '4') { cout << "\nВведите снова номер ячейки(инвентарь) которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                if (item[0] == 0 && choice != '4' || input_chest == 2 && choice == '4' && item[1] == 0) { error_input = 11; choiceswap = 'e'; break; }
            }
            else if (error_input == 4 || error_input == 5 || error_input == 6) {
                if (choice == '1') { cout << "\nВведите снова номер ячейки в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 1 && choice == '2') { cout << "\nВведите снова номер ячейки(инвентарь) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '2') { cout << "\nВведите снова номер ячейки(сундук) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '3') { cout << "\nВведите снова номер ячейки(инвентарь) в которую хотите переместить(0 для выхода): "; cin >> item[1]; }
                else if (input_chest == 2 && choice == '4') { cout << "\nВведите снова номер ячейки(сундук) в которую хотите переместить(0 для выхода): "; cin >> item[0]; }
                if (item[1] == 0 && choice != '4' || input_chest == 2 && choice == '3' && item[0] == 0) { error_input = 11; choiceswap = 'e'; break; }
                if (choice == '1' || input_chest == 2 && choice == '2') {
                    if (item[0] == item[1])error_input = 7;
                    else error_input = 0;
                }
            }
            if (choice == '1') {
                if (item[0] < 0 || item[0]>20) {
                    if (item[1] < 0 || item[1]>20 && error_input != 7)error_input = 2;
                    else if (item[1] > 0 && item[1] <= 20)error_input = 3;
                }
                else if (item[1] < 0 || item[1]>20) {
                    if (item[0] < 0 || item[0]>20 && error_input != 7)error_input = 2;
                    if (item[0] > 0 && item[0] <= 20)error_input = 4;
                }
                else { if (error_input != 7)error_input = 0; }
            }
            else if (input_chest == 1 && choice == '2') {
                if (item[0]<0 || item[0]>timedrop[index_input_tempdrop].tempdrop.size()) {
                    if (item[1] < 0 || item[1]>20)error_input = 2;
                    else if (item[1] > 0 && item[1] <= 20)error_input = 3;
                }
                else if (item[1] < 0 || item[1]>20) {
                    if (item[0]<0 || item[0]>timedrop[index_input_tempdrop].tempdrop.size())error_input = 2;
                    if (item[0] > 0 && item[0] <= timedrop[index_input_tempdrop].tempdrop.size())error_input = 4;
                }
                else error_input = 0;
            }
            else if (input_chest == 2 && choice == '2') {
                if (item[0] < 0 || item[0]>12) {
                    if (item[1] < 0 || item[1]>12)error_input = 2;
                    else if (item[1] > 0 && item[1] <= 12)error_input = 3;
                }
                else if (item[1] < 0 || item[1]>12) {
                    if (item[0] < 0 || item[0]>12)error_input = 2;
                    if (item[0] > 0 && item[0] <= 12)error_input = 4;
                }
            }
            else if (input_chest == 2 && choice == '3') {
                if (item[0] < 0 || item[0]>12) {
                    if (item[1] < 0 || item[1]>20)error_input = 2;
                    else if (item[1] > 0 && item[1] <= 20)error_input = 3;
                }
                else if (item[1] < 0 || item[1]>20) {
                    if (item[0] < 0 || item[0]>12)error_input = 2;
                    if (item[0] > 0 && item[0] <= 12)error_input = 4;
                }
            }
            else if (input_chest == 2 && choice == '4') {
                if (item[1] < 0 || item[1]>20) {
                    if (item[0] < 0 || item[0]>12)error_input = 2;
                    else if (item[0] > 0 && item[0] <= 12)error_input = 3;
                }
                else if (item[0] < 0 || item[0]>12) {
                    if (item[1] < 0 || item[1]>20)error_input = 2;
                    if (item[1] > 0 && item[1] <= 20)error_input = 4;
                }
                else error_input = 0;
            }
        } while (error_input != 0);
        if (error_input != 11) {
            item[0] -= 1; item[1] -= 1;
            if (choice == '1') {
                if (inventory[item[0]].empty() && inventory[item[1]].empty() == false || inventory[item[0]].empty() == false && inventory[item[1]].empty()) {
                    if (choiceswap == '1' && inventory[item[0]].empty() == false) {
                        for (int l = inventory[item[0]].find('x') + 1; l != inventory[item[0]].size(); l++) {
                            if (l > inventory[item[0]].find(' '))temp_text[0] += inventory[item[0]][l];
                            else if (l < inventory[item[0]].find(' '))temp_text[2] += inventory[item[0]][l];
                        }
                        temp_text[2] = std::to_string(stoi(temp_text[2]) - 1);
                        if (stoi(temp_text[2]) == 0)inventory[item[0]] = "";
                        else inventory[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                        inventory[item[1]] = "x1 " + temp_text[0];
                    }
                    else if (choiceswap == '1' && inventory[item[0]].empty())swap(inventory[item[0]], inventory[item[1]]);
                    else if (choiceswap == '2' && inventory[item[0]].empty() == false)swap(inventory[item[0]], inventory[item[1]]);
                    else if (choiceswap == '2' && inventory[item[0]].empty())swap(inventory[item[0]], inventory[item[1]]);
                }
                else if (inventory[item[0]].empty() == false && inventory[item[1]].empty() == false) {
                    for (int l = inventory[item[0]].find('x') + 1; l != inventory[item[0]].size(); l++) {
                        if (l > inventory[item[0]].find(' '))temp_text[0] += inventory[item[0]][l];
                        else if (l < inventory[item[0]].find(' '))temp_text[2] += inventory[item[0]][l];
                    }
                    for (int l = inventory[item[1]].find('x') + 1; l != inventory[item[1]].size(); l++) {
                        if (l > inventory[item[1]].find(' '))temp_text[1] += inventory[item[1]][l];
                        else if (l < inventory[item[1]].find(' '))temp_text[3] += inventory[item[1]][l];
                    }
                    if (choiceswap == '2') {
                        if (temp_text[0] != temp_text[1])swap(inventory[item[0]], inventory[item[1]]);
                        else if (temp_text[0] == temp_text[1]) {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                                swap(inventory[item[0]], inventory[item[1]]);
                            }
                            else if (stoi(temp_text[2]) == 12 && stoi(temp_text[3]) < 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) < 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) == 12) {
                                if (stoi(temp_text[3]) == 12)error_input = 6;
                                else if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                                    inventory[item[0]] = "";
                                    inventory[item[1]] = "12x " + temp_text[0];
                                }
                                else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                                    temp_text[2] = std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3])));
                                    inventory[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                                    inventory[item[1]] = "x12 " + temp_text[0];
                                }
                                else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                                    temp_text[3] = std::to_string((stoi(temp_text[3]) + stoi(temp_text[2])));
                                    inventory[item[0]] = "";
                                    inventory[item[1]] = "x" + temp_text[3] + " " + temp_text[0];
                                }
                            }
                        }
                    }
                    else if (choiceswap == '1') {
                        if (temp_text[0] != temp_text[1])error_input = 5;
                        else if (temp_text[0] == temp_text[1]) {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                                swap(inventory[item[0]], inventory[item[1]]);
                            }
                            else if (stoi(temp_text[3]) == 12)error_input = 6;
                            else if (stoi(temp_text[3]) < 12) {
                                temp_text[2] = std::to_string(stoi(temp_text[2]) - 1);
                                if (stoi(temp_text[2]) == 0)inventory[item[0]] = "";
                                else { inventory[item[0]] = "x" + temp_text[2] + " " + temp_text[0]; }
                                temp_text[3] = std::to_string(stoi(temp_text[3]) + 1);
                                inventory[item[1]] = "x" + temp_text[3] + " " + temp_text[1];
                            }
                        }
                    }
                }
            }
            else if (input_chest == 1 && choice == '2') {
                for (int i = timedrop[index_input_tempdrop].tempdrop[item[0]].find('x') + 1; i != timedrop[index_input_tempdrop].tempdrop[item[0]].size(); i++) {
                    if (i > timedrop[index_input_tempdrop].tempdrop[item[0]].find(' '))temp_text[0] += timedrop[index_input_tempdrop].tempdrop[item[0]][i];
                    else if (i < timedrop[index_input_tempdrop].tempdrop[item[0]].find(' '))temp_text[2] += timedrop[index_input_tempdrop].tempdrop[item[0]][i];
                }
                if (inventory[item[1]].empty() == false) {
                    for (int i = inventory[item[1]].find('x') + 1; i != inventory[item[1]].size(); i++) {
                        if (i > inventory[item[1]].find(' '))temp_text[1] += inventory[item[1]][i];
                        else if (i < inventory[item[1]].find(' '))temp_text[3] += inventory[item[1]][i];
                    }
                    if (temp_text[0] != temp_text[1])error_input = 13;
                    else if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка"))error_input = 24;
                    else {
                        if (choiceswap == '1') {
                            if (stoi(temp_text[3]) != 12) {
                                inventory[item[1]] = "x" + std::to_string(stoi(temp_text[3]) + 1) + " " + temp_text[1];
                                if (stoi(temp_text[2]) - 1 == 0) {
                                    swap(timedrop[index_input_tempdrop].tempdrop[item[0]], timedrop[index_input_tempdrop].tempdrop[timedrop[index_input_tempdrop].tempdrop.size() - 1]);
                                    timedrop[index_input_tempdrop].tempdrop.resize(timedrop[index_input_tempdrop].tempdrop.size() - 1);
                                    if (timedrop[index_input_tempdrop].tempdrop.size() == 0) {
                                        swap(timedrop[index_input_tempdrop], timedrop[timedrop.size() - 1]);
                                        world[structure[timedrop[timedrop.size() - 1].loc_in_structure].location[0]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[1]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[2]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[3]] = ' ';
                                        swap(structure[timedrop[timedrop.size() - 1].loc_in_structure], structure[structure.size() - 1]);
                                        structure.resize(structure.size() - 1);
                                        timedrop.resize(timedrop.size() - 1);
                                    }
                                }
                                else {
                                    timedrop[index_input_tempdrop].tempdrop[item[0]] = "x" + std::to_string(stoi(temp_text[2]) - 1) + " " + temp_text[0];
                                }
                            }
                            else error_input = 6;
                        }
                        else if (choiceswap == '2') {
                            if (stoi(temp_text[3]) == 12)error_input = 6;
                            else if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                                swap(timedrop[index_input_tempdrop].tempdrop[item[0]], timedrop[index_input_tempdrop].tempdrop[timedrop[index_input_tempdrop].tempdrop.size() - 1]);
                                timedrop[index_input_tempdrop].tempdrop.resize(timedrop[index_input_tempdrop].tempdrop.size() - 1);
                                if (timedrop[index_input_tempdrop].tempdrop.size() == 0) {
                                    swap(timedrop[index_input_tempdrop], timedrop[timedrop.size() - 1]);
                                    world[structure[timedrop[timedrop.size() - 1].loc_in_structure].location[0]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[1]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[2]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[3]] = ' ';
                                    swap(structure[timedrop[timedrop.size() - 1].loc_in_structure], structure[structure.size() - 1]);
                                    structure.resize(structure.size() - 1);
                                    timedrop.resize(timedrop.size() - 1);
                                    input_chest = 0;
                                }
                                inventory[item[1]] = "12x " + temp_text[0];
                            }
                            else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                                temp_text[2] = std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3])));
                                timedrop[index_input_tempdrop].tempdrop[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                                inventory[item[1]] = "x12" + temp_text[0];
                            }
                            else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                                temp_text[3] = std::to_string((stoi(temp_text[3]) + stoi(temp_text[2])));
                                swap(timedrop[index_input_tempdrop].tempdrop[item[0]], timedrop[index_input_tempdrop].tempdrop[timedrop[index_input_tempdrop].tempdrop.size() - 1]);
                                timedrop[index_input_tempdrop].tempdrop.resize(timedrop[index_input_tempdrop].tempdrop.size() - 1);
                                if (timedrop[index_input_tempdrop].tempdrop.size() == 0) {
                                    swap(timedrop[index_input_tempdrop], timedrop[timedrop.size() - 1]);
                                    world[structure[timedrop[timedrop.size() - 1].loc_in_structure].location[0]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[1]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[2]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[3]] = ' ';
                                    swap(structure[timedrop[timedrop.size() - 1].loc_in_structure], structure[structure.size() - 1]);
                                    structure.resize(structure.size() - 1);
                                    timedrop.resize(timedrop.size() - 1);
                                    input_chest = 0;
                                }
                                inventory[item[1]] = "x" + temp_text[3] + " " + temp_text[0];

                            }
                        }
                    }
                }
                else {
                    if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                        inventory[item[1]] = "x" + temp_text[2] + " " + temp_text[0];
                        swap(timedrop[index_input_tempdrop].tempdrop[item[0]], timedrop[index_input_tempdrop].tempdrop[timedrop[index_input_tempdrop].tempdrop.size() - 1]);
                        timedrop[index_input_tempdrop].tempdrop.resize(timedrop[index_input_tempdrop].tempdrop.size() - 1);
                        if (timedrop[index_input_tempdrop].tempdrop.size() == 0) {
                            swap(timedrop[index_input_tempdrop], timedrop[timedrop.size() - 1]);
                            world[structure[timedrop[timedrop.size() - 1].loc_in_structure].location[0]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[1]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[2]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[3]] = ' ';
                            swap(structure[timedrop[timedrop.size() - 1].loc_in_structure], structure[structure.size() - 1]);
                            structure.resize(structure.size() - 1);
                            timedrop.resize(timedrop.size() - 1);
                            input_chest = 0;
                        }
                    }
                    else if (choiceswap == '1') {
                        if (stoi(temp_text[2]) == 1) {
                            swap(timedrop[index_input_tempdrop].tempdrop[item[0]], timedrop[index_input_tempdrop].tempdrop[timedrop[index_input_tempdrop].tempdrop.size() - 1]);
                            timedrop[index_input_tempdrop].tempdrop.resize(timedrop[index_input_tempdrop].tempdrop.size() - 1);
                            if (timedrop[index_input_tempdrop].tempdrop.size() == 0) {
                                swap(timedrop[index_input_tempdrop], timedrop[timedrop.size() - 1]);
                                world[structure[timedrop[timedrop.size() - 1].loc_in_structure].location[0]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[1]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[2]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[3]] = ' ';
                                swap(structure[timedrop[timedrop.size() - 1].loc_in_structure], structure[structure.size() - 1]);
                                structure.resize(structure.size() - 1);
                                timedrop.resize(timedrop.size() - 1);
                                input_chest = 0;
                            }
                        }
                        else {
                            timedrop[index_input_tempdrop].tempdrop[item[0]] = "x" + std::to_string(stoi(temp_text[2]) - 1) + " " + temp_text[0];
                        }
                        inventory[item[1]] = "x1 " + temp_text[0];
                    }
                    else if (choiceswap == '2') {
                        swap(timedrop[index_input_tempdrop].tempdrop[item[0]], timedrop[index_input_tempdrop].tempdrop[timedrop[index_input_tempdrop].tempdrop.size() - 1]);
                        timedrop[index_input_tempdrop].tempdrop.resize(timedrop[index_input_tempdrop].tempdrop.size() - 1);
                        if (timedrop[index_input_tempdrop].tempdrop.size() == 0) {
                            swap(timedrop[index_input_tempdrop], timedrop[timedrop.size() - 1]);
                            world[structure[timedrop[timedrop.size() - 1].loc_in_structure].location[0]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[1]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[2]][structure[timedrop[timedrop.size() - 1].loc_in_structure].location[3]] = ' ';
                            swap(structure[timedrop[timedrop.size() - 1].loc_in_structure], structure[structure.size() - 1]);
                            structure.resize(structure.size() - 1);
                            timedrop.resize(timedrop.size() - 1);
                            input_chest = 0;
                        }
                        inventory[item[1]] = "x" + temp_text[2] + " " + temp_text[0];
                    }
                }
            }
            else if (input_chest == 2 && choice == '2' || choice == '3' || choice == '4') {
                if (choice == '3' || choice == '4') {
                    if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                        for (int i = dropchest[index_input_chestdrop].drop[item[0]].find('x')+1; i != dropchest[index_input_chestdrop].drop[item[0]].size(); i++) {
                            if (i > dropchest[index_input_chestdrop].drop[item[0]].find(' '))temp_text[0] += dropchest[index_input_chestdrop].drop[item[0]][i];
                            else if (i < dropchest[index_input_chestdrop].drop[item[0]].find(' '))temp_text[2] += dropchest[index_input_chestdrop].drop[item[0]][i];
                        }
                    }
                    if (inventory[item[1]].empty() == false) {
                        for (int i = inventory[item[1]].find('x') + 1; i != inventory[item[1]].size(); i++) {
                            if (i > inventory[item[1]].find(' '))temp_text[1] += inventory[item[1]][i];
                            else if (i < inventory[item[1]].find(' '))temp_text[3] += inventory[item[1]][i];
                        }
                    }
                }
                else if (choice == '2') {
                    if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                        for (int i = dropchest[index_input_chestdrop].drop[item[0]].find('x') + 1; i != dropchest[index_input_chestdrop].drop[item[0]].size(); i++) {
                            if (i > dropchest[index_input_chestdrop].drop[item[0]].find(' '))temp_text[0] += dropchest[index_input_chestdrop].drop[item[0]][i];
                            else if (i < dropchest[index_input_chestdrop].drop[item[0]].find(' '))temp_text[2] += dropchest[index_input_chestdrop].drop[item[0]][i];
                        }
                    }
                    if (dropchest[index_input_chestdrop].drop[item[1]].empty() == false) {
                        for (int i = dropchest[index_input_chestdrop].drop[item[1]].find('x') + 1; i != dropchest[index_input_chestdrop].drop[item[1]].size(); i++) {
                            if (i > dropchest[index_input_chestdrop].drop[item[1]].find(' '))temp_text[1] += dropchest[index_input_chestdrop].drop[item[1]][i];
                            else if (i < dropchest[index_input_chestdrop].drop[item[1]].find(' '))temp_text[3] += dropchest[index_input_chestdrop].drop[item[1]][i];
                        }
                    }
                }
                if (choice == '2') {
                    if (dropchest[index_input_chestdrop].drop[item[0]].empty() && dropchest[index_input_chestdrop].drop[item[1]].empty() == false || dropchest[index_input_chestdrop].drop[item[0]].empty() == false && dropchest[index_input_chestdrop].drop[item[1]].empty()) {
                        if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                            swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                        }
                        else if (choiceswap == '1' && dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                            temp_text[2] = std::to_string(stoi(temp_text[2]) - 1);
                            if (stoi(temp_text[2]) == 0)dropchest[index_input_chestdrop].drop[item[0]] = "";
                            else dropchest[index_input_chestdrop].drop[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                            dropchest[index_input_chestdrop].drop[item[1]] = "x1 " + temp_text[0];
                        }
                        else if (choiceswap == '1' && dropchest[index_input_chestdrop].drop[item[0]].empty())swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                        else if (choiceswap == '2' && dropchest[index_input_chestdrop].drop[item[0]].empty() == false)swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                        else if (choiceswap == '2' && dropchest[index_input_chestdrop].drop[item[0]].empty())swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                    }
                    else if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false && dropchest[index_input_chestdrop].drop[item[1]].empty() == false) {
                        for (int l = dropchest[index_input_chestdrop].drop[item[0]].find('x') + 1; l != dropchest[index_input_chestdrop].drop[item[0]].size(); l++) {
                            if (l > dropchest[index_input_chestdrop].drop[item[0]].find(' '))temp_text[0] += dropchest[index_input_chestdrop].drop[item[0]][l];
                            else if (l < dropchest[index_input_chestdrop].drop[item[0]].find(' '))temp_text[2] += dropchest[index_input_chestdrop].drop[item[0]][l];
                        }
                        for (int l = dropchest[index_input_chestdrop].drop[item[1]].find('x') + 1; l != dropchest[index_input_chestdrop].drop[item[1]].size(); l++) {
                            if (l > dropchest[index_input_chestdrop].drop[item[1]].find(' '))temp_text[1] += dropchest[index_input_chestdrop].drop[item[1]][l];
                            else if (l < dropchest[index_input_chestdrop].drop[item[1]].find(' '))temp_text[3] += dropchest[index_input_chestdrop].drop[item[1]][l];
                        }
                        if (choiceswap == '2') {
                            if (temp_text[0] != temp_text[1])swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                            else if (temp_text[0] == temp_text[1]) {
                                if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                                    swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                                }
                                else if (stoi(temp_text[2]) == 12 && stoi(temp_text[3]) < 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) < 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) == 12) {
                                    if (stoi(temp_text[3]) == 12)error_input = 6;
                                    else if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                                        dropchest[index_input_chestdrop].drop[item[0]] = "";
                                        dropchest[index_input_chestdrop].drop[item[1]] = "x12 " + temp_text[0];
                                    }
                                    else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                                        temp_text[2] = std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3])));
                                        dropchest[index_input_chestdrop].drop[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                                        dropchest[index_input_chestdrop].drop[item[1]] = "x12 " + temp_text[0];
                                    }
                                    else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                                        temp_text[3] = std::to_string((stoi(temp_text[3]) + stoi(temp_text[2])));
                                        dropchest[index_input_chestdrop].drop[item[0]] = "";
                                        dropchest[index_input_chestdrop].drop[item[1]] = "x" + temp_text[3] + " " + temp_text[0];
                                    }
                                }
                            }
                        }
                        else if (choiceswap == '1') {
                            if (temp_text[0] != temp_text[1])error_input = 5;
                            else if (temp_text[0] == temp_text[1]) {
                                if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                                    swap(dropchest[index_input_chestdrop].drop[item[0]], dropchest[index_input_chestdrop].drop[item[1]]);
                                }
                                else if (stoi(temp_text[3]) == 12)error_input = 6;
                                else if (stoi(temp_text[3]) < 12) {
                                    temp_text[2] = std::to_string(stoi(temp_text[2]) - 1);
                                    if (stoi(temp_text[2]) == 0)dropchest[index_input_chestdrop].drop[item[0]] = "";
                                    else { dropchest[index_input_chestdrop].drop[item[0]] = "x" + temp_text[2] + " " + temp_text[0]; }
                                    temp_text[3] = std::to_string(stoi(temp_text[3]) + 1);
                                    dropchest[index_input_chestdrop].drop[item[1]] = "x" + temp_text[3] + " " + temp_text[1];
                                }
                            }
                        }
                    }
                }
                else if (choice == '3') {
                    if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false && inventory[item[1]].empty() || dropchest[index_input_chestdrop].drop[item[0]].empty() && inventory[item[1]].empty() == false) {
                        if (choiceswap == '2') {
                            ejectitem[2] = "1";
                            if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                                ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                            }
                            if (inventory[item[1]].empty() == false) {
                                ejectitem[0] = "+ " + inventory[item[1]];
                                ejectitem[1] = "- " + inventory[item[1]];
                            }
                            swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                        }
                        else if (choiceswap == '1') {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                                ejectitem[2] = "1";
                                if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                                    ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                    ejectitem[1] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                                }
                                if (inventory[item[1]].empty() == false) {
                                    ejectitem[0] = "+ " + inventory[item[1]];
                                    ejectitem[1] = "- " + inventory[item[1]];
                                }
                                swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                            }
                            else if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                                if (stoi(temp_text[2]) == 1) {
                                    dropchest[index_input_chestdrop].drop[item[0]] = "";
                                    inventory[item[1]] = "x1 " + temp_text[0];
                                }
                               else {
                                    temp_text[2] = std::to_string(stoi(temp_text[2]) - 1);
                                    dropchest[index_input_chestdrop].drop[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                                    inventory[item[1]] = "x1 " + temp_text[0];
                                }
                                ejectitem[2] = "1";
                                ejectitem[0] = "- x1 " + temp_text[0];
                                ejectitem[1] = "+ x1 " + temp_text[0];
                            }
                            else error_input = 14;
                        }
                    }
                    else if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false && inventory[item[1]].empty() == false) {
                        if (temp_text[0] == temp_text[1] || temp_text[0].find('-') + 1 == temp_text[0].find("топор") && temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") && temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                                swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                                ejectitem[2] = "2";
                                ejectitem[0] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "- " + inventory[item[1]];
                            }
                            else if (choiceswap == '1') {
                                if (stoi(temp_text[3]) != 12) {
                                    if (stoi(temp_text[2]) == 1)dropchest[index_input_chestdrop].drop[item[0]] = "";
                                    else dropchest[index_input_chestdrop].drop[item[0]] = "x" + std::to_string(stoi(temp_text[2]) - 1) + " " + temp_text[1];;
                                    inventory[item[1]] = "x" + std::to_string(stoi(temp_text[3]) + 1) + " " + temp_text[1];
                                    ejectitem[2] = "1";
                                    ejectitem[0] = "- x1 " + temp_text[1];
                                    ejectitem[1] = "+ x1 " + temp_text[1];
                                }
                                else error_input = 6;
                            }
                            else if (choiceswap == '2') {
                                if (stoi(temp_text[2]) == 12 && stoi(temp_text[3]) < 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) == 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) < 12) {
                                    if (stoi(temp_text[3]) == 12)error_input = 6;
                                    else if (12 - stoi(temp_text[3]) == stoi(temp_text[2])) {
                                        ejectitem[2] = "1";
                                        ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                        ejectitem[1] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                                        dropchest[index_input_chestdrop].drop[item[0]] = "";
                                        inventory[item[1]] = "x12 " + temp_text[0];
                                    }
                                    else if (12 - stoi(temp_text[3]) < stoi(temp_text[2])) {
                                        temp_text[2] = std::to_string(stoi(temp_text[2]) - (12 - stoi(temp_text[3])));
                                        ejectitem[2] = "1";
                                        ejectitem[0] = "- x" + std::to_string(12 - stoi(temp_text[3])) + " " + temp_text[0];
                                        ejectitem[1] = "+ x" + std::to_string(12 - stoi(temp_text[3])) + " " + temp_text[0];
                                        dropchest[index_input_chestdrop].drop[item[0]] = "x" + temp_text[2] + " " + temp_text[0];
                                        inventory[item[1]] = "x12" + temp_text[1];
                                    }
                                    else if (12 - stoi(temp_text[3]) > stoi(temp_text[2])) {
                                        temp_text[2] = std::to_string((stoi(temp_text[2]) + stoi(temp_text[3])));
                                        ejectitem[2] = "1";
                                        ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                        ejectitem[1] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                                        dropchest[index_input_chestdrop].drop[item[0]] = "";
                                        inventory[item[1]] = "x" + temp_text[2] + " " + temp_text[1];
                                    }
                                }
                            }
                        }
                        else {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                                error_input = 24;
                            }
                            else if (choiceswap == '1')error_input = 5;
                            else {
                                ejectitem[2] = "2";
                                ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "+ " + inventory[item[1]];
                                swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                            }
                        }
                    }
                }
                else if (choice == '4') {
                    if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false && inventory[item[1]].empty() || dropchest[index_input_chestdrop].drop[item[0]].empty() && inventory[item[1]].empty() == false) {
                        if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                            ejectitem[2] = "1";
                            if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                                ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                            }
                            if (inventory[item[1]].empty() == false) {
                                ejectitem[0] = "+ " + inventory[item[1]];
                                ejectitem[1] = "- " + inventory[item[1]];
                            }
                            swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                        }
                        else if (choiceswap == '2') {
                            ejectitem[2] = "1";
                            if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false) {
                                ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "+ " + dropchest[index_input_chestdrop].drop[item[0]];
                            }
                            if (inventory[item[1]].empty() == false) {
                                ejectitem[0] = "+ " + inventory[item[1]];
                                ejectitem[1] = "- " + inventory[item[1]];
                            }
                            swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                        }
                        else if (choiceswap == '1') {
                            if (inventory[item[1]].empty() == false) {
                                if (stoi(temp_text[3]) - 1 == 0) {
                                    inventory[item[1]] = "";
                                    dropchest[index_input_chestdrop].drop[item[0]] = "x1 " + temp_text[1];
                                }
                                else {
                                    temp_text[3] = std::to_string(stoi(temp_text[3]) - 1);
                                    inventory[item[1]] = "x" + temp_text[3] + " " + temp_text[1];
                                    dropchest[index_input_chestdrop].drop[item[0]] = "x1 " + temp_text[1];
                                }
                                ejectitem[2] = "1";
                                ejectitem[0] = "+ x1 " + temp_text[1];
                                ejectitem[1] = "- x1 " + temp_text[1];
                            }
                            else error_input = 14;
                        }
                    }
                    else if (dropchest[index_input_chestdrop].drop[item[0]].empty() == false && inventory[item[1]].empty() == false) {
                        if (temp_text[0] == temp_text[1] || temp_text[0].find('-') + 1 == temp_text[0].find("топор") && temp_text[1].find('-') + 1 == temp_text[1].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") && temp_text[1].find('-') + 1 == temp_text[1].find("кирка")) {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка")) {
                                ejectitem[2] = "2";
                                ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "+ " + inventory[item[1]];
                                swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                            }
                            else if (choiceswap == '1') {
                                if (stoi(temp_text[2]) != 12) {
                                    if (stoi(temp_text[3]) == 1)inventory[item[1]] = "";
                                    else inventory[item[1]] = "x" + std::to_string(stoi(temp_text[3]) - 1) + " " + temp_text[1];
                                    dropchest[index_input_chestdrop].drop[item[0]] = "x" + std::to_string(stoi(temp_text[2]) + 1) + " " + temp_text[0];
                                    ejectitem[2] = "1";
                                    ejectitem[0] = "+ x1 " + temp_text[1];
                                    ejectitem[1] = "- x1 " + temp_text[1];
                                }
                                else error_input = 6;
                            }
                            else if (choiceswap == '2') {
                                if (stoi(temp_text[2]) == 12 && stoi(temp_text[3]) < 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) == 12 || stoi(temp_text[2]) < 12 && stoi(temp_text[3]) < 12) {
                                    if (stoi(temp_text[2]) == 12)error_input = 6;
                                    else if (12 - stoi(temp_text[2]) == stoi(temp_text[3])) {
                                        ejectitem[2] = "1";
                                        ejectitem[0] = "+ " + inventory[item[1]];
                                        ejectitem[1] = "- " + inventory[item[1]];
                                        inventory[item[1]] = "";
                                        dropchest[index_input_chestdrop].drop[item[0]] = "x12 " + temp_text[1];
                                    }
                                    else if (12 - stoi(temp_text[2]) < stoi(temp_text[3])) {
                                        temp_text[3] = std::to_string(stoi(temp_text[3]) - (12 - stoi(temp_text[2])));
                                        ejectitem[2] = "1";
                                        ejectitem[0] = "+ x" + std::to_string(12 - stoi(temp_text[2])) + " " + temp_text[1];
                                        ejectitem[1] = "- x" + std::to_string(12 - stoi(temp_text[2])) + " " + temp_text[1];
                                        inventory[item[1]] = "x" + temp_text[3] + " " + temp_text[1];
                                        dropchest[index_input_chestdrop].drop[item[0]] = "x12" + temp_text[1];
                                    }
                                    else if (12 - stoi(temp_text[2]) > stoi(temp_text[3])) {
                                        ejectitem[2] = "1";
                                        ejectitem[0] = "+ " + temp_text[3] + " " + temp_text[1];
                                        ejectitem[1] = "- " + temp_text[3] + " " + temp_text[1];
                                        temp_text[3] = std::to_string((stoi(temp_text[3]) + stoi(temp_text[2])));
                                        inventory[item[1]] = "";
                                        dropchest[index_input_chestdrop].drop[item[0]] = "x" + temp_text[3] + " " + temp_text[0];
                                    }
                                }
                            }
                        }
                        else {
                            if (temp_text[0].find('-') + 1 == temp_text[0].find("топор") || temp_text[1].find('-') + 1 == temp_text[0].find("топор") || temp_text[0].find('-') + 1 == temp_text[0].find("кирка") || temp_text[10].find('-') + 1 == temp_text[0].find("кирка")) {
                                error_input = 24;
                            }
                            else if (choiceswap == '1') {
                                error_input = 5;
                            }
                            else {
                                ejectitem[2] = "2";
                                ejectitem[0] = "- " + dropchest[index_input_chestdrop].drop[item[0]];
                                ejectitem[1] = "+ " + inventory[item[1]];
                                swap(dropchest[index_input_chestdrop].drop[item[0]], inventory[item[1]]);
                            }
                        }
                    }
                }
            }
        }
    }
    else choiceswap = 'e';
    temp_text[0] = ""; temp_text[1] = ""; temp_text[2] = ""; temp_text[3] = ""; choice = ' ';
}
//MoveItem - Перемещать/стаковать предмет
void SwapObject(int line_of_sight, int loc[4]) {
    do {
        if (error_input != 13 || error_input != 24) {
            ClearLineScreen();
            if (input_chest == 0)InputConsoleInvent();
            else if (input_chest == 1) { InputConsoleTimeDrop(loc); InputConsoleInvent(); }
            else if (input_chest == 2) { InputConsoleDropChest(); InputConsoleInvent(); }
            else if (input_chest == 3) { InputConsoleCraftTable(); InputConsoleInvent(); }
            cout << "\nВыберете действие:\n[1] - Переместить x1 премет | [2] - Переместить все преметы в ячейке | [e] - Выйти в инвентарь\nВаш выбор: "; cin >> choiceswap;
            while (choiceswap != '1' && choiceswap != '2' && choiceswap != 'e' && choiceswap != 'E') {
                error_input = 1;
                ClearLineScreen();
                if (input_chest == 0)InputConsoleInvent();
                else if (input_chest == 1) { InputConsoleTimeDrop(loc); InputConsoleInvent(); }
                else if (input_chest == 2) { InputConsoleDropChest(); InputConsoleInvent(); }
                else if (input_chest == 3) { InputConsoleCraftTable(); InputConsoleInvent(); }
                cout << "\nВыберете действие:\n[1] - Переместить x1 премет | [2] - Переместить все преметы в ячейке | [e] - Выйти в инвентарь";
                if (error_input == 0) { cout << "\nВаш выбор: "; cin >> choiceswap; }
                else if (error_input != 0) { cout << "\nВведите ваш выбор снова снова: "; cin >> choiceswap; }
            }
            error_input = 0;
            if (choiceswap != 'e' && choiceswap != 'E')MoveItem(error_input, item, line_of_sight, loc);
        }
        else MoveItem(error_input, item, line_of_sight, loc);
    } while (choiceswap != 'e' && choiceswap != 'E' && input_chest != 0);
    choiceswap = ' ';
}
//SwapObject - Меню перемещания предметов
//void ToInteract(int loc[4], sf::Sound& open_chest, sf::Sound& close_chest, sf::Sound& craft_sound, sf::Sound& throw_away);//Взаимодейстиве с структурами
void CraftTable();//Вызов верстака или крафт в инвентаре
void InventoryPlayer(int& error_input, int item[2], int line_of_sight, int loc[4],sf::Sound& craft_sound, sf::Sound& throw_away){
    ClearLineScreen();
    static char choice = ' ';
    do {
        ClearLineScreen();
        if (craft_aftermath != "") { CraftAftermathDelete(); }
        if (craft.size() == 4 && input_chest == 0) {
            for (int i = 0; i != 4; i++) {
                if (craft[i].empty() == false) { ItemCraftAdd(); break; }
            }
        }
        else if (craft.size() == 9 && input_chest == 3) {
            for (int i = 0; i != 9; i++) {
                if (craft[i].empty() == false) { ItemCraftAdd(); break; }
            }
        }
        if (input_chest == 0)InputConsoleInvent();
        else if (input_chest == 1) { InputConsoleTimeDrop(loc); InputConsoleInvent(); }
        else if (input_chest == 2) { InputConsoleDropChest(); InputConsoleInvent(); }
        else if (input_chest == 3) { InputConsoleCraftTable(); InputConsoleInvent(); }
        do {
            if (input_chest == 0)cout << "\nВыберете действие:\n[1] - Переместить предмет | [2] - Выбросить предмет | [3] - Поместить предмет в руку \n[c] - Крафт | [e] - Выйти из инвентаря";
            else if (input_chest == 1 || input_chest == 2)cout << "\nВыберете действие:\n[1] - Переместить предмет | [2] - Поместить предмет в руку \n[e] - Выйти из инвентаря";
            else if (input_chest == 3)cout << "\nВыберете действие:\n[1] - Переместить предмет | [2] - Поместить предмет в руку  | [c] - Крафт\n[e] - Выйти из инвентаря";
            if (error_input == 0) { cout << "\nВаш выбор: "; cin >> choice; }
            else if (error_input != 0) { cout << "\nВведите ваш выбор снова снова: "; cin >> choice; }
            if (choice == '1' || choice == '2' || choice == '3' || choice == 'c' || choice == 'C' || choice == 'e' || choice == 'E' && input_chest == 0 || input_chest == 3)error_input = 0;
            else if (choice == '1' || choice == '2' || choice == 'e' || choice == 'E' && input_chest == 1 || input_chest == 2)error_input = 0;
            else {
                if (input_chest == 0) { error_input = 1; ClearLineScreen(); InputConsoleInvent(); }
                else if (input_chest == 1) { error_input = 1; ClearLineScreen(); InputConsoleTimeDrop(loc); InputConsoleInvent(); }
                else if (input_chest == 2) { error_input = 1; ClearLineScreen(); InputConsoleDropChest(); InputConsoleInvent(); }
                else if (input_chest == 3) { error_input = 1; ClearLineScreen(); InputConsoleCraftTable(); InputConsoleInvent(); }
            }
        } while (error_input != 0);
        if (choice == '1')SwapObject(line_of_sight, loc);
        else if (choice == '2' && input_chest == 0)EjectionOfObjects(line_of_sight, loc, structure, timedrop, throw_away);
        else if (choice == '2' && input_chest == 1 || choice == '2' && input_chest == 2 || choice == '2' && input_chest == 3)PutInHand(line_of_sight, loc);
        else if (choice == '3' && input_chest == 0)PutInHand(line_of_sight, loc);
        else if (choice == 'c' || choice == 'C') {
            if (craft_aftermath == "") { CraftTable(); Craft(craft_sound); }
            else error_input = 21;
        }
        else if (choice == 'e' || choice == 'E')input_chest = 0;
        ClearLineScreen();
    } while (choice != 'e' && choice != 'E');
    ClearLineScreen();
    choice = ' ';
}
//InventoryPlayer - Инввентарь игрока
#pragma endregion
#pragma region Structure
void GiveItemTimeDrop(int loc[4], sf::Sound& craft_sound, sf::Sound& throw_away) {
    ClearLineScreen();
    for (int i = 0; i != timedrop.size(); i++) {
        if (structure[timedrop[i].loc_in_structure].location[0] == loc[0] && structure[timedrop[i].loc_in_structure].location[1] == loc[1]) {
            if (line_of_sight == 1 && structure[timedrop[i].loc_in_structure].location[2] == loc[2] - 1 && structure[timedrop[i].loc_in_structure].location[3] == loc[3]) {
                index_input_tempdrop = i; break;
            }
            else if (line_of_sight == 2 && structure[timedrop[i].loc_in_structure].location[2] == loc[2] + 1 && structure[timedrop[i].loc_in_structure].location[3] == loc[3]) {
                index_input_tempdrop = i; break;
            }
            else if (line_of_sight == 3 && structure[timedrop[i].loc_in_structure].location[2] == loc[2] && structure[timedrop[i].loc_in_structure].location[3] == loc[3] + 1) {
                index_input_tempdrop = i; break;
            }
            else if (line_of_sight == 4 && structure[timedrop[i].loc_in_structure].location[2] == loc[2] && structure[timedrop[i].loc_in_structure].location[3] == loc[3] - 1) {
                index_input_tempdrop = i; break;
            }
        }
    }
    input_chest = 1;
    while (input_chest != 0) {
        InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away);
    }
}
//Добавление предметов в временный сундук
void CraftTable() {
    if (input_chest == 0) {
        craft.resize(4);
        for (int i = 0; i != 4; i++)craft[i] = "";
    }
    else if (input_chest == 3) {
        craft.resize(9);
        for (int i = 0; i != 9; i++)craft[i] = "";
    }
}
//CraftTable - Вызов верстака или крафт в инвентаре
void FindIndexChest(int loc[4]) {
    for (int i = 0; i != dropchest.size(); i++) {
        if (structure[dropchest[i].loc_in_structure].location[0] == loc[0] && structure[dropchest[i].loc_in_structure].location[1] == loc[1]) {
            if (line_of_sight == 1 && structure[dropchest[i].loc_in_structure].location[2] == loc[2] - 1 && structure[dropchest[i].loc_in_structure].location[3] == loc[3]) {
                index_input_chestdrop = i; break;
            }
            else if (line_of_sight == 2 && structure[dropchest[i].loc_in_structure].location[2] == loc[2] + 1 && structure[dropchest[i].loc_in_structure].location[3] == loc[3]) {
                index_input_chestdrop = i; break;
            }
            else if (line_of_sight == 3 && structure[dropchest[i].loc_in_structure].location[2] == loc[2] && structure[dropchest[i].loc_in_structure].location[3] == loc[3] + 1) {
                index_input_chestdrop = i; break;
            }
            else if (line_of_sight == 4 && structure[dropchest[i].loc_in_structure].location[2] == loc[2] && structure[dropchest[i].loc_in_structure].location[3] == loc[3] - 1) {
                index_input_chestdrop = i; break;
            }
        }
    }
}
//FindIndexChest - Поиск индекса сундука в массиве structure
void Chest(int loc[4],sf::Sound& open_chest, sf::Sound& close_chest, sf::Sound& craft_sound, sf::Sound& throw_away) {
    ClearLineScreen();
    FindIndexChest(loc);
    input_chest = 2;
    open_chest.play();
    while (input_chest != 0) {
        InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away);
    }
    close_chest.play();
}
//Вызова сундука
void CheckEmptyCraft() {
    if (craft.size() == 4) {
        for (int i = 0; i != 4; i++) {
            if (craft[i].empty() == false) { error = 13; break; }
        }
    }
    else if (craft.size() == 9) {
        for (int i = 0; i != 9; i++) {
            if (craft[i].empty() == false) { error = 13; break; }
        }
    }
    if (craft_aftermath != "")error = 13;
}
//CheckEmptyCraft - Проверка на пустоту крафта(это зделано для того, если при крафте в версатке/крафта в инвентаре, остались предметы в ячейках крафта(остаються если нету для них места в инвентаре) и вы не сможете пользоваться крафтом пока не освобоите ячейки в инветаре
void ToInteract(int loc[4], sf::Sound& open_chest, sf::Sound& close_chest, sf::Sound& craft_sound, sf::Sound& throw_away) {
    if (line_of_sight == 1) {
        if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'T')GiveItemTimeDrop(loc, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'C')Chest(loc, open_chest, close_chest, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'W') {
            CheckEmptyCraft();
            if (error != 13) { input_chest = 3; CraftTable(); InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away); }
        }
        else error = 2;
    }
    else if (line_of_sight == 2) {
        if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'T')GiveItemTimeDrop(loc, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'C')Chest(loc, open_chest, close_chest, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'W') {
            CheckEmptyCraft();
            if (error != 13) { input_chest = 3; CraftTable(); InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away); }
        }
        else error = 2;
    }
    else if (line_of_sight == 3) {
        if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'T')GiveItemTimeDrop(loc, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'C')Chest(loc, open_chest, close_chest, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'W') {
            CheckEmptyCraft();
            if (error != 13) { input_chest = 3; CraftTable(); InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away); }
        }
        else error = 2;
    }
    else if (line_of_sight == 4) {
        if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'T')GiveItemTimeDrop(loc, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'C')Chest(loc, open_chest, close_chest, craft_sound, throw_away);
        else if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'W') {
            CheckEmptyCraft();
            if (error != 13) { input_chest = 3; CraftTable(); InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away); }
        }
        else error = 2;
    }
}
//ToInterac - Определение структуры с которым взаемодействует игрок
void PlantPut(int loc[4], string check_text[2],sf::Sound& planting, sf::Sound& put_structure1, sf::Sound& put_structure2) {
    srand(time(0));
    if (line_of_sight == 1) {
        if (world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == ' ' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == '^') {
            structure.resize(structure.size() + 1);
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2] - 1; structure[structure.size() - 1].location[3] = loc[3];
            if (stoi(check_text[1]) > 1)inventory[hand] = "x" + std::to_string(stoi(check_text[1]) - 1) + " " + check_text[0];
            else if (stoi(check_text[1]) == 1)inventory[hand] = "";
            if (check_text[0] == "сундук") {
                structure[structure.size() - 1].name = 'C';
                structure[structure.size() - 1].hp = 4;
                dropchest.resize(dropchest.size() + 1);
                dropchest[dropchest.size() - 1].loc_in_structure = structure.size() - 1;
                world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = 'C';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "верстак") {
                structure[structure.size() - 1].name = 'W';
                structure[structure.size() - 1].hp = 4;
                world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = 'W';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "саженца") {
                planting.play();
                tree.resize(tree.size() + 1);
                tree[tree.size() - 1].loc_in_structure = structure.size() - 1;
                structure[structure.size() - 1].name = 'y';
                structure[structure.size() - 1].hp = 0.5;
                tree[tree.size() - 1].hp = rand() % (140 - 120 + 1) + 120;
                world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = 'y';
            }
            else if (check_text[0] == "факел") {
                structure[structure.size() - 1].name = 'i';
                structure[structure.size() - 1].hp = 0.5;
                world[loc[0]][loc[1]][loc[2] - 1][loc[3]] = 'i';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
        }
        else error = 10;
    }
    else if (line_of_sight == 2) {
        if (world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == ' ' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'v') {
            if (stoi(check_text[1]) > 1)inventory[hand] = "x" + std::to_string(stoi(check_text[1]) - 1) + " " + check_text[0];
            else if (stoi(check_text[1]) == 1)inventory[hand] = "";
            structure.resize(structure.size() + 1);
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2] + 1; structure[structure.size() - 1].location[3] = loc[3];
            if (check_text[0] == "сундук") {
                structure[structure.size() - 1].name = 'C';
                structure[structure.size() - 1].hp = 4;
                dropchest.resize(dropchest.size() + 1);
                dropchest[dropchest.size() - 1].loc_in_structure = structure.size() - 1;
                world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'C';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "верстак") {
                structure[structure.size() - 1].name = 'W';
                structure[structure.size() - 1].hp = 4;
                world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'W';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "саженца") {
                planting.play();
                tree.resize(tree.size() + 1);
                tree[tree.size() - 1].loc_in_structure = structure.size() - 1;
                structure[structure.size() - 1].name = 'y';
                structure[structure.size() - 1].hp = 0.5;
                tree[tree.size() - 1].hp = (rand() % (140 - 120 + 1) + 120);
                world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'y';
            }
            else if (check_text[0] == "факел") {
                structure[structure.size() - 1].name = 'i';
                structure[structure.size() - 1].hp = 0.5;
                world[loc[0]][loc[1]][loc[2] + 1][loc[3]] = 'i';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
        }
        else error = 10;
    }
    else if (line_of_sight == 3) {
        if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == ' ' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == '>') {
            if (stoi(check_text[1]) > 1)inventory[hand] = "x" + std::to_string(stoi(check_text[1]) - 1) + " " + check_text[0];
            else if (stoi(check_text[1]) == 1)inventory[hand] = "";
            structure.resize(structure.size() + 1);
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2]; structure[structure.size() - 1].location[3] = loc[3] + 1;
            if (check_text[0] == "сундук") {
                structure[structure.size() - 1].name = 'C';
                structure[structure.size() - 1].hp = 4;
                dropchest.resize(dropchest.size() + 1);
                dropchest[dropchest.size() - 1].loc_in_structure = structure.size() - 1;
                world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = 'C';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "верстак") {
                structure[structure.size() - 1].name = 'W';
                structure[structure.size() - 1].hp = 4;
                world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = 'W';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "саженца") {
                planting.play();
                tree.resize(tree.size() + 1);
                tree[tree.size() - 1].loc_in_structure = structure.size() - 1;
                structure[structure.size() - 1].name = 'y';
                structure[structure.size() - 1].hp = 0.5;
                tree[tree.size() - 1].hp = (rand() % (140 - 120 + 1) + 120);
                world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = 'y';
            }
            else if (check_text[0] == "факел") {
                structure[structure.size() - 1].name = 'i';
                structure[structure.size() - 1].hp = 0.5;
                world[loc[0]][loc[1]][loc[2]][loc[3] + 1] = 'i';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
        }
        else error = 10;
    }
    else if (line_of_sight == 4) {
        if (world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == ' ' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == '<') {
            if (stoi(check_text[1]) > 1)inventory[hand] = "x" + std::to_string(stoi(check_text[1]) - 1) + " " + check_text[0];
            else if (stoi(check_text[1]) == 1)inventory[hand] = "";
            structure.resize(structure.size() + 1);
            structure[structure.size() - 1].location[0] = loc[0]; structure[structure.size() - 1].location[1] = loc[1];
            structure[structure.size() - 1].location[2] = loc[2]; structure[structure.size() - 1].location[3] = loc[3] - 1;
            if (check_text[0] == "сундук") {
                structure[structure.size() - 1].name = 'C';
                structure[structure.size() - 1].hp = 4;
                dropchest.resize(dropchest.size() + 1);
                dropchest[dropchest.size() - 1].loc_in_structure = structure.size() - 1;
                world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = 'C';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "верстак") {
                structure[structure.size() - 1].name = 'W';
                structure[structure.size() - 1].hp = 4;
                world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = 'W';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
            else if (check_text[0] == "саженца") {
                planting.play();
                tree.resize(tree.size() + 1);
                tree[tree.size() - 1].loc_in_structure = structure.size() - 1;
                structure[structure.size() - 1].name = 'y';
                structure[structure.size() - 1].hp = 0.5;
                tree[tree.size() - 1].hp = (rand() % (140 - 120 + 1) + 120);
                world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = 'y';
            }
            else if (check_text[0] == "факел") {
                structure[structure.size() - 1].name = 'i';
                structure[structure.size() - 1].hp = 0.5;
                world[loc[0]][loc[1]][loc[2]][loc[3] - 1] = 'i';
                if (rand() % (2 - 1 + 1) + 1 == 1)put_structure1.play();
                else put_structure2.play();
            }
        }
        else error = 10;
    }
    check_text[0] = ""; check_text[1] = "";
}
//PlantPut - Поставить структуру(сундук, верстак и т.д.
#pragma endregion
#pragma region SpawnRespawn
void SpawnStone(int i, int l) {
    srand(time(0));
    int lines, columns;
    for (int j = 0; j != 4; j++) {
        if (l == 0) {
            if (i == 0) {
                lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6;
                columns = rand() % ((world[i][l][0].size() - 2) - 6 + 1) + 6;
            }
            else if (i == world.size() - 1) {
                lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1;
                columns = rand() % ((world[i][l][0].size() - 2) - 6 + 1) + 6;
            }
            else if (i > 0 && i < world.size() - 1) {
                lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6;
                columns = rand() % ((world[i][l][0].size() - 2) - 6 + 1) + 6;
            }
        }
        else if (i == 0) {
            if (world[i].size() > world[i + 1].size()) {
                if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
            else if (world[i].size() < world[i + 1].size() || world[i].size() == world[i + 1].size()) {
                if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
        }
        else if (i == world.size() - 1) {
            if (world[i].size() > world[i - 1].size()) {
                if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
            else if (world[i].size() < world[i - 1].size() || world[i].size() == world[i - 1].size()) {
                if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
        }
        else if (i > 0 && i < world.size() - 1) {
            if (world[i].size() > world[i + 1].size() && world[i].size() > world[i - 1].size()) {
                if (world[i - 1].size() > world[i + 1].size()) {
                    if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
                else if (world[i - 1].size() < world[i + 1].size()) {
                    if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
                else if (world[i - 1].size() == world[i + 1].size()) {
                    if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
            }
            else if (world[i].size() > world[i + 1].size() && world[i].size() == world[i - 1].size()) {
                if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
            else if (world[i].size() == world[i + 1].size() && world[i].size() > world[i - 1].size()) {
                if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 1) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
            else if (world[i].size() == world[i + 1].size() && world[i].size() == world[i - 1].size()) {
                if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
            else if (world[i].size() < world[i + 1].size() && world[i + 1].size() == world[i - 1].size()) {
                if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 1) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
            }
            else if (world[i + 1].size() > world[i].size() && world[i + 1].size() > world[i - 1].size()) {
                if (world[i].size() > world[i - 1].size()) {
                    if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 6) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
                else if (world[i].size() < world[i - 1].size() || world[i].size() == world[i - 1].size()) {
                    if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
            }
            else if (world[i - 1].size() > world[i].size() && world[i - 1].size() > world[i + 1].size()) {
                if (world[i].size() > world[i + 1].size()) {
                    if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
                else if (world[i].size() < world[i + 1].size() || world[i].size() == world[i + 1].size()) {
                    if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                }
            }
        }
        if (world[i][l][lines][columns] == ' ') { world[i][l][lines][columns] = 'S'; break; }
    }
}
//SpawnStone - Генерация камня
void RespawnStone() {
    int amountstone = 0, i = 0, l = 0;
    for (i = 0; i != world.size(); i++) {
        for (l = 0; l != world[i].size(); l++) {
            if (research_map[i][l] == true) {
                if (timerespawne >= 120) {
                    for (int j = 0; j != world[i][l].size(); j++) {
                        for (int f = 0; f != world[i][l][j].size(); f++) {
                            if (world[i][l][j][f] == 'S')amountstone++;
                        }
                    }
                    if (amountstone <= 2) {
                        for (int j = 0; j != 3; j++) {
                            SpawnStone(i, l);
                        }
                    }
                    timerespawne = 0;
                }
            }
        }
    }
}
//RespawnStone - Проверяет достиг ли счетчик времени определенного значения для респавна камня
void TreeGrowth(double timeStructure) {
    int num = 1;
    if (tree.size() != 0) {
        for (int i = 0; i != tree.size(); i++)tree[i].hp -= timeStructure;
    }
    while (num != 0) {
        if (tree.size() != 0) {
            for (int i = 0; i != tree.size(); i++) {
                if (tree[i].hp <= 0) {
                    world[structure[tree[i].loc_in_structure].location[0]][structure[tree[i].loc_in_structure].location[1]][structure[tree[i].loc_in_structure].location[2]][structure[tree[i].loc_in_structure].location[3]] = 'Y';
                    for (int l = 0; l != tree.size(); l++) {
                        if (structure.size() - 1 == tree[l].loc_in_structure) {
                            tree[l].loc_in_structure = tree[i].loc_in_structure;
                            break;
                        }
                    }
                    for (int l = 0; l != timedrop.size(); l++) {
                        if (structure.size() - 1 == timedrop[l].loc_in_structure) {
                            timedrop[l].loc_in_structure = tree[i].loc_in_structure;
                            break;
                        }
                    }
                    for (int l = 0; l != dropchest.size(); l++) {
                        if (structure.size() - 1 == dropchest[l].loc_in_structure) {
                            dropchest[l].loc_in_structure = tree[i].loc_in_structure;
                            break;
                        }
                    }
                    swap(structure[tree[i].loc_in_structure], structure[structure.size() - 1]);
                    swap(tree[i], tree[tree.size() - 1]);
                    structure.resize(structure.size() - 1); tree.resize(tree.size() - 1);
                    num = 1; break;
                }
                else num = 0;
            }
        }
        else num = 0;
    }
}
//TreeGrowth - Рост саженцев
#pragma endregion
#pragma region PlaySound
void ReproductionSoundTree(sf::Sound& breaking_wood_1, sf::Sound& breaking_wood_2, sf::Sound& breaking_wood_3, sf::Sound& breaking_wood_4, sf::Sound& breaking_wood_5, sf::Sound& hit_hand_1, sf::Sound& hit_hand_2, sf::Sound& hit_hand_3, sf::Sound& hit_hand_4) {
    //hit_hand_1.play();
    if (inventory[hand].find("топор") != string::npos) {
        int num = rand() % (5 - 1 + 1) + 1;
        if (num == 1) breaking_wood_1.play();
        else if (num == 2) breaking_wood_2.play();
        else if (num == 3) breaking_wood_3.play();
        else if (num == 4) breaking_wood_4.play();
        else if (num == 5) breaking_wood_5.play();
    }
    else {
        int num = rand() % (4 - 1 + 1) + 1;
        if (num == 1)hit_hand_1.play();
        else if (num == 2) hit_hand_2.play();
        else if (num == 3) hit_hand_3.play();
        else if (num == 4) hit_hand_4.play();
    }
}
//ReproductionSoundTre - звук ударов по дереву
void ReproductionSoundMove(sf::Sound& move_1, sf::Sound& move_2, sf::Sound& move_3, sf::Sound& move_4, sf::Sound& move_5) {
    int num = rand() % (5 - 1 + 1) + 1;
    if (num == 1)move_1.play();
    else if (num == 2)move_2.play();
    else if (num == 3)move_3.play();
    else if (num == 4)move_4.play();
    else if (num == 5)move_5.play();
}
//ReproductionSoundMove - звук ходьбы
#pragma endregion
void CheckTrueChoice(char& choice, int chek_input[3],int loc[4], string check_text[2],int& invent_info,sf::Sound& take,sf::Music& day_sound, sf::Music& night_sound) {
    do {
        cout << endl << "[w] - Вперед | [s] - Назад | [d] - В право | [a] - В лево";
        if (inventory[hand].empty() == false) {
            check_text[0] = ""; check_text[1] = "";
            for (int i = inventory[hand].find('x') + 1; i != inventory[hand].size(); i++) {
                if (i > inventory[hand].find(' '))check_text[0] += inventory[hand][i];
                else if (i < inventory[hand].find(' '))check_text[1] += inventory[hand][i];
            }
            if (check_text[0] == "факел" || check_text[0] == "сундук" || check_text[0] == "верстак") { chek_input[3] = 1; cout << " [p] - Поставить"; }
            else if (check_text[0] == "саженца") { chek_input[3] = 1; cout << " [p] - Посадить"; }
            else chek_input[3] = 0;
        }
        if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'T' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'C' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'W' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'T' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'C' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'W' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'T' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'C' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'W' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'T' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'C' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'W') { cout << " | [e] - Взаимодействовать"; chek_input[2] = 1; }
        else chek_input[2] = 0;
        if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'Y' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'Y' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'Y' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'Y' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'y' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'y' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'y' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'y' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'C' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'C' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'C' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'C' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'W' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'W' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'W' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'W' || world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'i' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'i' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'i' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'i') {
            cout << " | [c] - Рубить";
            chek_input[0] = 1;
        }
        else chek_input[0] = 0;
        if (world[loc[0]][loc[1]][loc[2]][loc[3] + 1] == 'S' || world[loc[0]][loc[1]][loc[2]][loc[3] - 1] == 'S' || world[loc[0]][loc[1]][loc[2] + 1][loc[3]] == 'S' || world[loc[0]][loc[1]][loc[2] - 1][loc[3]] == 'S') { cout << " | [b] - Розбить"; chek_input[1] = 1; }
        else chek_input[1] = 0;
        cout << endl << "[m] - Карта | [i] - Инвентарь | [0] - Меню";
        if (error == 0) { cout << "\nВаш выбор: "; cin >> choice; }
        else { cout << "\nВведите ваш выбор снова: "; cin >> choice; }
        if (choice == 'w' || choice == 'W' || choice == 's' || choice == 'S' || choice == 'd' || choice == 'D' || choice == 'a' || choice == 'A' || choice == 'm' || choice == 'M' || choice == 'c' || choice == 'C' || choice == 'b' || choice == 'B' || choice == 'i' || choice == 'I' || choice == 'e' || choice == 'E' || choice == 'p' || choice == 'P' || choice == '0')error = 0;
        else {
            error = 7;
            ClearLineScreen();
            InputLineOfSight(line_of_sight, loc);
            InputWorldConsole(world, loc, invent_info, line_of_sight,take, day_sound, night_sound);
        }
    } while (error != 0);
}
//CheckTrueChoice - код из функции Player
int Player() {
    #pragma region Sounds
    #pragma region Chop
    sf::SoundBuffer Breaking_Wood_Buff1;
    if (!Breaking_Wood_Buff1.loadFromFile("breaking_wood_1.ogg")) {
        return -1;
    }
    sf::Sound breaking_wood_1;
    breaking_wood_1.setBuffer(Breaking_Wood_Buff1);

    sf::SoundBuffer Breaking_Wood_Buff2;
    if (!Breaking_Wood_Buff2.loadFromFile("breaking_wood_2.ogg")) {
        return -1;
    }
    sf::Sound breaking_wood_2;
    breaking_wood_2.setBuffer(Breaking_Wood_Buff2);

    sf::SoundBuffer Breaking_Wood_Buff3;
    if (!Breaking_Wood_Buff1.loadFromFile("breaking_wood_3.ogg")) {
        return -1;
    }
    sf::Sound breaking_wood_3;
    breaking_wood_3.setBuffer(Breaking_Wood_Buff3);

    sf::SoundBuffer Breaking_Wood_Buff4;
    if (!Breaking_Wood_Buff4.loadFromFile("breaking_wood_4.ogg")) {
        return -1;
    }
    sf::Sound breaking_wood_4;
    breaking_wood_4.setBuffer(Breaking_Wood_Buff4);

    sf::SoundBuffer Breaking_Wood_Buff5;
    if (!Breaking_Wood_Buff5.loadFromFile("breaking_wood_5.ogg")) {
        return -1;
    }
    sf::Sound breaking_wood_5;
    breaking_wood_5.setBuffer(Breaking_Wood_Buff5);
        #pragma endregion
    #pragma region Hand
    sf::SoundBuffer Hit_HandBuff1;
    if (!Hit_HandBuff1.loadFromFile("hit_hand_1.ogg")) {
        return -1;
    }
    sf::Sound hit_hand_1;
    hit_hand_1.setBuffer(Hit_HandBuff1);

    sf::SoundBuffer Hit_HandBuff2;
    if (!Hit_HandBuff2.loadFromFile("hit_hand_2.ogg")) {
        return -1;
    }
    sf::Sound hit_hand_2;
    hit_hand_2.setBuffer(Hit_HandBuff2);

    sf::SoundBuffer Hit_HandBuff3;
    if (!Hit_HandBuff3.loadFromFile("hit_hand_3.ogg")) {
        return -1;
    }
    sf::Sound hit_hand_3;
    hit_hand_3.setBuffer(Hit_HandBuff3);

    sf::SoundBuffer Hit_HandBuff4;
    if (!Hit_HandBuff4.loadFromFile("hit_hand_4.ogg")) {
        return -1;
    }
    sf::Sound hit_hand_4;
    hit_hand_4.setBuffer(Hit_HandBuff4);
    #pragma endregion
    #pragma region Planting
    sf::SoundBuffer PlantingBuff;
    if (!PlantingBuff.loadFromFile("planting.ogg")) {
        return -1;
    }
    sf::Sound planting;
    planting.setBuffer(PlantingBuff);
    #pragma endregion
    #pragma region Chest
    sf::SoundBuffer Open_ChestBuff;
    if (!Open_ChestBuff.loadFromFile("open_chest.ogg")) {
        return -1;
    }
    sf::Sound open_chest;
    open_chest.setBuffer(Open_ChestBuff);

    sf::SoundBuffer Close_ChestBuff;
    if (!Close_ChestBuff.loadFromFile("close_chest.ogg")) {
        return -1;
    }
    sf::Sound close_chest;
    close_chest.setBuffer(Close_ChestBuff);
    #pragma endregion
    #pragma region Move
    sf::SoundBuffer MoveBuff1;
    if (!MoveBuff1.loadFromFile("move_1.ogg")) {
        return -1;
    }
    sf::Sound move_1;
    move_1.setBuffer(MoveBuff1);

    sf::SoundBuffer MoveBuff2;
    if (!MoveBuff2.loadFromFile("move_2.ogg")) {
        return -1;
    }
    sf::Sound move_2;
    move_2.setBuffer(MoveBuff2);

    sf::SoundBuffer MoveBuff3;
    if (!MoveBuff3.loadFromFile("move_3.ogg")) {
        return -1;
    }
    sf::Sound move_3;
    move_3.setBuffer(MoveBuff3);

    sf::SoundBuffer MoveBuff4;
    if (!MoveBuff4.loadFromFile("move_4.ogg")) {
        return -1;
    }
    sf::Sound move_4;
    move_4.setBuffer(MoveBuff4);

    sf::SoundBuffer MoveBuff5;
    if (!MoveBuff5.loadFromFile("move_5.ogg")) {
        return -1;
    }
    sf::Sound move_5;
    move_5.setBuffer(MoveBuff5);
    #pragma endregion
    #pragma region Map
    sf::SoundBuffer Record_MapBuff;
    if (!Record_MapBuff.loadFromFile("record_map.ogg")) {
        return -1;
    }
    sf::Sound record_map;
    record_map.setBuffer(Record_MapBuff);

    sf::SoundBuffer Paper_OpenBuff;
    if (!Paper_OpenBuff.loadFromFile("paper_open.ogg")) {
        return -1;
    }
    sf::Sound paper_open;
    paper_open.setBuffer(Paper_OpenBuff);
    #pragma endregion
    #pragma region Take_ThrAw_Item
    sf::SoundBuffer TakeBuff;
    if (!TakeBuff.loadFromFile("take.ogg")) {
        return -1;
    }
    sf::Sound take;
    take.setBuffer(TakeBuff);

    sf::SoundBuffer Throw_AwayBuff;
    if (!Throw_AwayBuff.loadFromFile("throw_away.ogg")) {
        return -1;
    }
    sf::Sound throw_away;
    throw_away.setBuffer(Throw_AwayBuff);
    #pragma endregion
    #pragma region Break_Struct
    sf::SoundBuffer Break_StructBuff;
    if (!Break_StructBuff.loadFromFile("break_struct.ogg")) {
        return -1;
    }
    sf::Sound break_struct;
    break_struct.setBuffer(Break_StructBuff);
    #pragma endregion
    #pragma region Pickaxe
    sf::SoundBuffer PickaxeBuff1;
    if (!PickaxeBuff1.loadFromFile("pickaxe1.ogg")) {
        return -1;
    }
    sf::Sound pickaxe1;
    pickaxe1.setBuffer(PickaxeBuff1);

    sf::SoundBuffer PickaxeBuff2;
    if (!PickaxeBuff2.loadFromFile("pickaxe2.ogg")) {
        return -1;
    }
    sf::Sound pickaxe2;
    pickaxe2.setBuffer(PickaxeBuff2);
    #pragma endregion
    #pragma region CraftSound
    sf::SoundBuffer CraftBuff;
    if (!CraftBuff.loadFromFile("craft_sound.ogg")) {
        return -1;
    }
    sf::Sound craft_sound;
    craft_sound.setBuffer(CraftBuff);
    #pragma endregion
    #pragma region Put_Structure
    sf::SoundBuffer Put_StructureBuff1;
    if (!Put_StructureBuff1.loadFromFile("put_structure1.ogg")) {
        return -1;
    }
    sf::Sound put_structure1;
    put_structure1.setBuffer(Put_StructureBuff1);

    sf::SoundBuffer Put_StructureBuff2;
    if (!Put_StructureBuff2.loadFromFile("put_structure2.ogg")) {
        return -1;
    }
    sf::Sound put_structure2;
    put_structure2.setBuffer(Put_StructureBuff2);
    #pragma endregion
    #pragma region Break_Tool
    sf::SoundBuffer Break_ToolBuff;
    if (!Break_ToolBuff.loadFromFile("break_tool.ogg")) {
        return -1;
    }
    sf::Sound break_tool;
    break_tool.setBuffer(Break_ToolBuff);
    #pragma endregion
    #pragma region Day_Night
    sf::Music day_sound;
    if (!day_sound.openFromFile("day_sound.ogg")) {
        return -1;
    }

    sf::Music night_sound;
    if (!night_sound.openFromFile("night_sound.ogg")) {
        return -1;
    }
    #pragma endregion

    #pragma endregion
    char choice = '5';//choice - Выбор действий
    int loc[4] = {}, chek_input[4] = { 0,0,0,0 }, invent_info = 0;
    //loc - Координаты игрока в мире| chek_input - проверка на то или иное дейсвтие(Например, если игрок хочет поставить что-то, но у него нет в руке что можна поставить, то в массив занесеться число 1 в определенный индекс свойственный разным ситуациям)
    string check_text[2] = { "","" };
    double timePlayer = 0, timeStructure = 0, timeDelay = 0, timeWorld = 0;
    static bool time_tracking = false;
    ChekPlayer(world, loc);
    while (choice != '4') {
        auto startPlayer = chrono::steady_clock::now();
        auto startStructure = chrono::steady_clock::now();
        auto startDelay = chrono::steady_clock::now();
        auto startWorld = chrono::steady_clock::now();
        ClearLineScreen();
        InputLineOfSight(line_of_sight, loc);//
        InputWorldConsole(world, loc, invent_info, line_of_sight,take, day_sound, night_sound);//
        CheckTrueChoice(choice, chek_input, loc, check_text, invent_info,take, day_sound, night_sound);
        if (choice == '0') {
            auto endPlayer = chrono::steady_clock::now();
            auto endStructure = chrono::steady_clock::now();
            if (check_fatigue == true) {
                auto endDelay = chrono::steady_clock::now();
                chrono::duration<double> elapsedSDelay = endDelay - startDelay;
                timeDelay = elapsedSDelay.count();
            }
            auto endWorld = chrono::steady_clock::now();
            chrono::duration<double> elapsedPlayer = endPlayer - startPlayer;
            chrono::duration<double> elapsedStructure = endStructure - startStructure;
            chrono::duration<double> elapsedWorld = endWorld - startWorld;
            timePlayer = elapsedPlayer.count();
            timeStructure = elapsedStructure.count(); timerespawne += timeStructure;
            timeWorld = elapsedWorld.count(); time_world += timeWorld;
            DeleteTimeDrop(timePlayer);
            RespawnStone(); TreeGrowth(timeStructure);
            Menu(choice,day_sound,night_sound);
            time_tracking = true;
            auto startStructure = chrono::steady_clock::now();
            auto startWorld = chrono::steady_clock::now();
        }
        else if (choice == 'w' || choice == 'W') { ReproductionSoundMove(move_1, move_2, move_3, move_4, move_5); DeleteLineOfSight(line_of_sight, loc); StepW(world, loc, line_of_sight, record_map); }
        else if (choice == 's' || choice == 'S') { ReproductionSoundMove(move_1, move_2, move_3, move_4, move_5); DeleteLineOfSight(line_of_sight, loc); StepS(world, loc, line_of_sight, record_map); }
        else if (choice == 'd' || choice == 'D') { ReproductionSoundMove(move_1, move_2, move_3, move_4, move_5); DeleteLineOfSight(line_of_sight, loc); StepD(world, loc, line_of_sight, record_map); }
        else if (choice == 'a' || choice == 'A') { ReproductionSoundMove(move_1, move_2, move_3, move_4, move_5); DeleteLineOfSight(line_of_sight, loc); StepA(world, loc, line_of_sight, record_map); }
        else if (choice == 'p' || choice == 'P') {
            if (chek_input[3] == 1) { chek_input[3] = 0; PlantPut(loc, check_text,planting,put_structure1, put_structure2); }
            else error = 9;
        }
        else if (choice == 'e' || choice == 'E') {
            if (chek_input[2] == 1) { chek_input[2] = 0; ToInteract(loc, open_chest, close_chest, craft_sound, throw_away); }
            else error = 8;
        }
        else if (choice == 'c' || choice == 'C') {
            if (inventory[hand].find('-') + 1 == inventory[hand].find("кирка"))error = 17;
            else {
                if (chek_input[0] == 1) {
                    auto endDelay = chrono::steady_clock::now();
                    chrono::duration<double> elapsedSDelay = endDelay - startDelay;
                    timeDelay = elapsedSDelay.count();
                    ProductionDelay(timeDelay);
                    if (check_fatigue == false) {
                        ReproductionSoundTree(breaking_wood_1, breaking_wood_2, breaking_wood_3, breaking_wood_4, breaking_wood_5, hit_hand_1, hit_hand_2, hit_hand_3, hit_hand_4);
                        chek_input[1] = 0;
                        ProductionDelay(timeDelay);
                        check_fatigue = true;
                        ChopBreak(world, loc, line_of_sight, chek_input, invent_info, structure, timedrop, break_struct, break_tool);
                        chek_input[0] = 0;
                    }
                    else error = 20;
                }
                else error = 5;
            }
        }
        else if (choice == 'b' || choice == 'B') {
            if (inventory[hand].find('-') + 1 != inventory[hand].find("кирка")) {
                if (inventory[hand].find('-') + 1 == inventory[hand].find("топор"))error = 18;
                else error = 19;
            }
            else {
                string g;
                if (chek_input[1] == 1) { 
                    auto endDelay = chrono::steady_clock::now();
                    chrono::duration<double> elapsedSDelay = endDelay - startDelay;
                    timeDelay = elapsedSDelay.count();
                    ProductionDelay(timeDelay);
                    if (check_fatigue == false) {
                        if (rand() % (2 - 1 + 1) + 1 == 1)pickaxe1.play();
                        else pickaxe2.play();
                        chek_input[0] = 0;
                        ProductionDelay(timeDelay);
                        check_fatigue = true;
                        ChopBreak(world, loc, line_of_sight, chek_input, invent_info, structure, timedrop, break_struct, break_tool);
                        chek_input[1] = 0;
                    }
                    else error = 20;
                }
                else error = 6;
            }
        }
        else if (choice == 'm' || choice == 'M') { paper_open.play(); Map(world, loc); }
        else if (choice == 'i' || choice == 'I') {
            auto endPlayer = chrono::steady_clock::now();
            chrono::duration<double> elapsedPlayer = endPlayer - startPlayer;
            timePlayer = elapsedPlayer.count();
            DeleteTimeDrop(timePlayer);
            InventoryPlayer(error_input, item, line_of_sight, loc, craft_sound, throw_away);
            time_tracking = true;
        }
        ClearLineScreen();
        if (choice != 'c' && choice != 'C' && choice != 'b' && choice != 'B' && choice != '0') {
            auto endDelay = chrono::steady_clock::now();
            chrono::duration<double> elapsedSDelay = endDelay - startDelay;
            timeDelay = elapsedSDelay.count();
            ProductionDelay(timeDelay);
        }
        auto endWorld = chrono::steady_clock::now();

        chrono::duration<double> elapsedWorld = endWorld - startWorld;
        timeWorld = elapsedWorld.count();
        if (time_world >= 1230 || time_world <= 390)time_world += ceil(timeWorld * 2.2);
        else time_world += ceil(timeWorld * 1.5);

        auto endStructure = chrono::steady_clock::now();
        chrono::duration<double> elapsedStructure = endStructure - startStructure;


        timeStructure = elapsedStructure.count();
        timerespawne += timeStructure;
        RespawnStone(); 
        TreeGrowth(timeStructure);

        if (time_tracking == false) {
            auto endPlayer = chrono::steady_clock::now();
            chrono::duration<double> elapsedPlayer = endPlayer - startPlayer;
            timePlayer = elapsedPlayer.count();
            DeleteTimeDrop(timePlayer);
        }
        else time_tracking = false;
    }
    return 0;
}
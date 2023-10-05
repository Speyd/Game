#include <iostream>
#include <cmath>
#include <cstdlib>
#include <list>
#include "Windows.h"
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
#define t "\t\t\t\t\t\t\t\t";
#define t1 "\t\t\t\t\t\t\t";
#define t2 "\t\t\t\t\t\t";
#define t3 "\t\t\t\t\t";
#define line_symbol1 "   --------------------------------------------------------------------------\n";
#define line_symbol2 "     ------------------------------\n";
typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    string drop[12];//Ячейки под предметы в сундуке
}InfoChestItem;//Структура котрая хранит информацию о сундуках
extern vector<InfoChestItem> dropchest;//Массив котрый хранит информацию о сундуках
typedef struct {
    char name;//Имя структуры(дерево, камень и т.д.)
    double hp;//Прочность
    int location[4];//Координаты места в мире
}InfoStucture;//Главная структура которая хранит информацию про структуры мира
typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    double hp;//Время исчезновения
    vector<string> tempdrop;//То, что хранит в себе временный сундук
}InfoTempItem;//Иформация про временный сундук
typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    double hp;//Время роста
}InfoGrowthTree;//Иформация про рост деревьев
extern vector<InfoGrowthTree> tree;//Иформация про рост деревьев
extern int hand;//Предмет который помещен в руку
extern vector<string> craft;//Область крафта в инвентаре(2х2 ячейки) и в верстаке(3х3 ячейки)
extern string craft_aftermath;//Переменная которая хранит в себе то, что скрафтил игрок в инвентаре-верстаке
extern vector<InfoTempItem> timedrop;//Временые сундуки
vector<vector<int>> research_map;//Миникарта
extern double timerespawne;//Переменная которая хранит врремя до респавна камня на чанках
vector<vector<vector<vector<char>>>> world;//Динамический массив который хранит мир
extern int line_of_sight;//Линия взгляда игрока (верх,низ,право,лево)
extern string inventory[20];//Инвентарь игрока
extern vector<InfoStucture> structure;//Динамический массив который хранит информацию о всех структурах мира
extern double time_world;//Время в мире
extern int day;//День
extern void ClearLineScreen();
int graphic = 1, setting_music = 1;//graphic - Переменная для настроек в главном меню | setting_music - настройка музыки(1 - вкл/2 - выкл)
#pragma region File
void OutInfoFile(const char* text, const char* text1, const char* text2, vector<vector<vector<vector<char>>>>& world, vector<InfoTempItem>& timedrop) {
    ifstream info(text); ifstream info1(text1); ifstream info2(text2);
    string text_file, num[2] = { "","" }, temp_num = "";
    char check_symbol=' ';
    int lines = 0, columns = 0, amount_structure = 0, amount_timedrop = 0, index_timedrop = 0, amount_dropchest = 0, index_dropchest = 0, amount_tree = 0;
    while (!info.eof()) {
        getline(info, text_file);
        if (text_file[0] == '/')check_symbol = '/';
        if (text_file[0] != '\\') {
            for (int i = text_file.find(':') + 1; i != text_file.size(); i++) {
                if (text_file.find("Блок") == 0) {
                    if (text_file.find(':') + 1 == i)num[0] = "";
                    num[0] += text_file[i];
                }
                else if (text_file.find("Под") == 0) {
                    if (text_file.find(':') + 1 == i) {
                        lines = 0;
                        num[1] = "";
                    }
                    num[1] += text_file[i];
                }
                else if (check_symbol == '/')break;
                else temp_num += text_file[i];
            }
            if (check_symbol == '/') {
                for (int i = 0; i != text_file.size(); i++) {
                    if (text_file[i] != '/') {
                        world[stoi(num[0])][stoi(num[1])][lines][columns] = text_file[i];
                        columns++;
                    }
                }
                lines++; columns = 0;
            }
            if (text_file.find("Размерность") == 0) world.resize(stoi(temp_num));
            else if (text_file.find("Количество-под-блоков") == 0)world[stoi(num[0])].resize(stoi(temp_num));
            else if (text_file.find("Количество-строк") == 0)world[stoi(num[0])][stoi(num[1])].resize(stoi(temp_num));
            else if (text_file.find("Количество-столбцов") == 0) {
                for (int i = 0; i != world[stoi(num[0])][stoi(num[1])].size(); i++)world[stoi(num[0])][stoi(num[1])][i].resize(stoi(temp_num));
            }
        }
        else check_symbol = ' ';
        temp_num = "";
    }
    check_symbol = ' ';
    while (!info1.eof()) {
        temp_num = "";
        getline(info1, text_file);
        if (text_file[0] != '\\') {
            for (int i = text_file.find(':') + 1; i != text_file.size(); i++) {
                if (text_file.find("name") == 0)structure[amount_structure].name += text_file[i];
                else temp_num += text_file[i];
            }
            if (text_file.find("Размерностьs") == 0)structure.resize(stoi(temp_num));
            else if (text_file.find("Размерностьt") == 0)timedrop.resize(stoi(temp_num));
            else if (text_file.find("Размерностьc") == 0)dropchest.resize(stoi(temp_num));
            else if (text_file.find("Размерностьd") == 0)tree.resize(stoi(temp_num));
            else if (text_file.find("Timerespawne") == 0)timerespawne = stoi(temp_num);
            else if (text_file.find("loc_in_structureT") == 0)timedrop[amount_timedrop].loc_in_structure = stoi(temp_num);
            else if (text_file.find("loc_in_structureD") == 0) { tree[amount_tree].loc_in_structure = stoi(temp_num); amount_tree++; }
            else if (text_file.find("loc_in_structureC") == 0)dropchest[amount_dropchest].loc_in_structure = stoi(temp_num);
            else if (text_file.find("itemt") == 0) {
                timedrop[amount_timedrop].tempdrop.resize(timedrop[amount_timedrop].tempdrop.size() + 1);
                timedrop[amount_timedrop].tempdrop[index_timedrop] = temp_num;
                index_timedrop++;
            }
            else if (text_file.find("itemc") == 0) {
                dropchest[amount_dropchest].drop[index_dropchest] = temp_num;
                index_dropchest++;
            }
            else if (text_file.find("shp") == 0)structure[amount_structure].hp = stoi(temp_num);
            else if (text_file.find("thp") == 0)timedrop[amount_timedrop].hp = stoi(temp_num);
            else if (text_file.find("dhp") == 0)tree[amount_tree].hp = stoi(temp_num);
            else if (text_file.find("b") == 0)structure[amount_structure].location[0] = stoi(temp_num);
            else if (text_file.find("pb") == 0)structure[amount_structure].location[1] = stoi(temp_num);
            else if (text_file.find("line") == 0)structure[amount_structure].location[2] = stoi(temp_num);
            else if (text_file.find("column") == 0)structure[amount_structure].location[3] = stoi(temp_num);
        }
        else {
            if (structure[amount_structure].name == 'T')amount_timedrop++;
            else if (structure[amount_structure].name == 'C')amount_dropchest++;
            amount_structure++;
        }
    }
    check_symbol = ' '; num[0] = "";
    while (!info2.eof()) {
        string temp_text = ""; temp_num = ""; num[1] = "";
        getline(info2, text_file);
        if (text_file.find(':') + 1 != text_file.size() && text_file[0] != '\\' && text_file[0] != '}') {
            for (int i = text_file.find(':') + 1; i != text_file.size(); i++) {
                if (text_file.find("Блок") == 0) {
                    if (i == text_file.find(':') + 1) { num[0] = ""; check_symbol = '/'; }
                    num[0] += text_file[i];
                }
                else if (check_symbol == '/') {
                    if (text_file.find(':') + 1 == i) {
                        for (int l = 0; l != text_file.find(':'); l++)num[1] += text_file[l];
                    }
                    temp_num += text_file[i];
                }
                else if (text_file.find("Инвентарь") == 0) { check_symbol = '{'; break; }
                else if (check_symbol == '{') {
                    if (i == text_file.find(':') + 1) {
                        for (int l = 0; l != text_file.find(':'); l++)temp_num += text_file[l];
                    }
                    temp_text += text_file[i];
                }
                else if (text_file.find("item") == 1)temp_text += text_file[i];
                else temp_num += text_file[i];
            }
        }
        if (text_file[0] == '\\' || text_file[0] == '}')check_symbol = ' ';
        else if (text_file.find("Линия") == 0)line_of_sight = stoi(temp_num);
        else if (text_file.find("Размерностьcraft") == 0)craft.resize(stoi(temp_num));
        else if (text_file.find("Размерность") == 0)research_map.resize(stoi(temp_num));
        else if (text_file.find("Рука") == 0)hand = stoi(temp_num);
        else if (text_file.find("Работа музыки") == 0)setting_music = stoi(temp_num);
        else if (text_file.find("Время") == 0)time_world = stof(temp_num);
        else if (text_file.find("День") == 0)day = stof(temp_num);
        else if (text_file.find("Блок") != 0 && check_symbol == '/') {
            research_map[stoi(num[0])].resize(research_map[stoi(num[0])].size() + 1);
            research_map[stoi(num[0])][stoi(num[1])] = stoi(temp_num);
        }
        else if (text_file.find("item") == 1) {
            temp_num = text_file[0];
            if (text_file.find(':') + 1 != text_file.size())craft[stoi(temp_num)] = temp_text;
        }
        else if (text_file.find("Craft_aftermath") == 0)craft_aftermath = temp_num;
        else if (text_file.find("Инвентарь") != 0 && check_symbol == '{') {
            if (temp_text != "")inventory[stoi(temp_num)] = temp_text;
        }
    }
    info.close(); info1.close(); info2.close();
    string g;
}
//OutInfoFile - вывод информации из файлов в переменные/массивы
void ChekFile(const char* text, const char* text1, const char* text2, int sizefile[3]) {
    fstream file(text); fstream file1(text1); fstream file2(text2);
    file.seekg(0, std::ios::end);
    file1.seekg(0, std::ios::end);
    file2.seekg(0, std::ios::end);
    sizefile[0] = file.tellg(); sizefile[1] = file1.tellg(); sizefile[2] = file2.tellg();
    file.close(); file1.close(); file2.close();
}
//ChekFile - проверка файлов на пустоту
#pragma endregion
#pragma region World
void WorldGeneration(vector<vector<vector<vector<char>>>>& world) {
    srand(time(0));
    int lines, columns;
    for (int i = 0; i != world.size(); i++) {
        for (int l = 0; l != world[i].size(); l++) {
            int num = rand() % (16 - 14 + 1) + 14;
            for (int h = 0; h != num; h++) {
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
                else if (i == 0 ) {
                    if (world[i].size() > world[i + 1].size()) {
                        if(l<world[i+1].size()-1){ lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i + 1].size()-1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l < world[i].size()-1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size()-1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                    else if (world[i].size() < world[i + 1].size() || world[i].size() == world[i + 1].size()) {
                        if (l < world[i].size()-1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size()-1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                }
                else if (i == world.size()-1) {
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
                if ((rand() % (100 - 1 + 1) + 1) > 40)world[i][l][lines][columns] = 'Y';
                else world[i][l][lines][columns] = 'S';
            }
            for (int j = 0; j != world[i][l].size(); j++) {
                for (int f = 0; f != world[i][l][j].size(); f++) {
                    if (i == 0 && l == 0) {
                        if (j == 5 && f >= 5 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                        else if (j > 5 && j <= world[i][l].size() && f == 5)world[i][l][j][f] = '|';
                    }
                    else if (i == 0 && l == world[i].size() - 1) {
                        if (j == 5 && f > 0 && f < world[i][l][j].size() - 6)world[i][l][j][f] = '_';
                        else if (j > 5 && j <= world[i][l].size() && f == world[i][l][j].size() - 6)world[i][l][j][f] = '|';
                    }
                    else if (i == world.size() - 1 && l == 0) {
                        if (j >= 1 && j <= world[i][l].size() - 6 && f == 5)world[i][l][j][f] = '|';
                        else if (j == world[i][l].size() - 6 && f >= 5 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                    }
                    else if (i == world.size() - 1 && l == world[i].size() - 1) {
                        if (j >= 1 && j <= world[i][l].size() - 6 && f == world[i][l][j].size() - 6)world[i][l][j][f] = '|';
                        else if (j == world[i][l].size() - 6 && f >= 1 && f <= world[i][l][j].size() - 6)world[i][l][j][f] = '_';
                    }
                    else {
                        if (l == 0 && i != 0 && i != world.size() - 1) {
                            if (f == 5 && j >= 1 && j <= world[i][l].size())world[i][l][j][f] = '|';
                        }
                        else if (l == world[i].size() - 1 && i != 0 && i != world.size() - 1) {
                            if (f == world[i][l][j].size() - 6 && j >= 1 && j <= world[i][l].size() - 1)world[i][l][j][f] = '|';
                        }
                        else if (l >= 1 && l < world[i].size() - 1 && i == 0) {
                            if (j == 5 && f >= 1 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                        }
                        else if (l >= 1 && l < world[i].size() - 1 && i == world.size() - 1) {
                            if (j == world[i][l].size() - 6 && f >= 1 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                        }
                    }
                    if (i < world.size() - 1 && world[i].size() > world[i + 1].size()) {
                        if (l == world[i].size() - 1) {
                            if (j == world[i][l].size() - 6 && f > 0 && f < world[i][l][j].size() - 6)world[i][l][j][f] = '_';
                            if (j > world[i][l].size() - 6 && j <= world[i][l].size() - 1 && f == world[i][l][j].size() - 6)world[i][l][j][f] = ' ';
                        }
                        else if (i< world.size() - 1 && l > world[i + 1].size() - 1 && l != world[i].size() - 1) {
                            if (j == world[i][l].size() - 6 && f > 0 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                        }
                        else if (i< world.size()-1 && l == world[i + 1].size() - 1) {
                            if (j == world[i][l].size() - 6 && f > world[i][l][j].size() - 6 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                            if (j > world[i][l].size() - 6 && j <= world[i][l].size() && f == world[i][l][j].size() - 6)world[i][l][j][f] = '|';
                        }
                    }
                    if (i > 0 && world[i].size() > world[i - 1].size()) {
                        if (l == world[i].size() - 1) {
                            if (j > 0 && j < 5 && f == world[i][l][j].size() - 6)world[i][l][j][f] = ' ';
                            if (j == 4 && f > 0 && f < world[i][l][j].size() - 6)world[i][l][j][f] = '_';
                        }
                        if (i>0 && l == world[i - 1].size() - 1) {
                            if (j > 0 && j < 5 && f == world[i][l][j].size() - 6)world[i][l][j][f] = '|';
                            if (j == 4 && f > world[i][l][j].size() - 6 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                        }
                        if (i>0 && l > world[i - 1].size() - 1 && l < world[i].size() - 1) {
                            if (j == 4 && f > 0 && f < world[i][l][j].size() - 1)world[i][l][j][f] = '_';
                        }
                    }
                    if (f == 0 && j != 0 || f == world[i][l][j].size() - 1 && j != 0)world[i][l][j][f] = '|';
                    else if (j == 0 && world[i][l][j][f] != '|' || j == world[i][l].size() - 1 && world[i][l][j][f] != '|')world[i][l][j][f] = '_';
                    else if (world[i][l][j][f] != '|' && world[i][l][j][f] != '0' && world[i][l][j][f] != '_' && world[i][l][j][f] != '-' && world[i][l][j][f] != 'Y' && world[i][l][j][f] != 'S')world[i][l][j][f] = ' ';
                }
            }
        }
    }
}
//WorldGeneration - генерация мира
void WordSize(vector<vector<vector<vector<char>>>>& world) {
    srand(time(0));
    int num = rand() % (6 - 4 + 1) + 4;
    world.resize(num);
    research_map.resize(num);
    for (int i = 0; i != world.size(); i++) {
        num = rand() % (7 - 4 + 1) + 4;
        world[i].resize(num);
        research_map[i].resize(num);
        for (int l = 0; l != world[i].size(); l++) {
            world[i][l].resize(32);
            for (int j = 0; j != world[i][l].size(); j++)world[i][l][j].resize(48);
        }
    }
}
//WordSize - размернсоть мира(блок/куб/строка/столбец)
#pragma endregion
void InputWildall() {
    setlocale(LC_ALL, "ru_RU.utf8");
    ClearLineScreen();
    cout << t;
    cout << u8"\033[1m╔╦═╦╗╔══╗╔╗ ╔══╗╔══╗╔╗ ╔╗ \n" << t;
    cout << u8"║║║║║╚║║╝║║ ╚╗╗║║╔╗║║║ ║║ \n" << t;
    cout << u8"║║║║║╔║║╗║╚╗╔╩╝║║╠╣║║╚╗║╚╗\n" << t;
    cout << u8"╚═╩═╝╚══╝╚═╝╚══╝╚╝╚╝╚═╝╚═╝\n\033[0m";
    setlocale(LC_ALL, "Rus");
}
int Creation_Launch() {
    srand(time(0));
    #pragma region Music_Sound
    sf::SoundBuffer ButtonBuff;
    if (!ButtonBuff.loadFromFile("button.ogg")) {
        return -1;
    }
    sf::Sound button;
    button.setBuffer(ButtonBuff);

    sf::Music music_menu;
    if (!music_menu.openFromFile("music_menu.ogg")) {
        return -1;
    }
    #pragma endregion
    music_menu.play();
    int choice = 1, sizefile[3] = { 0,0,0 }, page = 5;//amount_save = 0,
    //choice - Переменная для выбора;sizefile - Хранит в себе размер файла, page - для страниц в меню
    string enter = "1";//enter - переменная для нажатия enter
    #pragma region Chek_Choice/Selection_Choice
    do {
        InputWildall();
        cout << t;
        cout << " [1] - Создать новый мир\n" << t; 
        cout << " [2] - Продолжить игру\n" << t;
        cout << " [3] - Настройки\n" << t;
        cout<<" [4] - Об игре\n";
        if (choice >= 0 && choice <= 4) { cout << t cout << " Ваш выбор: "; cin >> choice; }
        else if (choice < 1 || choice>4) { cout << t; cout << " Неверный выбор!\n" << t;cout<< " Введите ваш выбор снова: "; cin >> choice; }
        if (choice == 3) {
            button.play();
            do {
                do {
                    InputWildall();
                    cout << t;
                    cout << "\tНастройки\n" << t;
                    cout << "[1] - Графика\n" << t;
                    cout<<"[2] - Музыка\n";
                    if (choice == 5) { cout << t; cout << " Нету такого выбора!\n" << t1;cout<< "  Введите ваш выбор снова(0 для выхода): "; cin >> choice; }
                    else { cout << t; cout << "Ваш выбор(0 для выхода): "; cin >> choice; }
                    if (choice > 2 || choice < 0)choice = 5;
                } while (choice > 2 && choice < 0);
                button.play();
                if (choice == 1) {
                    int start_setting_graphic = graphic;
                    do {
                        InputWildall();
                        cout << t;
                        cout << "\t\033[1mГрафика\033[0m\n";
                        if (graphic == 1) { cout << t; cout << "[1] - Низкая графика   <---- Ваш выбор\n" << t;cout << "[2] - Супер низкая графика\n"; }
                        else if (graphic == 2) { cout << t; cout << "[1] - Низкая графика\n" << t;cout<< "[2] - Супер низкая графика   <---- Ваш выбор\n"; }
                        else { cout << t; cout << "[1] - Низкая графика\n" << t;cout << "[2] - Супер низкая графика\n"; }
                        choice = graphic;
                        if (graphic < 0 || graphic > 2) { cout << t; cout << "   Нету такого выбора!\n" << t1;cout << "   Введите ваш выбор снова(0 для выхода): "; cin >> graphic; }
                        else { cout << t; cout << "Ваш выбор(0 для выхода): "; cin >> graphic; }
                        if (graphic < 0 || graphic>2)choice = 3;
                        if (graphic == 0) { graphic = choice; choice = 0; }
                        button.play();
                    } while (choice != 0);
                    if (graphic < 1 || graphic > 2)graphic = start_setting_graphic;
                }else if (choice == 2) {
                    int start_setting_music = setting_music;
                    do {
                        InputWildall();
                        cout << t; cout << "\t\033[1mМузыка\033[0m\n";
                        if (setting_music == 1) { cout << t; cout << "[1] - Вкл. музыка   <---- Ваш выбор\n" << t; cout << "[2] - Выл. музыка\n"; }
                        else if (setting_music == 2) { cout << t; cout << "[1] - Вкл. музыка\n" << t; cout << "[2] - Выл. музыка   <---- Ваш выбор\n"; }
                        else { cout << t; cout << "[1] - Вкл. музыка\n" << t;cout << "[2] - Выл. музыка\n"; }
                        choice = setting_music;
                        if (choice == 3) { cout << t; cout << "   Нету такого выбора!\n" << t2; cout<<"Введите ваш выбор снова(0 для выхода / изменения настроек): "; cin >> setting_music; }
                        else { cout << t1; cout << "  Ваш выбор(0 для выхода/изменения настроек): "; cin >> setting_music; }
                        if (setting_music < 0 || setting_music>2)choice = 3;
                        if (setting_music == 0) { setting_music = choice; choice = 0; }
                        button.play();
                    } while (choice != 0);
                    
                    if (start_setting_music == 1 && setting_music == 2)music_menu.stop();
                    else if (start_setting_music == 2 && setting_music == 1)music_menu.play();
                    else setting_music = start_setting_music;
                }
            } while (choice != 0);
        }
        if (choice == 4) {
            button.play();
            choice = 1;
            do {
                InputWildall();
                if (choice == 1) {
                    if (page == 1)choice++;
                }
                else if (choice == 2) {
                    if (page == 1)choice--;
                    else if (page == 2)choice++;
                }
                else if (choice == 3) {
                    if (page == 1)choice--;
                }
                if (choice == 1) {
                    cout << t;
                    cout << "     \033[1mСтруктуры мира\033[0m\n" << t;
                    cout << "  0 - игрок\n" << t;
                    cout << "  Y - дерево\n" << t;
                    cout << "  y - саженец\n" << t;
                    cout << "  S - камень\n" << t;
                    cout << "  T - временный сундук\n" << t;
                    cout << "  C - сундук\n" << t;
                    cout << "  W - верстак\n" << t;
                    cout << "  i - факел\n";
                }
                else if (choice == 2) {
                    cout << t;
                    cout << "     \033[1mМеханики крафта\033[0m\n" << t3;
                    cout << "   1. Кафтить можна как в инвентаре(ячейки 2х2), так и на верстаке(ячейки 3х3)\n" << t3;
                    cout << line_symbol1;cout << t3;
                    cout << "   2. Если при крафте предмета у вас нету места в инвентаре, то этот предмет -\n" << t3;
                    cout << "      автоматически добавиться к вам в инвентарь после его освобождения\n" << t3;
                    cout << line_symbol1; cout << t3;
                    cout << "   3. Также если у вас нету места и вы скрафтили какой нибудь предмет, то вы -\n" << t3;
                    cout << "      не сможете крафтить пока не освободите инвентарь\n"<<t3;
                    cout << line_symbol1;
                }
                else if (choice == 3) {
                    cout << t1;
                    cout << "     \033[1mПредметы которые можна скрафтить\033[0m\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      1. Доски\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      2. Палки\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      3.Верстак\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      4.Сундук\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      5.Деревянный - каменный топор\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      6.Деревянная - каменная кирка\n" << t1;
                    cout << line_symbol2; cout << t1;
                    cout << "      7.Факел\n" << t1;
                    cout << line_symbol2;
                }
                cout << t;
                cout << "     "<<choice << " из 3 страниц\n";
                if (choice == 2) { cout << t2; cout << " [1] - Предыдущая страница | [2] - Следуйщая страница\n"; }
                if (choice == 1) { cout << t; cout << "[1] - Следуйщая страница\n"; }
                if (choice == 3) { cout << t; cout << "[1] - Предыдущая страница\n"; }
                if (page == 4) { cout << t; cout << "   Нету такого выбора\n" << t1;cout << "   Введите ваш выбор снова(0 для выхода): "; cin >> page; }
                else { cout << t; cout << " Ваш выбор(0 для выхода): "; cin >> page; }
                if (choice == 1 || choice == 3) {
                    if (page < 0 || page>1)page = 4;
                }
                else if (choice == 2) {
                    if (page < 0 || page>2)page = 4;
                }
                button.play();
            } while (page != 0);
            choice = 0;
        }
        else button.play();
    } while (choice < 1 || choice>4);
    #pragma endregion
    ChekFile("WorldParameter.txt", "WorldStructure.txt", "PlayerInfo.txt", sizefile);
    if (choice == 2) {
        if (sizefile[0] != 0 && sizefile[1] != 0 && sizefile[2] != 0) {
            OutInfoFile("WorldParameter.txt", "WorldStructure.txt", "PlayerInfo.txt", world, timedrop);
            cin.ignore();
            while (enter.empty() != true) {
                cout << t;
                cout << " Нажмите enter для начала игры: ";
                getline(cin, enter);
            }
            if (graphic == 2) { ClearLineScreen(); cout << "Вы умерли! Графика стала настолько плохой, что перестала отрисовывать мир!:)"; }
        }
        else if (sizefile[0] == 0 && sizefile[1] == 0 && sizefile[2] == 0) {
            cout << t;
            cout << " ----------------------------\n" << t;
            cout << " У вас нету сохранений!\n" << t;cout << " Мир создастся автоматически!\n";
            choice = 1;
        }
        else if (sizefile[0] == 0 || sizefile[1] == 0 || sizefile[2] == 0) {
            do {
                cout << t;
                cout <<" -------------------------\n" << t2; cout << "Повреждена целостность данных(некоторые данные отсутствуют)!\n" << t;
                cout << " Создаем новый мир ?\n" << t;
                cout<< " [1] - Да | [2] - Нет\n" << t;
                if (choice == 3) { cout << " Нету такого выбора!\n" << t;cout<< "Введите ваш выбор снова: "; cin >> choice; }
                else { cout << " Ваш выбор: "; cin >> choice; }
                if (choice < 1 || choice>2) { 
                    choice = 3; 
                    InputWildall();
                    cout << t;
                    cout << " [1] - Создать новый мир\n" << t;
                    cout << " [2] - Продолжить игру\n" << t;
                    cout << " [3] - Настройки\n" << t;
                    cout << " [4] - Об игре\n";
                }
            } while (choice < 1 || choice>2);
            button.play();
        }
    }
    if (choice == 1) {
        if (sizefile[0] != 0 && sizefile[1] != 0 && sizefile[2] != 0) {
            do {
                cout << t1;
                cout << "Вы уверенны что хотите создать новый мир?\n" << t1;
                cout << "Сохранения с прежнего мира удаляться!\n" << t1;
                cout<<"[1] - Создать новый мир | [2] - Не создавать\n" << t;
                if (choice == 3) { cout << "Нету такого выбора!\n" << t;cout<< "Введите ваш выбор снова: "; cin >> choice; }
                else { cout << "Ваш выбор: "; cin >> choice; }
                if (choice < 1 || choice>2) { choice = 3; ClearLineScreen(); }
            } while (choice < 1 || choice>2);
            button.play();
        }
        if (choice != 2) {
            ofstream file("WorldParameter.txt"); ofstream file1("WorldStructure.txt"); ofstream file2("PlayerInfo.txt");
            file.close(); file1.close(); file2.close();
            WordSize(world);
            WorldGeneration(world);
            int block = rand() % ((world.size() - 1) - 0 + 1) + 0; int bod_block = (rand() % ((world[block].size() - 1) - 0 + 1) + 0);
            research_map[block][bod_block] = true;
            world[block][bod_block][(rand() % ((world[block][0].size() - 10) - 8 + 1) + 8)][(rand() % ((world[block][0][0].size() - 10) - 8 + 1) + 8)] = '0';
            cout << t;
            Sleep(3000);
            InputWildall();
            cout << t;
            cout << " [1] - Создать новый мир\n" << t;
            cout << " [2] - Продолжить игру\n" << t;
            cout << " [3] - Настройки\n" << t;
            cout << " [4] - Об игре\n" << t;
            cout << " ----------------------------\n" << t;cout<< " Мир создался..." << endl;
            cin.ignore();
            while (enter.empty() != true) {
                cout << t;
                cout << " Нажмите enter для начала игры: ";
                getline(cin, enter);
            }
            if (graphic == 2) { ClearLineScreen(); cout << "Вы умерли! Графика стала настолько плохой, что перестала отрисовывать мир!:)"; }
        }
    }
    if(setting_music == 1)music_menu.stop();
    return 0;
}
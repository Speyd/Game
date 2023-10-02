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
typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    string drop[12];//Ячейки под предметы в судуке
}InfoChestItem;//Структура котрая хранит информацию об сундуках
extern vector<InfoChestItem> dropchest;//Массив котрый хранит информацию об сундуках
typedef struct {
    char name;//Имя структуры(дерево, камень и т.д.)
    double hp;//Прочность
    int location[4];//Координаты места в мире
}InfoStucture;//Информацию об всех структурах мира
extern vector<InfoStucture> structure;//Динамический массив который хранит информацию об всех структурах мира
typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    double hp;//Время роста
}InfoGrowthTree;//Иформация про рост деревьев
extern vector<InfoGrowthTree> tree;//Иформация про рост деревьев
typedef struct {
    int loc_in_structure;//Место, где находиться в главном массиве структур
    double hp;//Время изчезновения
    vector<string> tempdrop;//То, что хрнит в себе временный сундук
}InfoTempItem;//Иформация про временный сундук
extern vector<InfoTempItem> timedrop;//Временный сундук
extern int hand;//Предмет который помещен в руку
extern double timerespawne;//Переменная которая хранит врремя до респавна камня на чанках
extern vector<string> craft;//Область крафта в инвентаре(2х2 ячейки) и в верстаке(3х3 ячейки)
extern string craft_aftermath;//предмет который скрафтил ирок в инвентаре/верстаке
extern vector<vector<int>> research_map;//Миникарта
extern string inventory[20];//Инвентарь игрока
extern vector<vector<vector<vector<char>>>> world;////Динамический массив который хранит мир
extern int line_of_sight;//Линия взгляда игрока (верх,низ,право,леов)
extern int day;//День
extern double time_world;//Время в мире
void InputInfoWorldFile(const char* text, vector<vector<vector<vector<char>>>>& world) {
    ofstream info(text);
    info << "Размерность:" << world.size() << endl;
    for (int i = 0; i != world.size(); i++) {
        info << "Блок:" << i << endl << "Количество-под-блоков:" << world[i].size() << endl;
        for (int l = 0; l != world[i].size(); l++) {
            info << "Под блок:" << l << endl;
            info << "Количество-строк:" << world[i][l].size() << endl << "Количество-столбцов:" << world[i][l][0].size() << endl;
            for (int j = 0; j != world[i][l].size(); j++) {
                for (int f = 0; f != world[i][l][j].size(); f++) {
                    if (j == 0 && f == 0)info << "/";
                    info << world[i][l][j][f];
                    if (j == world[i][l].size() - 1 && f == world[i][l][j].size() - 1)info << endl << "\\";
                }
                info << endl;
            }
            info << endl;
        }
        info << endl;
    }
    info.close();
}
void InputInfoPlayerFile(const char* text) {
    ofstream info(text);
    info << "Линия взгляда:" << line_of_sight << endl;
    info << "Время:" << time_world << endl;
    info << "День:" << day << endl;
    info << "Рука:" << hand << endl;
    info << "Размерность:" << research_map.size() << endl;
    for (int i = 0; i != research_map.size(); i++) {
        info << "Блок:" << i << endl;
        for (int l = 0; l != research_map[i].size(); l++) {
            if (research_map[i][l] == true)info << l << ":1" << endl;
            else info << l << ":0" << endl;
        }
        info << "\\" << endl;;
    }
    info << "Инвентарь:{" << endl;
    for (int i = 0; i != 20; i++) {
        if (inventory[i].empty() == false)info << i << ":" << inventory[i] << endl;
        else info << i << ":" << endl;
    }
    info << "}" << endl;
    info << "Размерностьcraft:" << craft.size() << endl;
    for (int i = 0; i != craft.size(); i++) {
        info << i << "item:" << craft[i] << endl;
    }
    info << "Craft_aftermath:" << craft_aftermath;
    info.close();
}
void InputInfoStructureFile(const char* text) {
    ofstream info(text);
    info << "Размерностьs:" << structure.size() << endl << "Размерностьt:" << timedrop.size() << endl << "Размерностьc:" << dropchest.size() << endl << "Размерностьd:" << tree.size() << endl << "Timerespawne:" << timerespawne << endl;
    for (int i = 0; i != structure.size(); i++) {
        info << "name:" << structure[i].name;
        if (structure[i].hp == 0) {
            for (int l = 0; l != timedrop.size(); l++) {
                if (timedrop[l].loc_in_structure == i) { info << "\nthp:" << timedrop[l].hp; break; }
            }
        }
        else info << "\nshp:" << structure[i].hp;
        info << "\nb:" << structure[i].location[0] << "\npb:" << structure[i].location[1] << "\nline:" << structure[i].location[2] << "\ncolumn:" << structure[i].location[3];
        if (timedrop.size() != 0) {
            for (int l = 0; l != timedrop.size(); l++) {
                if (timedrop[l].loc_in_structure == i) {
                    info << "\nloc_in_structureT:" << timedrop[l].loc_in_structure << endl;
                    info << "drop:{" << endl;
                    for (int j = 0; j != timedrop[l].tempdrop.size(); j++) {
                        info << "itemt:" << timedrop[l].tempdrop[j] << endl;
                    }
                    info << "}" << endl;
                }
            }
        }
        if (tree.size() != 0) {
            for (int l = 0; l != tree.size(); l++) {
                if (tree[l].loc_in_structure == i) {
                    info << "\ndhp:" << tree[l].hp << endl;
                    info << "loc_in_structureD:" << tree[l].loc_in_structure << endl;
                }
            }
        }
        if (dropchest.size() != 0) {
            for (int l = 0; l != dropchest.size(); l++) {
                if (dropchest[l].loc_in_structure == i) {
                    info << "\nloc_in_structureC:" << dropchest[l].loc_in_structure << endl;
                    info << "drop:{" << endl;
                    for (int j = 0; j != 12; j++) {
                        info << "itemc:" << dropchest[l].drop[j] << endl;
                    }
                    info << "}" << endl;
                }
            }
        }
        info << "\n\\" << endl;
    }
    info.close();
}
int InputInfo() {
    InputInfoWorldFile("WorldParameter.txt", world);
    InputInfoPlayerFile("PlayerInfo.txt");
    InputInfoStructureFile("WorldStructure.txt");
    return 0;
}
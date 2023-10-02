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
    int loc_in_structure;//�����, ��� ���������� � ������� ������� ��������
    string drop[12];//������ ��� �������� � ������
}InfoChestItem;//��������� ������ ������ ���������� �� ��������
extern vector<InfoChestItem> dropchest;//������ ������ ������ ���������� �� ��������
typedef struct {
    char name;//��� ���������(������, ������ � �.�.)
    double hp;//���������
    int location[4];//���������� ����� � ����
}InfoStucture;//���������� �� ���� ���������� ����
extern vector<InfoStucture> structure;//������������ ������ ������� ������ ���������� �� ���� ���������� ����
typedef struct {
    int loc_in_structure;//�����, ��� ���������� � ������� ������� ��������
    double hp;//����� �����
}InfoGrowthTree;//��������� ��� ���� ��������
extern vector<InfoGrowthTree> tree;//��������� ��� ���� ��������
typedef struct {
    int loc_in_structure;//�����, ��� ���������� � ������� ������� ��������
    double hp;//����� ������������
    vector<string> tempdrop;//��, ��� ����� � ���� ��������� ������
}InfoTempItem;//��������� ��� ��������� ������
extern vector<InfoTempItem> timedrop;//��������� ������
extern int hand;//������� ������� ������� � ����
extern double timerespawne;//���������� ������� ������ ������ �� �������� ����� �� ������
extern vector<string> craft;//������� ������ � ���������(2�2 ������) � � ��������(3�3 ������)
extern string craft_aftermath;//������� ������� �������� ���� � ���������/��������
extern vector<vector<int>> research_map;//���������
extern string inventory[20];//��������� ������
extern vector<vector<vector<vector<char>>>> world;////������������ ������ ������� ������ ���
extern int line_of_sight;//����� ������� ������ (����,���,�����,����)
extern int day;//����
extern double time_world;//����� � ����
void InputInfoWorldFile(const char* text, vector<vector<vector<vector<char>>>>& world) {
    ofstream info(text);
    info << "�����������:" << world.size() << endl;
    for (int i = 0; i != world.size(); i++) {
        info << "����:" << i << endl << "����������-���-������:" << world[i].size() << endl;
        for (int l = 0; l != world[i].size(); l++) {
            info << "��� ����:" << l << endl;
            info << "����������-�����:" << world[i][l].size() << endl << "����������-��������:" << world[i][l][0].size() << endl;
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
    info << "����� �������:" << line_of_sight << endl;
    info << "�����:" << time_world << endl;
    info << "����:" << day << endl;
    info << "����:" << hand << endl;
    info << "�����������:" << research_map.size() << endl;
    for (int i = 0; i != research_map.size(); i++) {
        info << "����:" << i << endl;
        for (int l = 0; l != research_map[i].size(); l++) {
            if (research_map[i][l] == true)info << l << ":1" << endl;
            else info << l << ":0" << endl;
        }
        info << "\\" << endl;;
    }
    info << "���������:{" << endl;
    for (int i = 0; i != 20; i++) {
        if (inventory[i].empty() == false)info << i << ":" << inventory[i] << endl;
        else info << i << ":" << endl;
    }
    info << "}" << endl;
    info << "�����������craft:" << craft.size() << endl;
    for (int i = 0; i != craft.size(); i++) {
        info << i << "item:" << craft[i] << endl;
    }
    info << "Craft_aftermath:" << craft_aftermath;
    info.close();
}
void InputInfoStructureFile(const char* text) {
    ofstream info(text);
    info << "�����������s:" << structure.size() << endl << "�����������t:" << timedrop.size() << endl << "�����������c:" << dropchest.size() << endl << "�����������d:" << tree.size() << endl << "Timerespawne:" << timerespawne << endl;
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
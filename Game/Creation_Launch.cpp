#include <iostream>
#include <cmath>
#include <cstdlib>
#include <list>
#include "windows.h"
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
}InfoStucture;//������� ��������� ������� ������ ���������� ��� ��������� ����
typedef struct {
    int loc_in_structure;//�����, ��� ���������� � ������� ������� ��������
    double hp;//����� ������������
    vector<string> tempdrop;//��, ��� ����� � ���� ��������� ������
}InfoTempItem;//��������� ��� ��������� ������
typedef struct {
    int loc_in_structure;//�����, ��� ���������� � ������� ������� ��������
    double hp;//����� �����
}InfoGrowthTree;//��������� ��� ���� ��������
extern vector<InfoGrowthTree> tree;//��������� ��� ���� ��������
extern int hand;//������� ������� ������� � ����
extern vector<string> craft;//������� ������ � ���������(2�2 ������) � � ��������(3�3 ������)
extern string craft_aftermath;//���������� ������� ������ � ���� ��, ��� �������� ����� � ���������-��������
extern vector<InfoTempItem> timedrop;//�������� �������
vector<vector<int>> research_map;//���������
extern double timerespawne;//���������� ������� ������ ������ �� �������� ����� �� ������
vector<vector<vector<vector<char>>>> world;//������������ ������ ������� ������ ���
extern int line_of_sight;//����� ������� ������ (����,���,�����,����)
extern string inventory[20];//��������� ������
extern vector<InfoStucture> structure;//������������ ������ ������� ������ ���������� �� ���� ���������� ����
extern double time_world;//����� � ����
extern int day;//����
extern void ClearLineScreen();
int graphic = 1;//���������� ��� �������� � ������� ����
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
                if (text_file.find("����") == 0) {
                    if (text_file.find(':') + 1 == i)num[0] = "";
                    num[0] += text_file[i];
                }
                else if (text_file.find("���") == 0) {
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
            if (text_file.find("�����������") == 0) world.resize(stoi(temp_num));
            else if (text_file.find("����������-���-������") == 0)world[stoi(num[0])].resize(stoi(temp_num));
            else if (text_file.find("����������-�����") == 0)world[stoi(num[0])][stoi(num[1])].resize(stoi(temp_num));
            else if (text_file.find("����������-��������") == 0) {
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
            if (text_file.find("�����������s") == 0)structure.resize(stoi(temp_num));
            else if (text_file.find("�����������t") == 0)timedrop.resize(stoi(temp_num));
            else if (text_file.find("�����������c") == 0)dropchest.resize(stoi(temp_num));
            else if (text_file.find("�����������d") == 0)tree.resize(stoi(temp_num));
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
                if (text_file.find("����") == 0) {
                    if (i == text_file.find(':') + 1) { num[0] = ""; check_symbol = '/'; }
                    num[0] += text_file[i];
                }
                else if (check_symbol == '/') {
                    if (text_file.find(':') + 1 == i) {
                        for (int l = 0; l != text_file.find(':'); l++)num[1] += text_file[l];
                    }
                    temp_num += text_file[i];
                }
                else if (text_file.find("���������") == 0) { check_symbol = '{'; break; }
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
        else if (text_file.find("�����") == 0)line_of_sight = stoi(temp_num);
        else if (text_file.find("�����������craft") == 0)craft.resize(stoi(temp_num));
        else if (text_file.find("�����������") == 0)research_map.resize(stoi(temp_num));
        else if (text_file.find("����") == 0)hand = stoi(temp_num);
        else if (text_file.find("�����") == 0)time_world = stof(temp_num);
        else if (text_file.find("����") == 0)day = stof(temp_num);
        else if (text_file.find("����") != 0 && check_symbol == '/') {
            research_map[stoi(num[0])].resize(research_map[stoi(num[0])].size() + 1);
            research_map[stoi(num[0])][stoi(num[1])] = stoi(temp_num);
        }
        else if (text_file.find("item") == 1) {
            temp_num = text_file[0];
            if (text_file.find(':') + 1 != text_file.size())craft[stoi(temp_num)] = temp_text;
        }
        else if (text_file.find("Craft_aftermath") == 0)craft_aftermath = temp_num;
        else if (text_file.find("���������") != 0 && check_symbol == '{') {
            if (temp_text != "")inventory[stoi(temp_num)] = temp_text;
        }
    }
    info.close(); info1.close(); info2.close();
    string g;
   /* for (int i = 0; i != structure.size(); i++) {
        cout << structure[i].name << endl;
    }
    cin >> g;*/
}
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

                /*if (i == 0 && l == 0) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 6 + 1) + 6; }
                else if (i == world.size() - 1 && l == 0) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 6 + 1) + 6; }
                else if (l == 0 && i != 0 && i != world.size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 6 + 1) + 6; }
                else if (i != world.size() - 1 && world[i].size() > world[i + 1].size()) {
                    if (i == 0 && l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (i == 0 && l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (i == 0 && l > world[i + 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (i == 0 && l > world[i + 1].size() - 1 && l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    else if (i != 0) {
                        if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l > world[i + 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        if (world[i].size() < world[i - 1].size()) {
                            if (l > world[i + 1].size() - 1 && l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                        else if (world[i].size() > world[i - 1].size()) {
                            if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l > world[i - 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                            else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                    }
                }
                else if (i != world.size() - 1 && world[i].size() < world[i + 1].size()) {
                    if (i == 0 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (i == 0 && l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    else if (i != 0) {
                        if (world[i].size() < world[i - 1].size() && i != 0) {
                            if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                        if (world[i].size() > world[i - 1].size() && i != 0) {
                            if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l > world[i - 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                    }
                }
                if (i>0 && i != world.size() - 1 && l != 0 && world[i].size() == world[i + 1].size()) {
                    if (i == 0 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                    else if (i == 0 && l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    else if (i != 0) {
                        if (i>0 && world[i].size() > world[i - 1].size()) {
                            if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l > world[i - 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l > world[i - 1].size() - 1 && l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                        if (i>0 && world[i].size() < world[i - 1].size()) {
                            if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                        if (i>0 && world[i].size() == world[i - 1].size()) {
                            if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                            else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                        }
                    }
                }
                else if (i != 0 && l != 0 && world[i].size() == world[i - 1].size()) {
                    if (i< world.size() - 1 && world[i].size() > world[i + 1].size() && i != world.size() - 1) {
                        if (l < world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i + 1].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l > world[i + 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                    else if (i < world.size() - 1 && world[i].size() < world[i + 1].size() && i != world.size() - 1) {
                        if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                    else if (i < world.size() - 1 && world[i].size() == world[i + 1].size()) {
                        if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 2) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                    if (i == world.size() - 1) {
                        if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                }
                if (i == world.size() - 1 && l != 0) {
                    if (i>0 && world[i].size() < world[i - 1].size()) {
                        if (l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                    else if (i>0 && world[i].size() > world[i - 1].size()) {
                        if (l < world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 1 + 1) + 1; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l == world[i - 1].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l > world[i - 1].size() - 1 && l < world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 2) - 1 + 1) + 1; }
                        else if (l > world[i - 1].size() - 1 && l == world[i].size() - 1) { lines = rand() % ((world[i][l].size() - 7) - 6 + 1) + 6; columns = rand() % ((world[i][l][0].size() - 7) - 1 + 1) + 1; }
                    }
                }
                */
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
#pragma endregion
void ChekFile(const char* text, const char* text1, const char* text2, int sizefile[3]) {
    fstream file(text); fstream file1(text1); fstream file2(text2);
    file.seekg(0, std::ios::end);
    file1.seekg(0, std::ios::end);
    file2.seekg(0, std::ios::end);
    sizefile[0] = file.tellg(); sizefile[1] = file1.tellg(); sizefile[2] = file2.tellg();
    file.close(); file1.close(); file2.close();
}
int Creation_Launch() {
    srand(time(0));
    int choice = 1, sizefile[3] = { 0,0,0 }, page = 5;//amount_save = 0,
    //choice - ���������� ��� ������;sizefile - ������ � ���� ������ �����, page - ��� ������� � ����
    string enter = "1";//enter - ���������� ��� ������� enter
    do {
        ClearLineScreen(); cout << "\t������� ����\n�������� ��������:\n[1] - ������� ����� ���\n[2] - ���������� ����\n[3] - ���������\n[4] - �� ����\n";
        if (choice >= 0 && choice <= 4) { cout << "��� �����: "; cin >> choice; }
        else if (choice < 1 || choice>4) { cout << "�������� �����!\n������� ��� ����� �����: "; cin >> choice; }
        if (choice == 3) {
            do {
                do {
                    ClearLineScreen(); cout << "\t\t���������\n[1] - �������\n";
                    if (choice == 5) { cout << "���� ������ ������!\n������� ��� ����� �����(0 ��� ������): "; cin >> choice; }
                    else { cout << "��� �����(0 ��� ������): "; cin >> choice; }
                    if (choice > 1 || choice < 0)choice = 5;
                } while (choice > 1 && choice < 0);
                if (choice == 1) {
                    do {
                        ClearLineScreen(); cout << "\t\t�������\n";
                        if (graphic == 1) { cout << "[1] - ������ �������   <---- ��� �����\n[2] - ����� ������ �������\n"; }
                        else if (graphic == 2) { cout << "[1] - ������ �������\n[2] - ����� ������ �������   <---- ��� �����\n"; }
                        else cout << "[1] - ������ �������\n[2] - ����� ������ �������\n";
                        choice = graphic;
                        if (graphic == 3) { cout << "���� ������ ������!\n������� ��� ����� �����(0 ��� ������): "; cin >> graphic; }
                        else { cout << "��� �����(0 ��� ������): "; cin >> graphic; }
                        if (graphic < 0 || graphic>2)choice = 3;
                        if (graphic == 0) { graphic = choice; choice = 0; }
                    } while (choice != 0);
                }
            } while (choice != 0);
        }
        if (choice == 4) {
            choice = 1;
            do {
                ClearLineScreen();
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
                    cout << "\t\t��������� ����:\nY - ������\ny - �������\nS - ������\nT - ��������� ������\nC - ������\nW - �������";
                }
                else if (choice == 2) {
                    cout << "�������� ������:\n\n1. ������� ����� ��� � ���������(������ 2�2) ��� � �� ��������(������ 3�3)\n\n2. ���� ��� ������ �������� � ��� ���� ����� � ���������, �� ���� ������� ������������� ���������� � ��� � ��������� ����� ������������ ���������\n\n3. ����� ���� � ��� ���� ����� � �� ��������� ����� ������ �������, �� �� �� ������� �������� ���� �� ���������� ���������\n";
                }
                else if (choice == 3) {
                    cout << "�������� ������� ����� ���������:\n\n1. �����\n\n2. �����\n\n3. �������\n\n4. ������\n\n5. ����������-�������� �����\n\n6. ����������-�������� �����\n";
                }
                cout << "\n" << choice << " �� 3 �������\n";
                if (choice == 2)cout << "[1] - ���������� �������� | [2] - ��������� ��������";
                if (choice == 1)cout << "[1] - ��������� ��������";
                if (choice == 3)cout << "[1] - ���������� ��������";
                if (page == 4) { cout << "\n���� ������ ������\n������� ��� ����� �����(0 ��� ������): "; cin >> page; }
                else { cout << "\n��� �����(0 ��� ������): "; cin >> page; }
                if (choice == 1 || choice == 3) {
                    if (page < 0 || page>1)page = 4;
                }
                else if (choice == 2) {
                    if (page < 0 || page>2)page = 4;
                }
            } while (page != 0);
            choice = 0;
        }
    } while (choice < 1 || choice>4);
    ChekFile("WorldParameter.txt", "WorldStructure.txt", "PlayerInfo.txt", sizefile);
    if (choice == 2) {
        if (sizefile[0] != 0 && sizefile[1] != 0 && sizefile[2] != 0) {
            OutInfoFile("WorldParameter.txt", "WorldStructure.txt", "PlayerInfo.txt", world, timedrop);
            while (enter.empty() != true) {
                cout << "������� enter ��� ������ ����: ";
                getline(cin, enter);
            }
            if (graphic == 2) { ClearLineScreen(); cout << "�� ������! ������� ����� ��������� ������, ��� ��������� ������������ ���!:)"; }
        }
        else if (sizefile[0] == 0 && sizefile[1] == 0 && sizefile[2] == 0) {
            cout << "� ��� ���� ����������!\n��� ��������� �������������!\n";
            choice = 1;
        }
        else if (sizefile[0] == 0 || sizefile[1] == 0 || sizefile[2] == 0) {
            do {
                cout << "���������� ����������� ������(��������� ������ �����������)!\n������� ����� ���?\n[1] - �� | [2] - ���";
                if (choice == 3) { cout << "\n���� ������ ������!\n������� ��� ����� �����: "; cin >> choice; }
                else { cout << "\n��� �����: "; cin >> choice; }
                if (choice < 1 || choice>2) { choice = 3; ClearLineScreen(); }
            } while (choice < 1 || choice>2);
        }
    }
    if (choice == 1) {
        if (sizefile[0] != 0 && sizefile[1] != 0 && sizefile[2] != 0) {
            do {
                cout << "�� �������� ��� ������ ������� ����� ���?\n���������� � �������� ���� ���������!\n[1] - ������� ����� ��� | [2] - �� ���������\n";
                if (choice == 3) { cout << "���� ������ ������!\n������� ��� ����� �����: "; cin >> choice; }
                else { cout << "��� �����: "; cin >> choice; }
                if (choice < 1 || choice>2) { choice = 3; ClearLineScreen(); }
            } while (choice < 1 || choice>2);
        }
        if (choice != 2) {
            ofstream file("WorldParameter.txt"); ofstream file1("WorldStructure.txt"); ofstream file2("PlayerInfo.txt");
            file.close(); file1.close(); file2.close();
            Sleep(3000);
            WordSize(world);
            WorldGeneration(world);
            int block = rand() % ((world.size() - 1) - 0 + 1) + 0; int bod_block = (rand() % ((world[block].size() - 1) - 0 + 1) + 0);
            research_map[block][bod_block] = true;
            world[block][bod_block][(rand() % ((world[block][0].size() - 10) - 8 + 1) + 8)][(rand() % ((world[block][0][0].size() - 10) - 8 + 1) + 8)] = '0';
            cout << "��� ��������..." << endl;
            cin.ignore();
            while (enter.empty() != true) {
                cout << "������� enter ��� ������ ����: ";
                getline(cin, enter);
            }
            if (graphic == 2) { ClearLineScreen(); cout << "�� ������! ������� ����� ��������� ������, ��� ��������� ������������ ���!:)"; }
        }
    }
    return 0;
}
#include "gl/glut.h"
#include <iostream>
#include <time.h>
#include "gl\glaux.h"
#include <Windows.h>
#include<fstream>
#include "resource.h"
#pragma warning(disable: 4996)
#pragma comment (lib, "legacy_stdio_definitions.lib")
using namespace std;
struct RECORDS {
	char NickName[20];
	int point;
	bool active;
};
char NickName[20];
RECORDS records[11];
BOOL CALLBACK DlgProcADD(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcRecords(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
HINSTANCE _hInst;
struct Bullet {
	int STARTtime;
	Bullet* next;
	Bullet* back;
	int x, y;
};
struct Ship {
	int life;
	int STARTtime;
	int x, y;
	int score = 0;
	int blink;
	Bullet* bl;
	bool win;
};
struct Enemy {
	Enemy* next;
	Enemy* back;
	Bullet* bl;
	int n;
	int life;
	float x, y;
	int blink;
	bool up;
};

unsigned int  Start1, Start2, lose, win;
float WW = 1000;
float WH = 600.0;
int z = 0;
Ship* player;
Enemy* enemy;
Enemy* boss=NULL;
Bullet* bl_en = NULL;
bool keys[256];
int STARTtime;
int STARTtime2;
int wave,max_wave=6;
bool start;

void LoadRecords() {
	int i = 0;
	char buf[MAX_PATH];
	buf[0] = 0;
	strcat(buf, "records.rk");
	ifstream in(buf, ios::binary | ios::in);
	if (in)
		while (true) {
			in.read((char*)&records[i], sizeof(RECORDS));
			if (in.eof()) { break; }
			i++;
		}
}
void SaveRecords() {
	LoadRecords();
	for (int i = 0; i < 10; i++) {
		if (player->score > records[i].point) {
			for (int j = 9; j >= i + 1; j--) {
				records[j].active = records[j - 1].active;
				strcpy(records[j].NickName, records[j - 1].NickName);
				records[j].point = records[j - 1].point;
			}
			cout << "Yes";
			DialogBox(0, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProcADD);
			records[i].point = player->score;
			records[i].active = true;
			strcpy(records[i].NickName, NickName);
			NickName[0] = 0;
			break;
		}
	}
	char buf[MAX_PATH];
	buf[0] = 0;
	//strcat(buf, path);
	strcat(buf, "records.rk");
	ofstream f(buf, ios::binary | ios::out);
	for (int i = 0; i<10; i++) {
		f.write((char*)&records[i], sizeof(RECORDS));
	}
	f.close();
}

//Функция загрузки текстур
void LoadTextures() {
	AUX_RGBImageRec *load = auxDIBImageLoadA("img/start.bmp");
		glGenTextures(1, &Start1);
		glBindTexture(GL_TEXTURE_2D, Start1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, load->sizeX, load->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, load->data);

		load = auxDIBImageLoadA("img/start2.bmp");
		glGenTextures(1, &Start2);
		glBindTexture(GL_TEXTURE_2D, Start2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, load->sizeX, load->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, load->data);
		
		load = auxDIBImageLoadA("img/lose.bmp");
		glGenTextures(1, &lose);
		glBindTexture(GL_TEXTURE_2D, lose);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, load->sizeX, load->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, load->data);

		load = auxDIBImageLoadA("img/win.bmp");
		glGenTextures(1, &win);
		glBindTexture(GL_TEXTURE_2D, win);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, load->sizeX, load->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, load->data);
}


//-----------------------------------------------------------------
//Функции рисования цифр
void null(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 5.0, oneY = (y1 - y2) / 8.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1, 0);
	glVertex3f(x1 + oneX * 4, y1, 0);
	glVertex3f(x1 + oneX * 4, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX * 4, y1 - oneY, 0);
	glVertex3f(x1 + oneX * 5, y1 - oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y2, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX * 4, y2 + oneY, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX * 4, y2 + oneY * 2, 0);
	glVertex3f(x2 - oneX * 4, y2 + oneY * 3, 0);
	glVertex3f(x2 - oneX * 3, y2 + oneY * 3, 0);
	glVertex3f(x2 - oneX * 3, y2 + oneY * 2, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX * 3, y2 + oneY * 3, 0);
	glVertex3f(x2 - oneX * 3, y2 + oneY * 4, 0);
	glVertex3f(x2 - oneX * 2, y2 + oneY * 4, 0);
	glVertex3f(x2 - oneX * 2, y2 + oneY * 3, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX * 2, y2 + oneY * 4, 0);
	glVertex3f(x2 - oneX * 2, y2 + oneY * 5, 0);
	glVertex3f(x2 - oneX * 1, y2 + oneY * 5, 0);
	glVertex3f(x2 - oneX * 1, y2 + oneY * 4, 0);
	glEnd();
}
void one(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 3.0, oneY = (y1 - y2) / 8.0;
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1, 0);
	glVertex3f(x2, y1, 0);
	glVertex3f(x2, y2, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - 2 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 2 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + 2 * oneX, y1 - oneY, 0);
	glVertex3f(x1 + 2 * oneX, y1 - 2 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 2 * oneY, 0);
	glEnd();
}
void two(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - 2 * oneY, 0);
	glVertex3f(x1, y1 - 2 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX * 3, y1 - oneY, 0);
	glVertex3f(x1 + oneX * 4, y1 - oneY, 0);
	glVertex3f(x1 + oneX * 4, y1 - 3 * oneY, 0);
	glVertex3f(x1 + oneX * 3, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX * 2, y1 - 3 * oneY, 0);
	glVertex3f(x1 + oneX * 3, y1 - 3 * oneY, 0);
	glVertex3f(x1 + oneX * 3, y1 - 4 * oneY, 0);
	glVertex3f(x1 + oneX * 2, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + 3 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y2 + 3 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2, y2, 0);
	glVertex3f(x1, y2, 0);
	glEnd();
}
void three(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - 2 * oneY, 0);
	glVertex3f(x1, y1 - 2 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + 2 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 4 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x2, y1 - oneY, 0);
	glVertex3f(x2, y1 - 3 * oneY, 0);
	glVertex3f(x2 - oneX, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glEnd();
}
void four(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1, 0);
	glVertex3f(x2, y1, 0);
	glVertex3f(x2, y2, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX * 2, y1 - oneY, 0);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x2 - oneX, y1 - 2 * oneY, 0);
	glVertex3f(x2 - 2 * oneX, y1 - 2 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1 - 2 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y1 - 2 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - 3 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 4 * oneY, 0);
	glVertex3f(x1, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 3 * oneY, 0);
	glVertex3f(x2 - oneX, y2 + 3 * oneY, 0);
	glVertex3f(x2 - oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1, y2 + 2 * oneY, 0);
	glEnd();


}
void five(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, 0);
	glVertex3f(x2, y1, 0);
	glVertex3f(x2, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 4 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glEnd();
}
void six(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1, 0);
	glVertex3f(x2 - oneX, y1, 0);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 4 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x2, y1 - oneY, 0);
	glVertex3f(x2, y1 - 2 * oneY, 0);
	glVertex3f(x2 - oneX, y1 - 2 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glEnd();
}
void seven(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1 - oneY, 0);
	glVertex3f(x1, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x2, y1 - oneY, 0);
	glVertex3f(x2, y1 - oneY * 3, 0);
	glVertex3f(x2 - oneX, y1 - oneY * 3, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - 2 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x2 - oneX, y1 - 3 * oneY, 0);
	glVertex3f(x2 - oneX, y1 - 4 * oneY, 0);
	glVertex3f(x2 - 2 * oneX, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + 3 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y2 + 3 * oneY, 0);
	glVertex3f(x1 + 2 * oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glVertex3f(x1, y2, 0);
	glEnd();
}
void eight(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1, 0);
	glVertex3f(x2 - oneX, y1, 0);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x2, y1 - oneY, 0);
	glVertex3f(x2, y1 - 3 * oneY, 0);
	glVertex3f(x2 - oneX, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 4 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 3 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 3 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + 3 * oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glEnd();
}
void nine(float x1, float y1, float x2, float y2) {
	float oneX = (x2 - x1) / 4.0, oneY = (y1 - y2) / 7.0;
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1, 0);
	glVertex3f(x1 + 3 * oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - oneY, 0);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1, y1 - 3 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 3 * oneY, 0);
	glVertex3f(x1 + 3 * oneX, y1 - 4 * oneY, 0);
	glVertex3f(x1 + oneX, y1 - 4 * oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x2 - oneX, y1 - oneY, 0);
	glVertex3f(x2, y1 - oneY, 0);
	glVertex3f(x2, y2 + oneY, 0);
	glVertex3f(x2 - oneY, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + 2 * oneY, 0);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x1, y2 + oneY, 0);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3f(x1 + oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2 + oneY, 0);
	glVertex3f(x2 - oneX, y2, 0);
	glVertex3f(x1 + oneX, y2, 0);
	glEnd();
}
//------------------------------------------------------------------

//Функция инициализации игры
void newGame() {
	for (int i = 0; i < 10; i++) {
		records[i].point = 0;
	}
	STARTtime2 = GetTickCount();
	wave = 0;
	STARTtime = GetTickCount() - 2000;
	if (player != NULL) delete player;
	player = new Ship;
	player->STARTtime = GetTickCount();
	player->x = 0;
	player->y = WH - 50;
	player->life = 3;
	player->score = 0;
	player->bl = NULL;
	player->blink = 0;
	player->win = false;
	if (enemy != NULL) delete enemy;
	enemy = NULL;
	if (boss != NULL) delete boss;
	boss = NULL;
	if (bl_en != NULL) delete bl_en;
	bl_en = NULL;
}

//отрисовка босса
void DrawBoss() {
	if (boss != NULL) {
		if (boss->blink == 0) {
			glRectf(boss->x + 45, boss->y, boss->x + 65, boss->y - 5);
			glRectf(boss->x + 40, boss->y - 5, boss->x + 45, boss->y - 10);
			glRectf(boss->x + 55, boss->y - 5, boss->x + 60, boss->y - 10);
			glRectf(boss->x + 75, boss->y - 5, boss->x + 95, boss->y - 10);
			glRectf(boss->x + 50, boss->y - 10, boss->x + 55, boss->y - 15);
			glRectf(boss->x + 35, boss->y - 10, boss->x + 40, boss->y - 15);
			glRectf(boss->x + 70, boss->y - 10, boss->x + 75, boss->y - 15);
			glRectf(boss->x + 85, boss->y - 10, boss->x + 90, boss->y - 15);

			glRectf(boss->x, boss->y - 15, boss->x + 15, boss->y - 20);
			glRectf(boss->x + 30, boss->y - 15, boss->x + 55, boss->y - 20);
			glRectf(boss->x + 65, boss->y - 15, boss->x + 70, boss->y - 20);
			glRectf(boss->x + 80, boss->y - 15, boss->x + 85, boss->y - 20);

			glRectf(boss->x, boss->y - 20, boss->x + 5, boss->y - 25);
			glRectf(boss->x + 15, boss->y - 20, boss->x + 30, boss->y - 25);
			glRectf(boss->x + 35, boss->y - 20, boss->x + 40, boss->y - 25);
			glRectf(boss->x + 45, boss->y - 20, boss->x + 50, boss->y - 25);
			glRectf(boss->x + 55, boss->y - 20, boss->x + 70, boss->y - 25);
			glRectf(boss->x + 80, boss->y - 20, boss->x + 85, boss->y - 25);

			glRectf(boss->x + 5, boss->y - 25, boss->x + 10, boss->y - 30);
			glRectf(boss->x + 20, boss->y - 25, boss->x + 25, boss->y - 30);
			glRectf(boss->x + 30, boss->y - 25, boss->x + 35, boss->y - 30);
			glRectf(boss->x + 40, boss->y - 25, boss->x + 45, boss->y - 30);
			glRectf(boss->x + 50, boss->y - 25, boss->x + 55, boss->y - 30);
			glRectf(boss->x + 60, boss->y - 25, boss->x + 65, boss->y - 30);
			glRectf(boss->x + 70, boss->y - 25, boss->x + 85, boss->y - 30);

			glRectf(boss->x + 5, boss->y - 30, boss->x + 10, boss->y - 35);
			glRectf(boss->x + 20, boss->y - 30, boss->x + 30, boss->y - 35);
			glRectf(boss->x + 70, boss->y - 30, boss->x + 75, boss->y - 35);
			glRectf(boss->x + 80, boss->y - 30, boss->x + 90, boss->y - 35);

			glRectf(boss->x + 10, boss->y - 35, boss->x + 20, boss->y - 40);
			glRectf(boss->x + 40, boss->y - 35, boss->x + 60, boss->y - 40);
			glRectf(boss->x + 75, boss->y - 35, boss->x + 80, boss->y - 40);
			glRectf(boss->x + 85, boss->y - 35, boss->x + 90, boss->y - 40);
			glRectf(boss->x + 105, boss->y - 35, boss->x + 110, boss->y - 55);

			glRectf(boss->x + 10, boss->y - 40, boss->x + 15, boss->y - 45);
			glRectf(boss->x + 35, boss->y - 40, boss->x + 40, boss->y - 45);
			glRectf(boss->x + 50, boss->y - 40, boss->x + 65, boss->y - 45);
			glRectf(boss->x + 90, boss->y - 40, boss->x + 95, boss->y - 45);
			glRectf(boss->x + 100, boss->y - 40, boss->x + 105, boss->y - 45);

			glRectf(boss->x + 5, boss->y - 45, boss->x + 10, boss->y - 90);

			glRectf(boss->x + 30, boss->y - 45, boss->x + 35, boss->y - 50);
			glRectf(boss->x + 40, boss->y - 45, boss->x + 45, boss->y - 50);
			glRectf(boss->x + 55, boss->y - 45, boss->x + 65, boss->y - 50);
			glRectf(boss->x + 75, boss->y - 45, boss->x + 80, boss->y - 50);
			glRectf(boss->x + 90, boss->y - 45, boss->x + 100, boss->y - 50);

			glRectf(boss->x + 30, boss->y - 50, boss->x + 45, boss->y - 55);
			glRectf(boss->x + 55, boss->y - 50, boss->x + 65, boss->y - 55);
			glRectf(boss->x + 80, boss->y - 50, boss->x + 90, boss->y - 55);
			glRectf(boss->x + 100, boss->y - 50, boss->x + 105, boss->y - 55);

			glRectf(boss->x + 35, boss->y - 55, boss->x + 60, boss->y - 60);
			glRectf(boss->x + 85, boss->y - 55, boss->x + 90, boss->y - 60);
			glRectf(boss->x + 95, boss->y - 55, boss->x + 105, boss->y - 60);

			glRectf(boss->x + 15, boss->y - 60, boss->x + 20, boss->y - 65);
			glRectf(boss->x + 70, boss->y - 60, boss->x + 75, boss->y - 65);
			glRectf(boss->x + 85, boss->y - 60, boss->x + 100, boss->y - 65);

			glRectf(boss->x + 10, boss->y - 65, boss->x + 15, boss->y - 70);
			glRectf(boss->x + 20, boss->y - 65, boss->x + 40, boss->y - 70);
			glRectf(boss->x + 75, boss->y - 65, boss->x + 95, boss->y - 70);

			glRectf(boss->x + 10, boss->y - 70, boss->x + 15, boss->y - 75);
			glRectf(boss->x + 20, boss->y - 70, boss->x + 25, boss->y - 75);
			glRectf(boss->x + 30, boss->y - 70, boss->x + 35, boss->y - 75);
			glRectf(boss->x + 40, boss->y - 70, boss->x + 45, boss->y - 75);
			glRectf(boss->x + 65, boss->y - 70, boss->x + 70, boss->y - 75);
			glRectf(boss->x + 80, boss->y - 70, boss->x + 85, boss->y - 75);

			glRectf(boss->x + 10, boss->y - 75, boss->x + 15, boss->y - 80);
			glRectf(boss->x + 20, boss->y - 75, boss->x + 40, boss->y - 80);
			glRectf(boss->x + 55, boss->y - 75, boss->x + 60, boss->y - 80);
			glRectf(boss->x + 70, boss->y - 75, boss->x + 85, boss->y - 80);

			glRectf(boss->x + 15, boss->y - 80, boss->x + 20, boss->y - 85);
			glRectf(boss->x + 25, boss->y - 80, boss->x + 30, boss->y - 85);
			glRectf(boss->x + 60, boss->y - 80, boss->x + 65, boss->y - 85);
			glRectf(boss->x + 70, boss->y - 80, boss->x + 75, boss->y - 85);
			glRectf(boss->x + 85, boss->y - 80, boss->x + 90, boss->y - 85);

			glRectf(boss->x + 50, boss->y - 85, boss->x + 55, boss->y - 90);
			glRectf(boss->x + 60, boss->y - 85, boss->x + 75, boss->y - 90);
			glRectf(boss->x + 85, boss->y - 85, boss->x + 90, boss->y - 90);

			glRectf(boss->x + 10, boss->y - 90, boss->x + 25, boss->y - 95);
			glRectf(boss->x + 40, boss->y - 90, boss->x + 45, boss->y - 95);
			glRectf(boss->x + 70, boss->y - 90, boss->x + 80, boss->y - 95);
			glRectf(boss->x + 90, boss->y - 90, boss->x + 95, boss->y - 95);
			glRectf(boss->x + 55, boss->y - 90, boss->x + 60, boss->y - 95);

			glRectf(boss->x + 25, boss->y - 95, boss->x + 40, boss->y - 100);
			glRectf(boss->x + 45, boss->y - 95, boss->x + 55, boss->y - 100);
			glRectf(boss->x + 75, boss->y - 95, boss->x + 95, boss->y - 100);
		}else boss->blink--;
	}
}
//отрисовка очков
void DrawScore() {
	int temp = player->score;
	for (int i = 0; i < 6; i++) {
		if (temp != 0) {
			switch (temp%10)
			{
			case 0: 
				null(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 1:
				one(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 2:
				two(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 3:
				three(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 4:
				four(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 5:
				five(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 6:
				six(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 7:
				seven(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 8:
				eight(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			case 9:
				nine(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
				break;
			}
			temp = temp / 10;
		}
		else {
			null(WW - 100 - 30 * i, WH - 3, WW - 100 - 30 * (i - 1) - 5, WH - 47);
		}
	}
}
//отрисока жизней
void DrawHeart(int x,int y) {
	glRectf(x + 8, y, x + 20, y - 4);
	glRectf(x + 24, y, x + 36, y - 4);
	glRectf(x + 4, y-4, x + 40, y - 8);
	glRectf(x , y - 8, x + 44, y - 24);
	glRectf(x + 4, y - 24, x + 40, y - 28);
	glRectf(x + 8, y - 28, x + 36, y - 32);
	glRectf(x + 12, y - 32, x + 32, y - 36);
	glRectf(x + 16, y - 36, x + 28, y - 40);
	glRectf(x + 20, y - 40, x + 24, y - 44);
}
//отрисовка врагов
void DrawEnemy() {
	Enemy* temp=enemy;
	glColor3f(0, 0, 0);
	while (temp != NULL) {
		if (temp->blink>0) { //если моргает не отрисовывать
			temp->blink --;
			temp = temp->next;
			continue;
		}
		switch (temp->n) {
		case 0:
			glRectf(temp->x, temp->y - 20, temp->x + 10, temp->y - 30);
			glRectf(temp->x + 10, temp->y - 10, temp->x + 20, temp->y - 20);
			glRectf(temp->x + 10, temp->y - 30, temp->x + 20, temp->y - 40);
			glRectf(temp->x + 20, temp->y - 20, temp->x + 30, temp->y - 30);
			glRectf(temp->x + 20, temp->y, temp->x + 30, temp->y - 10);
			glRectf(temp->x + 20, temp->y - 40, temp->x + 30, temp->y - 50);
			glRectf(temp->x + 30, temp->y, temp->x + 50, temp->y - 50);
			glRectf(temp->x + 50, temp->y - 10, temp->x + 60, temp->y - 20);
			glRectf(temp->x + 50, temp->y - 30, temp->x + 60, temp->y - 40);
			glRectf(temp->x + 60, temp->y, temp->x + 90, temp->y - 10);
			glRectf(temp->x + 60, temp->y - 20, temp->x + 90, temp->y - 30);
			glRectf(temp->x + 60, temp->y - 40, temp->x + 90, temp->y - 50);
			break;
		case 1:
			glRectf(temp->x, temp->y, temp->x + 10, temp->y - 10);
			glRectf(temp->x + 10, temp->y - 10, temp->x + 20, temp->y - 20);
			glRectf(temp->x, temp->y - 20, temp->x + 10, temp->y - 50);
			glRectf(temp->x, temp->y - 60, temp->x + 10, temp->y - 70);
			glRectf(temp->x + 10, temp->y - 50, temp->x + 20, temp->y - 60);
			glRectf(temp->x + 20, temp->y, temp->x + 30, temp->y - 10);
			glRectf(temp->x + 20, temp->y - 20, temp->x + 30, temp->y - 30);
			glRectf(temp->x + 20, temp->y - 40, temp->x + 30, temp->y - 50);
			glRectf(temp->x + 20, temp->y - 60, temp->x + 30, temp->y - 70);
			glRectf(temp->x + 30, temp->y, temp->x + 50, temp->y - 20);
			glRectf(temp->x + 30, temp->y - 50, temp->x + 50, temp->y - 70);
			glRectf(temp->x + 30, temp->y - 30, temp->x + 40, temp->y - 40);
			glRectf(temp->x + 40, temp->y - 20, temp->x + 70, temp->y - 50);
			glRectf(temp->x + 50, temp->y - 50, temp->x + 60, temp->y - 60);
			glRectf(temp->x + 60, temp->y - 60, temp->x + 70, temp->y - 70);
			glRectf(temp->x + 50, temp->y - 10, temp->x + 60, temp->y - 20);
			glRectf(temp->x + 60, temp->y, temp->x + 70, temp->y - 10);
			break;
		case 2:
			break;
		}
		temp = temp->next;
	}
}
//отрисовка коробля
void DrawShip() {
	if (player->blink == 0) {
		glColor3f(0, 0, 0);
		glRectf(player->x + 10, player->y - 10, player->x + 30, player->y - 20);
		glRectf(player->x, player->y, player->x + 20, player->y - 10);

		glRectf(player->x + 20, player->y - 20, player->x + 30, player->y - 30);

		glRectf(player->x + 10, player->y - 30, player->x + 20, player->y - 40);
		glRectf(player->x + 30, player->y - 30, player->x + 40, player->y - 40);
		glRectf(player->x + 20, player->y - 40, player->x + 30, player->y - 50);

		glRectf(player->x + 10, player->y - 50, player->x + 30, player->y - 60);
		glRectf(player->x, player->y - 60, player->x + 20, player->y - 70);

		glRectf(player->x + 40, player->y - 10, player->x + 60, player->y - 60);

		glRectf(player->x + 60, player->y - 10, player->x + 70, player->y - 20);
		glRectf(player->x + 60, player->y - 30, player->x + 70, player->y - 40);
		glRectf(player->x + 60, player->y - 50, player->x + 70, player->y - 60);

		glRectf(player->x + 70, player->y - 20, player->x + 80, player->y - 30);

		glRectf(player->x + 70, player->y - 40, player->x + 80, player->y - 50);

		glRectf(player->x + 80, player->y - 30, player->x + 100, player->y - 40);
	}
	else player->blink--;
}
//отрисовка пуль
void DrawBullet() {
	Bullet* temp = player->bl;
	while (temp != NULL) {
		glColor3f(0, 0, 0);
		glRectf(temp->x, temp->y, temp->x + 15, temp->y + 5);
		temp = temp->next;
	}
		temp = bl_en;
		while (temp != NULL) {
			glColor3f(0, 0, 0);
			glRectf(temp->x, temp->y, temp->x + 15, temp->y + 5);
			temp = temp->next;
		}
}

//рисуем свмое первое менб
void DrawStart() {
	static bool blick = false;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	if (GetTickCount() - STARTtime2 >= 500) {
		blick = !blick;
		STARTtime2 = GetTickCount();
	}
	if (blick)
		glBindTexture(GL_TEXTURE_2D, Start2);
	else
		glBindTexture(GL_TEXTURE_2D, Start1);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(0, 1); glVertex2f(0, WH);
	glTexCoord2f(1, 1); glVertex2f(WW, WH);
	glTexCoord2f(1, 0); glVertex2f(WW, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}
//отрисовка экрана проигрыша
void DrawLose() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lose);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(0, 1); glVertex2f(0, WH);
	glTexCoord2f(1, 1); glVertex2f(WW, WH);
	glTexCoord2f(1, 0); glVertex2f(WW, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}
//отрисовка экрана победы
void DrawWin() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, win);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0); glVertex2f(0, 0);
	glTexCoord2f(0, 1); glVertex2f(0, WH);
	glTexCoord2f(1, 1); glVertex2f(WW, WH);
	glTexCoord2f(1, 0); glVertex2f(WW, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glutSwapBuffers();
}
//отрисовка игры
void DrawGame() {
	glClearColor(0.45, 0.647, 0.51, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(0, WH - 50);
	glVertex2f(WW, WH - 50);
	glEnd();
	DrawBoss();
	DrawBullet();
	DrawShip();
	DrawEnemy();
	for (int i = 0; i < player->life; i++) {
		DrawHeart(3 + 50 * i, WH - 3);
	}
	DrawScore();
	glutSwapBuffers();
}
void Draw()
{
	if (!start) {
		if (player->life < 0) {
			DrawLose();
		}
		else
			if (player->win) {
				DrawWin();
			}
			else DrawGame();
	}
	else {
		DrawStart();
	}

}
//Функция проверок в игре
void check() {
	Enemy* tempEnemy = enemy;
	Bullet* tempBL;
	while (tempEnemy != NULL) { //Проверяем всех врагов
		tempBL = player->bl;
		while (tempBL != NULL) { //Пробегаемся по пулям
			if (((tempBL->x + 15 >= tempEnemy->x) && (tempBL->y <= tempEnemy->y) && (tempBL->y >= tempEnemy->y - 72) && (tempEnemy->n == 1)) ||
				((tempBL->x + 15 >= tempEnemy->x) && (tempBL->y <= tempEnemy->y) && (tempBL->y >= tempEnemy->y - 52) && (tempEnemy->n == 0))) {
				//Если попали снимаем жизни у врага
				tempEnemy->life--;
				tempEnemy->blink = 50; //создаем эффект мигания
									   //-----------------------------------------------------------------
									   //Удаляем пулю их списка
				if (tempBL->back != NULL) tempBL->back->next = tempBL->next;
				else player->bl = tempBL->next;
				if (tempBL->next != NULL) tempBL->next->back = tempBL->back;
				Bullet* temp = tempBL;
				tempBL = temp->next;
				delete temp;
				//-------------------------------------------------------------------
				if (tempEnemy->life == 0) {// Если у врага закончились жизни удаляем его
					if (tempEnemy->back != NULL) tempEnemy->back->next = tempEnemy->next;
					else enemy = tempEnemy->next;
					if (tempEnemy->next != NULL) tempEnemy->next->back = tempEnemy->back;
					Enemy* temp = tempEnemy;
					if (tempEnemy->back != NULL) tempEnemy = tempEnemy->back;
					else tempEnemy = enemy;
					if (temp->n == 1) player->score += 50;// Добавляем очки
					if (temp->n == 0) player->score += 75;//
					delete temp;

					break;
				}
			}
			else {
				//Если есть бос проверяем попали ли в него
				if ((boss != NULL) && (tempBL->x + 15 >= boss->x) && (tempBL->y <= boss->y) && (tempBL->y >= boss->y - 102)) {
					boss->life -= 1; //Снимаем жизни
					boss->blink = 50; //Эффект мигания
									  //------------------------------------------------------------
									  //Удаляем пулю
					if (tempBL->back != NULL) tempBL->back->next = tempBL->next;
					else player->bl = tempBL->next;
					if (tempBL->next != NULL) tempBL->next->back = tempBL->back;
					Bullet* temp = tempBL;
					tempBL = temp->next;
					delete temp;
					//-----------------------------------------------------------
					if (boss->life == 0) player->win = true; //Если босс убит помечаем, что мы выйграли

				}
				else tempBL = tempBL->next; //Переходим проверять следующую пул
			}
		}
		if (tempEnemy != NULL) {
			//Проверка на столкновение с врагом
			if (tempEnemy->n == 0) {
				if (((player->x + 100 >= tempEnemy->x) && (player->x + 100 <= tempEnemy->x + 80)) || ((player->x >= tempEnemy->x) && (player->x <= tempEnemy->x + 80)))
					if (((player->y <= tempEnemy->y) && (player->y >= tempEnemy->y - 50)) || ((player->y - 70 <= tempEnemy->y) && (player->y - 70 >= tempEnemy->y - 50))) {
						//если столкнулись снимаем жизнь
						player->life--;
						player->blink = 50; //эффект мигания
											//---------------------------------------------------------------------
											//Удаляем врага
						if (tempEnemy->back != NULL) tempEnemy->back->next = tempEnemy->next;
						else enemy = tempEnemy->next;
						if (tempEnemy->next != NULL) tempEnemy->next->back = tempEnemy->back;
						Enemy* temp = tempEnemy;
						if (tempEnemy->back != NULL) tempEnemy = tempEnemy->back;
						else tempEnemy = enemy;
						delete temp;
						//-----------------------------------------------------------
					}
			}
			else if ((tempEnemy != NULL) && (tempEnemy->n == 1)) //аналогично как выше
				if (((player->x + 100 >= tempEnemy->x) && (player->x + 100 <= tempEnemy->x + 70)) || ((player->x >= tempEnemy->x) && (player->x <= tempEnemy->x + 70)))
					if (((player->y <= tempEnemy->y) && (player->y >= tempEnemy->y - 70)) || ((player->y - 70 <= tempEnemy->y) && (player->y - 70 >= tempEnemy->y - 70))) {
						player->life--;
						player->blink = 50;
						if (tempEnemy->back != NULL) tempEnemy->back->next = tempEnemy->next;
						else enemy = tempEnemy->next;
						if (tempEnemy->next != NULL) tempEnemy->next->back = tempEnemy->back;
						Enemy* temp = tempEnemy;
						if (tempEnemy->back != NULL) tempEnemy = tempEnemy->back;
						else tempEnemy = enemy;
						delete temp;
					}
		}
		if (tempEnemy != NULL) tempEnemy = tempEnemy->next;
	}
	tempEnemy = enemy;

	tempBL = bl_en;
	while (tempBL != NULL) {
		//Проверяем попали ли в нас пули врака
		if ((player->x + 100 >= tempBL->x) && (player->x <= tempBL->x + 15))
			if ((player->y >= tempBL->y) && (player->y - 72 <= tempBL->y)) {
				player->life--;
				player->blink = 50;
				//---------------------------------------------------------
				//Удаляем пулю
				if (tempBL->back != NULL) tempBL->back->next = tempBL->next;
				else bl_en = tempBL->next;
				if (tempBL->next != NULL) tempBL->next->back = tempBL->back;
				Bullet* temp = tempBL;
				tempBL = tempBL->next;
				delete temp;
				//--------------------------------------------------------
				continue;
			}
		if (tempBL != NULL) tempBL = tempBL->next;
	}
	if ((player->life < 0) || (player->win)) {
		Draw();
		SaveRecords();
	}
}
//спауним линию стреляющих врагов
void addFirstLine() {
	int random = 700;
	if (enemy == NULL) {
		enemy = new Enemy;
		enemy->back = NULL;
		enemy->x = WW +(rand() % random);
		enemy->y = WH - 55;
		enemy->n = 0;
		enemy->bl = NULL;
		enemy->life = 6;
		enemy->blink = false;
		enemy->next = new Enemy;
		enemy->next->back = enemy;
		Enemy* temp = enemy->next;
		for (int i = 1; i < 5; i++) {
			temp->blink = false;
			temp->bl = NULL;
			temp->x= WW +i*(rand()% random);
			temp->n = 0;
			temp->life = 6;
			temp->y = WH - 55 - 120*i;
			if (i != 4) {
				temp->next = new Enemy;
				temp->next->back = temp;
				temp = temp->next;
			}
			else temp->next = NULL;
		}
	}
	else {
		if (boss != NULL) {
				Enemy* temp = enemy;
				while (temp->next != NULL) temp = temp->next;
				temp->next = new Enemy;
				temp->next->back = temp;
				temp = temp->next;
				temp->next = NULL;
				temp->n = 0;
				temp->bl = NULL;
				temp->life = 6;
				temp->blink = false;
				temp->x = WW + (rand() % random);
				switch (rand() % 5) {
				case 0:
					temp->y = WH - 55;
					break;
				case 1:
					temp->y = WH - 55 - 120;
					break;
				case 2:
					temp->y = WH - 55 - 120 * 2;
					break;
				case 3:
					temp->y = WH - 55 - 120 * 3;
					break;
				case 4:
					temp->y = WH - 55 - 120 * 4;
					break;
				}
			}
	}

}
//спауним линию не стреляющих врагов
void addSecondLine() {
	int random = 700;
	if (enemy == NULL) {
		enemy = new Enemy;
		enemy->back = NULL;
		enemy->x = WW + (rand() % random);
		enemy->y = WH - 55;
		enemy->n = 1;
		enemy->bl = NULL;
		enemy->next = new Enemy;
		enemy->blink = false;
		enemy->life = 15;
		enemy->next->back = enemy;
		Enemy* temp = enemy->next;
		for (int i = 1; i < 5; i++) {
			temp->blink=false;
			temp->bl = NULL;
			temp->life = 15;
			temp->x = WW + i * (rand() % random);
			temp->n = 1;
			temp->y = WH - 55 - 120 * i;
			if (i != 4) {
				temp->next = new Enemy;
				temp->next->back = temp;
				temp = temp->next;
			}
			else temp->next = NULL;
		}
	}
	else {
		if (boss != NULL) {
				Enemy* temp = enemy;
				while (temp->next != NULL) temp = temp->next;
				temp->next = new Enemy;
				temp->next->back = temp;
				temp = temp->next;
				temp->next = NULL;
				temp->n = 1;
				temp->bl = NULL;
				temp->life = 15;
				temp->blink = false;
				temp->x = WW + (rand() % random);
				switch (rand() % 5) {
				case 0:
					temp->y = WH - 55;
					break;
				case 1:
					temp->y = WH - 55 - 120;
					break;
				case 2:
					temp->y = WH - 55 - 120 * 2;
					break;
				case 3:
					temp->y = WH - 55 - 120 * 3;
					break;
				case 4:
					temp->y = WH - 55 - 120 * 4;
					break;
				}
		}
	}
}

//отслеживаем отжатие клавишы
void KeyUP(unsigned char key, int x, int y) {
	switch (key)
	{
	case 87:
	case 246:
	case 119:
	case 214:
		keys[87] = false;
		break;
	case 244:
	case 65:
	case 97:
	case 212:
		keys[65] = false;
		break;
	case 226:
	case 100:
	case 194:
	case 68:
		keys[68] = false;
		break;
	case 251:
	case 115:
	case 88:
	case 219:
		keys[88] = false;
		break;
	default:
		keys[key] = false;
	}

}
//отслеживаем нажатие клавишы
void KeyDown(unsigned char key, int x, int y) {
	
	if ((start)&&((key!=234)&&(key!=202)&&(key!=82)&&(key!=114))){ 
		start = false; 
	}else
		if (start) {
			DialogBox(_hInst, MAKEINTRESOURCE(IDD_DIALOG2), 0, DlgProcRecords);
		}
	else{
		if ((player->life < 0) || (player->win)) {
			start = true;
			newGame();
		}
		else {
			switch (key)
			{
			case 87:
			case 246:
			case 119:
			case 214:
				keys[87] = true;
				break;
			case 244:
			case 65:
			case 97:
			case 212:
				keys[65] = true;
				break;
			case 226:
			case 100:
			case 194:
			case 68:
				keys[68] = true;
				break;
			case 251:
			case 115:
			case 88:
			case 219:
				keys[88] = true;
				break;
			default:
				keys[key] = true;
			}
		}
	}
}
//добавляем пулю в список
void addBullet(Bullet* bl, int x, int y, int n) {
	if (bl != NULL) {//если в списке что-то есть
		while (bl->next != NULL) bl = bl->next;
		if ((GetTickCount() - bl->STARTtime >= 200)||(n!=0)) {
			bl->next = new Bullet;
			bl->next->x = x;
			bl->next->y = y;
			bl->next->next = NULL;
			bl->next->back = bl;
			bl->next->STARTtime = GetTickCount();
		}
	}
	else {
		if (n == 0) {//если пустой и добавляем игроку
			player->bl = new Bullet;
			player->bl->next = NULL;
			player->bl->x = x;
			player->bl->y = y;
			player->bl->back = NULL;
			player->bl->STARTtime = GetTickCount();
		}
		else { // если пустой и добавляем врагу
				bl_en = new Bullet;
				bl_en->next = NULL;
				bl_en->x = x;
				bl_en->y = y;
				bl_en->back = NULL;
				bl_en->STARTtime = GetTickCount();
		}
	}
}
//обновляем положение врагов
void UpdateEnemy() {
	Enemy* temp = enemy;
	while (temp != NULL) {
		temp->x -=1.3;
		if (temp->x <= 0) {
			Enemy* temp2 = temp;
			if (temp->back == NULL) {
				enemy = enemy->next;
				temp = enemy;
				if (enemy!=NULL) enemy->back = NULL;
			}
			else {
				temp->back->next = temp->next;
				if (temp->next!=NULL) temp->next->back = temp->back;
				temp = temp->next;
			}
			delete temp2;
		}else temp = temp->next;
	}
	temp = enemy;
	int j = 1;
	if (boss != NULL) {
		if (boss->x > WW - 110) boss->x -= 1.3;
		else {
			if ((boss->y-100 > 0) && (boss->up == false)) {
				boss->y -= 2;
				if (boss->y-100 < 2) boss->up = true;
			}
			else {
				if ((boss->y < WH - 50) && (boss->up == true)) {
					boss->y += 2;
					if (boss->y > WH - 52) boss->up = false;
				}
			}
		}
	}
	if (GetTickCount() - STARTtime >= 2000) {//каждые 2 секунды враг стреляет
		while (temp != NULL) {
			if ((temp->n == 0)&&(temp->x<WW)) addBullet(bl_en, temp->x, temp->y - 27, j);
			temp = temp->next;
			j++;
		}
		if (boss!=NULL) addBullet(bl_en, boss->x+5, boss->y - 72, -1);
		STARTtime = GetTickCount();
	}
}
//обновляем положение пуль
void UpdateBullet() {
		Bullet* temp = player->bl;
		while (temp != NULL) {
			temp->x += 10;
			if (temp->x > WW) {
				player->bl = player->bl->next;
				delete temp;
				if (player->bl == NULL) break;
				player->bl->back = NULL;
				temp = player->bl;
				continue;
			}
			temp = temp->next;
		}
		temp = bl_en;
		while (temp != NULL) {
				temp->x -= 10;
				if (temp->x < 0) {
					Bullet* temp2 = temp;
					temp =temp->next;
					if (temp != NULL) temp->back = temp2->back;
					if (temp2->back != NULL) temp2->back->next = temp;
					else {
						bl_en = temp;
					}
					delete temp2;
					continue;
				}
				temp = temp->next;
		}
		
}
void Update() {

	if (GetTickCount() - player->STARTtime >= 1) {
		player->STARTtime = GetTickCount();
		if (keys[87]) {
			if (player->y + 8 <= WH - 50) player->y += 8;
		}
		if (keys[88]) {
			if (player->y - 8 >= 70) player->y -= 8;
		}
		if (keys[65]) {
			if (player->x - 8 >= 0) player->x -= 8;
		}
		if (keys[68]) {
			if (player->x + 8 <= WW - 230) player->x += 8;
		}
		UpdateEnemy();
		UpdateBullet();
	}
	if (keys[32]) addBullet(player->bl, player->x + 100, player->y - 37, 0);

	if ((enemy == NULL)&&(max_wave !=wave)&&(GetTickCount()-STARTtime2>=500)) { 
		//если нет врагов и мы не дошли до босса добавляем новую волну
		switch (rand()%2+1)
		{
		case 1:
			addFirstLine();
		case 2:
			addSecondLine();
		default:
			addFirstLine();
		}
		wave++;
	}
	else {
		if ((enemy==NULL)&&(max_wave == wave) && (boss == NULL)) {
			//если дошли до босса добавляем его
			boss = new Enemy;
			boss->x = WW + 105;
			boss->y = WH-50;
			boss->life = 50;
			boss->up = false;
			boss->bl = NULL;
			boss->blink = 0;
			STARTtime2 = GetTickCount();
		}
		else {
			//если дошли до босса каждые 4 секунды спаумим врагов
			if ((GetTickCount() - STARTtime2 >= 4000)) {
				switch (rand() % 2 + 1)
				{
				case 1:
					addFirstLine();
					break;
				case 2:
					addSecondLine();
					break;
				default:
					addFirstLine();
					break;
				}
				STARTtime2 = GetTickCount();
			}
		}
	}
	if ((player->life>-1)&&(!player->win)&&!start) check();
}
void Timer(int)
{
	if (!start) {
		if ((player->win) || (player->life == 0)) {  }
		Update();
	}
	glutPostRedisplay();
	glutTimerFunc(1, Timer, 0);
}




int main(int argc, char **argv) {

	start = true;
	srand(time(0));
	newGame();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WW, WH);
	glutInitWindowPosition(40, 20);
	glutCreateWindow("Space impact");
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WW, 0, WH, -1, 1);
	glutDisplayFunc(Draw);
	glutTimerFunc(50, Timer, 0);
	glutKeyboardUpFunc(KeyUP);
	glutKeyboardFunc(KeyDown);
	glEnable(GL_DEPTH_TEST);
	LoadTextures();
	glutMainLoop();
}
BOOL CALLBACK DlgProcADD(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit = GetDlgItem(hDlg, IDC_EDIT1);
	switch (msg) {
	case WM_INITDIALOG: {
		SendMessage(hEdit, EM_LIMITTEXT, 19, 0);
		return TRUE;
	}
	case WM_LBUTTONDOWN: {
		return TRUE;
	}
	case WM_CLOSE: {
		if (NickName[0] == 0 || NickName[0] == ' ') {
			MessageBox(0, "Заполните поле", "Ошибка", MB_OK);
		}
		else EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK) {
			GetWindowText(hEdit, NickName, 19);
			if (NickName[0] == 0 || NickName[0] == ' ') {
				MessageBox(0, "Заполните поле", "Ошибка", MB_OK);
			}
			else EndDialog(hDlg, 0);
		}
		return TRUE;
	}
	case WM_SIZE: {

		return TRUE;
	}
	}
	return FALSE;
}
BOOL CALLBACK DlgProcRecords(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	HWND hNickName[10], hScore[10];
	hNickName[0] = GetDlgItem(hDlg, IDC_NICKNAME1);
	hNickName[1] = GetDlgItem(hDlg, IDC_NICKNAME2);
	hNickName[2] = GetDlgItem(hDlg, IDC_NICKNAME3);
	hNickName[3] = GetDlgItem(hDlg, IDC_NICKNAME4);
	hNickName[4] = GetDlgItem(hDlg, IDC_NICKNAME5);
	hNickName[5] = GetDlgItem(hDlg, IDC_NICKNAME6);
	hNickName[6] = GetDlgItem(hDlg, IDC_NICKNAME7);
	hNickName[7] = GetDlgItem(hDlg, IDC_NICKNAME8);
	hNickName[8] = GetDlgItem(hDlg, IDC_NICKNAME9);
	hNickName[9] = GetDlgItem(hDlg, IDC_NICKNAME10);
	hScore[0] = GetDlgItem(hDlg, IDC_SCORE1);
	hScore[1] = GetDlgItem(hDlg, IDC_SCORE2);
	hScore[2] = GetDlgItem(hDlg, IDC_SCORE3);
	hScore[3] = GetDlgItem(hDlg, IDC_SCORE4);
	hScore[4] = GetDlgItem(hDlg, IDC_SCORE5);
	hScore[5] = GetDlgItem(hDlg, IDC_SCORE6);
	hScore[6] = GetDlgItem(hDlg, IDC_SCORE7);
	hScore[7] = GetDlgItem(hDlg, IDC_SCORE8);
	hScore[8] = GetDlgItem(hDlg, IDC_SCORE9);
	hScore[9] = GetDlgItem(hDlg, IDC_SCORE10);
	switch (msg) {
	case WM_INITDIALOG: {
		LoadRecords();
		for (int i = 0; i < 10; i++) {
			if (records[i].active == true) {
				char buf[10];
				itoa(records[i].point, buf, 10);
				SetWindowText(hNickName[i], records[i].NickName);
				SetWindowText(hScore[i], buf);
			}
			else {
				SetWindowText(hNickName[i], "");
				SetWindowText(hScore[i], "");
			}
		}
		return TRUE;
	}
	case WM_CLOSE: {
		EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, 0);
		}
		return TRUE;
	}
	case WM_SIZE: {

		return TRUE;
	}
	}
	return FALSE;
}
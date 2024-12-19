#include<iostream>
#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<glut.h>
#include<GLU.h>
#include<GL.h>

#define LEFT 100 //좌로 이동    //키보드값들 
#define RIGHT 102 //우로 이동 
#define UP 101 //회전 
#define DOWN 103 //soft drop
#define SPACE 98

#define MAIN_X 11 //게임판 가로크기 
#define MAIN_Y 23 //게임판 세로크기 
#define MAIN_X_ADJ 3 //게임판 위치조정 
#define MAIN_Y_ADJ 1 //게임판 위치조정 
#define STATUS_X_ADJ MAIN_X_ADJ+MAIN_X+1

#define ACTIVE_BLOCK -2 // 게임판배열에 저장될 블록의 상태들 
#define CEILLING -1     // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현 
#define EMPTY 0         // 블록이 이동할 수 없는 공간은 양수로 표현 
#define WALL 1
#define INACTIVE_BLOCK 2 // 이동이 완료된 블록값 


int blocks[7][4][4][4] = {
{{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
{{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},
 {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
{{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},
 {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
{{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},
 {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
{{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},
 {0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
{{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},
 {0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
{{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},
 {0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
}; //블록모양 저장 4*4공간에 블록을 표현 blcoks[b_type][b_rotation][i][j]로 사용 

int b_type; //블록 종류를 저장 
int b_rotation;  //블록 회전값 저장 
int b_type_next; //다음 블록값 저장 

int main_org[MAIN_Y][MAIN_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 main_cpy로 복사됨 
int main_cpy[MAIN_Y][MAIN_X];  //즉  maincpy는 게임판이 모니터에 표시되기 전의 정보를 가지고 있음 

int bx, by; //이동중인 블록의 게임판상의 x,y좌표를 저장 
int mode = 0, speed = 800; // 게임 실행 종료 판단.
int new_block_on = 1;  //새로운 블럭이 필요함을 알리는 flag 
int crush_on = 0; //현재 이동중인 블록이 충돌상태인지 알려주는 flag 
int level_up_on = 0; //다음레벨로 진행(현재 레벨목표가 완료되었음을) 알리는 flag 
int space_key_on = 0;  //hard drop상태임을 알려주는 flag 

void reset(void); //게임판 초기화 


void reset_main(void); //메인 게임판(main_org[][]를 초기화)
void reset_main_cpy(void); //copy 게임판(main_cpy[][]를 초기화)

void draw_main(void);  //게임판을 그림 
void new_block(void);  //새로운 블록을 하나 만듦 
void drop_block(void); //블록을 아래로 떨어트림 

int check_crush(int bx, int by, int rotation);  //bx, by위치에 rotation회전값을 같는 경우 충돌 판단 
void move_block(int dir); //dir방향으로 블록을 움직임 
void check_line(void); //줄이 가득찼는지를 판단하고 지움 
void game_start(void);
void nomal_clear(void);
void boss_clear(void);
void check_game_over(void);

using namespace std;

int score = 0;
int tmp;

GLfloat alhp = 19.0;
GLfloat ifzero;
GLUquadric* pobj;

int ailen_dmg = 0;
int head_turn = 0;
int body_tumb = 0;
int dmg_cnt = 0;

GLfloat bcr = 0.6, bcg = 0.8, bcb = 0.1;
GLfloat bcrindex = 0.1;


GLfloat bodycolor[] = { bcr, bcg, bcb ,1.0 };
const GLfloat red[] = { 0.8f, 0.0, 0.0, 1.0 };
const GLfloat yellow[] = { 0.8f, 0.8, 0.0, 1.0 };
const GLfloat blue[] = { 0.0, 0.2f, 1.0, 1.0 };
const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
const GLfloat polished[] = { 100.0 };
const GLfloat dull[] = { 0.0 };
const GLfloat light_pos[] = { 0.0, 0.0, 0.0, 1.0 };

void skin_state() {
    if (bcr > 1.0) {
        bcrindex = -0.1;
    }
    else if (bcr <= 0.2) bcrindex = 0.1;
    if (alhp > 15) {
        bcr = 0.6;
        bcg = 0.8;
        bcb = 0.1;
    }
    else if (alhp <= 15) {
        bcr += bcrindex;
        bcg = 0.5;
    }
    bodycolor[0] = bcr;
    bodycolor[1] = bcg;
}

void alien_eye() {
    glPushMatrix();
    glRotatef(head_turn % 360, 0, 1, 0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glColor3f(0.0, 0.0, 0.0);
    glScalef(1.5, 2.0, 1.0);
    glTranslatef(0.3, 0.15, 0.8);
    glutSolidSphere(0.2, 50, 50);
    glTranslatef(-0.6, 0.0, 0.0);
    glutSolidSphere(0.2, 50, 50);
    glPopMatrix();
}

void alien_deadeye() {
    glPushMatrix();
    glRotatef(head_turn % 360, 0, 1, 0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glColor3f(0.0, 0.0, 0.0);
    glScalef(1.5, 2.0, 1.0);
    glTranslatef(0.3, 0.15, 0.8);
    glutSolidSphere(0.2, 50, 50);
    glTranslatef(-0.6, 0.0, 0.0);
    glutSolidSphere(0.2, 50, 50);
    glTranslatef(0, 0, 0.2);
    glColor3f(1, 0, 0);
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    glScalef(9, 1, 1);
    glutSolidCube(0.03);
    glPopMatrix();
    glPushMatrix();
    glRotatef(45, 0, 0, -1);
    glScalef(9, 1, 1);
    glutSolidCube(0.03);
    glPopMatrix();
    glTranslatef(0.6, 0.0, 0.0);
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    glScalef(9, 1, 1);
    glutSolidCube(0.03);
    glPopMatrix();
    glPushMatrix();
    glRotatef(45, 0, 0, -1);
    glScalef(9, 1, 1);
    glutSolidCube(0.03);
    glPopMatrix();

    glPopMatrix();
}

void ailen_arm() {
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bodycolor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glColor3f(0.6, 0.8, 0.1);
    glTranslatef(0.6, -1.5, 0);
    glutSolidSphere(0.3, 50, 50);
    glPushMatrix();
    glPopMatrix();
    glTranslatef(-1.2, 0, 0);
    glutSolidSphere(0.3, 50, 50);
    glPopMatrix();
}

void ailen_body() {
    glColor3f(0.6, 0.8, 0.1);
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bodycolor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glTranslatef(0.0, -3.0, 0.0);
    glRotatef(90, -1, 0, 0);
    glutSolidCone(1, 4, 50, 50);
    glTranslatef(0.0, 0.0, 0.0);
    glutSolidSphere(1, 50, 50);
    glPopMatrix();
}

void ailen_head() {
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bodycolor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glColor3f(0.6, 0.8, 0.1);
    glScalef(1.2, 1.0, 1.0);
    glutSolidSphere(1, 30, 30);
    if (mode == 2) alien_deadeye();
    else if (mode == 3) alien_deadeye();
    else alien_eye();
    glPopMatrix();
}

void ailen_leg() {
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bodycolor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glColor3f(0.6, 0.8, 0.1);
    glTranslatef(0.7, -4.0, 0.0);
    glScalef(1.0, 2.0, 1.0);
    glutSolidSphere(0.3, 50, 50);
    glTranslatef(-1.4, 0, 0);
    glutSolidSphere(0.3, 50, 50);
    glPopMatrix();
}

void crown() {

    glPushMatrix();
    glTranslatef(0, 1, 0);
    glRotated(90.0, -1, 0, 0);
    glColor3f(0.8f, 0.8, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glutSolidTorus(0.2, 0.6, 50, 50); //도넛

    glPushMatrix();
    glTranslated(-0.6, 0.0, 0.0);
    glutSolidCone(0.2, 0.5, 50, 50); //왕관 틀
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.0, -0.6, 0.15);
    glutSolidCone(0.2, 0.5, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.6, 0.0, 0.0);
    glutSolidCone(0.2, 0.5, 50, 50);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 0, 0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialfv(GL_FRONT, GL_SHININESS, polished);
    glTranslated(0.0, -0.8, 0.0);
    glutSolidSphere(0.1, 50, 50);  //빨간 보석
    glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
}

void draw_alien() {
    glPushMatrix();
    glTranslatef(15, 16, 0.0);
    glRotatef(body_tumb, -1, 0, 0);
    ailen_head();
    ailen_body();
    ailen_arm();
    ailen_leg();
    glPopMatrix();
}

void draw_bossali() {
    glPushMatrix();
    glTranslatef(15, 16, 0.0);
    glRotatef(body_tumb, -1, 0, 0);
    crown();
    ailen_head();
    ailen_body();
    ailen_arm();
    ailen_leg();
    glPopMatrix();
}

void ailen_hp() {

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);
    glVertex3f(11.0, 10.0, 0.0);
    glVertex3f(alhp, 10.0, 0.0);
    glVertex3f(alhp, 10.5, 0.0);
    glVertex3f(11.0, 10.5, 0.0);
    glEnd();

}

void MyDis() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (mode == 0)reset();
    else if (mode == 1) game_start();
    else if (mode == 2)boss_clear();
    else nomal_clear();
    glFlush();
}

void MyInit() {
    const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
    glEnable(GL_LIGHTING); // 태양
    glEnable(GL_LIGHT0); // 지역조명
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glEnable(GL_DEPTH_TEST);
}

void MyRes(GLint NewWidth, GLint NewHeight) {
    glViewport(0, 0, NewWidth, NewHeight);
    GLfloat WidthFactor = (GLfloat)NewWidth / (GLfloat)300;
    GLfloat HeightFactor = (GLfloat)NewHeight / (GLfloat)300;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 20.0, 0.0, 20.0, -10.0, 10.0);
}

void mykey(unsigned char key, int x, int y) {
    switch (key) {
    case ' ':
        move_block(SPACE);
        break;

    case 'r':
        reset_main();
        new_block();
        break;
    }
    glutPostRedisplay();
}

void MySpeciar(int key, int x, int y) {
    switch (key) {
    case LEFT: //왼쪽키 눌렀을때  
        if (check_crush(bx - 1, by, b_rotation) == true) move_block(LEFT);
        break;                            //왼쪽으로 갈 수 있는지 체크 후 가능하면 이동
    case RIGHT: //오른쪽 방향키 눌렀을때- 위와 동일하게 처리됨 
        if (check_crush(bx + 1, by, b_rotation) == true) move_block(RIGHT);
        break;
    case DOWN: //아래쪽 방향키 눌렀을때-위와 동일하게 처리됨 
        if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
        break;
    case UP: //위쪽 방향키 눌렀을때 
        if (check_crush(bx, by, (b_rotation + 1) % 4) == true) move_block(UP);
        //회전할 수 있는지 체크 후 가능하면 회전
        else if (crush_on == 1 && check_crush(bx, by - 1, (b_rotation + 1) % 4) == true) move_block(99);
    }
    glutPostRedisplay();
}

void MyTimer(int value) {
    drop_block(); // 블록을 한칸 내림 
    if (ifzero == 3) mode = 2;
    if (ifzero == 1) mode = 3;
    if (new_block_on == 1) new_block(); // 뉴 블럭 flag가 있는 경우 새로운 블럭 생성 
    check_game_over();
    glutPostRedisplay();
    glutTimerFunc(speed, MyTimer, 1);
}

void AilenT(int value) {
    if (ailen_dmg == 1 && dmg_cnt == 1) {

        head_turn += 10;
        if (head_turn > 360) {
            ailen_dmg = 0;
            head_turn = 0;
            dmg_cnt = 0;
            if (mode == 3) {
                mode = 1;
                ifzero++;
            }
        }
    }
    else if (ailen_dmg == 1 && dmg_cnt > 1) {
        body_tumb += 15;
        if (body_tumb > 360) {
            ailen_dmg = 0;
            body_tumb = 0;
            dmg_cnt = 0;
            if (mode == 3) {
                mode = 1;
                ifzero++;
            }
        }
    }
    skin_state();
    glutPostRedisplay();
    glutTimerFunc(40, AilenT, 2);
}

void MyMainMenu(int entryID) {   //게임 속도 설정
    switch (entryID)
    {
    case 1:
        speed = 800;
        break;
    case 2:
        speed = 600;
        break;
    case 3:
        speed = 400;
        break;
    case 4:
        speed = 260;
        break;
    }
    mode = 1;
    glutTimerFunc(speed, MyTimer, 1);
}

void MenuFunc() {   //게임 옵션 메뉴
    GLint MyMainMenuID = glutCreateMenu(MyMainMenu);
    glutAddMenuEntry("Lv 1", 1);
    glutAddMenuEntry("Lv 2", 2);
    glutAddMenuEntry("Lv 3", 3);
    glutAddMenuEntry("Lv 4", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void new_block(void) { //새로운 블록 생성  
    int i, j;

    bx = (MAIN_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데) 
    by = 0;  //블록 생성위치 y좌표(제일 위) 
    b_type = b_type_next; //다음블럭값을 가져옴 
    b_type_next = rand() % 7; //다음 블럭을 만듦 
    b_rotation = 0;  //회전은 0번으로 가져옴 

    new_block_on = 0; //new_block flag를 끔  

    for (i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성  
        for (j = 0; j < 4; j++) {
            if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
        }
    }

}

void drop_block(void) {
    int i, j;

    if (crush_on && check_crush(bx, by + 1, b_rotation) == true) crush_on = 0; //밑이 비어있으면 crush flag 끔 
    if (crush_on && check_crush(bx, by + 1, b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면 
        for (i = 0; i < MAIN_Y; i++) { //현재 조작중인 블럭을 굳힘 
            for (j = 0; j < MAIN_X; j++) {
                if (main_org[i][j] == ACTIVE_BLOCK) main_org[i][j] = INACTIVE_BLOCK;
            }
        }
        crush_on = 0; //flag를 끔 
        check_line(); //라인체크를 함 
        new_block_on = 1; //새로운 블럭생성 flag를 켬    
        return; //함수 종료 
    }
    if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN); //밑이 비어있으면 밑으로 한칸 이동 
    if (check_crush(bx, by + 1, b_rotation) == false) crush_on++; //밑으로 이동이 안되면  crush flag를 켬
}

int check_crush(int bx, int by, int b_rotation) { //지정된 좌표와 회전값으로 충돌이 있는지 검사 
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) { //지정된 위치의 게임판과 블럭모양을 비교해서 겹치면 false를 리턴 
            if (blocks[b_type][b_rotation][i][j] == 1 && main_org[by + i][bx + j] > 0) return false;
        }
    }
    return true; //하나도 안겹치면 true리턴 
};

void move_block(int dir) { //블록을 이동시킴 
    int i, j;

    switch (dir) {
    case LEFT: //왼쪽방향 
        for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움 
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) { //왼쪽으로 한칸가서 active block을 찍음 
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j - 1] = ACTIVE_BLOCK;
            }
        }
        bx--; //좌표값 이동 
        break;

    case RIGHT:    //오른쪽 방향. 왼쪽방향이랑 같은 원리로 동작 
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j + 1] = ACTIVE_BLOCK;
            }
        }
        bx++;
        break;

    case DOWN:    //아래쪽 방향. 왼쪽방향이랑 같은 원리로 동작
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i + 1][bx + j] = ACTIVE_BLOCK;
            }
        }
        by++;
        break;

    case UP: //키보드 위쪽 눌렀을때 회전시킴. 
        for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움  
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림) 
        for (i = 0; i < 4; i++) { //회전된 블록을 찍음 
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
            }
        }
        break;

    case 99: //블록이 바닥, 혹은 다른 블록과 닿은 상태에서 한칸위로 올려 회전이 가능한 경우 
              //이를 동작시키는 특수동작 
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
            }
        }
        b_rotation = (b_rotation + 1) % 4;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i - 1][bx + j] = ACTIVE_BLOCK;
            }
        }
        by--;
        break;
    case SPACE: //스페이스키 눌렀을때 
        space_key_on = 1; //스페이스키 flag를 띄움 
        while (crush_on == 0) { //바닥에 닿을때까지 이동시킴 
            drop_block();
        }
    }
}

void check_line(void) {
    int i, j, k, l;

    int    block_amount; //한줄의 블록갯수를 저장하는 변수 
    int combo = 0; //콤보갯수 저장하는 변수 지정및 초기화 

    for (i = MAIN_Y - 2; i > 3;) { //i=MAIN_Y-2 : 밑쪽벽의 윗칸부터,  i>3 : 천장(3)아래까지 검사 
        block_amount = 0; //블록갯수 저장 변수 초기화 
        for (j = 1; j < MAIN_X - 1; j++) { //벽과 벽사이의 블록갯루를 셈 
            if (main_org[i][j] > 0) block_amount++;
        }
        if (block_amount == MAIN_X - 2) { //블록이 가득 찬 경우 
            score++;
            tmp = score - 1;
            alhp -= 1;
            dmg_cnt += 1;
            ailen_dmg = 1;
            if (alhp <= 11) {
                alhp = 19.0;
                ifzero++;
            }
            for (k = i; k > 1; k--) { //윗줄을 한칸씩 모두 내림(윗줄이 천장이 아닌 경우에만) 
                for (l = 1; l < MAIN_X - 1; l++) {
                    if (main_org[k - 1][l] != CEILLING) main_org[k][l] = main_org[k - 1][l];
                    if (main_org[k - 1][l] == CEILLING) main_org[k][l] = EMPTY;
                    //윗줄이 천장인 경우에는 천장을 한칸 내리면 안되니까 빈칸을 넣음 
                }
            }
        }
        else i--;
    }
}

void draw_start(void) {
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 0.4, 0.4);
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 20.0, 0.0);
    glVertex3f(20.0, 20.0, 0.0);
    glVertex3f(20.0, 0.0, 0.0);
    glEnd();
    glPushMatrix();
    glTranslatef(-20, -19, 0);
    glScalef(2, 2, 2);
    draw_bossali();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void draw_end(void) {
    glDisable(GL_LIGHTING);
    glColor3f(0.3, 0.6, 0.6);
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 20.0, 0.0);
    glVertex3f(20.0, 20.0, 0.0);
    glVertex3f(20.0, 0.0, 0.0);
    glEnd();
    glPushMatrix();
    glTranslatef(-20, -19, 0);
    glScalef(2, 2, 2);
    draw_bossali();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void draw_end_nomal(void) {
    glDisable(GL_LIGHTING);
    glColor3f(0.6, 0.3, 0.3);
    glBegin(GL_POLYGON);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 20.0, 0.0);
    glVertex3f(20.0, 20.0, 0.0);
    glVertex3f(20.0, 0.0, 0.0);
    glEnd();
    glPushMatrix();
    glTranslatef(-20, -19, 0);
    glScalef(2, 2, 2);
    draw_alien();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void reset(void) {
    draw_start();
    crush_on = 0;
    alhp = 19;
    ifzero = 0;
    reset_main();
    b_type_next = rand() % 7;
    new_block();
}


void reset_main(void) { //게임판을 초기화  
    int i, j;
    alhp = 19;
    for (i = 0; i < MAIN_Y; i++) { // 게임판을 0으로 초기화  
        for (j = 0; j < MAIN_X; j++) {
            main_org[i][j] = 0;
            main_cpy[i][j] = 100;
        }
    }
    for (j = 1; j < MAIN_X; j++) { //y값이 3인 위치에 천장을 만듦 
        main_org[3][j] = CEILLING;
    }
    for (i = 1; i < MAIN_Y - 1; i++) { //좌우 벽을 만듦  
        main_org[i][0] = WALL;
        main_org[i][MAIN_X - 1] = WALL;
    }
    for (j = 0; j < MAIN_X; j++) { //바닥벽을 만듦 
        main_org[MAIN_Y - 1][j] = WALL;
    }
}

void reset_main_cpy(void) { //main_cpy를 초기화 
    int i, j;
    for (i = 0; i < MAIN_Y; i++) {         //게임판에 게임에 사용되지 않는 숫자를 넣음 
        for (j = 0; j < MAIN_X; j++) {  //이는 main_org와 같은 숫자가 없게 하기 위함 
            main_cpy[i][j] = 100;
        }
    }
}

void draw_main(void) {
    for (int j = 1; j < MAIN_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌 
        if (main_org[3][j] == EMPTY) main_org[3][j] = CEILLING;
    }

    for (int j = 0; j < MAIN_Y; j++) {
        for (int i = 0; i < MAIN_X; i++) {
            switch (main_org[j][i]) {
            case EMPTY: //빈칸모양 
                break;
            case CEILLING: //천장모양 
                glColor3f(0.7, 0.7, 0.7);
                glBegin(GL_LINES);
                glVertex3f(1.3 + i * 0.8, 20 - (0.4 + j * 0.8), 0.0);
                glVertex3f(0.5 + i * 0.8, 20 - (0.4 + j * 0.8), 0.0);
                glEnd();
                break;
            case WALL: //벽모양 
                glColor3f(0.5, 0.5, 0.5);
                glBegin(GL_POLYGON);
                glVertex3f(0.51 + i * 0.8, 20 - (0.22 + j * 0.8), 0.0);
                glVertex3f(1.29 + i * 0.8, 20 - (0.22 + j * 0.8), 0.0);
                glVertex3f(1.29 + i * 0.8, 20 - (0.98 + j * 0.8), 0.0);
                glVertex3f(0.51 + i * 0.8, 20 - (0.98 + j * 0.8), 0.0);
                glEnd();
                break;
            case INACTIVE_BLOCK: //굳은 블럭 모양 
                glColor3f(0.3, 0.3, 0.8);
                glBegin(GL_LINE_LOOP);
                glVertex3f(0.55 + i * 0.8, 20 - (0.25 + j * 0.8), 0.0);
                glVertex3f(1.25 + i * 0.8, 20 - (0.25 + j * 0.8), 0.0);
                glVertex3f(1.25 + i * 0.8, 20 - (0.95 + j * 0.8), 0.0);
                glVertex3f(0.55 + i * 0.8, 20 - (0.95 + j * 0.8), 0.0);
                glEnd();
                break;
            case ACTIVE_BLOCK: //움직이고있는 블럭 모양 
                glColor3f(0.3, 0.3, 0.8);
                glBegin(GL_POLYGON);
                glVertex3f(0.55 + i * 0.8, 20 - (0.25 + j * 0.8), 0.0);
                glVertex3f(1.25 + i * 0.8, 20 - (0.25 + j * 0.8), 0.0);
                glVertex3f(1.25 + i * 0.8, 20 - (0.95 + j * 0.8), 0.0);
                glVertex3f(0.55 + i * 0.8, 20 - (0.95 + j * 0.8), 0.0);
                glEnd();
                break;
            }
        }
    }
}

void game_start() {
    if (score > tmp) {
        cout << "현재 점수는 :" << score * 1000 << endl;
        tmp += 1;
    }
    if (new_block_on == 1) new_block();
    if (ifzero == 0)    draw_alien();
    else if (ifzero == 2) {
        draw_bossali();
        speed = 250;
    }
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    if (ifzero < 3)ailen_hp();
    draw_main();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

}

void nomal_clear() {
    draw_end_nomal();
    crush_on = 0;
    alhp = 19;
    reset_main();
    b_type_next = rand() % 7;
    new_block();
}

void boss_clear() {
    draw_end();
    crush_on = 0;
    alhp = 19;
    ifzero = 0;
    reset_main();
    b_type_next = rand() % 7;
    new_block();
}

void check_game_over() {
    for (int i = 1; i < MAIN_X - 2; i++) {
        if (main_org[3][i] > 0)mode = 0;
    }
}

void GameRule() {
    cout << " ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << endl;
    for (int i = 0; i < 4; i++) cout << "l                                                                             l" << endl;
    cout << "l   ←왼쪽이동   ↑모양변경   오른쪽이동→   spacebar 즉시하강   r다시 시작   l" << endl;
    for (int i = 0; i < 4; i++) cout << "l                                                                             l" << endl;
    cout << " ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(1000, 100);
    reset_main();
    glutCreateWindow("외계인 테트리스");
    GameRule();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    MyInit();
    glutDisplayFunc(MyDis);
    glutReshapeFunc(MyRes);
    glutKeyboardFunc(mykey);
    glutSpecialFunc(MySpeciar);
    glutTimerFunc(speed, MyTimer, 1);
    glutTimerFunc(40, AilenT, 2);
    MenuFunc();
    glutMainLoop();
    return 0;
}
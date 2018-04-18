#include <windows.h>
#include<stdio.h>//FILE,fopen,...;
#include "resource.h"
#include "class.h"

#include <fmod.hpp>			//FMOD관련
#pragma comment (lib, "fmodex_vc.lib")
using namespace FMOD;


/////////////////////////////
///////////////////////////전역변수

#define B_MAX 11 //BULLET MAX
#define E_MAX 10 //ENEMY MAX //  


LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LPCWSTR lpszClass=TEXT("Shooting Game : : Chanwoo Kim");
HWND hWnd;
HDC hdc;
HINSTANCE g_hInst;
HBITMAP bPlayer, bEnemy, bBullet;

FILE* fp;
int bj=0;//bullet j
int start=0;
double speed=2;
		
Player *p;	
Enemy *e[E_MAX];
Bullet *b[B_MAX];

Score s;


System* g_pSystem;//FMOD관련
Sound* g_pSound[5];
Channel* g_pChannel;


void init();
void inputmove();
void inputshot();
void draw();
void logicCrash();//총알,적충돌처리
void logicEnemy();//enemy생성
void DrawBitmap(HDC hdc,int x,int y, HBITMAP hBit);	
void DrawScore(HDC hdc);//점수판
void DrawStart(HDC hdc);


////////////////////////////////////////////////////////////////////////<<< WinMain >>>////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPED |WS_CAPTION |WS_SYSMENU  |WS_MINIMIZEBOX, //|WS_MAXIMIZEBOX ,
		CW_USEDEFAULT,CW_USEDEFAULT,650,738,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
		
	while(GetMessage(&Message,0,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

//////////////////////////////////////////////////////////////////////////////////////////////WND PROC////////
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{

	PAINTSTRUCT ps;
	srand(GetTickCount());
	switch(iMessage) {
		case WM_CREATE:
			bBullet=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP1));//이미지 로드
			bEnemy =LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP2));
			bPlayer=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BITMAP3));
			

			System_Create(&g_pSystem);										//FMOD,, 사운드로드
			g_pSystem->init(100, FMOD_INIT_NORMAL, 0);
			g_pSystem->createSound("resource\\Start.wav", FMOD_HARDWARE, 0, &g_pSound[0]);
			g_pSystem->createSound("resource\\Bullet.wav", FMOD_HARDWARE, 0, &g_pSound[1]);
			g_pSystem->createSound("resource\\Enemyfly.wav", FMOD_HARDWARE, 0, &g_pSound[2]);
			g_pSystem->createSound("resource\\EnemyDie.wav", FMOD_HARDWARE, 0, &g_pSound[3]);
			g_pSystem->createSound("resource\\PlayerDie.wav", FMOD_HARDWARE, 0, &g_pSound[4]);

			


			g_pSystem->playSound(FMOD_CHANNEL_FREE , g_pSound[0] , 0 , &g_pChannel );

			init();//// class,전역변수 init ////

			SetTimer(hWnd,1,15,NULL);
			SetTimer(hWnd,2,7000,NULL);
			SetTimer(hWnd,3,120,NULL);

			return 0;
			

		case WM_PAINT:								////   WM_PAINT init,draw,
			hdc=BeginPaint(hWnd,&ps);
			draw();
			EndPaint(hWnd,&ps);
			return 0;
		
		case WM_TIMER:								//// WM_TIMER
			switch(wParam){
				case 1:	//Player조작 0.015초
					
					inputmove();
					logicCrash();
					InvalidateRect(hWnd,NULL,FALSE);
					return 0;

				case 2:		//5초
					start=1;
					logicEnemy();

					return 0;

				case 3:		//0.12초
					inputshot();
					 g_pSystem->update(); 
					return 0;

			}

			return 0;

		case WM_DESTROY:
			g_pSystem->release();
			g_pSystem->close();
			KillTimer(hWnd,1);
			KillTimer(hWnd,2);
			KillTimer(hWnd,3);
			PostQuitMessage(0);
			return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//////////////////////////////////////////////////////////////////////functions///////////////

void init(){
				
			speed=2;
			bj=0;
			s.setZero();
			start=0;
			
			p=new Player;
			p->setXY(220,620);
			p->setHp(1);
			
			for(int i=0;i<B_MAX;i++){
				b[i]=new Bullet;
				b[i]->setXY(1000,100);		
			}

			for(int i=0;i<E_MAX;i++){
				e[i]=new Enemy;	
				e[i]->setXY(rand()%8*60+10,800);//적 위치생성 Y -65*i-65
			}
			
			
}

void inputmove(){
	if ( GetKeyState(VK_LEFT) & 0x8000) p->moveX(-5);
	if ( GetKeyState(VK_RIGHT)& 0x8000) p->moveX(5);
	if ( GetKeyState(VK_UP)   & 0x8000) p->moveY(-5);
	if ( GetKeyState(VK_DOWN) & 0x8000) p->moveY(5);
}
void inputshot(){

	if( GetKeyState(VK_SPACE) & 0x8000 ){
		if(start==1){
			g_pSystem->playSound(FMOD_CHANNEL_FREE , g_pSound[1] , 0 , &g_pChannel );
			if(bj<B_MAX)
			{
				b[bj]->shoot(p->getX()+27,p->getY()-5); 
				bj++;

			
			}
			if(bj>=B_MAX)
				bj=0;
			

		}
	}

}

void draw(){

	HDC hdcMem;
	RECT rect;
	HBITMAP hbmMem, hbmMemOld;

	GetClientRect(hWnd, &rect);
	hdcMem = CreateCompatibleDC(hdc);
	hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	hbmMemOld = (HBITMAP)SelectObject(hdcMem, hbmMem);


	DrawBitmap(hdcMem,p->getX(),p->getY(),bPlayer); //Player					
	for(int i=0;i<B_MAX;i++){
		DrawBitmap(hdcMem,b[i]->getX(),b[i]->getY(),bBullet);//Bullet
	}
	for(int i=0;i<E_MAX;i++){
		DrawBitmap(hdcMem,e[i]->getX(),e[i]->getY(),bEnemy);//Enemy
	}	
	DrawScore(hdcMem);	
	if(start==0)		
		DrawStart(hdcMem);

	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);//double buffering
	
	SelectObject(hdcMem, hbmMemOld); 
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);

}

void logicCrash(){
	for(int i=0;i<B_MAX;i++){//Bullet<->Enemy 충돌체크, 총알이동
		for(int j=0;j<E_MAX;j++){
			if(b[i]->Check(e[j])){
				if(p->getHp()!=0){
					s.scorePlus(100);
					//	PlaySound(MAKEINTRESOURCE(IDR_WAVE4),g_hInst,SND_RESOURCE|SND_ASYNC);
					g_pSystem->playSound(FMOD_CHANNEL_FREE , g_pSound[3] , 0 , &g_pChannel );

				}
				b[i]->setXY(1000,800);
				e[j]->setXY(700, 800);
			}
		}
		b[i]->moveb(-12);
	}
	for(int i=0;i<E_MAX;i++){	//Enemy<->Player 충돌체크, 이동			
		
		if(e[i]->Check(p))	
			{
			//	PlaySound(MAKEINTRESOURCE(IDR_WAVE5),g_hInst,SND_RESOURCE|SND_ASYNC);
				
				g_pSystem->playSound(FMOD_CHANNEL_FREE , g_pSound[4] , 0 , &g_pChannel );

				e[i]->setXY(700 ,800);
				p->setXY(1000,800);
				p->setHp(0);
				
				if(p->getHp()==0){
					start=0;
					fopen_s(&fp,"highscore","wt");	//save high score 
					fprintf(fp,"%d",s.getHigh());
					if(fp==NULL)MessageBox(hWnd,L"file error",L"file error",MB_OK);
					fclose(fp);
					
					
					if(MessageBox(hWnd,L"\tGAME OVER\n\tAgain?",L"GAME OVER",MB_YESNO)==IDYES){
						init();
					}
					else
						PostMessage(hWnd,WM_DESTROY,0,0);
				}
		}
		e[i]->movee(2+(int)speed);
	}					
}

void logicEnemy(){

	for(int i=0;i<E_MAX;i++){
		e[i]=new Enemy;
		e[i]->setXY(rand()%8*60+10,-65*i-65);	//적 위치생성
	}
	speed+=0.5;
	
}

void DrawBitmap(HDC hdc,int x,int y, HBITMAP hBit){
	HDC MemDC;
	HBITMAP OldBitmap;
	int bx,by;
	BITMAP bit;

	MemDC=CreateCompatibleDC(hdc);

	OldBitmap=(HBITMAP)SelectObject(MemDC,hBit);
	
	GetObject(hBit,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	BitBlt(hdc,x,y,bx,by,MemDC,0,0,SRCCOPY);

	SelectObject(MemDC,OldBitmap);
	DeleteDC(MemDC);
}


void DrawScore(HDC hdc){
	SelectObject(hdc,(HBRUSH)GetStockObject(DKGRAY_BRUSH));	
	Rectangle(hdc,500,0,700,900);//회색 점수판영역
	SetBkColor(hdc,RGB(64,64,64));
	
	TextOut(hdc,515,110,TEXT("Space Penguin"),lstrlen(TEXT("Space Penguin")));		
	
	TextOut(hdc,510,140,L"HighScore :",11);
	TextOut(hdc,590,140,s.highscore(),lstrlen(s.highscore()));
	
	TextOut(hdc,540,160,L"Score :",7);
	TextOut(hdc,590,160,s.tscore(),lstrlen(s.tscore()));
	
	TextOut(hdc,520,600,L"Move : ←→↑↓",lstrlen(L"Move : ←→↑↓"));
	TextOut(hdc,520,620,L"Attack : Space ",lstrlen(L"Attack : Space "));

}

void DrawStart(HDC hdc)
{
	SetBkColor(hdc,RGB(64,64,64));
	TextOut(hdc,185,300,L"----------READY----------",25);

			
}
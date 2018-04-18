class Object{//Player, Enemy, Bullet 부모클래스
protected:
	int x,y;//location
	int width,length;
	int hp;

public:
	int getX(){return x;}
	int getY(){return y;}
	int getW(){return width;}
	int getL(){return length;}
	int getHp(){return hp;}

	void setX(int _x){x=_x;}
	void setY(int _y){y=_y;}
	void setXY(int _x,int _y){x=_x;y=_y;}
	void setHp(int _hp){hp=_hp;}

};


class Player  : public Object{
public:
	Player(){//60*65
		x=220;
		y=620;
		width=60;
		length=65;
		hp=1;
	}
	
	void moveX(int i){
		
		if(x<950)
		if(x>440)
			x=440;
		else if(x<0)
			x=0;
		else
			x+=i;
	}
	
	void moveY(int i){
		if(y<750)
		if(y>635)
			y=635;
		else if(y<0)
			y=0;
		else
			y+=i;
	}

};

class Enemy : public Object{//60*64
public:
	Enemy(){
		x=rand()%7*70+10;
		y=-100;
		width=60;
		length=65;
		hp=1;
	}

	void movee(int i){
		if(y<800)
			y+=i;
	}

//총알이 적 범위안에 들어가면 충돌,
//플레이어가 적범위안에 들어가면 충돌
// !(범위가 안겹치면)==충돌
	int Check(Object* Player)
	{
		int ax, ay, aw, al; // a(x,y,width,length)
		int	bx, by, bw, bl;	

		ax=this->getX();
		ay=this->getY();
		aw=this->getX()+this->getW();
		al=this->getY()+this->getL();

		bx=Player->getX()+10;
		by=Player->getY()+10;
		bw=Player->getX()+Player->getW()-10;
		bl=Player->getY()+Player->getL()-10;

		if (al < by) return 0;    /* 사각형 A가 사각형 B의 위에 있는 경우 */
		if (ay > bl) return 0;    /* 사각형 A가 사각형 B의 아래에 있는 경우 */
		if (aw < bx) return 0;    /* 사각형 A가 사각형 B의 왼쪽에 있는 경우 */
		if (ax > bw) return 0;    /* 사각형 A가 사각형 B의 오른쪽에 있는 경우 */
	 
		return 1;
	}

};

class Bullet : public Object{
public:
	Bullet(){
		x=1000;
		y=650;
		width=6;
		length=14;
	}

	void shoot(int _x,int _y){
		x=_x;
		y=_y;
	}

	void moveb(int i){
		if(this->getY()>0)
			this->setY(this->getY()+i);
		else
			x=1000;	
	}

	int Check(Object* Enemy)
	{
		int ax, ay, aw, al; // a(x,y,width,length)
		int	bx, by, bw, bl;	

		ax=this->getX();
		ay=this->getY();
		aw=this->getX()+this->getW();
		al=this->getY()+this->getL();

		bx=Enemy->getX();
		by=Enemy->getY();
		bw=Enemy->getX()+Enemy->getW();
		bl=Enemy->getY()+Enemy->getW();


		if (al < by) return 0;    /* 사각형 A가 사각형 B의 위에 있는 경우 */
		if (ay > bl) return 0;    /* 사각형 A가 사각형 B의 아래에 있는 경우 */
		if (aw < bx) return 0;    /* 사각형 A가 사각형 B의 왼쪽에 있는 경우 */
		if (ax > bw) return 0;    /* 사각형 A가 사각형 B의 오른쪽에 있는 경우 */
	 
		return 1;
	}

		
	
};


class Score{
	int score;
	int high;
	TCHAR cScore[10],cHigh[10];

public:
	Score()
	{
		score=0;		
	}

	void scorePlus(int _score)
	{
		score+=_score;
	}

	void setZero()
	{
		score=0;
	}

	TCHAR* tscore() 
	{	
		wsprintf(cScore,L"%d",score);
		return cScore; 
	}	
	
	TCHAR* highscore()
	{
		FILE *fp;
		
		fopen_s(&fp,"highscore","rt");
		if(fp==NULL){
			high=score;
		}
		else{
			fscanf_s(fp,"%d",&high);
			fclose(fp);
		}

		if(high<score)
			high=score;
		wsprintf(cHigh,L"%d",high);		
		return cHigh; 
	}

	int getScore()
	{
		return score;
	}
	
	int getHigh()
	{
		return high;
	}

};


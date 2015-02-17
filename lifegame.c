/*
 * Version: 1.0.1
 * shenwei  沈伟
 * 
 *    这是一个能产生动态效果的程序，用来仿真“生命”的繁衍过程。
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <curses.h>

//#define _DEBUG
#define _USE_COLOR

#define M	40
#define N	92

char area[M][N],n_area[M][N],o_area[M][N];
int age[M][N];

int life;
int tot_cnt;

char s[80];

void pr_area()
{
    int i,j;
    char c;
    int color_id;

    //	printf("Life: %04d [%5d]\n\n",life,tot_cnt);

    clear();

    for(i=0;i<M;i++){
	for(j=0;j<N;j++){
	    if(area[i][j]!=' '){ 
		/*
		if(age[i][j]>=21) c='*';
		else if(age[i][j]>=14) c='*';
		else if(age[i][j]>=7) c='+';
		else c='+';
		*/
		c = ' ';
#ifdef _USE_COLOR
		if(age[i][j]>=21) color_id = 1;
		else if(age[i][j]>=14) color_id = 2;
		else if(age[i][j]>=7) color_id = 3;
		else color_id = 4;
#else
		color_id = 1;
#endif
		move(i,j);
    		attron(COLOR_PAIR(color_id));
		waddnstr(stdscr,&c,1);
    		attroff(COLOR_PAIR(color_id));
	    }
	}
	refresh();
    }
}

void init_area(int delt_a)
{
    int c,cnt,i,j;

    c = 0;
    for(cnt=0;cnt<(M*N);cnt++){
	i = rand() % M;
	j = rand() % N;

	if((rand()%3)==1){
	    area[i][j] = '*';
	    age[i][j] = (rand()%26);
	    c++;
	}
	if(c>delt_a) break;
    }
#if 1
    for(i=0;i<M;i++){
	for(j=0;j<N;j++){
	    if((rand()%10)==1){
		area[i][j] = '*';
		cnt++;
		if(cnt>10) break;
	    }
	}
    }
#endif
}

/*
 *		[n-] [n] [n+1]
 *      [m-1]     *   *    *
 *	[m]       *   *    *
 *	[m+1]     *   *    *
 *
 */

void GetEnv(char *pa,int m,int n)
{
    int i,j,x,y;

    for(i=0;i<3;i++){
	x = m-1+i;
	if(x>=M) continue;
	if(x<0) continue;
	for(j=0;j<3;j++){
	    y = n-1+j;
	    if(y>=N) continue;
	    if(y<0) continue;
	    *(pa+3*i+j) = area[x][y];
	}
    }
}

/*
 * return:
 *
 * 	== 1: New
 *	== 0: Life (or No change!)
 *	== 2: dead
 */
int How(int m,int n)
{
    int i,j;
    char A[3][3];
    int val=0;

    memset(A,' ',9);

    GetEnv(A,m,n);

#ifdef _DEBUG
    printf("---[%d][%d]\n",m,n);
    for(i=0;i<3;i++){
	for(j=0;j<3;j++){
	    printf("%c",A[i][j]);
	}
	printf("\n");
    }
    printf("---\n");
#endif

    for(i=0;i<3;i++){
	for(j=0;j<3;j++){
	    if((i==1) && (j==1)) continue;
	    if(A[i][j]!=' ') val++;
	}
    }

#ifdef _DEBUG
    printf("> [%d]\n",val);
#endif

    if(area[m][n]==' '){
	if(val==3) return 1;
	return 0;
    }else{
	if((val==2) || (val==3)) return 0;
	return 2;
    }
}

void do_game()
{
    int i,j;

    for(i=0;i<M;i++){
	for(j=0;j<N;j++){
	    switch(How(i,j)){
		case 0:
		    age[i][j]--;
		    if(age[i][j]==0){
			n_area[i][j] = ' ';
			break;
		    }
		    n_area[i][j] = area[i][j];
#ifdef _DEBUG
		    printf("old!\n");
#endif
		    break;
		case 1:
		    n_area[i][j] = '*';
		    age[i][j] = 10+(rand()%16);
#ifdef _DEBUG
		    printf("New!\n");
#endif
		    break;
		case 2:
		default:
		    n_area[i][j] = ' ';
#ifdef _DEBUG
		    printf("ded!\n");
#endif
		    break;
	    }
#ifdef _DEBUG
	    //gets(s);
#endif
	}
    }
}

int same_area()
{
    int i,j;
    int ret=0;

    for(i=0;i<M;i++){
	for(j=0;j<N;j++){
	    if(o_area[i][j]!=' '){
		if (n_area[i][j]==' ') ret = 1;
	    }else{
		if (n_area[i][j]!=' ') ret = 1;
	    }
	    area[i][j] = n_area[i][j];
	}
    }
    if(ret==0){
	//		printf(" ! SAME !\n");
	return 1;
    }
    return 0;
}

int GetTotCnt()
{
    int i,j,cnt;
    cnt = 0;
    for(i=0;i<M;i++){
	for(j=0;j<N;j++){
	    if(area[i][j]!=' ') cnt++;
	}
    }
    return cnt;
}

void save_area( char *ps, char *pd )
{
    int i,j,x;

    for(i=0;i<M;i++){
	x = i*N;
	for(j=0;j<N;j++){
	    *(pd+x+j) = *(ps+x+j);
	}
    }
}

void main(void)
{
    //time_t ti,cnt;
    int ti,cnt;

    initscr();

    if(start_color()!=OK) return;

    init_pair(1,COLOR_RED, COLOR_GREEN);
    init_pair(2,COLOR_GREEN, COLOR_RED);
    init_pair(3,COLOR_RED, COLOR_BLUE);
    init_pair(4,COLOR_RED, COLOR_WHITE);


    box(stdscr,'+','+');
    refresh();

#if 1
    ti = time(NULL);
    srand(ti);
    memset(area,' ',sizeof(area));
    init_area((M-1)*(N-1));

    while(1){

	life = 1;

	cnt = 0;
	while(1){
	    //system("clear");
	    memset(n_area,' ',sizeof(n_area));
	    do_game();
	    tot_cnt = GetTotCnt();
	    pr_area();
	    if(same_area()){
		init_area(28);	
	    }
	    if(tot_cnt==0) break;
	    life++;
	    if((life%2)==1) save_area(area,o_area);

	    /*
	       cnt++;
	       if(cnt<2) usleep(50000);
	       else{
	       cnt = 0;
	       usleep(100000);
	       }
	       */
	    usleep(20000);
	}
	init_area(200-tot_cnt);
    }
#endif

    endwin();
}

/*
 * Eof
 */

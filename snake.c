#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define N 4

typedef struct point {
	int x;
	int y;
}point;

pthread_mutex_t locker;
point *snake;
int n; //number for snake
int m = 0; //number for gems
int exit_flag = 1;
point *gems;
int key;

char sost_up, sost_down, sost_left, sost_right;
char sost_up_prev, sost_down_prev, sost_left_prev, sost_right_prev;

void* change_sost(void *p){
	while (exit_flag){
		
	}
}


void* generator(void *p){
	int x, y, i, flag;
	
	while (exit_flag){
		pthread_mutex_lock(&locker);
		flag = 1;
		m++;
		gems = (point *)realloc(gems, m*sizeof(point));
		do{
		x = rand() % COLS; 
		y = rand() % LINES;
		for (i = 0; i < n; i++){
			if ((x == snake[i].x) && (y == snake[i].y)) flag = 0;
		}
		}while (flag == 0);
		gems[m-1].x = x;
		gems[m-1].y = y;
		mvaddstr(gems[m-1].y, gems[m-1].x, "x");
		refresh();
		pthread_mutex_unlock(&locker);
		sleep(1);
	}
}


int main(int argc, char *argv[])
{	
	int i, j, off, noff, tmp;
	char sost;
	int direction;
	point buf;
	point *gem_buf;
	int flag_buf;
	int flag_gem;
	int loose_flag = 0;

	pthread_t pid_gen, pid_sost;
	int status_gen, status_sost;
	void *res_gen, *res_sost;

	pthread_mutex_init(&locker, NULL);
	
	
	srand(time(NULL));
	
	snake = (point *)malloc(N*sizeof(point));
	n = N;
	direction = 1;

	if ( NULL == initscr() ) {
		fprintf(stderr,"Could not init curses\n");
		return __LINE__;
	}
	
	tmp = LINES / 2;	
	for ( i = 1 ; i < n ; i++) {
		snake[i].x = COLS / 2;
		snake[i].y = tmp;
		tmp = (tmp + LINES - 1) % LINES;
	}
	snake[0].x = COLS / 2;
	snake[0].y = tmp;


	status_gen = pthread_create(&pid_gen, NULL, generator, (void*) 1);
	if (status_gen != 0){
			fprintf(stderr, "main error: can't create thread_gen, status = %d\n", status_gen);
			exit(-11);
	}

	off = 0;
	noecho();
	keypad(stdscr,TRUE);
	refresh();
	sost = 'w';

	for ( i = 0 ; i < n ; i++ ) {
		mvaddstr(snake[i].y,snake[i].x,"*");
	}
	refresh();

	status_sost = pthread_create(&pid_sost, NULL, change_sost, (void*) 1);
	if (status_sost != 0){
			fprintf(stderr, "main error: can't create thread_gen, status = %d\n", status_sost);
			exit(-11);
	}

	while (('q' != (key = getch())) && (loose_flag == 0)){
		pthread_mutex_lock(&locker);
		for ( i = 0 ; i < n ; i++ ) {
			mvaddstr(snake[i].y,snake[i].x," ");
		}
		switch(key){
		case KEY_UP:
			if (sost == 'd'){
				direction = !(direction);
				if (direction){
					noff = (noff - 2) % n;
					off = (off - 1) % n;
				}
				else{
					noff = (noff + 2) % n;
					off = (off + 1) % n;
				}
			}
			if (direction) noff = ( off + 1 ) % n;
			else noff = (off+ n - 1) % n;
			
			buf.y = (snake[off].y + LINES - 1)% LINES;
			buf.x = snake[off].x;
			flag_gem = -1;
			for (i = 0; i < m; i++){
				if ((buf.x == gems[i].x) && (buf.y == gems[i].y)) flag_gem = i;
			}
			if (flag_gem != -1){
				gem_buf = (point *)malloc((m-1)*sizeof(point));
				j = 0;
				for (i = 0; i < m; i++){
					if (i != flag_gem){
						gem_buf[j] = gems[i];
						j++;
					}
				}
				free(gems);
				gems = gem_buf;
				n++;
				snake = (point *)realloc(snake, n*sizeof(point));
				for ( j = (n-2) ; j >= noff; j--){
					snake[j+1] = snake[j];
				}
				if (!direction){ 
					noff = (noff + 1) % n;
					off = (off + 1) % n;
				}
			}
			snake[noff].x = buf.x;
			snake[noff].y = buf.y;
			

			
			if (direction) off = ( off + 1 ) % n;
			else off = (off + n - 1) % n;
			sost = 'w';
			break;
		case KEY_DOWN:
			if (sost == 'w'){
				direction = !(direction);
				if (direction){
					noff = (noff - 2) % n;
					off = (off - 1) % n;
				}
				else{
					noff = (noff + 2) % n;
					off = (off +1) % n;
				}
			}
			if (direction) noff = ( off + 1 ) % n;
			else noff = (off + n - 1) % n;
			buf.y = (snake[off].y + 1)% LINES;
			buf.x = snake[off].x;
			flag_gem = -1;
			for (i = 0; i < m; i++){
				if ((buf.x == gems[i].x) && (buf.y == gems[i].y)) flag_gem = i;
			}
			if (flag_gem != -1){
				gem_buf = (point *)malloc((m-1)*sizeof(point));
				j = 0;
				for (i = 0; i < m; i++){
					if (i != flag_gem){
						gem_buf[j] = gems[i];
						j++;
					}
				}
				free(gems);
				gems = gem_buf;
				n++;
				snake = (point *)realloc(snake, n*sizeof(point));
				for ( j = (n-2) ; j >= noff; j--){
					snake[j+1] = snake[j];
				}
				if (!direction){
					off = (off + 1) % n;
					noff = (noff + 1) % n;
				}

			}
			snake[noff].x = buf.x;
			snake[noff].y = buf.y;


			if (direction) off = ( off + 1 ) % n;
			else off = (off + n - 1) % n;
			sost = 'd';
			break;
		case KEY_RIGHT:
			if (sost == 'l'){
				direction = !(direction);
				if (direction){
					noff = (noff - 2) % n;
					off = (off - 1) % n;
				}
				else{
					noff = (noff + 2) % n;
					off = (off +1) % n;
				}
			}
			if (direction) noff = ( off + 1 ) % n;
			else noff = (off + n - 1) % n;
			buf.x = (snake[off].x +1) % COLS;
			buf.y = snake[off].y;
			flag_gem = -1;
			for (i = 0; i < m; i++){
				if ((buf.x == gems[i].x) && (buf.y == gems[i].y)) flag_gem = i;
			}
			if (flag_gem != -1){
				gem_buf = (point *)malloc((m-1)*sizeof(point));
				j = 0;
				for (i = 0; i < m; i++){
					if (i != flag_gem){
						gem_buf[j] = gems[i];
						j++;
					}
				}
				free(gems);
				gems = gem_buf;
				n++;
				snake = (point *)realloc(snake, n*sizeof(point));
				for ( j = (n-2) ; j >= noff; j--){
					snake[j+1] = snake[j];
				}
				if (!direction){ 
					off = (off + 1) % n;
					noff = (noff + 1) % n;
				}

			}
			snake[noff].x = buf.x;
			snake[noff].y = buf.y;
			

			if (direction) off = ( off + 1 ) % n;
			else off = (off + n - 1) % n;
			sost = 'r';
			break;
		case KEY_LEFT:
			if (sost == 'r'){
				direction = !(direction);
				if(direction){
					noff = (noff - 2) % n;
					off = (off - 1) % n;
				}
				else{
					noff = (noff + 2) % n;
					off = (off + 1) % n;
				}
			}
			if (direction) noff = ( off + 1 ) % n;
			else noff = (off + n - 1) % n;
			buf.x = (snake[off].x + COLS - 1) % COLS;
			buf.y = snake[off].y;
			flag_gem = -1;
			for (i = 0; i < m; i++){
				if ((buf.x == gems[i].x) && (buf.y == gems[i].y)) flag_gem = i;
			}
			if (flag_gem != -1){
				gem_buf = (point *)malloc((m-1)*sizeof(point));
				j = 0;
				for (i = 0; i < m; i++){
					if (i != flag_gem){
						gem_buf[j] = gems[i];
						j++;
					}
				}
				free(gems);
				gems = gem_buf;
				n++;
				snake = (point *)realloc(snake, n*sizeof(point));
				for ( j = (n-2) ; j >= noff; j--){
					snake[j+1] = snake[j];
				}
				if (!direction){
					off = (off + 1) % n;
					noff = (noff + 1) % n;
				}

			}
			snake[noff].x = buf.x;
			snake[noff].y = buf.y;
			

			if (direction) off = ( off + 1 ) % n;
			else off = (off + n -1) % n;
			sost = 'l';
			break;
		default:
			break;
		}
		for ( i = 0 ; i < n ; i++ ) {
			if (i == noff) mvaddstr(snake[i].y,snake[i].x,"O");
			else mvaddstr(snake[i].y,snake[i].x,"*");
		}
		refresh();
		for (i = 0; i < n; i++)
			for (j = i+1; j < n; j++) 
					if ((snake[i].x == snake[j].x) && (snake[i].y == snake[j].y)) loose_flag = 1;
		pthread_mutex_unlock(&locker);
	}
	exit_flag = 0;
	status_gen = pthread_join(pid_gen, &res_gen);
	if (status_gen != 0){
			fprintf(stderr, "main error: can't join thread_gen, status = %d\n", status_gen);
			exit(-12);
	}

	status_sost = pthread_join(pid_sost, &res_sost);
	if (status_sost != 0){
			fprintf(stderr, "main error: can't join thread_gen, status = %d\n", status_sost);
			exit(-12);
	}


	echo();
	clear();
	refresh();
	endwin();
	free(gems);
	free(snake);
	if (loose_flag) printf("You loose...\n");
    return 0;
}

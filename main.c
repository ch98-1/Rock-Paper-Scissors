/*The MIT License(MIT)

Copyright(c) 2015 Corwin Hansen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

char login[256];//login name

char filename[256];//filename for user

FILE *save;//save file

clock_t last, now;//current ald last clock

typedef struct{
	char p;//what player did. r for rock, p for paper, s for scissors, q for quit
	char c;//what computer did. r for rock, p for paper, s for scissors, q for quit
	unsigned char t;//time since last round or quit in seconds.
}round;

round *play;//array of all rounds

unsigned long int rounds;//number ou round in the array

unsigned char gettime(void){//get time in seconds since last call to this function
	now = clock();//get new clock
	double t = (double)(now - last) / CLOCKS_PER_SEC;//get time in seconds
	last = clock();//set current time as last clock
	return t > UCHAR_MAX ? UCHAR_MAX : (unsigned char)t;//return time or maximum unsigned char
}

void help(void){//print out usage
	printf("To start, do \"rps {name}\" to login as that user, and do \"rpc\" to login as guest user.\nEach new user will create a new save file in running directory containing all rounds play'd by that user, time since last round, and when he started or quit.\nUsername of \"guest\" will login as a guest.\nAfter that command, you can type in any word starting with \'r\' or \'R\' to play as rock, \'p\' or \'P\' to play as paper, and 's' or 'S' to play as scissors.\nAny command starting with \'e\', \'E\', \'q\', or \'Q\' will act as exit command quitting the game.\nIf login was \"help\", \"-help\", \"--help\" or command started with \'h\' or \'H\', it will print out the help message\nCommand starting with \'c\' or \'C\' will count the win, tie and loss of the current login.\n");
}

void add(char p, char c, unsigned char t){//add data to array
	rounds++;//increment rounds
	play = realloc(play, rounds * sizeof(round));//make array bigger
	play[rounds - 1].p = p;//set data
	play[rounds - 1].c = c;
	play[rounds - 1].t = t;
}

void write(void){//write current round to file
	fprintf(save, "%c%c%c", play[rounds - 1].p, play[rounds - 1].c, play[rounds - 1].t);//write to file
}

void quit(void){//write current round to file
	add('q', 'q', gettime());//add quit signal to array
	write();//write current round
	free(play);//free array
	fclose(save);//close file
}

char choose(void){//choose rock, paper, or scissors
	unsigned long int n = rounds;//number of rounds
	char p = play[n - 1].p, c = play[n - 1 ].c;//player and computer
	if (p == c || n == 0 || p == 'q'){//if it was a tie or the first game
		srand(time(NULL) + clock());//seed rand
		int r = rand()%3;//get rand
		return r == 1 ? 'r' : r == 2 ? 'p' : 's';//choose which to play randomly
	}
	else if ((p == 'r' && c == 's') || (p == 'p' && c == 'r') || (p == 's' && c == 'p')){//if player wins
		switch (c){//go backwords around winning triangle
		case 'r':return 's';
		case 'p':return 'r';
		case 's':return 'p';
		}
	}
	else{//if computer wins
		return p;//play what they just play'd
	}
}

char *get_string(char p){//get string name for r, p, and s
	switch (p){
	case 'r':
		return "rock";
	case 'p':
		return "paper";
	case 's':
		return "scissors";
	}
}

void result(char p, char c){//display result from computer calculation and playrs choice
	add(p, c, gettime());//add data to array
	write();//write current round
	printf("Computer played %s\n", get_string(c));//display what computer play'd
	if (p == c){//if it was a tie
		printf("It's a tie\n");//display result
	}
	else if ((p == 'r' && c == 's') || (p == 'p' && c == 'r') || (p == 's' && c == 'p')){//if you win
		printf("You won\n");//display result
	}
	else{//if computer wins
		printf("The computer won\n");//display result
	}
}

void count(void){
	unsigned long int w = 0, t = 0, l = 0, i;//win's ties, and losses
	char p, c;//player and computer
	for (i = 0; i < rounds; i++){//for each object
		p = play[i].p, c = play[i].c;//player and computer
		if (p == 'q') continue;//skip checking if it was quit
		if (p == c){//if it was a tie
			t++;//increment tie
		}
		else if ((p == 'r' && c == 's') || (p == 'p' && c == 'r') || (p == 's' && c == 'p')){//if you win
			w++;//incrememnt win
		}
		else{//if computer wins
			l++;//increment loss
		}
	}
	printf("You won %lu times\nYou lost %lu times\nYou ties %lu times\n", w, l, t);//print out the result
}

int main(int argc, char *argv[]){
	if (argc > 2) {
		help();//print usage if too many arguments
		exit(EXIT_SUCCESS);
	}
	if (argc == 1) {//if no argument, default to guest
		strcpy(login, "guest");
	}
	if (argc == 2) {//if there is login or help argument
		if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0){
			help();//print usage if too many arguments
			exit(EXIT_SUCCESS);
		}
		strcpy(login, strtok(argv[1], "\\/;:\"\'+=!@#$%^&*()|~`?.,<>\a\b\f\n\r\t\v"));//remove all unneeded characters and copy argument as login
	}
	last = clock();//reset clock at login
	printf("Logging in as %s\n", login);//print message about login
	strcpy(filename, login);//copy login
	strcat(filename, ".txt");//add file extension
	rounds = 0;//reset array size
	play = NULL;//reset array
	if (NULL != (save = fopen(filename, "r"))){//if there was a previous save file
		//load file data in to memory
		char p;//things to read
		char c;
		unsigned char t;
		while (fscanf(save, "%c%c%c", &p, &c, &t) != EOF){//while file can be read
			add(p, c, t);//add data to array
		}
		fclose(save);//close file
	}
	save = fopen(filename, "ab");//open or create file
	atexit(quit);
	while (1){
		char input[256];//input value
		if(scanf("%s", input) == EOF) goto exit;//read line and exit if at end of file
		switch (input[0]){//chech the first characterof string
		case 'q':
			goto exit;//exit
		case 'Q':
			goto exit;//exit
		case 'e':
			goto exit;//exit	
		case 'E':
			goto exit;//exit
		case 'h':
			help();//display help
			break;
		case 'H':
			help();//display help
			break;
		case 'R':
			result('r', choose());//let computer process the input
			break;
		case 'r':
			result('r', choose());//let computer process the input
			break;
		case 'P':
			result('p', choose());//let computer process the input
			break;
		case 'p':
			result('p', choose());//let computer process the input
			break;
		case 'S':
			result('s', choose());//let computer process the input
			break;
		case 's':
			result('s', choose());//let computer process the input
			break;
		case 'C':
			count();//count number of win, tie and loss for current player
			break;
		case 'c':
			count();//count number of win, tie and loss for current player
			break;
		default:
			help();//display help
			break;
		}
	}
	exit:
	exit(EXIT_SUCCESS);
}

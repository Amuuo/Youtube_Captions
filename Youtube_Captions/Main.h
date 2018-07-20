#pragma once
#define __USE_MINGW_ANSI_STDIO 0
#include<algorithm>
#include<stdlib.h>
#include<iostream>
#include<iterator>
#include<fstream>
#include<sstream>
#include<stdio.h>
#include<cctype>
#include<string>
#include<vector>
#include<set>
#include<utility>
#include<map>
#include<unordered_map>
#include"Video.h"
using namespace std;


#ifdef _WIN32
	#include<conio.h>
	#include<Windows.h>
#endif

#ifdef __linux__
	
	#include<termios.h>
	
	void initTermios(int echo)
	{
		tcgetattr(0, &old); //grab old terminal i/o settings
		new1 = old; //make new settings same as old settings
		new1.c_lflag &= ~ICANON; //disable buffered i/o
		new1.c_lflag &= echo ? ECHO : ~ECHO; //set echo mode
		tcsetattr(0, TCSANOW, &new1); //apply terminal io settings
	}

	/* Restore old terminal i/o settings */
	void resetTermios(void)
	{
		tcsetattr(0, TCSANOW, &old);
	}
	char _getch(int echo)
	{
		char ch;
		initTermios(echo);
		ch = getchar();
		resetTermios();
		return ch;
	}

	char _getch(void)
	{
		return getch_(0);
	}

#endif

string sed{"sed -i 's/\\(<\\/c>\\)/ /g;s/\\(<\\)/"
           " /g;s/\\(-->\\)/ /g;s/\\(align.*%\\)/"
           " /g;s/\\(c\\..*>\\)/ /g;s/\\(c>\\)/"
           " /g;s/ /\\n/g;s/\\n/ /g;s/0.*>/ /g' "};

vector<string> cleanFiles{};
int            mention{2};
char           selection{'v'};
bool           response{};
string         screenText{};
bool           firstWordSearch{true};

string getFileContents(string);
void   getTime(vector<string>&, vector<Video>&);
void   launchUrls(vector<Video>&, vector<string>);
void   getFileNames(vector<Video>&, string);
void   saveTimestampURLtoClipboard(set<string>&);
void   cleanupFiles(vector<Video>&);
void   getTimestampURLs(Video&, int);
void   redrawScreen(string);
void   redrawScreen();
#pragma once

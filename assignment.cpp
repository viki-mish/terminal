#include <iostream>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<stdio.h>

#include <dirent.h>//DIRECTORIES

#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#include <time.h>

#include <locale.h>
#include <langinfo.h>
#include <stdint.h>

#include <termios.h>//RAWMODE
using namespace std;

#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))




//=========================================================================================================================
//                                                       GLOBAL DATA
//=========================================================================================================================
		void sethome();
		struct termios orig_termios;
		string home = "";  //---------- it is ............/home/username
		struct passwd *pw = getpwuid(getuid());

//****************************************** for printing and scrolling
		int KEY_ESCAPE=27;
		int start ;
		int endd ;
		int curpos ;
		int limit ;
		int cursor;
		int window;


//=========================================================================================================================
//                                                       IMP functions
//=========================================================================================================================

//-------------clearing screen------
void clearscreen(){
	// cout<<"hello"<<endl;
	cout<<"\033[H\033[2J\033[3J";
}

void sethome(){
	const char *homedir = pw->pw_dir;

		home = homedir;
		cout<<home<<endl;
}

void resizewin(){
	cout << "\e[8;30;120t";
}


//==========================================================================================================================
//                                                      NORMAL MODE
//==========================================================================================================================


//---------enable raw mode-------------

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
     raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON |IEXTEN| ISIG);

      raw.c_cflag |= (CS8);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}





//===========================================================================================================================
//                                                       FILES SECTION
//===========================================================================================================================

//-------------getting names of the files and directories and storing them in a vector.-----------------

struct dirent  *dp;
struct stat     statbuf;
struct passwd  *pwd;
struct group   *grp;
struct tm      *tmm;
char            datestring[256];

vector<string> getfilesdetails(string dir){ //taking file names in a vector


	struct stat file_stats;
    DIR *dirp;
    struct dirent* dent;

    vector<string> name;
    // vector<unsigned int> size;
    // vector<auto> v;

    dirp=opendir(dir.c_str()); // list files in the current directory (replace with any other path if you need to)
    do {
        dent = readdir(dirp);
        if (dent)
        {
            // printf("  file \"%s\"", dent->d_name);
            name.push_back(dent->d_name);

        }
    } while (dent);
    closedir(dirp);
    sort(name.begin(),name.end());


    // int n = name.size();
    // for(int i = 0;i<n;i++){
    // 	cout<<name[i]<<" ";//<<size[i]<<endl;
    // }

    return name;

}


//-----------------------reading perms of files and dirs----------------------
string perms_(mode_t  perms){

		string perm="";
		//dir
		perm = perm + ((perms & S_IFDIR) ? "d" : "-");
		
		//owner
		perm = perm + ((perms & S_IRUSR) ? "r" : "-");
        perm = perm + ((perms & S_IWUSR) ? "w" : "-");
        perm = perm + ((perms & S_IXUSR) ? "x" : "-");
       
       	//group
        perm = perm + ((perms & S_IRGRP) ? "r" : "-");
        perm = perm + ((perms & S_IWGRP) ? "w" : "-");
        perm = perm + ((perms & S_IXGRP) ? "x" : "-");

        //other
        perm = perm + ((perms & S_IROTH) ? "r" : "-");
        perm = perm + ((perms & S_IWOTH) ? "w" : "-");
        perm = perm + ((perms & S_IXOTH) ? "x" : "-");

        return perm;


}


//--------------getting file details and printing ----------------------
void print_N_files(vector<string> files){//, int start,int esdd){

	// int n = files.size();
	// if(n<21){
	// 	endd = s	// }

	// vector<string> files = getfilesdetails();
		int endd_=limit;
		int start_ = 0;
		if(limit>=endd){
			start_=start;
			endd_=endd;
		}
		
        for(int i = start_;i<endd_;i++){


	        	// if(files[i] != files[i]){
	        	// 	continue;
	        	// 	cout<<"hi"<<endl;
	        	// }

        		string file_name;
        		if(files[i].size()>19){
        			file_name=files[i].substr(0,16)+"...";
        		}
        		else{
        			file_name=files[i];
        		}

        		printf(" %-18.18s " , file_name.c_str());

	            /* Get entry's information. */
	            if (stat(files[i].c_str(), &statbuf) == -1)
	                continue;


	            /* Print out type, permissions, and number of links. */
	            printf("\t%10.10s", perms_(statbuf.st_mode).c_str());
	            // printf(" (%3o)", statbuf.st_mode&0777);
	            // printf("%4d", statbuf.st_nlink);


	            /* Print out owner's name if it is found using getpwuid(). */
	            if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
	                printf(" \t%.28s", pwd->pw_name);
	            else
	                printf(" %-8d", statbuf.st_uid);

	            // cout<<"--------------";



	            /* Print out group name if it is found using getgrgid(). */
	            if ((grp = getgrgid(statbuf.st_gid)) != NULL)
	                printf(" \t%-8.28s", grp->gr_name);
	            else
	                printf(" %-8d", statbuf.st_gid);


	            /* Print size of file. */
	            printf(" %8.4fK", (float)statbuf.st_size/((1.0)*1024));//-------------------------size


	            tmm = localtime(&statbuf.st_mtime);


	            /* Get localized date string. */
	            strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tmm);


	            printf(" \t%2.30s \r\n", datestring);//---------------------------date
        }

}





//=============================================================================================================================
//                                                     SCROLLING IN NORMAL MODE
//=============================================================================================================================
void upkey(){

	if(cursor>1){
		cursor--;
		curpos--;
		gotoxy(0,14);
		cout<<"curpos: "<<curpos<<", limit: "<<limit<<", start: "<<start<<", endd: "<<endd<<"\r\n";
		gotoxy(0,cursor);
	}
}
//************************************************************* navigation keys *******************************************
void downkey(){

	if(cursor<window and curpos<limit){
		// cout<<curpos<<"\r\n";
		curpos++;
		cursor++;
		gotoxy(0,14);
		cout<<"curpos: "<<curpos<<", limit: "<<limit<<", start: "<<start<<", endd: "<<endd<<"\r\n";
		gotoxy(0,cursor);
	}
}

void rightkey(){

	// if(curpos!=endd and curpos!=limit)gotoxy(0,curpos++);
	cout<<"hi";

}

void leftkey(){

	// if(curpos!=endd and curpos!=limit)gotoxy(0,curpos++);

}

void k(){
	if(cursor==1 and curpos>0){
		char cwd[PATH_MAX];
	   	getcwd(cwd, sizeof(cwd));
	   	string currwd = cwd;
	   	clearscreen();
	   	curpos--;
		start--;
		endd--;
		vector<string> files = getfilesdetails(currwd);
		print_N_files(files);
		gotoxy(0,14);
		cout<<"curpos: "<<curpos<<", limit: "<<limit<<", start: "<<start<<", endd: "<<endd<<"\r\n";
		gotoxy(0,cursor);

	}
}
void l(){// navigates down on l if not the end of files and cursor at the last point

	if(cursor==window and curpos<limit-1){
		char cwd[PATH_MAX];
	   	getcwd(cwd, sizeof(cwd));
	   	string currwd = cwd;
	   	clearscreen();
	   	curpos++;
		start++;
		endd++;
		vector<string> files = getfilesdetails(currwd);
		print_N_files(files);
		gotoxy(0,14);
		cout<<"curpos: "<<curpos<<", limit: "<<limit<<", start: "<<start<<", endd: "<<endd<<"\r\n";
		gotoxy(0,cursor);

	}

}
void enter(){

	gotoxy(0,15);
	cout<<"enter dbaya gya hai at curpos: "<<curpos;
	gotoxy(0,cursor);

}
void backspace(){

}
void h(){

}


//=============================================================================================================================
//                                                      DETECT KEYS
//=============================================================================================================================



static int kbhit(void)
{
    int c = 0;

    struct termios term,oterm;

    tcgetattr(0, &oterm);
    memcpy(&term, &oterm, sizeof(term));
    term.c_lflag &= ~(ICANON | ECHO);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &term);
    c = cin.get();
    tcsetattr(0, TCSANOW, &oterm);
    if (c != -1){ ungetc(c, stdin);}

    return ((c != -1) ? 1 : 0);
}

static int kbesc(void)
{
    int c;

    if (!kbhit()){  return KEY_ESCAPE;}
    c = cin.get();
    if (c == '[') {
        switch (cin.get()) {
            case 'A':
                 upkey();
                break;
            case 'B':
                 downkey();
                
                break;
            case 'C':
                 rightkey();

                break;
            case 'D':
                 leftkey();

                break;
            default:
                c = 0;
                break;
        }
    } else {
        c = 0;
    }
    if (c == 0) while (kbhit()) cin.get();
    return c;
}

static int kbget(void)
{
    int c;

    c = cin.get();

    if(c==KEY_ESCAPE){
	    return kbesc();
    }
    else{


    	switch(c){
    		case 13:
    			enter();
    			break;
    		case 107:
    			k();
    			break;
    		case 108:
    			l();
    			break;
    		case 104:
    			h();
    			break;
    		case 127:
    			backspace();
    			break;
    	}


    	return c;
    }

}





void enable_scrolling(){  //dont press esccape.

	int c ;
	gotoxy(0,1);

	while((c = kbget()) and c!=113){

	}


}




//=============================================================================================================================
//                                                           NORMAL MODE
//=============================================================================================================================


//------------------------------------------------------------- entering normal mode (non canonical mode)------------------------------

void normal_mode(string cwd){
	resizewin();
	enableRawMode();
	vector<string> files = getfilesdetails(cwd);
	limit=files.size();
	start = 0;
	endd = 4;
	window = 4;
	curpos = 0;
	cursor = 1;

	print_N_files(files);

	enable_scrolling();
	clearscreen();


}




int main(int argc, char const *argv[])
{


	clearscreen();
	//----------get current working directory------------
	char cwd[PATH_MAX];
   	getcwd(cwd, sizeof(cwd));
   	string currwd = cwd;
   	normal_mode(currwd);




	return 0;
}
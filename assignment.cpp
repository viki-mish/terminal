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
		int flag = 0;
		int notenabled=1;
		int hset=0;

		stack<string> stkleft;
		stack<string> stkright;



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
void normal_mode(string);
static int kbget(void);


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

//---------------------------------------------------------------------------------------------------------------------
//                                               reading perms of files and dirs
//---------------------------------------------------------------------------------------------------------------------
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



//----------------------------------------------------------------------------------------------------------------------------
//                                                    getting file details and printing 
// ---------------------------------------------------------------------------------------------------------------------------

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

	            if(!hset){
	            	string a = pwd->pw_name;
	            	home = "/home/"+a;
	            	hset=1;
	            }




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

bool checkifdir(string file_name){
	if (stat(file_name.c_str(), &statbuf) == -1)
        ;
    string per=perms_(statbuf.st_mode);

    if(per[0]=='d'){
    	return true;
    }
    return false;
}




//=============================================================================================================================
//                                      SCROLLING IN NORMAL MODE (up and down keys)
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

	if(cursor<window and curpos<limit and cursor<limit){
		// cout<<curpos<<"\r\n";
		curpos++;
		cursor++;
		gotoxy(0,14);
		cout<<"curpos: "<<curpos<<", limit: "<<limit<<", start: "<<start<<", endd: "<<endd<<"\r\n";
		gotoxy(0,cursor);
	}
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

//=============================================================================================================================
//                                  NAVIGATING IN NORMAL MODE (RIGHT, H, ENTER, BACKSPACE, H  keys)
//=============================================================================================================================


void rightkey(){

	// if(curpos!=endd and curpos!=limit)gotoxy(0,curpos++);
	if(!stkright.empty()){
		string rightdir = stkright.top();
		stkright.pop();

		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
	   	string currwd = cwd;

		stkleft.push(currwd);

		chdir(rightdir.c_str());
		normal_mode(rightdir);
		flag = 1;

	}

}

void leftkey(){

	// if(curpos!=endd and curpos!=limit)gotoxy(0,curpos++);
	if(!stkleft.empty()){
		string leftdir = stkleft.top();
		stkleft.pop();

		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
	   	string currwd = cwd;

		stkright.push(currwd);

		chdir(leftdir.c_str());
		normal_mode(leftdir);
		flag=1;
	}

}

void enter(){

	gotoxy(4,15);
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
   	string currwd = cwd;
	vector<string> files = getfilesdetails(currwd);
	if(checkifdir(files[curpos])){
		
		cout<<"enter dbaya gya hai at curpos: "<<curpos<<" aur ye directory hai";

		string cwdd = currwd+"/"+files[curpos];
		stkleft.push(currwd);
		chdir(cwdd.c_str());
		normal_mode(cwdd);
		flag=1;
		return;
		
	}
	else{
		cout<<"enter dbaya gya hai at curpos: "<<curpos<<" aur ye file hai "<<cwd;
		pid_t pid = fork();
		if (pid == 0) {
		  execl("/usr/bin/xdg-open", "xdg-open", files[curpos].c_str(), (char *)0);
		  exit(1);
		}
	}
	gotoxy(0,16);
	chdir(cwd);
	gotoxy(0,cursor);

}
void backspace(){// moves one level up and clears left and right stack.


	// if(!stkleft.empty()){
	// 	leftkey();
	// }
	// else{
		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
	   	string currwd = cwd;
		
		stkleft = stack<string>();
		stkright = stack<string>();

	   	string cwdd = currwd+"/..";
		chdir(cwdd.c_str());
		normal_mode(cwdd);
		flag = 1;
		return;
	// }

}
void h(){

	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
   	string currwd = cwd;
	
	stkleft = stack<string>();
	stkright = stack<string>();

   	string cwdd = home;
	chdir(cwdd.c_str());
	normal_mode(cwdd);
	flag = 1;
	return;

}


//=============================================================================================================================
//                                                      COMMAND MODE
//=============================================================================================================================

string change(string s){
	int len = s.size();
	string fin = "";
	if(s[0]=='~'){
		fin= home+s.substr(1,len-1);
	}
	if(s[0]=='.'){
		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
	   	string currwd = cwd;

		fin = cwd + s.substr(1,len-1);
	}
	return fin;
}

vector<string> getcmd(string s){
	stringstream ss(s);
	vector<string> output;
	string str;
	while(getline(ss,str,' ')){
		output.push_back(str);
	}

	if(output[0]=="search"){
		return output;
	}
	int len = output.size();
	else{
		string a = output[len-1];
		a=change(a);
		output[len-1]=a;
	}
	return output;
}

void copy(vector<string> v){

	int len = v.size();
	string DEST = v[len-1];

	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
   	string currwd = cwd;

	for(int i = 0;i<len-1){

		string SRC = currwd + "/" + output[i];

		struct stat st;

		stat(SRC, &st);
		string k = DEST+"/"+output[i];

		std::ifstream src(SRC.c_str(), std::ios::binary);
	    std::ofstream dest(DEST.c_str(), std::ios::binary);

	    
	    dest << src.rdbuf();
		
		chmod(k, st.st_mode);
	}


}

int check(string s){
	if(s=="copy")
		return 1;
	else if(s=="move")
		return 2;
	else if(s=="rename")
		return 3;
	else if(s=="ceate_file")
		return 4;
	else if(s=="create_dir")
		return 5;
	else if(s=="delete_file")
		return 6;
	else if(s=="delete_dir")
		return 7;
	else if(s=="goto")
		return 8;
	else if(s=="search")
		return 9;
	else
		return 0;
}

void cmd(){
	gotoxy(0,15);
	cout<<"COMMAND MODE"<<"\r\n";
	// disableRawMode();

	char c ;

	int flag_ = 1;
	string s = "";
	while(flag_){
		c = cin.get();

		if((int)c==27){
			char cwd[PATH_MAX];
			getcwd(cwd, sizeof(cwd));
		   	string currwd = cwd;
		   	normal_mode(cwd);
		   	flag=1;
			flag_=0;
			break;
		}
		if((int)c==113){
			flag=1;
			return;
		}
		if((int)c==13){//enter
			cout<<" enter is pressed";
			vector<string> commands = getcmd(s);
			s="";
			int command = check(commands[0]);
			printf("\33[2K\r");
			switch(command){
				case 1:
					copy(commands);
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
				case 9:
					break;
			}

		}
		else{
			cout<<(char)c;
			s=s+c;
			// cout<<s;	
		}



	}



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
    		case 58:
    			cmd();
    			break;
    	}


    	return c;
    }

}

//==================================== LOOKING AT KEYPRESS AND EXITING ON q =================================================

// int cur = 13;

void enable_scrolling(){  //dont press esccape.

	int c ;
	gotoxy(0,1);

	while((c = kbget()) and c!=113 and !flag){

	}

	// gotoxy(0,cur);



}





//=============================================================================================================================
//                                                           NORMAL MODE
//=============================================================================================================================


//------------------------------------------------------------- entering normal mode (non canonical mode)------------------------------

void normal_mode(string cwd){
	clearscreen();
	resizewin();
	if(notenabled)
		enableRawMode();
	notenabled=0;
	vector<string> files = getfilesdetails(cwd);
	limit=files.size();
	start = 0;
	endd = 10;
	window = 10;
	curpos = 0;
	cursor = 1;

	print_N_files(files);

	enable_scrolling();
	// cout<<"exiting normal mode "<<cur-12;
	// cur++;
	

	clearscreen();
	// exit(0);


}



//=============================================================================================================================
//                                                           INIT PROGRAM
//=============================================================================================================================



int main(int argc, char const *argv[])
{


	clearscreen();
	//----------get current working directory------------
	char cwd[PATH_MAX];
   	getcwd(cwd, sizeof(cwd));
   	string currwd = cwd;
   	normal_mode(currwd);

   	// gotoxy(0,18);
   	// cout<<"hi"<<endl;


	return 0;
}
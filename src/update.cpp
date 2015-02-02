//symbol codes
//left-1
//right-2
//up-3
//down-4
//forward-5
//cross-6
//w-7
//b-8
//s-9
//x-10
//r-red-11
#include "stdafx.h"
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include "stddef.h"
#include "opencv2\core\core.hpp"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
using namespace cv;

int thc1;
int thc2;
int thc3;
int th_text_1=150;
int th_text_2=150;

IplImage* img;
IplImage* img2;
IplImage* img3;
IplImage* img4;
IplImage* imgGrayScale;
IplImage* c1;
IplImage* c2;
IplImage* cred;
IplImage* cth ;
IplImage* cth2 ;
IplImage* cthb;
int height;

int iLowH2 = 20;
int iHighH2 = 43;
int iLowS2 = 89; 
int iHighS2 = 255;
int iLowV2 = 94;
int iHighV2 = 255;

int iLowH = 170;
int iHighH = 179;
int iLowS = 103; 
int iHighS = 255;
int iLowV = 0;
int iHighV = 255;

int detected[1000][3]; 
int sorted_x[1000];
int sorted_y[1000];
int col[1000];
int row[1000];
int row_count=0;
int col_count=0;
int cur_node_count=0;


Mat grayimg1;
Mat grayimg2;
Mat imgthr1;
Mat imgthr2;

CvPoint matrix_points[40][40];
int matrix_type[40][40];
bool created[40][40];

int paths=0;

int path[30][30][2];//10 possible paths
					//30 possile nodes per path
					//2 elements per node x,y

int cur_path_node_count[30];

void print_paths()
{
	IplImage* img;
	img=cvLoadImage("C:/Users/Jay/Documents/test.png");
		//printf("Path %d\n",i);
		/*FILE * xFile;
		xFile = fopen ("C:/Users/Jay/Documents/x.txt","w");
		FILE * yFile;
		yFile = fopen ("C:/Users/Jay/Documents/y.txt","w");
		FILE * tFile;
		tFile = fopen ("C:/Users/Jay/Documents/t.txt","w");*/
		for(int i=0;i<paths;i++)
		{
			printf("Path %d",(i+1));
			int r=rand()%256;
			int g=rand()%256;
			int b=rand()%256;
		for(int t=cur_path_node_count[i]-1;t>=0;t--)
		{	
			//fprintf (pFile, "Name %d [%-10.10s]\n",n,name);
			//fprintf(pFile,"%d,%d\n",path[i][t][0],path[i][t][1]);
			//fprintf(xFile,"%d\n",matrix_points[path[t][0]-1][path[t][1]-1].x);
			//fprintf(yFile,"%d\n",matrix_points[path[t][0]-1][path[t][1]-1].y);
			//fprintf(tFile,"%d\n",matrix_type[path[t][0]-1][path[t][1]-1]);
			printf("(%d,%d),",path[i][t][0],path[i][t][1]);
			
			if(t!=0)
			{
				cvLine(img,matrix_points[path[i][t][0]-1][path[i][t][1]-1],matrix_points[path[i][t-1][0]-1][path[i][t-1][1]-1],cvScalar(r,g,b),10,8,0);
			}
		}
		printf("\n");
		}
		/*fclose (xFile);
		fclose (yFile);
		fclose (tFile);
		FILE * rFile;
		rFile=fopen("C:/Users/Jay/Documents/ready.txt","w");
		fprintf(rFile,"%d\n",1);
		fclose (rFile);*/
	
	cvShowImage("Path",img);
}
void init_created()
{
	for(int i=0;i<row_count;i++)
	{
	for(int j=0;j<col_count;j++)
	{
	created[i][j]=false;
	}
	}
}

bool iscreated(int x,int y)
{
	if(created[x-1][y-1]==true)
	{
	return true;
	}
	else
	{
	return false;
	}
}

int gettfc(int x,int y)
{
	if(x>row_count || x<1 || y>col_count || y<1)
	{
	return 0;
	}
	else
	{
	return matrix_type[x-1][y-1];
	}
}

typedef struct node
{
	int row;
	int col;
	int type;
	struct node* left;
	struct node* right;
	struct node* up;
	struct node* down;
	struct node* parent;
	char last_dir_traversal;
};


typedef struct stack_node
{
	struct node* identity;
	struct stack_node* bottom;
	struct stack_node* top;
	bool branch;
	char branch_dir;
};

node* nodes[40][40];
node* start_node=NULL;
node* end_node=NULL;


stack_node* tos=NULL;


void print_stack()
{
	stack_node* tracker;
	tracker=tos;
	printf("\nStack contains:");
	while(tracker->bottom!=NULL)
	{
		printf("%d(%d,%d):%c,",tracker->identity->type,tracker->identity->row,tracker->identity->col,tracker->branch_dir);
		tracker=tracker->bottom;
	}
	printf("%d(%d,%d):%c,",tracker->identity->type,tracker->identity->row,tracker->identity->col,tracker->branch_dir);
	printf("\n");
}
stack_node* stack_push(node* elem,bool branch,char branch_dir)
{
	stack_node* nsp=(stack_node*)malloc(sizeof(stack_node));
	nsp->identity=elem;
	
	nsp->top=NULL;
	nsp->branch=branch;
	nsp->branch_dir=branch_dir;
	if(tos!=NULL)
	{
	nsp->bottom=tos;
	tos->top=nsp;
	tos=nsp;
	}
	else
	{
	nsp->bottom=NULL;
	tos=nsp;
	printf("tos:%d\n",tos->identity->type);
	}
	return tos;
}

stack_node* stack_pop()
{
	if(tos->bottom!=NULL)
	{
	//stack_node* free_me=NULL;
	/*if(tos!=NULL)
	{
	free_me=tos;
	}*/
	tos=tos->bottom;
	tos->top=NULL;
	//if(free_me)free(free_me);
	printf("%d(%d,%d),%c",tos->identity->type,tos->identity->row,tos->identity->col,tos->branch_dir);
	return tos;
	}
	else
	{
		return NULL;
	}
}
/*
void store_path()
{
	printf("%d,%d\n",end_node->row,end_node->col);
	node* tracker=end_node;
	//node* parent=NULL;
	while(true)
	{
		path[cur_path_count][cur_path_node_count[cur_path_count]][0]=tracker->row;
		path[cur_path_count][cur_path_node_count[cur_path_count]][1]=tracker->col;
		cur_path_node_count[cur_path_count]++;
		
		if(tracker==start_node)
		{
		break;
		}
		tracker=tracker->parent;
	}
	cur_path_count++;
}*/
void init_nodes()
{
	for(int i=0;i<row_count;i++)
	{
	for(int j=0;j<col_count;j++)
	{
	nodes[i][j]=NULL;
	}
	}
}
node* get_node(int x,int y)
{
	if(x>row_count || x<1 || y>col_count || y<1)
	{
	return NULL;
	}
	else
	{
	return nodes[x-1][y-1];
	}
}
node* create_node(int type,node* parent,int row,int col)
{
	node* f_node;
	f_node=(node*)malloc(sizeof(node));
	f_node->type=type;
	f_node->up=NULL;
	f_node->left=NULL;
	f_node->right=NULL;
	f_node->down=NULL;
	f_node->row=row;
	f_node->col=col;
	f_node->parent=parent;
	created[row-1][col-1]=true;
	nodes[row-1][col-1]=f_node;
	f_node->last_dir_traversal='x';
	return f_node;
}

bool backtrack(node* check,node* checkfrom)
{
	node* tracker=checkfrom;
	while(true)
	{
	tracker=tracker->parent;
	if(tracker==get_node(6,2))
	{
	return false;
	}
	if(tracker==check)
	{
	return true;
	}
	}
}
bool check_loop(node* test,char cur_dir)
{
	bool char_flag=false;
	bool backtrack_flag=false;

	printf("%c==%c\n",test->last_dir_traversal,cur_dir);
	if(test->last_dir_traversal==cur_dir)
	{
		char_flag=true;
	}

	node* tracker=test->parent;
	int no_back=0;
	printf("checking for loops at %d(%d,%d)\n",test->type,test->row,test->col);
	//backtrack all nodes except test and tracker->parent for tracker
	
	node* bottom=get_node((tracker->row)+1,(tracker->col));//bottom
	node* right=get_node((tracker->row),(tracker->col)+1);//right
	node* top=get_node((tracker->row)-1,(tracker->col));//top
	node* left=get_node((tracker->row)+1,(tracker->col)-1);//left
	
	if(bottom!=NULL && bottom->up==tracker && bottom!=tracker->parent && bottom!=test)
	{
	printf("bottom chosen for backtrack\n");
	no_back++;
	backtrack_flag=backtrack(test,bottom);
	}
	if(right!=NULL && right->left==tracker && right!=tracker->parent && right!=test)
	{
	printf("right chosen for backtrack\n");
	no_back++;
	backtrack_flag=backtrack(test,right);
	
	}
	if(top!=NULL && top->down==tracker && top!=tracker->parent && top!=test)
	{
	printf("top chosen for backtrack\n");
	no_back++;
	backtrack_flag=backtrack(test,top);
	}
	if(left!=NULL && left->right==tracker && left!=tracker->parent && left!=test)
	{
	printf("left chosen for backtrack\n");
	no_back++;
	backtrack_flag=backtrack(test,left);
	}

	if(no_back==0)
	{
	printf("no backtrackable node so no loop exists!\n");
	backtrack_flag=false;
	}
	
	if(char_flag && backtrack_flag)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void explore_node(node* test,char dir)
{
	if(test->type==1)
	{
	int type_left=gettfc(test->row,(test->col)-1);
	if(type_left!=0 && iscreated(test->row,(test->col)-1)==false)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	test->left=create_node(type_left,test,test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';
	explore_node(test->left,'l');
	}
	else if(type_left!=0 && iscreated(test->row,(test->col)-1)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->left==NULL)
	{
	//new link to node is to be created
	test->left=get_node(test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';
	explore_node(test->left,'l');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)-1),'l'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->left->last_dir_traversal='l';
	explore_node(get_node(test->row,(test->col)-1),'l');
	}	
	}
	}
	else
	{
	test->left=NULL;
	}
	}
	if(test->type==2)
	{
	int type_right=gettfc(test->row,(test->col)+1);
	if(type_right!=0 && iscreated(test->row,(test->col)+1)==false)
	{
	test->right=create_node(type_right,test,test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';
	explore_node(test->right,'r');
	}
	else if(type_right!=0 && iscreated(test->row,(test->col)+1)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->right==NULL)
	{
	//new link to node is to be created
	test->right=get_node(test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';
	explore_node(test->right,'r');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)+1),'r'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->right->last_dir_traversal='r';
	explore_node(get_node(test->row,(test->col)+1),'r');
	}	
	}
	}
	else
	{
	test->right=NULL;
	}

	}
	if(test->type==3)
	{
	int type_up=gettfc((test->row)-1,test->col);
	if(type_up!=0 && iscreated((test->row)-1,test->col)==false)
	{
	test->up=create_node(type_up,test,(test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';
	explore_node(test->up,'u');
	}
	else if(type_up!=0 && iscreated((test->row)-1,test->col)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->up==NULL)
	{
	//new link to node is to be created
	test->up=get_node((test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';
	explore_node(test->up,'u');
	}
	else
	{
	//link already exists
	//check for loop
	printf("At up tri last dir=%c\n",test->up->last_dir_traversal);
	if(check_loop(get_node((test->row)-1,test->col),'u'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->up->last_dir_traversal='u';
	explore_node(get_node((test->row)-1,test->col),'u');
	}	
	}
	}
	else
	{
	test->up=NULL;
	}
	}
	if(test->type==4)
	{
	int type_down=gettfc((test->row)+1,test->col);
	if(type_down!=0 && iscreated((test->row)+1,test->col)==false)
	{
	test->down=create_node(type_down,test,(test->row)+1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	test->down->last_dir_traversal='d';
	explore_node(test->down,'d');
	}
	else if(type_down!=0 && iscreated((test->row)+1,test->col)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->down==NULL)
	{
	//new link to node is to be created
	test->down=get_node((test->row)+1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	test->down->last_dir_traversal='d';
	explore_node(test->down,'d');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node((test->row)+1,test->col),'d'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->down->last_dir_traversal='d';
	explore_node(get_node((test->row)+1,test->col),'d');
	}	
	}
	}
	else
	{
	test->down=NULL;
	}
	}
	if(test->type==5||test->type==7||test->type==8)
	{
		
	//if(test->parent->row==((test->row)-1) || get_node((test->row)-1,test->col)!=NULL && get_node((test->row)-1,test->col)->down!=NULL)
	if(dir=='d')
	{
	//move down
	printf("here12345\n");
	int type_down=gettfc((test->row)+1,test->col);
	if(type_down!=0 && iscreated((test->row)+1,test->col)==false)
	{
	test->down=create_node(type_down,test,(test->row)+1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	test->down->last_dir_traversal='d';explore_node(test->down,'d');
	}
	else if(type_down!=0 && iscreated((test->row)+1,test->col)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->down==NULL)
	{
	//new link to node is to be created
	test->down=get_node((test->row)+1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	test->down->last_dir_traversal='d';explore_node(test->down,'d');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node((test->row)+1,test->col),'d'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->down->last_dir_traversal='d';explore_node(get_node((test->row)+1,test->col),'d');
	}	
	}
	}
	else
	{
	test->down=NULL;
	}
	
	}
	//if(test->parent->row==((test->row)+1) || get_node((test->row)+1,test->col)!=NULL && get_node((test->row)+1,test->col)->up!=NULL)
	if(dir=='u')
	{
	//move up
	int type_up=gettfc((test->row)-1,test->col);
	if(type_up!=0 && iscreated((test->row)-1,test->col)==false)
	{
	test->up=create_node(type_up,test,(test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';explore_node(test->up,'u');
	}
	else if(type_up!=0 && iscreated((test->row)-1,test->col)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->up==NULL)
	{
	//new link to node is to be created
	test->up=get_node((test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';explore_node(test->up,'u');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node((test->row)-1,test->col),'u'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->up->last_dir_traversal='u';explore_node(get_node((test->row)-1,test->col),'u');
	}	
	}
	}
	else
	{
	test->up=NULL;
	}
	}
	//if(test->parent->col==((test->col)+1) || get_node(test->row,(test->col+1)) && get_node(test->row,(test->col+1))->left!=NULL)
	if(dir=='l')
	{
	//move left
	int type_left=gettfc(test->row,(test->col)-1);
	if(type_left!=0 && iscreated(test->row,(test->col)-1)==false)
	{
	test->left=create_node(type_left,test,test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';
	explore_node(test->left,'l');
	}
	else if(type_left!=0 && iscreated(test->row,(test->col)-1)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->left==NULL)
	{
	//new link to node is to be created
	test->left=get_node(test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';
	explore_node(test->left,'l');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)-1),'l'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->left->last_dir_traversal='l';
	explore_node(get_node(test->row,(test->col)-1),'l');
	}	
	}
	}
	else
	{
	test->left=NULL;
	}

	}
	//if(((test->parent->col)+1)==(test->col) || get_node((test->row),(test->col)-1) && get_node((test->row),(test->col)-1)->right!=NULL)
	if(dir=='r')
	{
	//move right
	int type_right=gettfc(test->row,(test->col)+1);
	if(type_right!=0 && iscreated(test->row,(test->col)+1)==false)
	{
	test->right=create_node(type_right,test,test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';explore_node(test->right,'r');
	}
	else if(type_right!=0 && iscreated(test->row,(test->col)+1)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->right==NULL)
	{
	//new link to node is to be created
	test->right=get_node(test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';explore_node(test->right,'r');
	
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)+1),'r'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->right->last_dir_traversal='r';explore_node(get_node(test->row,(test->col)+1),'r');
	}	
	}
	}
	else
	{
	test->right=NULL;
	}
	}
	}
	if(test->type==6)
	{
	int type_right=gettfc(test->row,(test->col)+1);
	//left
	int type_left=gettfc(test->row,(test->col)-1);
	//up
	int type_up=gettfc((test->row)-1,test->col);
	//down
	int type_down=gettfc((test->row)+1,test->col);
	int upflag=0;
	int downflag=0;
	int rightflag=0;
	int leftflag=0;
	if(dir=='d')
	{
	//cannot move up
	upflag=1;
	}
	if(dir=='u')
	{
	//cannot move down
	downflag=1;
	}
	if(dir=='l')
	{
	//cannot move right
	rightflag=1;
	}
	if(dir=='r')
	{
	//cannot move left
	leftflag=1;
	}
	if(type_right!=0 && iscreated(test->row,(test->col)+1)==false && rightflag==0)
	{
	test->right=create_node(type_right,test,test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';explore_node(test->right,'r');
	}
	else if(type_right!=0 && iscreated(test->row,(test->col)+1)==true && rightflag==0)
	{
	//already the node created in place and hence dont have to create it.
	if(test->right==NULL)
	{
	//new link to node is to be created
	test->right=get_node(test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';printf("%c\ntravelled right at square\n",test->right->last_dir_traversal);explore_node(test->right,'r');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)+1),'r'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->right->last_dir_traversal='r';explore_node(get_node(test->row,(test->col)+1),'r');
	}	
	}
	}
	else
	{
	test->right=NULL;
	}
	if(type_left!=0 && iscreated(test->row,(test->col)-1)==false && leftflag==0)
	{
	test->left=create_node(type_left,test,test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';
	explore_node(test->left,'l');
	}
	else if(type_left!=0 && iscreated(test->row,(test->col)-1)==true  && leftflag==0)
	{
	//already the node created in place and hence dont have to create it.
	if(test->left==NULL)
	{
	//new link to node is to be created
	test->left=get_node(test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';explore_node(test->left,'l');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)-1),'l'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->left->last_dir_traversal='l';
	explore_node(get_node(test->row,(test->col)-1),'l');
	}	
	}
	}
	else
	{
	test->left=NULL;
	}
	if(type_up!=0 && iscreated((test->row)-1,test->col)==false && upflag==0)
	{
	test->up=create_node(type_up,test,(test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';explore_node(test->up,'u');
	}
	else if(type_up!=0 && iscreated((test->row)-1,test->col)==true && upflag==0)
	{
	//already the node created in place and hence dont have to create it.
	if(test->up==NULL)
	{
	//new link to node is to be created
	test->up=get_node((test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';explore_node(test->up,'u');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node((test->row)-1,test->col),'u'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->up->last_dir_traversal='u';explore_node(get_node((test->row)-1,test->col),'u');
	}	
	}
	}
	else
	{
	test->up=NULL;
	}
	if(type_down!=0 && iscreated((test->row)+1,test->col)==false && downflag==0)
	{
	test->down=create_node(type_down,test,(test->row)+1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	test->down->last_dir_traversal='d';explore_node(test->down,'d');
	}
	else if(type_down!=0 && iscreated((test->row)+1,test->col)==true  && downflag==0)
	{
	//already the node created in place and hence dont have to create it.
	if(test->down==NULL)
	{
	//new link to node is to be created
	test->down=get_node((test->row)+1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	test->down->last_dir_traversal='d';explore_node(test->down,'d');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node((test->row)+1,test->col),'d'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->down->last_dir_traversal='d';explore_node(get_node((test->row)+1,test->col),'d');
	}	
	}
	}
	else
	{
	test->up=NULL;
	}
	}
	if(test->type==9)
	{
	//start node explore left,right,up.
	//right
	int type_right=gettfc(test->row,(test->col)+1);
	//left
	int type_left=gettfc(test->row,(test->col)-1);
	//up
	int type_up=gettfc((test->row)-1,test->col);
	
	if(type_right!=0 && iscreated(test->row,(test->col)+1)==false)
	{
	test->right=create_node(type_right,test,test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';explore_node(test->right,'r');
	}
	else if(type_right!=0 && iscreated(test->row,(test->col)+1)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->right==NULL)
	{
	//new link to node is to be created
	test->right=get_node(test->row,(test->col)+1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	test->right->last_dir_traversal='r';explore_node(test->right,'r');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)+1),'r'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->right->last_dir_traversal='r';explore_node(get_node(test->row,(test->col)+1),'r');
	}	
	}
	}
	else
	{
	test->right=NULL;
	}
	if(type_left!=0 && iscreated(test->row,(test->col)-1)==false)
	{
	test->left=create_node(type_left,test,test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';explore_node(test->left,'l');
	}
	else if(type_left!=0 && iscreated(test->row,(test->col)-1)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->left==NULL)
	{
	//new link to node is to be created
	test->left=get_node(test->row,(test->col)-1);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	test->left->last_dir_traversal='l';explore_node(test->left,'l');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node(test->row,(test->col)-1),'l'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->left->last_dir_traversal='l';
	explore_node(get_node(test->row,(test->col)-1),'l');
	}	
	}
	}
	else
	{
	test->left=NULL;
	}
	if(type_up!=0 && iscreated((test->row)-1,test->col)==false)
	{
	test->up=create_node(type_up,test,(test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';explore_node(test->up,'u');
	}
	else if(type_up!=0 && iscreated((test->row)-1,test->col)==true)
	{
	//already the node created in place and hence dont have to create it.
	if(test->up==NULL)
	{
	//new link to node is to be created
	test->up=get_node((test->row)-1,test->col);
	printf("Created a link from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	test->up->last_dir_traversal='u';explore_node(test->up,'u');
	}
	else
	{
	//link already exists
	//check for loop
	if(check_loop(get_node((test->row)-1,test->col),'u'))
	{
	printf("Loop detected...Exiting loop\n");
	}
	else
	{
	test->up->last_dir_traversal='u';explore_node(get_node((test->row)-1,test->col),'u');
	}	
	}
	}
	else
	{
	test->up=NULL;
	}
	}
	if(test->type==10)
	{
	printf("exploration reached end\n");
	//paths++;
	end_node=test;
	}
	if(test->type==11)
	{
	printf("red reached\n");
	}

	//test->explored=true;
	//explored[(test->row)-1][(test->col)-1]=true;
	//printf("explored %d(%d,%d)\n",test->type,test->row,test->col);
	//printf(explored[(test->row)-1][(test->col)-1]?"true":"false");
	//printf("\n");
}

node* get_graph_node(node* test,char dir)
{
	if(dir=='l')
	{
		if(test->left!=NULL)
		{
			return test->left;
		}
		else
		{
			return NULL;
		}
	}
	if(dir=='r')
	{
		if(test->right!=NULL)
		{
			return test->right;
		}
		else
		{
			return NULL;
		}
	}
	if(dir=='u')
	{
		if(test->up!=NULL)
		{
			printf("haay\n");
			return test->up;
		}
		else
		{
			printf("nai\n");
			return NULL;
		}
	}
	if(dir=='d')
	{
		if(test->down!=NULL)
		{
			return test->down;
		}
		else
		{
			return NULL;
		}
	}
}
void pathfinder(node* test,char dir)
{
	
	if(test->type==1)
	{
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_left=get_graph_node(test,'l');
	if(type_left!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	pathfinder(test->left,'l');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	if(test->type==2)
	{
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_right=get_graph_node(test,'r');
	if(type_right!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	pathfinder(test->right,'r');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	if(test->type==3)
	{
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_up=get_graph_node(test,'u');
	if(type_up!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	pathfinder(test->up,'u');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	if(test->type==4)
	{
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_down=get_graph_node(test,'d');
	if(type_down!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	pathfinder(test->down,'d');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	if(test->type==5 || test->type==7 || test->type==8)
	{	
	//if(test->parent->row==((test->row)-1) || get_node((test->row)-1,test->col)!=NULL && get_node((test->row)-1,test->col)->down!=NULL)
	if(dir=='d')
	{
	//move down
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_down=get_graph_node(test,'d');
	if(type_down!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->down->type,test->down->row,test->down->col);
	pathfinder(test->down,'d');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	//if(test->parent->row==((test->row)+1) || get_node((test->row)+1,test->col)!=NULL && get_node((test->row)+1,test->col)->up!=NULL)
	if(dir=='u')
	{
	//move up
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_up=get_graph_node(test,'u');
	if(type_up!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	pathfinder(test->up,'u');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	//if(test->parent->col==((test->col)+1) || get_node(test->row,(test->col+1)) && get_node(test->row,(test->col+1))->left!=NULL)
	if(dir=='l')
	{
	//move left
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_left=get_graph_node(test,'l');
	if(type_left!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	pathfinder(test->left,'l');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}

	}
	}
	//if(((test->parent->col)+1)==(test->col) || get_node((test->row),(test->col)-1) && get_node((test->row),(test->col)-1)->right!=NULL)
	if(dir=='r')
	{
	//move right
	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_right=get_graph_node(test,'r');
	if(type_right!=NULL)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	pathfinder(test->right,'r');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	}
	}
	}

	if(test->type==6)
	{

	stack_push(test,false,NULL);
	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);

	node* type_right=get_graph_node(test,'r');
	//left
	node* type_left=get_graph_node(test,'l');
	//up
	node* type_up=get_graph_node(test,'u');
	//down
	node* type_down=get_graph_node(test,'d');

	int upflag=0;
	int downflag=0;
	int rightflag=0;
	int leftflag=0;

	int up_taken=0;
	int down_taken=0;
	int left_taken=0;
	int right_taken=0;
	if(dir=='d')
	{
	//cannot move up
	upflag=1;
	}
	if(dir=='u')
	{
	//cannot move d7own
	downflag=1;
	}
	if(dir=='l')
	{
	//cannot move right
	rightflag=1;
	}
	if(dir=='r')
	{
	//cannot move left
	leftflag=1;
	}

	if(type_right!=NULL && rightflag==0)
	{
	if(right_taken==0 && left_taken==0 && up_taken==0 && down_taken==0)right_taken=1;
	else
	{
		//store
		stack_push(test->right,true,'r');
	}
	}
	if(type_left!=NULL && leftflag==0)
	{
	if(right_taken==0 && left_taken==0 && up_taken==0 && down_taken==0)left_taken=1;
	else
	{
		//store
		stack_push(test->left,true,'l');
	}
	}
	if(type_up!=NULL && upflag==0)
	{
		printf("uptaken\n");
		print_stack();
	if(right_taken==0 && left_taken==0 && up_taken==0 && down_taken==0)up_taken=1;
	else
	{
		//store
		stack_push(test->up,true,'u');
	}
	}
	if(type_down!=NULL && downflag==0)
	{
	if(right_taken==0 && left_taken==0 && up_taken==0 && down_taken==0)down_taken=1;
	else
	{
		//store
		stack_push(test->down,true,'d');
	}
	}

		if(right_taken==1)
		{
			pathfinder(test->right,'r');
		}
		else if(left_taken==1)
		{
			pathfinder(test->left,'l');
		}
		else if(up_taken==1)
		{
			pathfinder(test->up,'u');
		}
		else if(down_taken==1)
		{
			pathfinder(test->down,'d');
		}
		else
		{
			//dead end 
			//backtrack//
		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}

	}
		}
	if(test->type==9)
	{

	int up_taken=0;
	int left_taken=0;
	int right_taken=0;

	stack_push(test,false,NULL);

	printf("Pushed %d(%d,%d)\n",test->type,test->row,test->col);
	node* type_left=get_graph_node(test,'l');
	node* type_right=get_graph_node(test,'r');
	node* type_up=get_graph_node(test,'u');

	if(type_right!=NULL)
	{
	if(right_taken==0 && left_taken==0 && up_taken==0)right_taken=1;
	else
	{
		//store
		stack_push(test->right,true,'r');
		print_stack();
	}
	}
	if(type_left!=NULL)
	{
	if(right_taken==0 && left_taken==0 && up_taken==0)left_taken=1;
	else
	{
		//store
		stack_push(test->left,true,'l');
		print_stack();
	}
	}
	if(type_up!=NULL)
	{
		printf("uptaken\n");
		print_stack();
	if(right_taken==0 && left_taken==0 && up_taken==0)up_taken=1;
	else
	{
		//store
		stack_push(test->up,true,'u');
		print_stack();
	}
	}
	if(left_taken==1)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->left->type,test->left->row,test->left->col);
	pathfinder(test->left,'l');
	}
	else if(right_taken==1)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->right->type,test->right->row,test->right->col);
	pathfinder(test->right,'r');
	}
	else if(up_taken==1)
	{
	//printf(explored[test->row][(test->col)-1]?"true":"false");
	printf("Moving from %d(%d,%d)->%d(%d,%d)\n",test->type,test->row,test->col,test->up->type,test->up->row,test->up->col);
	pathfinder(test->up,'u');
	}
	else
	{
		//no further to travel as this is a dead end
		//check for prev branch nodes on stack 
		//if not found do nothing

		stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}

	}
	//test->explored=true;
	//explored[(test->row)-1][(test->col)-1]=true;
	//printf("explored %d(%d,%d)\n",test->type,test->row,test->col);
	//printf(explored[(test->row)-1][(test->col)-1]?"true":"false");
	//printf("\n");
	}
if(test->type==10)
	{
	stack_node* curr=stack_push(test,false,NULL);
	printf("exploration reached end\n");
	
	printf("<<<");
	while(curr->bottom!=NULL)
	{
		if(curr->branch!=true)
		{
		printf("%d(%d,%d),",curr->identity->type,curr->identity->row,curr->identity->col);
		path[paths][cur_path_node_count[paths]][0]=curr->identity->row;
		path[paths][cur_path_node_count[paths]][1]=curr->identity->col;
		cur_path_node_count[paths]++;
		}
		curr=curr->bottom;
	}
	printf("%d(%d,%d),",curr->identity->type,curr->identity->row,curr->identity->col);
	path[paths][cur_path_node_count[paths]][0]=curr->identity->row;
		path[paths][cur_path_node_count[paths]][1]=curr->identity->col;
		cur_path_node_count[paths]++;
	printf(">>>\n");
	paths++;
	print_stack();
	stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
}
	if(test->type==11)
	{
	stack_push(test,false,NULL);
	print_stack();
	printf("red reached\n");
	stack_node* new_tos=stack_pop();
	print_stack();
	printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch,new_tos->branch_dir);
	while(new_tos->branch!=true && new_tos->bottom!=NULL)
		{
			printf("%d(%d,%d),%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);
			print_stack();
			new_tos=stack_pop();
		}
		if(new_tos->bottom==NULL)
		{
			printf("Reached root node\n");
		}
		else if(new_tos->branch==true){printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);stack_pop();print_stack();printf("%d(%d,%d),xy%c",new_tos->identity->type,new_tos->identity->row,new_tos->identity->col,new_tos->branch_dir);pathfinder(new_tos->identity,new_tos->branch_dir);}
	//dead end
	}

	//test->explored=true;
	//explored[(test->row)-1][(test->col)-1]=true;
	//printf("explored %d(%d,%d)\n",test->type,test->row,test->col);
	//printf(explored[(test->row)-1][(test->col)-1]?"true":"false");
	//printf("\n");
}

void populate_matrices()
{
	for(int foo=0;foo<cur_node_count;foo++)
	{
	matrix_points[row[foo]-1][col[foo]-1].x=detected[foo][0];
	matrix_points[row[foo]-1][col[foo]-1].y=detected[foo][1];
	matrix_type[row[foo]-1][col[foo]-1]=detected[foo][2];
	printf("%d:%-2d,%-2d,%-2d:\n",foo,row[foo],col[foo],matrix_type[row[foo]-1][col[foo]-1]);
	}

	for(int i=0;i<row_count;i++)
	{
	for(int j=0;j<col_count;j++)
	{
	if(matrix_type[i][j]!=0)
	{
	printf("%-2d",matrix_type[i][j]);
	}
	else
	{
	printf("--");
	}
	}
	printf("\n");
	}
}

void arrange_columns()
{
	//allocate columns
	int avg=0;
	int sum=0;
	int current_col=1;
	int numofelems=0;
	int foo;
	int elem_count=0;
	for(foo=0;foo<cur_node_count-1;foo++)
	{
	sum=sum+detected[sorted_x[foo]][0];
	avg=(sum)/(numofelems+1);
	//printf("Avg is %d\n",avg);
	numofelems++;
	int diff_col=0;
	if(detected[sorted_x[foo+1]][0]>(avg+10))
	{
	for(int j=0;j<numofelems;j++)
	{
	col[sorted_x[foo-j]]=current_col;
	}
	//next column started;
	col_count++;
	current_col++;
	numofelems=0;
	avg=0;
	sum=0;
	if(foo==cur_node_count-2)
	{
	col[sorted_x[foo+1]]=current_col;
	diff_col=1;
	col_count++;
	}
	}
	
	if(foo==(cur_node_count-2) && diff_col==0)
	{
	for(int j=0;j<numofelems;j++)
	{
	col[sorted_x[foo-j]]=current_col;
	}
	//next column started;
	col_count++;
	current_col++;
	col[sorted_x[foo+1]]=current_col-1;
	}	
	elem_count++;
	}
	/*for(int j=0;j<numofelems;j++)
	{
	col[sorted_x[foo-j]]=current_col;
	}*/
	for(int foo=0;foo<cur_node_count;foo++)
	{
	printf("%d(%d,%d) lies in column %d\n",detected[foo][2],detected[foo][0],detected[foo][1],col[foo]);
	
	}
	printf("No of columns detected:%d.No of elems:%d\n",col_count,elem_count);
}

void arrange_rows()
{
	//allocate columns
	int avg=0;
	int sum=0;
	int current_col=1;
	int numofelems=0;
	int foo;
	int elem_count=0;
	for(foo=0;foo<cur_node_count-1;foo++)
	{
	printf("foo:%d,%d,%d",foo,cur_node_count,detected[sorted_y[foo]][1]);
	sum=sum+detected[sorted_y[foo]][1];
	avg=(sum)/(numofelems+1);
	printf("Avg is %d\n",avg);
	numofelems++;
	int diff_col=0;
	if(detected[sorted_y[foo+1]][1]>(avg+20))
	{
	for(int j=0;j<numofelems;j++)
	{
	row[sorted_y[foo-j]]=current_col;
	}
	//next column started;
	row_count++;
	current_col++;
	numofelems=0;
	avg=0;
	sum=0;

	if(foo==cur_node_count-2)
	{
	row[sorted_y[foo+1]]=current_col;
	diff_col=1;
	row_count++;
	}

	}
	
	if(foo==(cur_node_count-2) && diff_col==0)
	{
	for(int j=0;j<numofelems;j++)
	{
	row[sorted_y[foo-j]]=current_col;
	}
	//next column started;
	row_count++;
	current_col++;
	row[sorted_y[foo+1]]=current_col-1;
	//printf("%d\n",row[sorted_y[foo+1]]);
	}
	elem_count++;
	}
	/*for(int j=0;j<numofelems;j++)
	{
	col[sorted_x[foo-j]]=current_col;
	}*/
	for(int foo=0;foo<cur_node_count;foo++)
	{
	printf("%d(%d,%d) lies in row %d\n",detected[foo][2],detected[foo][0],detected[foo][1],row[foo]);
	}
	printf("No of rows detected:%d\nElement count%d;",row_count,elem_count);
}

void sort_detected_x()
{
	for(int k=0;k<cur_node_count;k++)
	{
	sorted_x[k]=k;
	}
	/*for(int k=0;k<cur_node_count;k++)
	{
	printf("%d ",sorted_x[k]);
	}*/
	printf("\n");
	int temp=0;
	for(int i=0;i<(cur_node_count-1);i++)
	{
	for(int j=0;j<(cur_node_count-i-1);j++)
	{
	if(detected[sorted_x[j]][0]>detected[sorted_x[j+1]][0])
	{
	//printf("%d->%d\n",detected[j][0],detected[j+1][0]);
	temp=sorted_x[j];
	sorted_x[j]=sorted_x[j+1];
	sorted_x[j+1]=temp;
	}
	}
	}

	/*for(int k=0;k<cur_node_count;k++)
	{
	printf("%d ",sorted_x[k]);
	}*/

	printf("\n");
	printf("Sorted by x\n");
	printf("--------------------\n");
 printf("no. type x_val y_val\n");
 printf("--------------------\n");
 for(int foo=0;foo<cur_node_count;foo++)
 {
	printf("%-4d%-5d%-6d%-5d\n",(sorted_x[foo]+1),detected[sorted_x[foo]][2],detected[sorted_x[foo]][0],detected[sorted_x[foo]][1]);
 }

}

void sort_detected_y()
{
	for(int k=0;k<cur_node_count;k++)
	{
	sorted_y[k]=k;
	}
	/*for(int k=0;k<cur_node_count;k++)
	{
	printf("%d ",sorted_x[k]);
	}*/
	printf("\n");
	int temp=0;
	for(int i=0;i<(cur_node_count-1);i++)
	{
	for(int j=0;j<(cur_node_count-i-1);j++)
	{
	if(detected[sorted_y[j]][1]>detected[sorted_y[j+1]][1])
	{
	//printf("%d->%d\n",detected[j][0],detected[j+1][0]);
	temp=sorted_y[j];
	sorted_y[j]=sorted_y[j+1];
	sorted_y[j+1]=temp;
	}
	}
	}

	/*for(int k=0;k<cur_node_count;k++)
	{
	printf("%d ",sorted_x[k]);
	}*/

	printf("\n");
	printf("Sorted by y\n");
	printf("--------------------\n");
 printf("no. type x_val y_val\n");
 printf("--------------------\n");
 for(int foo=0;foo<cur_node_count;foo++)
 {
	printf("%-4d%-5d%-6d%-5d\n",(sorted_y[foo]+1),detected[sorted_y[foo]][2],detected[sorted_y[foo]][0],detected[sorted_y[foo]][1]);
 }
}
char* find_tri_dir(CvPoint a,CvPoint b,CvPoint c)
   {
	   
	   int offset=3;
	   char dir[5];
	  //check if any of the two points have same x or y
	if(b.x<=a.x+offset && b.x>=a.x-offset)
	{
	if(c.x > a.x)
	{
	strcpy(dir,"r");
	}
	else
	{
	strcpy(dir,"l");
	}

	}
	else if(b.x<=c.x+offset && b.x>=c.x-offset)
	{
	if(a.x > b.x)
	{
	strcpy(dir,"r");
	}
	else
	{
	strcpy(dir,"l");
	}

	}
	else if(c.x<=a.x+offset && c.x>=a.x-offset)
	{
	if(b.x > a.x)
	{
	strcpy(dir,"r");
	}
	else
	{
	strcpy(dir,"l");
	}

	}
	if(height-b.y<=(height-a.y)+offset && height-b.y>=(height-a.y)-offset)
	{
	if(height-c.y > height-a.y)
	{
	strcpy(dir,"u");
	}
	else
	{
	strcpy(dir,"d");
	}
	}
	else if(height-b.y<=(height-c.y)+offset && height-b.y>=(height-c.y)-offset)
	{
	if(height-a.y > height-b.y)
	{
	strcpy(dir,"u");
	}
	else
	{
	strcpy(dir,"d");
	}
	}
	else if(height-c.y<=(height-a.y)+offset && height-c.y>=(height-a.y)-offset)
	{
	if(height-b.y > height-a.y)
	{
	strcpy(dir,"u");
	}
	else
	{
	strcpy(dir,"d");
	}
	}
	printf("%s\n",dir);
	return dir;
	

   }
void construct_matrix()
{
	Mat img=imread("C:/Users/Jay/Documents/test.png");
	CvSeq* contour;  //hold the pointer to a contour
	CvSeq* result;   //hold sequence of points of a contour
	CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
	//add case 1 elements to matrix
	 cvFindContours(c1, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
  
 //iterating through each contour
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);      
  //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
  if(result->total==3 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[3];
   for(int i=0;i<3;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
	//printf("Mid for :%d,%d\n",mid.x,mid.y);
	char dir[5];
	strcpy(dir,find_tri_dir(*pt[0],*pt[1],*pt[2]));
	if(strcmp(dir,"l")==0)
	{
	detected[cur_node_count][2]=1;
	}
	if(strcmp(dir,"r")==0)
	{
	detected[cur_node_count][2]=2;
	}
	if(strcmp(dir,"u")==0)
	{
	detected[cur_node_count][2]=3;
	}
	if(strcmp(dir,"d")==0)
	{
	detected[cur_node_count][2]=4;
	}
	detected[cur_node_count][0]=(pt[0]->x+pt[1]->x+pt[2]->x)/3;
	detected[cur_node_count][1]=(pt[0]->y+pt[1]->y+pt[2]->y)/3;
	//fontFace font;
	//cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
	CvPoint center;
	center.x=(pt[0]->x+pt[1]->x+pt[2]->x)/3;
	center.y=(pt[0]->y+pt[1]->y+pt[2]->y)/3;
	cur_node_count++;
	 circle(img,center,1,Scalar(0,0,255),-1,8,0);
	putText(img,dir,center,1,1,Scalar(255,255,255));
 } 
   if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }


   CvPoint center;

   int max_x=pt[0]->x;
   int min_x=pt[0]->x;
   int max_y=pt[0]->y;
   int min_y=pt[0]->y;
   for(int i=0;i<4;i++)
   {
	   if(pt[i]->x>max_x)
	   {
	   max_x=pt[i]->x;
	   }
	   if(pt[i]->y>max_y)
	   {
	   max_y=pt[i]->y;
	   }
	   if(pt[i]->x<min_x)
	   {
	   min_x=pt[i]->x;
	   }
	   if(pt[i]->y<min_y)
	   {
	   min_y=pt[i]->y;
	   }
   }
   center.x=(min_x+max_x)/2;
   center.y=(min_y+max_y)/2;
   Vec3b color = img.at<Vec3b>(center);
  /* if(color[0]>50)
   {
	   detected[cur_node_count][2]=8;
	    circle(img,center,1,Scalar(0,5,120),-1,8,0);
   putText(img,"b",center,1,1,Scalar(255,255,255));
   }
   else
   {*/
	   detected[cur_node_count][2]=5;
	    circle(img,center,1,Scalar(0,255,0),-1,8,0);
   putText(img,"f",center,1,1,Scalar(255,255,255));
   //}
   detected[cur_node_count][0]=center.x;
	detected[cur_node_count][1]=center.y;
	cur_node_count++;
  
   }

   if(result->total==12 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[12];
   for(int i=0;i<12;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   CvPoint center;

   int max_x=pt[0]->x;
   int min_x=pt[0]->x;
   int max_y=pt[0]->y;
   int min_y=pt[0]->y;
   for(int i=0;i<12;i++)
   {
	   if(pt[i]->x>max_x)
	   {
	   max_x=pt[i]->x;
	   }
	   if(pt[i]->y>max_y)
	   {
	   max_y=pt[i]->y;
	   }
	   if(pt[i]->x<min_x)
	   {
	   min_x=pt[i]->x;
	   }
	   if(pt[i]->y<min_y)
	   {
	   min_y=pt[i]->y;
	   }
   }
   center.x=(min_x+max_x)/2;
   center.y=(min_y+max_y)/2;
   detected[cur_node_count][0]=center.x;
	detected[cur_node_count][1]=center.y;
	detected[cur_node_count][2]=6;
	cur_node_count++;
   circle(img,center,1,Scalar(0,255,0),-1,8,0);
   putText(img,"c",center,1,1,Scalar(255,255,255));
   }
  //obtain the next contour
  contour = contour->h_next; 
 }
 cvFindContours(c2, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);      
  //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
   if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>120)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }


   CvPoint center;

   int max_x=pt[0]->x;
   int min_x=pt[0]->x;
   int max_y=pt[0]->y;
   int min_y=pt[0]->y;
   for(int i=0;i<4;i++)
   {
	   if(pt[i]->x>max_x)
	   {
	   max_x=pt[i]->x;
	   }
	   if(pt[i]->y>max_y)
	   {
	   max_y=pt[i]->y;
	   }
	   if(pt[i]->x<min_x)
	   {
	   min_x=pt[i]->x;
	   }
	   if(pt[i]->y<min_y)
	   {
	   min_y=pt[i]->y;
	   }
   }
   center.x=(min_x+max_x)/2;
   center.y=(min_y+max_y)/2;
   detected[cur_node_count][0]=center.x;
	detected[cur_node_count][1]=center.y;
	detected[cur_node_count][2]=7;
	cur_node_count++;
   circle(img,center,1,Scalar(100,255,0),-1,8,0);
   putText(img,"w",center,1,1,Scalar(0,0,0));
   }
  //obtain the next contour
  contour = contour->h_next; 
 }

 cvFindContours(cth, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);      
  //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
   if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }


   CvPoint center;

   int max_x=pt[0]->x;
   int min_x=pt[0]->x;
   int max_y=pt[0]->y;
   int min_y=pt[0]->y;
   for(int i=0;i<4;i++)
   {
	   if(pt[i]->x>max_x)
	   {
	   max_x=pt[i]->x;
	   }
	   if(pt[i]->y>max_y)
	   {
	   max_y=pt[i]->y;
	   }
	   if(pt[i]->x<min_x)
	   {
	   min_x=pt[i]->x;
	   }
	   if(pt[i]->y<min_y)
	   {
	   min_y=pt[i]->y;
	   }
   }
   center.x=(min_x+max_x)/2;
   center.y=(min_y+max_y)/2;
   detected[cur_node_count][0]=center.x;
	detected[cur_node_count][1]=center.y;
	detected[cur_node_count][2]=11;
	cur_node_count++;
   circle(img,center,1,Scalar(130,255,50),-1,8,0);
   putText(img,"r",center,1,1,Scalar(0,0,0));
   }
  //obtain the next contour
  contour = contour->h_next; 
 }

 cvInRangeS(cred, cvScalar(107, 169, 28), cvScalar(163, 255, 233),cthb);
	//cvShowImage("HsvT",cth);
 cvFindContours(cthb, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);      
  //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
   if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }


   CvPoint center;

   int max_x=pt[0]->x;
   int min_x=pt[0]->x;
   int max_y=pt[0]->y;
   int min_y=pt[0]->y;
   for(int i=0;i<4;i++)
   {
	   if(pt[i]->x>max_x)
	   {
	   max_x=pt[i]->x;
	   }
	   if(pt[i]->y>max_y)
	   {
	   max_y=pt[i]->y;
	   }
	   if(pt[i]->x<min_x)
	   {
	   min_x=pt[i]->x;
	   }
	   if(pt[i]->y<min_y)
	   {
	   min_y=pt[i]->y;
	   }
   }
   center.x=(min_x+max_x)/2;
   center.y=(min_y+max_y)/2;
   detected[cur_node_count][0]=center.x;
	detected[cur_node_count][1]=center.y;
	detected[cur_node_count][2]=8;
	cur_node_count++;
   circle(img,center,1,Scalar(130,255,50),-1,8,0);
   putText(img,"b",center,1,1,Scalar(0,0,0));
   }
  //obtain the next contour
  contour = contour->h_next; 
 }
 cvFindContours(cth2, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 CvPoint det[10];
 int det_count=0;
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);
 
  if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>10)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   CvPoint center;

   int max_x=pt[0]->x;
   int min_x=pt[0]->x;
   int max_y=pt[0]->y;
   int min_y=pt[0]->y;
   for(int i=0;i<4;i++)
   {
	   if(pt[i]->x>max_x)
	   {
	   max_x=pt[i]->x;
	   }
	   if(pt[i]->y>max_y)
	   {
	   max_y=pt[i]->y;
	   }
	   if(pt[i]->x<min_x)
	   {
	   min_x=pt[i]->x;
	   }
	   if(pt[i]->y<min_y)
	   {
	   min_y=pt[i]->y;
	   }
   }
   center.x=(min_x+max_x)/2;
   center.y=(min_y+max_y)/2;
   /*
   for(int i=0;i<4;i++){

   }
   CV_IMAGE_ELEM(img2, uchar,y, (x * 3) + 1);*/
   //drawing lines around the square
   cvLine(img4, *pt[0], *pt[1], cvScalar(100,100,0),4);
   cvLine(img4, *pt[1], *pt[2], cvScalar(100,100,0),4);
   cvLine(img4, *pt[2], *pt[3], cvScalar(100,100,0),4);
   cvLine(img4, *pt[3], *pt[0], cvScalar(100,100,0),4);
   det[det_count]=center;
   det_count++;
   }
   contour = contour->h_next; 
 }

 if(det_count==2)
 {
	 if(det[0].x>det[1].x)
	 {
	 //det[0]is center of end
	 detected[cur_node_count][0]=det[0].x;
	  detected[cur_node_count][1]=det[0].y;
	   detected[cur_node_count][2]=10;
	   cur_node_count++;
	   circle(img,det[0],1,Scalar(180,255,50),-1,8,0);
	putText(img,"END",det[0],1,1,Scalar(0,0,0));

	   detected[cur_node_count][0]=det[1].x;
	  detected[cur_node_count][1]=det[1].y;
	   detected[cur_node_count][2]=9;
	   cur_node_count++;
	    circle(img,det[1],1,Scalar(180,255,50),-1,8,0);
	putText(img,"START",det[1],1,1,Scalar(0,0,0));
	 }
	  if(det[0].x<det[1].x)
	 {
	 //det[0] is center of start
	  detected[cur_node_count][0]=det[0].x;
	  detected[cur_node_count][1]=det[0].y;
	   detected[cur_node_count][2]=9;
	   cur_node_count++;
	   circle(img,det[0],1,Scalar(180,255,50),-1,8,0);
	putText(img,"START",det[0],1,1,Scalar(0,0,0));

	   detected[cur_node_count][0]=det[1].x;
	  detected[cur_node_count][1]=det[1].y;
	   detected[cur_node_count][2]=10;
	   cur_node_count++;
	   circle(img,det[1],1,Scalar(180,255,50),-1,8,0);
	putText(img,"END",det[1],1,1,Scalar(0,0,0));
	 }
 }
/* //added start
 CvPoint start;
 start.x=textboxes[start_text].x+textboxes[start_text].width/2;
 detected[cur_node_count][0]=start.x;
 start.y=textboxes[start_text].y+textboxes[start_text].height/2;
 detected[cur_node_count][1]=start.y;
 detected[cur_node_count][2]=9;
 cur_node_count++;
 circle(img,start,1,Scalar(200,255,50),-1,8,0);
 putText(img,"start",start,1,1,Scalar(0,0,0));

 //added end
 CvPoint end;
 end.x=textboxes[end_text].x+textboxes[end_text].width/2;
 detected[cur_node_count][0]=end.x;
 end.y=textboxes[end_text].y+textboxes[end_text].height/2;
 detected[cur_node_count][1]=end.y;
 detected[cur_node_count][2]=10;
 cur_node_count++;
 circle(img,end,1,Scalar(200,255,50),-1,8,0);
 putText(img,"end",end,1,1,Scalar(0,0,0));*/

 cvReleaseMemStorage(&storage);
 printf("--------------------\n");
 printf("no. type x_val y_val\n");
 printf("--------------------\n");
 for(int foo=0;foo<cur_node_count;foo++)
 {
	printf("%-4d%-5d%-6d%-5d\n",(foo+1),detected[foo][2],detected[foo][0],detected[foo][1]);
 }
	cvNamedWindow("final11",CV_WINDOW_NORMAL);
  imshow("final11",img);
 
}
/*
char* ocr(Mat imgthr)
{
	TessBaseAPI tess; 
	tess.Init("","eng",OEM_DEFAULT);
	tess.SetImage((uchar*)imgthr.data, imgthr.size().width, imgthr.size().height, imgthr.channels(), imgthr.step1());
	tess.Recognize(0);
	char* out = tess.GetUTF8Text();
	return out;

}*/
void on_trackbar_text_1( int ,void * )
{
	//printf("text1%d\n",th_text_1);
	threshold(grayimg1,imgthr1,th_text_1,255,CV_THRESH_BINARY);
	imshow("text_1",imgthr1);
	/*if(strcmp(ocr(imgthr1),"START"))
	{
	printf("text1 is START\n");
	}
	else if(strcmp(ocr(imgthr1),"END"))
	{
	printf("text1 is END\n");
	}*/
	
}
void on_trackbar_text_2( int ,void * )
{
	//printf("text2%d\n",th_text_2);
	threshold(grayimg2,imgthr2,th_text_2,255,CV_THRESH_BINARY);
	imshow("text_2",imgthr2);
	/*if(strcmp(ocr(imgthr2),"START"))
	{
	printf("text2 is START\n");
	}
	else if(strcmp(ocr(imgthr2),"END"))
	{
	printf("text2 is END\n");
	}*/
}


 std::vector<cv::Rect> detectLetters(cv::Mat img)
{
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1); 
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        { 
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height) 
                boundRect.push_back(appRect);
        }
    return boundRect;
}

 void detect_start_stop()
 {
	
	Mat img =imread("C:/Users/Jay/Documents/test.png");
	//Read startstop
    //Detect
    /*std::vector<cv::Rect> letterBBoxes1=detectLetters(img);
    //Display
	int text_count=0;
    for(int i=0; i< letterBBoxes1.size(); i++)
	{
	int rect_x=letterBBoxes1[i].x-5;
	int rect_y=letterBBoxes1[i].y-5;
	int new_w=letterBBoxes1[i].width+10;
	int new_h=letterBBoxes1[i].height+10;

	letterBBoxes1[i].x=rect_x;
	letterBBoxes1[i].y=rect_y;
	letterBBoxes1[i].width=new_w;
	letterBBoxes1[i].height=new_h;

	textboxes[i]=letterBBoxes1[i];
       rectangle(img,letterBBoxes1[i],cv::Scalar(0,255,0),0,8,0);
	   Mat croppedRef(img,letterBBoxes1[i]);
	   Mat cropped;
	   croppedRef.copyTo(cropped);
	   //imshow("cropped",cropped);
	   vector<int>compression_params;
	   compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	   compression_params.push_back(9);
	  char name[50]="C:/Users/lenovo/Documents/text_seg";
	  char num[2];
	  strcat(name,itoa(i,num,10));
	  strcat(name,".png");
	   bool success=imwrite(name,cropped,compression_params);
	   if(success)
	   {
	   printf("%s saved\n",name);
	   }

	   text_count++;
	}
	if(text_count==2)
	{
	printf("echo there");
	char name1[50]="C:/Users/lenovo/Documents/text_seg0.png";
	char name2[50]="C:/Users/lenovo/Documents/text_seg1.png";

	Mat img11=imread(name1);
	Mat img22=imread(name2);
	
	cvtColor(img11,grayimg1,CV_BGR2GRAY);
	cvtColor(img22,grayimg2,CV_BGR2GRAY);

	cvNamedWindow("text_1",CV_WINDOW_NORMAL);
	cvNamedWindow("text_2",CV_WINDOW_NORMAL);

	createTrackbar("thr1", "text_1", &th_text_1, 255, on_trackbar_text_1);
	createTrackbar("thr2", "text_2", &th_text_2, 255, on_trackbar_text_2);

	threshold(grayimg1,imgthr1,th_text_1,255,CV_THRESH_BINARY);
	threshold(grayimg2,imgthr2,th_text_2,255,CV_THRESH_BINARY);
	on_trackbar_text_1(0,0);
	on_trackbar_text_2(0,0);
	/*char* out1=ocr(imgthr1);
	char* out2=ocr(imgthr2);

	if(strcmp(out1,"START")==0)
	{
	//i is start node
	}
	else if(strcmp(out1,"END")==0)
	{
	//i is start node
	}

	if(strcmp(out2,"START")==0)
	{
	//i is end node
	}
	else if(strcmp(out2,"END")==0)
	{
	//i is start node
	}

	}
	else
	{
	printf("Invalid no.(!=2) of text boxes detected\n");
	}
	cvNamedWindow("text",1);
	//cvShowImage("text",start_stop);
	imshow("text",img);*/
 }
void hsv_call(int,void*)
{
	img3=cvLoadImage("C:/Users/Jay/Documents/test.png");
	cvInRangeS(cred, cvScalar(iLowH, iLowS, iLowV), cvScalar(iHighH, iHighS, iHighV),cth);
	cvShowImage("HsvT",cth);

CvSeq* contour;  //hold the pointer to a contour
 CvSeq* result;   //hold sequence of points of a contour
 CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
 //finding all contours in the image
 cvFindContours(cth, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);
 
  if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>10)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   
   /*
   for(int i=0;i<4;i++){

   }
   CV_IMAGE_ELEM(img2, uchar,y, (x * 3) + 1);*/
   //drawing lines around the square
   cvLine(img3, *pt[0], *pt[1], cvScalar(100,100,0),4);
   cvLine(img3, *pt[1], *pt[2], cvScalar(100,100,0),4);
   cvLine(img3, *pt[2], *pt[3], cvScalar(100,100,0),4);
   cvLine(img3, *pt[3], *pt[0], cvScalar(100,100,0),4);
   }
   contour = contour->h_next; 
 }
 printf("here3");
  cvShowImage("Tracked C3",img3);
 cvReleaseMemStorage(&storage);

}

void hsv_call2(int,void*)
{
	img4=cvLoadImage("C:/Users/Jay/Documents/test.png");
	cvInRangeS(cred, cvScalar(iLowH2, iLowS2, iLowV2), cvScalar(iHighH2, iHighS2, iHighV2),cth2);
	cvShowImage("HsvT2",cth2);
CvSeq* contour;  //hold the pointer to a contour
 CvSeq* result;   //hold sequence of points of a contour
 CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
 //finding all contours in the image
 cvFindContours(cth2, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 CvPoint det[10];
 int det_count=0;
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);
 
  if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>10)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   /*
   for(int i=0;i<4;i++){

   }
   CV_IMAGE_ELEM(img2, uchar,y, (x * 3) + 1);*/
   //drawing lines around the square
   cvLine(img4, *pt[0], *pt[1], cvScalar(100,100,0),4);
   cvLine(img4, *pt[1], *pt[2], cvScalar(100,100,0),4);
   cvLine(img4, *pt[2], *pt[3], cvScalar(100,100,0),4);
   cvLine(img4, *pt[3], *pt[0], cvScalar(100,100,0),4);
   }
   contour = contour->h_next; 
 }
 
  cvShowImage("Tracked C4",img4);
 cvReleaseMemStorage(&storage);

}
void on_trackbar_c2( int, void* )
{
	printf("here");
	img2 =  cvLoadImage("C:/Users/Jay/Documents/test.png");
 cvThreshold(imgGrayScale,c2,thc2,255,CV_THRESH_BINARY);
 cvShowImage("Thresholded Image C2",c2);

  CvSeq* contour;  //hold the pointer to a contour
 CvSeq* result;   //hold sequence of points of a contour
 CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
 printf("here2");
 //finding all contours in the image
 cvFindContours(c2, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);
 
  if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>120)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   
   /*
   for(int i=0;i<4;i++){

   }
   CV_IMAGE_ELEM(img2, uchar,y, (x * 3) + 1);*/
   //drawing lines around the square
   cvLine(img2, *pt[0], *pt[1], cvScalar(0,100,0),4);
   cvLine(img2, *pt[1], *pt[2], cvScalar(0,100,0),4);
   cvLine(img2, *pt[2], *pt[3], cvScalar(0,100,0),4);
   cvLine(img2, *pt[3], *pt[0], cvScalar(0,100,0),4);
   }
   contour = contour->h_next; 
 }
 printf("here3");
  cvShowImage("Tracked C2",img2);

 cvReleaseMemStorage(&storage);
}

 void on_trackbar_c1( int, void* )
 {
img =  cvLoadImage("C:/Users/Jay/Documents/test.png");
 cvThreshold(imgGrayScale,c1,thc1,255,CV_THRESH_BINARY_INV);
 cvShowImage("Thresholded Image C1",c1);

 CvSeq* contour;  //hold the pointer to a contour
 CvSeq* result;   //hold sequence of points of a contour
 CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours
 
 //finding all contours in the image
 cvFindContours(c1, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
  
 //iterating through each contour
 while(contour)
 {
  //obtain a sequence of points of the countour, pointed by the variable 'countour'
  result = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contour)*0.03, 0);
           
  //if there are 3 vertices  in the contour and the area of the triangle is more than 100 pixels
  if(result->total==3 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[3];
   for(int i=0;i<3;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   
   //drawing lines around the triangle
   cvLine(img, *pt[0], *pt[1], cvScalar(255,0,0),4);
   cvLine(img, *pt[1], *pt[2], cvScalar(255,0,0),4);
   cvLine(img, *pt[2], *pt[0], cvScalar(255,0,0),4);
   CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
	CvPoint mid; 
	mid.x=(pt[0]->x+pt[1]->x+pt[2]->x)/3;
	mid.y=(pt[0]->y+pt[1]->y+pt[2]->y)/3;
	printf("Mid for :%d,%d\n",mid.x,mid.y);
	printf("%s\n",find_tri_dir(*pt[0],*pt[1],*pt[2]));
	char s[5];
	strcpy(s,find_tri_dir(*pt[0],*pt[1],*pt[2]));
   cvPutText(img,s, mid,&font,cvScalar(255,255,255));
  } 
   if(result->total==4 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[4];
   for(int i=0;i<4;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   
   /*
   for(int i=0;i<4;i++){

   }
   CV_IMAGE_ELEM(img2, uchar,y, (x * 3) + 1);*/
   //drawing lines around the square
   cvLine(img, *pt[0], *pt[1], cvScalar(0,100,0),4);
   cvLine(img, *pt[1], *pt[2], cvScalar(0,100,0),4);
   cvLine(img, *pt[2], *pt[3], cvScalar(0,100,0),4);
   cvLine(img, *pt[3], *pt[0], cvScalar(0,100,0),4);
   }

   if(result->total==12 && fabs(cvContourArea(result, CV_WHOLE_SEQ))>20)
  {
   //iterating through each point
   CvPoint *pt[12];
   for(int i=0;i<12;i++){
    pt[i] = (CvPoint*)cvGetSeqElem(result, i);
   }
   
   /*
   for(int i=0;i<4;i++){

   }
   CV_IMAGE_ELEM(img2, uchar,y, (x * 3) + 1);*/
   //drawing lines around the square
   cvLine(img, *pt[0], *pt[1], cvScalar(0,200,0),4);
   cvLine(img, *pt[1], *pt[2], cvScalar(0,200,0),4);
   cvLine(img, *pt[2], *pt[3], cvScalar(0,200,0),4);
   cvLine(img, *pt[3], *pt[4], cvScalar(0,200,0),4);
   cvLine(img, *pt[4], *pt[5], cvScalar(0,200,0),4);
   cvLine(img, *pt[5], *pt[6], cvScalar(0,200,0),4);
   cvLine(img, *pt[6], *pt[7], cvScalar(0,200,0),4);
   cvLine(img, *pt[7], *pt[8], cvScalar(0,200,0),4);
   cvLine(img, *pt[8], *pt[9], cvScalar(0,200,0),4);
   cvLine(img, *pt[9], *pt[10], cvScalar(0,200,0),4);
   cvLine(img, *pt[10], *pt[11], cvScalar(0,200,0),4);
   cvLine(img, *pt[11], *pt[0], cvScalar(0,200,0),4);
   }
  //obtain the next contour
  contour = contour->h_next; 
 }

  //show the image in which identified shapes are marked   
 
 cvShowImage("Tracked C1",img);

 cvReleaseMemStorage(&storage);
 }

 void find_paths()
 {
	
	img =  cvLoadImage("C:/Users/Jay/Documents/test.png");
	height=img->height;
  img2=  cvLoadImage("C:/Users/Jay/Documents/test.png");
  img3= cvLoadImage("C:/Users/Jay/Documents/test.png");
  img4= cvLoadImage("C:/Users/Jay/Documents/test.png");
 //show the original image
 //cvNamedWindow("Original");
 //cvShowImage("Original",img);

 //smooth the original image using Gaussian kernel to remove noise
	cvSmooth(img, img, CV_GAUSSIAN,3,3);

  //converting the original image into grayscale
 imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1); 
 cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);
 
 //cred=cvCreateImage(cvGetSize(img), 8, 1); 
 //cvCvtColor(img, cred, COLOR_BGR2HSV); 
 //cvNamedWindow("GrayScale Image");
// cvShowImage("GrayScale Image",imgGrayScale);

 //after the grayscale image is found we need to adjust the threshold according to the shapes or objects.
 //1.threshold for all black and blue (dark)shapes: square,triangle and plus sign.
 //2.threshold for white square
 //3.threshold for red squares

	//On finding individual threshold images run 
	 //shape identification on each and output on a single result image.

  //thresholding for case 1

thc1=23;
cvNamedWindow("Thresholded Image C1");
createTrackbar( "case1", "Thresholded Image C1", &thc1, 255, on_trackbar_c1 );
c1=cvCreateImage(cvGetSize(img), 8, 1); ;
cvThreshold(imgGrayScale,c1,thc1,255,CV_THRESH_BINARY_INV);
cvShowImage("Thresholded Image C1",c1);
cvNamedWindow("Tracked C1",CV_WINDOW_NORMAL);
on_trackbar_c1(0,0);
 cvShowImage("Tracked C1",img);

thc2=157;
cvNamedWindow("Thresholded Image C2");
createTrackbar( "case2", "Thresholded Image C2", &thc2, 255, on_trackbar_c2 );
c2=cvCreateImage(cvGetSize(img2), 8, 1);
cvThreshold(imgGrayScale,c2,thc2,255,CV_THRESH_BINARY);
cvShowImage("Thresholded Image C2",c2);
cvNamedWindow("Tracked C2",1);
on_trackbar_c2(0,0);
 cvShowImage("Tracked C2",img2);
/*cvDestroyAllWindows(); 
 cvReleaseImage(&img);
 cvReleaseImage(&imgGrayScale);*/




  //Create trackbars in "Control" window
 
 cvNamedWindow("Case3",CV_WINDOW_NORMAL);
 
 createTrackbar("LowH", "Case3", &iLowH, 179,hsv_call); //Hue (0 - 179)
 createTrackbar("HighH", "Case3", &iHighH, 179,hsv_call);

 createTrackbar("LowS", "Case3", &iLowS, 255,hsv_call); //Saturation (0 - 255)
 createTrackbar("HighS", "Case3", &iHighS, 255,hsv_call);

 createTrackbar("LowV", "Case3", &iLowV, 255,hsv_call);//Value (0 - 255)
 createTrackbar("HighV", "Case3", &iHighV, 255,hsv_call);

	cred=cvCreateImage(cvGetSize(img3), 8, 3);
	
	cvCvtColor(img3,cred,COLOR_BGR2HSV);
	cvShowImage("Case3",cred);
	
	
	cth =cvCreateImage(cvGetSize(img3), 8, 1);
	cthb=cvCreateImage(cvGetSize(img3), 8, 1);
 	cvInRangeS(cred, cvScalar(iLowH, iLowS, iLowV), cvScalar(iHighH, iHighS, iHighV),cth);
	cvNamedWindow("HsvT",1);
	cvShowImage("HsvT",cth);
	cvNamedWindow("Tracked C3",1);
	hsv_call(0,0);

	//Yellow case
	 cvNamedWindow("Case4",CV_WINDOW_NORMAL);
 
 createTrackbar("LowH2", "Case4", &iLowH2, 179,hsv_call2); //Hue (0 - 179)
 createTrackbar("HighH2", "Case4", &iHighH2, 179,hsv_call2);

 createTrackbar("LowS2", "Case4", &iLowS2, 255,hsv_call2); //Saturation (0 - 255)
 createTrackbar("HighS2", "Case4", &iHighS2, 255,hsv_call2);

 createTrackbar("LowV2", "Case4", &iLowV2, 255,hsv_call2);//Value (0 - 255)
 createTrackbar("HighV2", "Case4", &iHighV2, 255,hsv_call2);

	cth2 =cvCreateImage(cvGetSize(img4), 8, 1);
 	cvInRangeS(cred, cvScalar(iLowH2, iLowS2, iLowV2), cvScalar(iHighH2, iHighS2, iHighV2),cth2);
	cvNamedWindow("HsvT2",1);
	cvShowImage("HsvT2",cth2);
	cvNamedWindow("Tracked C4",1);
	hsv_call2(0,0);
	detect_start_stop();

 }
int main()
{
	cvNamedWindow("command");
	while(1)
	{
	switch(waitKey(10))
	{
	case 120:
	{
	printf("Exit");
	return 0;
	}
	case 109:
	{
	printf("Starting matrix construction...");
	construct_matrix();
	break;
	}
	case 111:
	{
	/*printf("OCR Started...");
	char ocr1[10];
	strcpy(ocr1,ocr(imgthr1));
	char ocr2[10];
	strcpy(ocr2,ocr(imgthr2));
	string socr1(ocr1);
	string socr2(ocr2);
	size_t f1=socr1.find("\n");
	size_t f2=socr2.find("\n");
	string tsocr1=socr1.substr(0,f1);
	string tsocr2=socr2.substr(0,f2);
	
	if(tsocr1.compare("START")==0)
	{
	printf("Image 0 is START\n");
	start_text=0;
	}
	if(tsocr1.compare("END")==0)
	{
	printf("Image 0 is END\n");
	end_text=0;
	}
	if(tsocr2.compare("START")==0)
	{
	printf("Image 1 is START\n");
	start_text=1;
	}
	if(tsocr2.compare("END")==0)
	{
	printf("Image 1 is END\n");
	end_text=1;
	}
	//cout<<tsocr1<<endl;
	//cout<<tsocr2<<endl;
	//printf("Image 0 is :%s%d\n",tsocr1,f1);
	//printf("Image 1 is :%s%d\n",tsocr2,f2);*/
	break;
	}
	case 99:
	{
	printf("Sorting by x");
	sort_detected_x();
	sort_detected_y();
	arrange_rows();
	arrange_columns();
	populate_matrices();
	init_created();
	init_nodes();
	start_node=create_node(9,NULL,6,2);
	explore_node(start_node,'u');
	//printf("%d\n",gettfc(0,0));
	//printf("%d\n",gettfc(6,2));
	
	//tos=(struct stack_node*)malloc(sizeof(stack_node));
	//tos->identity=start_node;
	pathfinder(start_node,'u');
	printf("\n%d paths were found.",paths);
	print_paths();
	break;
	}
	case 97:
	{
		//start_navigation();
		find_paths();
		break;
	}
	case 115:
	{
		//run snapshot code
		Mat cameraFeed;
		VideoCapture capture;
		capture.open(4);
		cvNamedWindow("webcam",CV_WINDOW_AUTOSIZE);
	//set height and width of capture frame

	//capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
	//capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop
	while(2){
		//store image to matrix
		capture.read(cameraFeed);
		//convert frame from BGR to HSV colorspace
		imshow("webcam",cameraFeed);
		//delay 30ms so that screen can refresh.
		//image will not appear without this waitKey() command
		if(waitKey(30)==116)
		{
			imshow("captured",cameraFeed);
			vector<int>compression_params;
	   compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	   compression_params.push_back(9);
		char name[50]="C:/Users/Jay/Documents/test.png";
	   bool success=imwrite(name,cameraFeed,compression_params);
	   if(success)
	   {
		   printf("Image saved\n");
	   }
			break;
		}
	}
	}
	}
	}
	waitKey(0);
	return 0;
}

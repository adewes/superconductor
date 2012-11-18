#define INC_BUFFERS 1
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string.h>

using namespace std;

#ifdef FLUX_WINDOWS
#include<windows.h>
#else
#include<unistd.h>
#include<pthread.h>
#endif


#define BUFFER_IC_H 0
#define BUFFER_IVC 1
#define BUFFER_PHI 2
#define BUFFER_DPHI_DX 3
#define BUFFER_PHI_CRIT 4
#define BUFFER_DPHI_DX_CRIT 5
#define BUFFER_DPHI_DT 6
#define BUFFER_IC_SW 7
#define BUFFER_IC_BINS 8
#define BUFFER_FFT 9
#define BUFFER_ESC_RATE 10
#define BUFFER_IC_LOG 11
#define BUFFER_FI 12
#define BUFFER_ERROR -1

#define BUFFER_PHI_2D 12
#define BUFFER_DPHI_DX_2D 13
#define BUFFER_DPHI_DT_2D 14
#define BUFFER_ACT_ENERGY 15

#define BUFFER_LEFT_VOLTAGE 16


#define BUFFER_Q		20
#define BUFFER_DQ_DT	21
#define BUFFER_DQ_DX	22

#define BUFFER_PHONONS 23

#define PREDEF_BUFFERS 24


#define ELEMENT_EMPTY 1
#define ELEMENT_XY 0
#define ELEMENT_MULTIVAL 2


#define STYLE_HISTO 1
#define STYLE_LINE 2
#define STYLE_POINTS 4
#define TRAIN 10000

#define MAX_VALS 10

typedef struct r
{
  double left,right,bottom,top;
}rect;

typedef struct el
{
	int type;
	int n_values;
	double x,y,z;
	double values[MAX_VALS];
	void *ptr;
} element;


typedef struct buf
{
	int length;
	int alloc;
	int flush;
	int type;
	int changed;
	int last;
	int locked;
	int style;
	int series;
	element max,min;
	char *name;
	char *filename;
	char *description;
	element *elements;
} buffer;

typedef struct gr
{
	char *name;
	void *WINDOW;
	int has_window;
	int *buffers;
	int *changed;
	int n_buffers;
} GRAPH;

char *read_file(char *filename);
int load_buffer(char *filename,int buf);
GRAPH get_graph(int x);
int get_n_graphs();
void set_graph(GRAPH g,int x);

int wait_for_unlock(int b);

double buffer_max(int buffer,int n);
double buffer_min(int buffer,int n);
void flip_xy(int b,int x,int y);
void buffer_invert(int buffer,int n);

void buffer_erase_rect(int buffer,rect r);

void lock_buffer(int b);
void unlock_buffer(int b);
element get_element(int b,int x);

void set_name(int buffer,char *name);
void set_description(int buffer,char *description);

int set_value(int b,int n,int m,double val);
double get_value(int b,int n,int m);
int delete_element(int b,int n);
void free_buffer(buffer b);
buffer copy_buffer(int b);
double buffer_max(buffer buf,int n);
double buffer_min(buffer buf,int n);


void differentiate(int b,int x,int y,int t);
int buffer_length(int b);
double approx(int buffer,double x);
double approx(int buffer,double v,int i);
void set_style(int buffer,int style);
void plot_graph(int graph,char *filename);

void graph_add_buffer(int graph,int buffer);
int new_graph(char *name);
int buffer_empty(int b);

void buffer_add_value(int b,double *values,int n_values);
void change_all(void);
void init_buffer(int b);
int new_buffer();
int n_buffers();
int new_buffer(char *name);
int new_buffer(char *name,char *filename);
int write_buffer(char *filename,int buf);
void plot_buffer(int buffer,char *filename);
void plot_buffer_array(int *buf,int l,char *filename);
char *buffer_name(int buf);
int flush_state(int buf);
void set_flush(int buf,int f);
char *buffer_filename(int buf);
int num_buffers();
void buffer_new_series(int buf);
int buffer_changed(int b);
buffer get_buffer(int b);
void flush_buffer(int b);
void buffer_set_minmax(int b,double min,double max);
void buffer_add_value(int b,double x,double y);
void buffer_add_value(int b,double x,double y,double z);
void buffer_add_element(int b,element e);
void buffer_add_empty(int b);
buffer get_last_buffer(int b);

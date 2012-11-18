#include<math.h>
#include<iostream>
#include<time.h>
#ifndef VISUAL_CPP
#include<sys/time.h>
#else

#endif

#include<stdio.h>
#include<stdlib.h>
#ifndef INTERPRET_CLASS
#include"interpret_class.h"
#endif
#ifndef NR
#include"nr.h"
#endif

#define PI 3.141592654

//TH and THLIN give the thresholds for the sparse-matrix generation of A and A^{-1}. Usually 1e-5 to 1e-6 are reasonable values...

#define TH 1.0e-6
#define THLIN 1e-6

#ifndef INC_BUFFERS
#include"buffers.h"
#endif

#define TYPE_JJ 1
#define TYPE_R  2
#define TYPE_C  3
#define TYPE_L  4
#define TYPE_CS 6
#define TYPE_VS 7 
#define TYPE_CSG 8
#define TYPE_VSG 9
#define TYPE_PS 10
#define TYPE_PSG 11
#define TYPE_NC 12
#define TYPE_KAPPA_JJ 13

#define TYPE_CON 14

#define MONITOR_VOLTAGE 1
#define MONITOR_CURRENT 2
#define MONITOR_PHASE 3

#define SIG_SINE 1
#define SIG_SAWTOOTH 2
#define SIG_RECT 3
#define SIG_PULSE 4
#define SIG_INC 5

#define SOLVER_LU 1
#define SOLVER_BILIN 2
#define SOLVER_NEWTON 3

#define MAX_ERROR 1e-3

//Pulse generator takes the following arguments: Amplitude, frequency, pulse duration (in percentage of the ground time period)


typedef struct BR
{
	int from,to,via;
} BRANCH;

typedef struct ND
{
	int N;
	char *NAME;
	char *BLOCK;
	int n_elements;
	int marked;
	int elements[1000];
} NODE;

typedef struct MT
{
	int type;
	int element;
	int buffer;
	int NODE_1,NODE_2;
} MONITOR;

typedef struct MS
{
	int N;
	int n_elements;
	int first;
	int broken;
	int elements[1000];
} MESH;

typedef struct EL
{
  int TYPE;
  char NAME[256];
  void *MODEL;
  int marked;
  char *BLOCK;
  char *SBLOCK;
  int NODE_1,NODE_2,NODE_3,NODE_4;
  char *name;
  double R,I,Ic,alpha,L,C,V,Phi,theta,kappa;
  double A,f,off;//Variables for the signal generator
  int SIG;//Signal type to generate
} ELEMENT;


typedef struct crct
{
   double a;
   ELEMENT *elements;
   NODE *nodes;
   NODE *node_names;
   int n_node_names;
   MESH *meshes;
   double STEPT;
   BRANCH *master,*opt;
   int n_master,n_opt;
   int n_block_stack;
   char **block_stack;
   MONITOR *MONITORS;
   int n_monitors;
   double **fjac;
   double *sparse_vec;
   double **mesh_matrix,**node_matrix,**voltage_matrix,**current_matrix;
   double **inv_voltage_matrix,**inv_current_matrix;
   int n_node_elements,n_mesh_elements;
   double *inverse_vec;
   unsigned long *sparse_ind;
   unsigned long *inverse_ind;
   int sparse_n;
   interpreter *INT;
   double t;//The time index.
   double *vec,*vec_save;
   int *indx;
   double IVPERIOD;
   double IVAMP,ICAMP;
   double INTT;
   int IVSTEPS,ICSTEPS;
   int **map;
   int SOLVER;
   double **lin,**lin_save,**inverse,**lin_diff,**inverse_2;
   double *solution;
   double *phi,*phi_p,*phi_pp;//The phases over the circuit elements.
   double *I,*V;//the (I,V)-solution vectors
   double *I_p,*V_p;//the past solution vectors
   double *I_pp,*V_pp;//the pre-past solution vectors
   double *I_int,*V_int;//The time integrals of the solution vectors (needed for nonlinear elements).
   double **JJ;//the jacobian matrix of the (initial) solution
   int n_elements;
   int n_nodes;
   char *BLOCK;
   int n_meshes;
}CIRCUIT;


typedef struct SE
{
	double *phi,*phi_p,*phi_pp;//The phases over the circuit elements.
	double *I,*V;//the (I,V)-solution vectors
	double *I_p,*V_p;//the past solution vectors
	double *I_pp,*V_pp;//the pre-past solution vectors
	double *I_int,*V_int;//The time integrals of the solution vectors (needed for nonlinear elements).
} STACK_ELEMENT;

void add_CON(int NODE_1,char *BLOCK1,int NODE_2,char *BLOCK2);
variable add_CON(opts o,interpreter *i);
void update_linear_matrix(double **lin,double *vec);
void CIRC_check_phases(void);
double SG(ELEMENT e);
void set_block(char *block);
int find_element(char *element,char *block);
int find_element(char *element);
double error_func();
int generate_linear_matrix(double **lin,double *vec,double set_lin);
void build_trees();
void init_solution();
int new_mesh();
int check_for_node(int N,char *block);
int new_node(char *name,char *block);
void print_node_tree();
void build_node_tree();
void CIRC_solve();
void CIRC_evolve();
double JJ(int x,int y);
double linear_coeff(int element,int var,double lin);
double mesh_func(int mesh);
void CIRC_delete_element(int N);
void CIRC_delete_node(int N);
void eliminate_connections(void);
void build_flat_node_tree(void);
double node_func(int node);
void build_mesh_tree(void);
void print_mesh_tree();
void generate_jacobian(void);
void CIRC_run(double T);
double CIRC_gnd_voltage(int node);
double CIRC_gnd_phase(int node);
double CIRC_gnd_path(int node,double *array);
int CIRC_add_monitor(int type,int element);
int CIRC_add_monitor(int type,int NODE_1,int NODE_2);
int CIRC_get_node(int node,char *block);
int CIRC_get_node(char *name,char *block);
variable node_name(opts o,interpreter *i);
void node_name(int NODE,char *block,char *name);
/*Following the function declarations for the interpreter interface*/
variable build_trees(opts o,interpreter *i);
variable CIRC_add_monitor(opts o,interpreter *i);
variable build_node_tree(opts o,interpreter *i);
variable CIRC_solve(opts o,interpreter *i);
variable CIRC_evolve(opts o,interpreter *i);
variable initialize(opts o,interpreter *i);
variable CIRC_run(opts o,interpreter *i);
variable CIRC_IV(opts o,interpreter *i);
variable CIRC_IC(opts o,interpreter *i);

ELEMENT build_element_nodes(element e);

double VSG(ELEMENT e);
double CSG(ELEMENT e);

void pop_block(void);
void push_block(void);

variable push_block(opts o,interpreter *i);
variable pop_block(opts o,interpreter *i);

void generate_jacobian(double **fjac);
void init_jacobian(double **fjac);

variable add_element(opts o,interpreter *i);
char *type_name(int type);
double func(int row);

void usrfun(double *vec,int n,double *fvec,double **fjac);
int get_node_N(char *name,char *block);

double element_func(int element);
void add_CSG(char *name,int NODE_1,int NODE_2,double A,double f,double off,int SIG);
void add_VSG(char *name,int NODE_1,int NODE_2,double A,double f,double off,int SIG);
void add_C(char *name,int NODE_1,int NODE_2,double C);
void add_L(char *name,int NODE_1,int NODE_2,double L);
void add_R(char *name,int NODE_1,int NODE_2,double R);
void add_JJ(char *name,int NODE_1,int NODE_2,double Ic,double alpha);
void add_CS(char *name,int NODE_1,int NODE_2,double I);
void add_VS(char *name,int NODE_1,int NODE_2,double V);
void add_PS(char *name,int NODE_1,int NODE_2,double Phi);
void add_PSG(char *name,int NODE_1,int NODE_2,double A,double f,double off,int SIG);
void add_NC(char *name,int NODE_1,int NODE_2,double I);
void add_kappa_JJ(char *name,int NODE_1,int NODE_2,double Ic,double alpha,double kappa);

void IV(char *element,char *source,int buf);
void IC(char *element,char *source,char *current,int buf);
double noise(double theta);

double CIRC_get_V(int element);
double CIRC_get_Ic(int el,int cu);


double *CIRC_STEPT_ptr();

void add_element(ELEMENT e);
void init_circuit_funcs(interpreter *i);
void initialize(void);

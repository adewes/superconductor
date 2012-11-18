#include<iostream>
#include<fstream>
#include<stdio.h>
#include<math.h>

#include <string>

using namespace std;

#ifndef VISUAL_CPP
#include<time.h>
#endif

#include<signal.h>
#include<sys/types.h>
#include<malloc.h>

#define INTERPRET_CLASS 1

#define INTSIZE 4
#define doubleSIZE 8

#ifdef FLUX_WINDOWS
#include <windows.h>
#include <process.h>
#else
#include<pthread.h>
#endif

#define OP_ADD 1
#define OP_SUB 2
#define OP_MUL 3
#define OP_DIV 4

#define OPT_NUM 1
#define OPT_STRING 2

#define VAR_ERROR -1
#define VAR_DOUBLE 1
#define VAR_INT 2
#define VAR_STRING 4
#define VAR_CHAR 8
#define VAR_TRUTH 16

#define INTERPRET 1

#define THREAD_KILLED 1
#define THREAD_RUNNING 2
#define THREAD_FINISHED 3
#define THREAD_SUSPENDED 4

#define FUNC_ERROR -1
#define FUNC_CPP 1 
#define FUNC_REAL 2

class interpreter;

typedef struct v
{
    int type;
    int length;
    int freed;
    int constant;
    int tied;
    int compartment;
    void *ptr;
    char *name;
} variable;

typedef struct opt_type
{
    int size;
    char *calling_func;
    variable *options;
} opts;

typedef struct fd
{
    int error;
    int type;
    char *definition;
    char **options;
    int olen;
    variable (*func)(opts,interpreter *i);
    char *name;
} func_d;

#define EX_FUNCTION 0
#define EX_EVAL 1

typedef struct ex
{
    int done;
    opts o;
    func_d f;
    int thread_number;
    int status;
	int action;
#ifdef FLUX_WINDOWS
    HANDLE thread;
#else
    pthread_t thread;
#endif    
    variable r;
	char *eval_str;
	int multithread; 
    interpreter *i;
} exopt;

typedef struct com
{
    int ID;
    int parent;    
}compartment;

void *execute(void * p);
void print(char *);
void handler(int sig);
void new_flux_handler(int sig);

class interpreter
{
public:
	void flush_output(void);
	char *get_output(void);
	void reset_handler(void);
	void add_to_output(const char *str,int len);
	ofstream *os;
	void operator<<(int i);
	char *output_all_variables(void);
	int get_thread_number(void);
	int register_thread(ex p);
	int finish_thread(int n);
	void print_rv(void);
	int kill_thread(int n);
	int kill_thread(char *n);
	int suspend_thread(int n);
	int registered_thread(ex p);
	int suspend_thread(char *n);
	char *get_string(variable a);
	int revive_thread(int n);
	int revive_thread(char *n);
	void operator<<(double d);
	void new_thread(exopt *ex,int wait);
	void operator<<(const char *str);
	interpreter();
	~interpreter();
	char *result(int *len);		
	int cast_string(char **ptr,opts o,int x);
	variable parse_assignment();
	void *my_malloc(int size,char *purpose);
	void *my_realloc(void *ptr,int size,char *purpose);
	void my_free(void *ptr,char *purpose);
	int cast_double(double *d,opts o,int x);
	int cast_int(int *i,opts o,int x);
	int cast_double(double *d,variable a);
	int cast_int(int *i,variable a);
	func_d parse_function();
	int condense(variable *a,variable *b);
	variable add_string(variable a,variable b);
	variable div_int(variable a,variable b);
	variable div_double(variable a,variable b);
	variable mul_int(variable a,variable b);
	variable mul_double(variable a,variable b);
	variable sub_int(variable a,variable b);
	variable sub_double(variable a,variable b);
	variable add_int(variable a,variable b);
	variable add_double(variable a,variable b);
	int to_int(variable *a);
	int to_string(variable *a);
	int to_double(variable *a);
	double eval_double(variable v);
	char *load_file(char *filename);
	void interpret_file(char *filename);
	variable get_variable(char *name);
	variable *get_variable_ptr(char *name);
	compartment create_compartment(int parent);
	void push_compartment(void);
	void pop_compartment(void);
	void delete_compartment(compartment d);
	void set_compartment(compartment c);
	int is_child(int ID);
	compartment get_parent(compartment c);
	void free_variable(variable v);
	int set_variable(variable v);
	variable *declare_variable(char *name,int type);
	void init_vars();
	void last_token();
	void init_fds();
	void init_interpreter();
	void set_output(void (*func)(char *str));
	func_d init_fd();
	void free_options(opts o);
	void free_option(variable o);
	variable copy_variable(variable a);
	void vprint(variable a);
	int check_for_function();
	void next_symbol(void);
    void cast_doubles(variable va,variable vb,double *a,double *b);
	void parse_special_expression(void);
	variable add(variable a,variable b);
	variable sub(variable a,variable b);
	variable mul(variable a,variable b);
    variable div(variable a,variable b);
    variable greater(variable a,variable b);
	variable less(variable a,variable b);
	variable equal(variable a,variable b);
    variable not_equal(variable a,variable b);
    variable less_or_equal(variable a,variable b);
	variable greater_or_equal(variable a,variable b);
	variable int_variable(int x);
	variable double_variable(double x);
	variable truth_variable(int x);
	char *thread_table(void);
	variable string_variable(char *x);
	variable eval(func_d f,opts o);
	variable eval_real(func_d f,opts o);
	int extract_literals(char *str,char ***storage);
	void declare_function(char *name,char **options,int olen,char *definition);
	void add_function(variable (*func)(opts,interpreter *),char *name);
	
	//Prototypes for simple functions...
	
	void add_fd(func_d f);
	func_d get_fd(char *name);
	opts parse_options();
	void add_string_constant(char *s,char *name);
	void add_int_constant(int c,char *name);
	void add_double_constant(double c,char *name);
	void tie_to_double(double *var,char *name);
	void tie_to_string(char *var,char *name);
	//void tie_to_string(char *str,char *name);
	void tie_to_int(int *i,char *name);
    void tie_to_int(int *i,char *name,int constant);
    void parse_expression(int single,int log);
	variable parse_variable();
	variable parse_string();
	int variable_lookup(char *name);
	int check_for_options();
	int check_for_char(const char c);
	int check_for_arguments();
	int check_for_number();
	int check_for_term();
	int check_for_keyword();
	int check_for_literal();
	int check_for_assignment();
	int check_for_comment();
	int check_for_variable();
	int check_for_string();
	void push_token();
	void pop_token();
	void parse_comment();
	void add_to_history(const char *str);
	void init_stacks();
	char *parse_literal();
	void load_input(char *i,int flush);
	void add_token(char *token);
	void flush_tokens();
	void free_input(void);
	char *eat_compartment(const char sdelim,const char edelim);
	char *eat_expression(void);
	void print_token();
	void interpret();
	int typecast(variable *v,variable a,int type);
	int is_true(variable v);
	int is_false(variable v);
    variable eval_code(const char *str,int log);
	int kill_all();
	char *eat_number();
	int suspend_all();
	int resume_all(void);
	void wait_for_terminate();
	void thread_eval_str(char *str,int wait);
	void eval_str(const char *str,int log);
	void eval_str_compartment(const char *str,int log);
	void parse_token();
	char *get_token();
	void push_string(char *c);
	void push_number(double n);
	void push_op(int op);
	double pop_number();
	char *pop_string();
	int pop_op();
	void clean_variables(compartment d);
	void clip_whitespaces();
	variable parse_single_term();
	variable parse_term(int prio);
	variable parse_follow_term(variable n,int prio);
	int parse_char(const char c);
	variable parse_number(int *dec);
	void console(void);
	char *get_history();
	void flush_history();
	const char *parse_op2(int prio);
	char atom();
	void next_atom();
	
	func_d *fd;
	variable *vars;
	int var_length;
	int fd_length;
	int calculating;	
	int interrupt;
    private:
        char *history;
        int hlen;
        compartment *com_stack;
        int com_stack_len;
        compartment *compartments;
        int com_len;
        int cid;
        compartment com;
        int thread_count;
        ex *threads;
        int nthreads;
		variable rv,retv;
		char *out;
		int olen;	
		char **tokens;
		int n_token;
		char *input;
		char *token;
		char **tstack;
		int tstack_length;
		double *nstack;
		int *opstack;
		char **cstack;
		int npos,opos,cpos;
		int end;
		void (*output)(char *);
		int c_op;
		double c_number;
		
		
};

char *status_name(int s);
variable suspend_thread(opts o,interpreter *i);
variable revive_thread(opts o,interpreter *i);
variable kill_thread(opts o,interpreter *i);
variable thread_table(opts o,interpreter *i);
variable echo(opts o,interpreter *i);
variable example(opts o,interpreter *i);
variable declare(opts o,interpreter *i);
variable constants(opts o,interpreter *i);
variable variables(opts o,interpreter *i);
variable functions(opts o,interpreter *i);
variable init_variable(void);

void set_interpreter(interpreter *i);

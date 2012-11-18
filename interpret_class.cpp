#include"interpret_class.h"

int interrupt=0;
interpreter *i;
int calculating=0;
int keylength=8;
char *keywords[]={"for","if","while","bg","sub","function","return","my"};
char *delims=" \n \t";
char *expression_t=";";

char *interpreter::result(int *len)
{
    char *ret=(char *)malloc(sizeof(char));
    ret[0]=0;
    cout << "Getting result:" << rv.type << "\n";
    if (rv.type!=-1)
    {
	ret=(char *)realloc((void *)ret,sizeof(char)*(rv.length+sizeof(variable)+1));
	memcpy((void *)ret,(void *)&rv,sizeof(rv));
	memcpy((void *)(ret+sizeof(rv)),(void *)rv.ptr,rv.length);
	ret[rv.length+sizeof(variable)]=0;
	*len=rv.length+sizeof(variable)+1;
    }
    else
    {
	*len=0;
    }
    return ret;
}

void set_interpreter(interpreter *i)
{
	::i=i;
}

void interpreter::add_to_output(const char *str,int len)
{
//    olen+=len;
//    out=(char *)realloc((void *)out,sizeof(char)*(olen+1));
 //   strncpy(out+olen-len,str,len+1);
 //   out[olen]=0;
}

char *interpreter::get_output(void)
{
	if(olen)
        return out;
    return NULL;
}

void interpreter::flush_output(void)
{
    free(out);
    out=(char *)malloc(sizeof(char));
    out[0]=0;
    olen=0;
}


void interpreter::operator<<(int i)
{
    char buf[1024];
    sprintf(buf,"%i",i);
    add_to_output(buf,strlen(buf));
    cout << i;
}

void interpreter::operator<<(double d)
{
    char buf[1024];
    sprintf(buf,"%g",d);
    add_to_output(buf,strlen(buf));
    cout << d;
}

void interpreter::operator<<(const char *str)
{
    add_to_output(str,strlen(str));
    cout << str;
}


interpreter::interpreter()
{
    out=(char *)malloc(sizeof(char));
    out[0]=0;
    olen=0;
    tokens=NULL;
    this->interrupt=0;
    n_token=0;
    input=NULL;
    tstack=NULL;
    tstack_length=0;
    os=(ofstream *)&cout;
    nstack=NULL;
    opstack=NULL;
    cstack=NULL;
    end=1;
    output=print;
    fd=NULL;
    fd_length=0;
    vars=NULL;
    var_length=0;
    calculating=0;
    threads=(ex *)malloc(sizeof(ex));
    nthreads=0;
    thread_count=0;
    com_len=0;
    cid=0;
    com_stack_len=0;
    history=(char *)malloc(sizeof(char));
    hlen=0;
    com_stack=(compartment *)malloc(sizeof(compartment));
    compartments=(compartment *)malloc(sizeof(compartment));
    com=create_compartment(-1);
    rv=init_variable();
    retv=init_variable();
    init_interpreter();
}

interpreter::~interpreter()
{

}

void print(char *str)
{
    cout << str;
}

void interpreter::init_stacks()
{
    if (tstack!=NULL)
	free(tstack);
    tstack_length=0;
    tstack=(char **)malloc(sizeof(char *));
}

//This declares a new variable
variable *interpreter::declare_variable(char *name,int type)
{
//    cout << "Declaring new " << type << " variable(" << name <<")" << " in compartment " << com.ID << "\n";
    variable v=init_variable();
    var_length++;
    vars=(variable *)realloc((void *)vars,sizeof(variable)*var_length);
    v.name=(char *)malloc(sizeof(char)*(strlen(name)+1));
    strcpy(v.name,name);
    v.type=type;
    v.ptr=NULL;
    v.length=0;
    v.constant=0;
    v.tied=0;
    if (type==VAR_DOUBLE)   
    {
	v.length=sizeof(double);
	v.ptr=(void *)malloc(sizeof(double));
	*((double *)v.ptr)=0.;
    }
    else if (type==VAR_INT)
    {
	v.length=sizeof(int);
	v.ptr=(void *)malloc(sizeof(int));
	*((int *)v.ptr)=0;	
    }
    else if (type==VAR_STRING)
    {
	v.length=1;
	v.ptr=(void *)malloc(sizeof(char));
    }
    v.compartment=com.ID;
    vars[var_length-1]=v;
    return &vars[var_length-1];
}

char *interpreter::load_file(char *filename)
{
    char *data;
    int len=0;
    int read=0;
    char buffer[1024];
    FILE *f;
    data=(char *)malloc(sizeof(char));
    f=fopen(filename,"r");
    if (f==NULL)
    {
        cout << "Error: Can't open file \"" << filename << "\"\n";
        return NULL;
    }
    while((read=fread((void *)buffer,1,1023,f)))
    {
        len+=read;
        data=(char *)realloc((void *)data,sizeof(char)*len);
        strncpy(data+len-read,buffer,read);
    }
    fclose(f);
    return data;
}

void interpreter::interpret_file(char *filename)
{
  char *data;
  data=load_file(filename);
  if (data!=NULL)
  {
//    load_input(data,1);
//    interpret();
	  thread_eval_str(data,1);
    free(data);
  }
}

void interpreter::add_int_constant(int c,char *name)
{
    variable *v=declare_variable(name,VAR_INT);
    *((int *)(v->ptr))=c;
    v->constant=1;
}

void interpreter::add_string_constant(char *s,char *name)
{
    char *ptr;
    variable *v=declare_variable(name,VAR_STRING);
    v->ptr=(void *)s;
    v->length=strlen(s)+1;
    v->constant=1;
}

void interpreter::add_double_constant(double c,char *name)
{
    variable *v=declare_variable(name,VAR_DOUBLE);
    *(double *)(v->ptr)=c;
    v->constant=1;   
}

void interpreter::tie_to_double(double *var,char *name)
{
    variable *v=declare_variable(name,VAR_DOUBLE);
    free(v->ptr);
    v->ptr=(void *)var;
    v->tied=1;
}

void interpreter::tie_to_string(char *var,char *name)
{
	variable *v=declare_variable(name,VAR_STRING);
	free(v->ptr);
	v->ptr=(void *)var;
	v->length=strlen(var)+1;
	v->tied=1;
}


void interpreter::tie_to_int(int *i,char *name,int constant)
{
    variable *v=declare_variable(name,VAR_INT);
    v->constant=constant;
    free(v->ptr);
    v->ptr=(void *)i;    
    v->tied=1;
}

void interpreter::tie_to_int(int *i,char *name)
{
    variable *v=declare_variable(name,VAR_INT);
    free(v->ptr);
    v->ptr=(void *)i;
    v->tied=1;
}


void interpreter::push_token()
{
    tstack_length++;
    if (tstack==NULL)
	tstack=(char **)malloc(sizeof(char *)*tstack_length);
    else
	tstack=(char **)realloc((void *)tstack,sizeof(char *)*tstack_length);
    tstack[tstack_length-1]=token;
}

void interpreter::pop_token()
{
    if (tstack_length>0)
    {
	token=tstack[tstack_length-1];
	tstack_length--;
	tstack=(char **)realloc((void *)tstack,sizeof(char *)*tstack_length);	
    }
}


//Suspends a certain thread.
variable suspend_thread(opts o,interpreter *i)
{
    variable v=init_variable();
    int p=0;
    char *s=NULL;
    if (i->cast_int(&p,o,0))
    {
        i->suspend_thread(p);    
    }
    else if (i->cast_string(&s,o,0))
    {
        i->suspend_thread(s);    
    }
	else
		i->suspend_all();
    return v;
}

//Revives a certain thread.
variable revive_thread(opts o,interpreter *i)
{
    variable v=init_variable();
    int p=0;
    char *s=NULL;
    if (i->cast_int(&p,o,0))
    {
        i->revive_thread(p);    
    }
    else if (i->cast_string(&s,o,0))
    {
        i->revive_thread(s);    
    }
	else
		i->resume_all();
    return v;
}

//Kills a certain thread.
variable kill_thread(opts o,interpreter *i)
{
    int p=0;
    char *s=NULL;
    variable v=init_variable();
    if (i->cast_int(&p,o,0))
    {
        i->kill_thread(p);    
    }
    else if (i->cast_string(&s,o,0))
    {
        i->kill_thread(s);    
    }
	else
		i->kill_all();
    return v;
}

variable thread_table(opts o,interpreter *i)
{
    variable v=init_variable();
    i->thread_table();
    return v;   
}

variable sleep(opts o,interpreter *i)
{
    int t=0;
    variable v=init_variable();
    if (i->cast_int(&t,o,0))
#ifdef FLUX_WINDOWS
                Sleep(t*1000);
#else
        sleep(t);
#endif
    return v;   
}

/*
variable flux_evalf(opts o,interpreter *i)
{
    variable v=init_variable();
    char *vars;
    char *filename;
    if (i->cast_string(&filename,o,0))
    {
        vars=read_file(filename);
        if (vars!=NULL)
        {
            i->eval_code(vars,1);
            free(vars);    
        }
    }
    return v;
}
*/

variable flux_eval(opts o,interpreter *i)
{
    variable v=init_variable();
    char *str;
    if (i->cast_string(&str,o,0))
    {
		v=i->eval_code(str,1);
    }
	return v;
}

void interpreter::print_rv(void)
{
	vprint(rv);
}


variable echo(opts o,interpreter *i)
{
    variable v=init_variable();
    for(int x=0;x<o.size;x++)
    {
	i->vprint(o.options[x]);
    }
    cout << "\n";
    return v;
}

variable exit(opts o,interpreter *i)
{
    variable v=init_variable();
#ifdef FLUX_WINDOWS
    ExitProcess(0);
#else
    exit(0);
#endif
	return v;
}


variable example(opts o,interpreter *i)
{
    variable v=init_variable();
    v.type=VAR_DOUBLE;
    v.ptr=NULL;
    *(i->os) << "I have got " << o.size << " options\n";
    if (o.size>=1)
    {
	i->vprint(o.options[0]);*(i->os) << "---\n";
    }
    *(i->os) << "Hello, world!\n";
    while(1)
    {
#ifdef FLUX_WINDOWS
    Sleep(1);
#else
    sleep(1);
#endif
    cout << "ZZZ...\n";
    }
    return v;
}

//This parses a literal of the form [a-zA-Z]+
char *interpreter::parse_literal(void)
{
    char *buffer=(char *)malloc(sizeof(char));
    int blength=0;
    clip_whitespaces();
    while (((int)atom()>=48 && (int)atom()<=57 ) || ((int)atom()>=65 && (int)atom()<=90 ) || ((int)atom()>=97 && (int)atom()<=122) || atom()=='_')
        {
	   blength++;
	   buffer=(char *)realloc((void *)buffer,sizeof(char)*(blength+1));
	   buffer[blength-1]=atom();
	   next_atom();
        }
    buffer[blength]=0;
    return buffer;
}

void interpreter::init_fds()
{
    fd=(func_d *)malloc(sizeof(func_d));
    fd_length=0;
}

func_d interpreter::init_fd()
{
    func_d fd;
    fd.func=NULL;
    fd.error=-1;
    fd.name=NULL;
    fd.type=FUNC_ERROR;
    return fd;
}

void interpreter::add_fd(func_d f)
{
    fd_length++;
    fd=(func_d *)realloc(fd,sizeof(func_d)*fd_length);
    fd[fd_length-1]=f;
    fd[fd_length-1].error=0;
}

void interpreter::declare_function(char *name,char **options,int olen,char *definition)
{
    cout << "Declaring new function:" << name << "\n";
    func_d fd=init_fd();
    fd.name=name;    
    fd.type=FUNC_REAL;
    fd.options=options;
    fd.olen=olen;
    fd.definition=definition;
    add_fd(fd);
}


void interpreter::add_function(variable (*func)(opts,interpreter *),char *name)
{
    func_d fd=init_fd();
    fd.func=func;
    fd.name=name;
    fd.type=FUNC_CPP;
//    cout << "Adding function " << fd.name << "\n";
    add_fd(fd);
}

func_d interpreter::get_fd(char *name)
{
    func_d f=init_fd();
    for(int x=0;x<fd_length;x++)
    {
	if (strcmp(fd[x].name,name)==0)
	{
	    return fd[x];
	}
    }
    f.error=-1;
    f.func=NULL;
    return f;
}

int interpreter::check_for_number()
{
    if((int)atom()>=48 & (int)atom()<=57 || atom()=='-' || atom()=='+')
	return 1;
    return 0;
}
int interpreter::check_for_literal()
{
    if(((int)atom()>=65 && (int)atom()<=90 ) || ((int)atom()>=97 && (int)atom()<=122) || (atom()=='_'))
	return 1;
    return 0;
}

void interpreter::reset_handler(void)
{
    signal(SIGINT,handler);
}

//This initializes the variable stack
void interpreter::init_vars()
{
    vars=(variable *)malloc(sizeof(variable));
    var_length=0;
    signal(SIGINT,handler);
}

variable interpreter::copy_variable(variable a)
{
   variable v=init_variable();
   if (a.type==-1 || a.length<=0)
        return v;
    v=a;
    v.ptr=malloc(v.length);
    v.ptr=memmove(v.ptr,a.ptr,v.length);
    if (a.name!=NULL && strlen(a.name)>0)
    {
	    v.name=(char *)malloc(strlen(a.name)+1);
	    if (v.name==NULL)
	    {
		    cerr << "Error: Cannot allocate memory!\n";
		    exit(0);
	    }
    	    strcpy(v.name,a.name);
    }
    return v;
}

void interpreter::push_compartment(void)
{
    com_stack_len++;
    com_stack=(compartment *)realloc((void *)com_stack,sizeof(compartment)*com_stack_len);
    com_stack[com_stack_len-1]=com;
}
        
void interpreter::pop_compartment(void)
{
    if (com_stack_len>0)
    {
        com_stack_len--;    
        com=com_stack[com_stack_len];
        com_stack=(compartment *)realloc((void *)com_stack,sizeof(compartment)*(com_stack_len>0?com_stack_len:1));
    }
}
        

//This creates a new compartment
compartment interpreter::create_compartment(int parent)
{
    compartment c;
    c.ID=cid++;
    c.parent=parent;
    com_len++;
    compartments=(compartment *)realloc((void *)compartments,sizeof(compartment)*com_len);
    compartments[com_len-1]=c;
    return c;
}

void interpreter::clean_variables(compartment d)
{
	int x;
    for(x=0;x<var_length;x++)
    {
        if (vars[x].compartment==d.ID)
        {
            vars[x].freed=1;
            free_variable(vars[x]);
        }
    }
    int frees=0;
    for(x=0;x<var_length;x++)
    {
        vars[x-frees]=vars[x];
        if (vars[x].freed==1)
            frees++;
    }
    var_length-=frees;
    vars=(variable *)realloc((void *)vars,sizeof(variable)*var_length);
}

//This deletes a compartment
void interpreter::delete_compartment(compartment d)
{
    int cut=0;
    clean_variables(d);
    for(int x=0;x<com_len;x++)
    {
        if (cut)
            compartments[x-1]=compartments[x];
        if(d.ID==compartments[x].ID)
            cut=1;
    }
    if (cut)
    {
        com_len--;
        compartments=(compartment *)realloc((void *)compartments,sizeof(compartment)*com_len);
    }
}

//This sets the current compartment to c
void interpreter::set_compartment(compartment c)
{
    com=c;
}

//This checks if ID is a child of the current compartment (true if both are equal)
int interpreter::is_child(int ID)
{
    int level=1;
    compartment c=com;
    if (c.ID==ID)
        return 1;
    while((c=get_parent(c)).ID!=-1)
    {
        level++;
        if (c.ID==ID)
            return level;
    }
    return 0;
}

compartment interpreter::get_parent(compartment c)
{
    compartment co;
    co.ID=-1;
    co.parent=-1;
    for(int x=0;x<com_len;x++)
    {
        if (compartments[x].ID==c.parent)
            return compartments[x];
    }   
    return co;
}

//This outputs all variables that are stored at runtime (except constants)
char *interpreter::output_all_variables(void)
{
    int len=0;
    char buf[10000];
    char *str;
    char *buffer=(char *)malloc(sizeof(char));
    for(int x=0;x<var_length;x++)
    {
        if (vars[x].constant==0 || 1)
        {
		str=get_string(vars[x]);
                sprintf(buf,"%s=%s;\n",vars[x].name,str);
		free(str);
                len+=strlen(buf);
                buffer=(char *)realloc((void *)buffer,(len+1)*sizeof(char));
                strcpy(buffer+len-strlen(buf),buf);
                buffer[len]=0;
        }
    }
    return buffer;
}

//This return the variable with name "name". If not found, it return an empty variable with type set to -1
variable *interpreter::get_variable_ptr(char *name)
{
    int found=0;
    int ilevels;
    int min=1000000;
    variable *v=NULL;
    for(int x=0;x<var_length;x++)
    {
	if (strcmp(vars[x].name,name)==0)
	{
            if ((ilevels=is_child(vars[x].compartment)))
            {
                if (ilevels<min)
                {
                   found=1;
		   min=ilevels;
//		   cout << "ptr-found " << name << " at level " << ilevels << " and compartment " << vars[x].compartment << "\n";
		   v=&vars[x];                }
            }
	}
    }
    if (found)
        return v;
    return NULL;
}

//This return the variable with name "name". If not found, it return an empty variable with type set to -1
variable interpreter::get_variable(char *name)
{
    int found=0;
    int ilevels;
    int v_ind=0;
    int min=1000000;
    variable v=init_variable();
    for(int x=0;x<var_length;x++)
    {
	if (strcmp(vars[x].name,name)==0)
	{
	if ((ilevels=is_child(vars[x].compartment)))
            {
		if (ilevels<min)
                {
	            v_ind=x;
                    found=1;
		    min=ilevels;
		}
            }
        }
    }
    if (found)
    {
	v=copy_variable(vars[v_ind]);
    	return v;
    }
    v.type=-1;
    v.ptr=NULL;
    v.length=0;
    v.name=NULL;
    return v;
}


int interpreter::variable_lookup(char *name)
{
    return 1;
}

//This checks if we have a variable, e.g. "foo;".
int interpreter::check_for_variable()
{
    push_token();
    if (check_for_literal())
    {
	char *var=parse_literal();
	if (variable_lookup(var))
	{
	    free(var);
	    if (check_for_char('('))
	    {
		pop_token();
		return 0;
	    }
	    pop_token();
	    return 1;
	}
	free(var);
    }
    pop_token(); 
    return 0;
}

int interpreter::check_for_char(const char c)
{
    while(atom()==' ')
	next_atom();
    if (atom()==c)
	return 1;
    return 0;
}

void interpreter::parse_comment()
{
	while(atom()!='\n')
		next_atom();
}


const char comment[]="#";
int interpreter::check_for_comment()
{
	push_token();
	if (token[0]==comment[0])
	{
		pop_token();
		return 1;
	}
	pop_token();
	return 0;
}

int interpreter::check_for_keyword(void)
{
    char *lit=NULL;
    push_token();
    if (check_for_literal())
    {
	lit=parse_literal();
	for(int x=0;x<keylength;x++)
	    if (strcmp(lit,keywords[x])==0)
	       {
		   free(lit);
		   pop_token();
                   return 1;
	       }
    }
    if (lit!=NULL)
	free(lit);
    pop_token();
    return 0;
}

//This checks if an expression is an assignment, e.g. "foo=bar;"
int interpreter::check_for_assignment()
{
	char *lit=NULL;
    push_token();
    if (check_for_variable())
    {
	    lit=parse_literal();
	    free(lit);
	if (check_for_char('='))
	{
	    next_atom();
	    if (check_for_char('=')==0)
	    {
	       pop_token();
	       return 1;
	    }
	}
    }
    pop_token();
    return 0;
}

void *interpreter::my_malloc(int size,char *purpose)
{
    cout << "Allocating for:" << purpose << "\n";
    return malloc(size);
}
void *interpreter::my_realloc(void *ptr,int size,char *purpose)
{
    cout << "Re-Allocating for:" << purpose << "\n";
    return realloc(ptr,size);
}

void interpreter::my_free(void *ptr,char *purpose)
{
    cout << "Freeing for:" << purpose << "\n";
    free(ptr);
}
	

//This parses a variable
variable interpreter::parse_variable()
{
    variable v=init_variable();
    char *lit;
    push_token();
    clip_whitespaces();
    if (check_for_literal())
    {
	lit=parse_literal();
	v=get_variable(lit);//This returns an LVALUE of the variable.
	free((void *)lit);
        return v;
    }
    pop_token();
    v.type=-1;
    return v;
}

func_d interpreter::parse_function()
{
    func_d f=init_fd();
    char *lit;
    lit=parse_literal();
    f=get_fd(lit);
    free(lit);
    return f;
}

//This checks if an expression is a function, e.g. "foo(bar);"
int interpreter::check_for_function()
{
    func_d fd=init_fd();
    push_token();
    char *lit;
    if (check_for_literal())
    {
	lit=parse_literal();
        fd=get_fd(lit);
	if (fd.type!=FUNC_ERROR)
	{
	    if (check_for_options())
	    {
		pop_token();
		free((void *)lit);
		return 1;
	    }
	}
	free((void *)lit);
    }
    pop_token();
    return 0;
}

int interpreter::check_for_options()
{
    opts o;
    push_token();
    if (parse_char('('))
    {
	if ((o=parse_options()).size>=0)
	{
	    free_options(o);
	    pop_token();
	    return 1;
	}
	free_options(o);
    }
    pop_token();
    return 0;
}

int interpreter::check_for_arguments()
{
    while((int)atom()==' ')
	next_atom();
    if ((int)atom()=='(')
	return 1;
    return 0;
}

opts interpreter::parse_options()
{
    opts o;
    int size=0;
    variable *options=(variable *)malloc(sizeof(variable));
    options[0].ptr=NULL;
    options[0].length=0;
    if ((int)atom()=='(')
    {
	variable var=init_variable();
	next_atom();
	if (atom()!=')')
	{
	   parse_expression(1,0);
	   var=copy_variable(rv);
	   size++;
	   options=(variable *)realloc((void *)options,sizeof(variable)*size);
	   options[size-1]=var;
	   while((int)atom()==',')
	   {
	       next_atom();
	       parse_expression(1,0);
	       var=copy_variable(rv);
	       size++;
	       options=(variable *)realloc((void *)options,sizeof(variable)*size);
	       options[size-1]=var;
	   }
	}
	next_atom();
    }
    o.size=size;
    o.options=options;
    return o;
}

//This frees the memory of an option structure
void interpreter::free_option(variable o)
{
    free_variable(o);
}

//This frees the memory of an opts structure
void interpreter::free_options(opts o)
{
    for(int x=0;x<o.size;x++)
	free_variable(o.options[x]);
    free((void *)o.options);
}

//This initializes the interpreter
void interpreter::init_interpreter()
{
    init_fds();
    init_vars();
//	add_function(flux_evalf,"evalf");
	add_function(flux_eval,"eval");
    add_function(sleep,"sleep");
    add_function(echo,"echo");
    add_function(exit,"exit");
    add_function(example,"example");
    add_function(declare,"declare");
    add_function(constants,"constants");
    add_function(variables,"variables");
    add_function(functions,"functions");
    add_function(::thread_table,"threads");
    add_function(::kill_thread,"kill");
    add_function(::suspend_thread,"suspend");
    add_function(::revive_thread,"resume");
}

int interpreter::typecast(variable *v,variable a,int type)
{
    double d=0.0;
    int i=0;
    switch (type)
    {
        case VAR_INT:
            if (cast_int(&i,a))
            {
                *v=int_variable(i);
                return 1;
            }
            
            break;  
        case VAR_DOUBLE:
            if (cast_double(&d,a))
            {
                *v=double_variable(d);
                return 1;
            }
            break;
    };    
    return 0;
}

int interpreter::set_variable(variable v)
{
    variable cast=init_variable();
    int c=0;
    if (v.name==NULL)
    {
	return -1;
    }
    variable *set=get_variable_ptr(v.name);
    if (set==NULL)
    {
	return -2;
    }
    if (set->type==-1)
       return -2;
    if (!set->constant)
	    {
		if (set->tied)
		{
			if (set->type!=v.type && set->tied)
			{
				variable dummy=init_variable();
				if (typecast(&dummy,v,set->type))
				{	
					memmove(set->ptr,dummy.ptr,set->length);
					free_variable(dummy);
					free_variable(v);
					return 0;
				}
				else
					return -1;
			}
			else
			{
				memcpy(set->ptr,v.ptr,set->length);
				free_variable(v);
//				*(set->ptr)=*(v.ptr);
			}
		}
		else
		{
			c=set->compartment;
			free_variable(*set);
			*set=v;
			set->compartment=c;
		}
		return 0;
	    }
            else
	    {
                return -1;
	    }
    return -1;
}

void interpreter::free_variable(variable v)
{
    if (v.type<0)return;
    if (v.length>0 && v.ptr!=NULL)
    {
	free((void *)v.ptr);
    }
    if (v.name!=NULL)
    {
	    free((void *)v.name);
    }
}


variable interpreter::parse_assignment()
{
    int stat=0;
    char *lit;
    variable n=init_variable();
    variable v=init_variable();
    lit=parse_literal();
    if (parse_char('='))
    {
	n=parse_term(0);
	free(n.name);
	n.name=lit;
	if ((stat=set_variable(n))!=0)
	{
	    if (stat==-2)
	    {
	       declare_variable(lit,n.type);
	       set_variable(n);
	    }
	    if (stat==-1)
	    {
		   free_variable(n);
	    }
        }
    }
	return v;
}

int interpreter::cast_double(double *d,variable a)
{
    if (a.type==VAR_DOUBLE)
    {
        *d=*((double *)a.ptr);
        return 1;
    }
    else if (a.type==VAR_INT)
    {
		*d=(double)(*((int *)a.ptr));
        return 1;
    }
    return 0;    
}

int interpreter::cast_int(int *i,variable a)
{
    if (a.type==VAR_INT)
    {
        *i=*((int *)a.ptr);
        return 1;
    }
    else if (a.type==VAR_DOUBLE)
    {
        *i=(int)*((double *)a.ptr);
        return 1;
    }
    return 0;
    
}

int interpreter::cast_double(double *d,opts o,int x)
{
    if (o.size<=x)
	return 0;
    return cast_double(d,o.options[x]);
}

int interpreter::cast_int(int *d,opts o,int x)
{
    if (o.size<=x)
	return 0;
    return cast_int(d,o.options[x]);
}

int interpreter::cast_string(char **ptr,opts o,int x)
{
	if (o.size<=x)
		return 0;
	if (o.options[x].type==VAR_STRING)
	{
		*ptr=(char *)o.options[x].ptr;
		return 1;
	}
	return 0;
}

//This 'eats' a compartment starting with sdelim and ending with edelim. The function takes care of nested compartments of the same type.
char *interpreter::eat_compartment(const char sdelim,const char edelim)
{
    int nests=0;
    char *buffer=(char *)malloc(sizeof(char)*2);
    int len=0;
    clip_whitespaces();
    if (parse_char(sdelim))
    {
	while(atom()!=edelim || nests!=0)
	{
	    if (atom()==sdelim)
		nests++;
	    if (atom()==edelim)
		nests--;
	    len++;
	    buffer=(char *)realloc((void *)buffer,sizeof(char)*(len+2));
            buffer[len-1]=(char)atom();
	    next_atom();
	}
        buffer[len]=';';
        buffer[len+1]=0x00;
	next_atom();
	return buffer;
    }
    buffer[0]=';';
    buffer[1]=0x00;
    return buffer;
}

int interpreter::is_true(variable v)
{
    if (v.type==VAR_TRUTH)
	return (*((int *)v.ptr))?1:0;
    else if (v.type==VAR_DOUBLE)
	return (*((double *)v.ptr))?1:0;
    else if (v.type==VAR_INT)
	return (*((int *)v.ptr))?1:0;
    return 0;
}

int interpreter::is_false(variable v)
{
    return 0;
}
	


void interpreter::parse_special_expression(void)
{
    char *lit=parse_literal();
    char *c1,*c2,*c3;
    char *var;
	int x,y;
    char **opts=NULL;
    int olen=0;
    char *init,*cond,*inc;
    for(x=0;x<keylength;x++)
    {
	    if (strcmp(keywords[x],lit)==0)
	{
	    switch(x)
	    {
                case 0://This is a "for"-type expression
                    c1=eat_compartment('(',')');
                    c2=eat_compartment('{','}');
                    push_token();
                    load_input(c1,0);
		    init=eat_expression();
		    cond=eat_expression();
		    inc=eat_expression();
                    pop_token();
		    push_compartment();
		    com=create_compartment(com.ID);
                    //Now we are ready to run the loop...
		    eval_str(init,0);//First init variable
                    eval_str(cond,0);//Then check the condition
		    while(is_true(rv))
		    {
      	 	        eval_str(c2,0);
					eval_str(inc,0);
					eval_str(cond,0);
		    }
		    delete_compartment(com);
		    pop_compartment();
		    free(c1);
                    free(c2);
                    free(inc);
                    free(cond);
                    free(init);
		    break;
                    case 1://This is an "if"-type expression
                    c1=eat_compartment('(',')');
                    c2=eat_compartment('{','}');
                    eval_str(c1,0);
                    if (is_true(rv))
                    {
                        eval_str_compartment(c2,1);
                    }
                    free(c1);
                    free(c2);
                    break;
                    case 2://This is a "while"-type expression
			c1=eat_compartment('(',')');
			c2=eat_compartment('{','}');
			push_compartment();
			com=create_compartment(com.ID);
			eval_str(c1,0);
                        while (is_true(rv))
                        {
			eval_str(c2,0);
                        eval_str(c1,0);
                        }
			delete_compartment(com);
			pop_compartment();
			free(c1);
			free(c2);
                    break;
                    case 3://this is a "&"-type expression
                        
                        break;
                        case 4://this is a "{"-type expression
                            c1=eat_compartment('{','}');
                            eval_str_compartment(c1,1);
                            free(c1);    
                        break;
                        case 5://This is a function declaration
                            c1=parse_literal();
                            //cout << "Declaring function " << c1 << "\n";
                            c2=eat_compartment('(',')');
                            //cout << "Arguments:" << c2 << "\n";
                            c3=eat_compartment('{','}');
                            //cout << "Definition:" << c3 << "\n";
                            olen=extract_literals(c2,&opts);
                            //for(y=0;y<olen;y++)
                            //    cout << "Argument No. " << y << " :" << opts[y] << "\n";
                            declare_function(c1,opts,olen,c3);
                            free(c2);
                    break;
                    case 6://This is a "return"-type of expression
                        parse_expression(1,0);
			if (rv.type>=0)
			{
				free_variable(retv);
                        	retv=copy_variable(rv);
			}
                        last_token();
                    break;
		    case 7://This is a "my"-statement: Declare new variable;
			    var=parse_literal();
			    declare_variable(var,VAR_DOUBLE);
			    break;
		default:break;//Don't know which kind of expression...
	    };
	}
    }
    free(lit);
}

void interpreter::last_token(void)
{
    token=NULL;
}

//Extracts the literals from an expression like a,b,c,d
int interpreter::extract_literals(char *str,char ***storage)
{
    int len=0;
    char *lit;
    char **args;
    args=(char **)malloc(sizeof(char *));
    push_token();
    token=str;
    while(check_for_literal())
    {
        len++;
        args=(char **)realloc((void *)args,sizeof(char *)*len);
        args[len-1]=parse_literal();
        next_atom();
    }
    *storage=args;
    pop_token();
    return len;
}

//This algorithm ''eats'' a single expression (terminating with a ";" or a ")") without evaluating it.
char *interpreter::eat_expression(void)
{
    char *exp=(char *)malloc(sizeof(char));
    int len=0;
    while(strchr(expression_t,atom())==NULL)
    {
	len++;
	exp=(char *)realloc((void *)exp,sizeof(char)*(len+2));	
	exp[len-1]=atom();
	next_atom();
    }
    next_atom();
    exp[len]=';';
    exp[len+1]=0x00;
    return exp;
}


//This parses an expression.
void interpreter::parse_expression(int single,int log)
{
    char *buf=token;
    clip_whitespaces();
    if (::interrupt!=0)
    {
	::interrupt=0;
	return;
    }
    if (check_for_comment())
    {
	    parse_comment();
//	    free_variable(rv);
//	    rv=init_variable();
    }
    else if (check_for_keyword())
    {
	parse_special_expression();
        free_variable(rv);
        rv=init_variable();
	//This is a special expression, e.g. for,if,while,do,...
    }
    else if (check_for_assignment())
    {
	//This is an assignment, e.g. foo=bar;
	variable v=init_variable();
	v=parse_assignment();
	free_variable(rv);
	rv=v;
    }
    else
    {
//	    variable v=init_variable();
	variable v=parse_term(0);
//	free_variable(v);
//	v=init_variable();
	//This is just a term, e.g. foo+bar;
	if (v.type>0)
	{
	   free_variable(rv);
	   rv=v;
//	   if (log)
//		   vprint(v);
	}
   }
   while(atom()==';' || atom()==10 || atom()==' ')
	next_atom();
   if (log)
    {
	    int pos=(int)(token-buf);
	if (strlen(buf)<pos)
	    pos=0;
	char c=buf[pos];
	buf[pos]=0;
	add_to_history(buf);
	buf[pos]=c;
    }
    if (atom()!=0 && single==0)
    { 
	    parse_expression(single,log);
    }
       //That's it!
}


void interpreter::cast_doubles(variable a,variable b,double *va,double *vb)
{
    if (a.type==VAR_INT)
        *va=(double)(*((int *)a.ptr));
    else if (a.type==VAR_DOUBLE)
        *va=*((double *)a.ptr);
    if (b.type==VAR_INT)
        *vb=(double)(*((int *)b.ptr));
    else if (b.type==VAR_DOUBLE)
        *vb=*((double *)b.ptr);
}


variable interpreter::greater(variable a,variable b)
{
    variable v=init_variable();
    double va,vb;
    cast_doubles(a,b,&va,&vb);
    if (va>vb)
	v=truth_variable(1);
    else
	v=truth_variable(0);
//    free_variable(a);
    return v;
}

variable interpreter::greater_or_equal(variable a,variable b)
{
    variable v=init_variable();
    double va,vb;
    cast_doubles(a,b,&va,&vb);
    if (va>=vb)
        v=truth_variable(1);
    else
        v=truth_variable(0);
//    free_variable(a);
    return v;
}


variable interpreter::less_or_equal(variable a,variable b)
{
    variable v=init_variable();
    double va,vb;
    cast_doubles(a,b,&va,&vb);
    if (va<=vb)
        v=truth_variable(1);
    else
        v=truth_variable(0);
//    free_variable(a);
    return v;
}

variable interpreter::equal(variable a,variable b)
{
    variable v=init_variable();
    double va,vb;
	if (a.type==VAR_STRING && b.type==VAR_STRING)
	{
		if (strcmp((char *)a.ptr,(char *)b.ptr)==0)
			v=truth_variable(1);
		else
			v=truth_variable(0);
		return v;
	}
    cast_doubles(a,b,&va,&vb);
    if (va==vb)
        v=truth_variable(1);
    else
        v=truth_variable(0);
//    free_variable(a);
    return v;
   
}


variable interpreter::not_equal(variable a,variable b)
{
    variable v=init_variable();
    double va,vb;
	if (a.type==VAR_STRING && b.type==VAR_STRING)
	{
		if (strcmp((char *)a.ptr,(char *)b.ptr)==0)
			v=truth_variable(0);
		else
			v=truth_variable(1);
		return v;
	}
    cast_doubles(a,b,&va,&vb);
    if (va!=vb)
        v=truth_variable(1);
    else
        v=truth_variable(0);
//    free_variable(a);
    return v;
   
}

variable interpreter::less(variable a,variable b)
{
    variable v=init_variable();
    double va,vb;
    cast_doubles(a,b,&va,&vb);
    if (va<vb)
        v=truth_variable(1);
    else
        v=truth_variable(0);
//    free_variable(a);
    return v;
}

//This divides two integer variables
variable interpreter::div_int(variable a,variable b)
{
    int ia,ib;
    ia=*((int *)a.ptr);
    ib=*((int *)b.ptr);
    //free_variable(a);
    return int_variable(ia/ib);
}

//This divides two double variables
variable interpreter::div_double(variable a,variable b)
{
    double da,db;
    da=*((double *)a.ptr);
    db=*((double *)b.ptr);
    //free_variable(a);
    return double_variable(da/db);
}

//This multiplies two integer variables
variable interpreter::mul_int(variable a,variable b)
{
    int ia,ib;
    ia=*((int *)a.ptr);
    ib=*((int *)b.ptr);
    //free_variable(a);
    return int_variable(ia*ib);
}

//This multiplies two double variables
variable interpreter::mul_double(variable a,variable b)
{
    double da,db;
    da=*((double *)a.ptr);
    db=*((double *)b.ptr);
    //free_variable(a);
    return double_variable(da*db);
}


//This substracts two integer variables
variable interpreter::sub_int(variable a,variable b)
{
    int ia,ib;
    ia=*((int *)a.ptr);
    ib=*((int *)b.ptr);
    //free_variable(a);
    return int_variable(ia-ib);
}

//This substracts two double variables
variable interpreter::sub_double(variable a,variable b)
{
    double da,db;
    da=*((double *)a.ptr);
    db=*((double *)b.ptr);
    //free_variable(a);
    return double_variable(da-db);
}


//This adds two integer variables
variable interpreter::add_int(variable a,variable b)
{
    int ia,ib;
    ia=*((int *)a.ptr);
    ib=*((int *)b.ptr);
    //free_variable(a);
    return int_variable(ia+ib);
}

//This adds two double variables
variable interpreter::add_double(variable a,variable b)
{
    double da,db;
    da=*((double *)a.ptr);
    db=*((double *)b.ptr);
    //free_variable(a);
    return double_variable(da+db);
}

//This adds two string variables
variable interpreter::add_string(variable a,variable b)
{
    char *new_str;
    char *str_a,*str_b;
    str_a=(char *)a.ptr;
    str_b=(char *)b.ptr;
    new_str=(char *)malloc(strlen(str_a)+strlen(str_b)+1);
    sprintf(new_str,"%s%s",str_a,str_b);
    return string_variable(new_str);
}

int interpreter::to_int(variable *a)
{
    double vd;
    int vi;
    char *p_str;
    switch (a->type)
    {
	case VAR_INT:return 0;
	case VAR_DOUBLE:vd=*((double *)a->ptr);free_variable(*a);*a=int_variable((int)vd);return 0;break;
	case VAR_STRING:return -1;break;
	default:return -1;break;
    }   
    return -1;
}

int interpreter::to_double(variable *a)
{
    int vi;
    switch(a->type)
    {
	case VAR_DOUBLE:return 0;break;
	case VAR_INT:vi=*((int *)a->ptr);free_variable(*a);*a=double_variable((double)vi);return 0;break;
	case VAR_STRING:return -1;break;
	default:return -1;break;
    }
    return -1;
}

int interpreter::to_string(variable *a)
{
    int vi;
    double vd;
    char *my_str;
    char buffer[1024];
    switch(a->type)
    {
	case VAR_STRING:return 0;break;
	case VAR_DOUBLE:vd=*((double *)a->ptr);sprintf(buffer,"%g",vd);my_str=(char *)malloc(strlen(buffer)+1);strncpy(my_str,buffer,strlen(buffer)+1);free_variable(*a);*a=string_variable(my_str);return 0;break;
	case VAR_INT:vi=*((int *)a->ptr);sprintf(buffer,"%i",vi);my_str=(char *)malloc(strlen(buffer)+1);strncpy(my_str,buffer,strlen(buffer)+1);free_variable(*a);*a=string_variable(my_str);return 0;break;
	default:return -1;break;
    }
    return -1;
}

//This ''condenses'' two variables to the same type, if possible.
int interpreter::condense(variable *a,variable *b)
{
    int types=a->type | b->type;
    if (types & VAR_STRING)
    {
	//Condense to strings
	if (to_string(a)!=0)return VAR_ERROR;
	if (to_string(b)!=0)return VAR_ERROR;
	return VAR_STRING;
    }
    else if (types & VAR_DOUBLE)
    {
	//Condense to doubles
	if (to_double(a)!=0)return VAR_ERROR;
	if (to_double(b)!=0)return VAR_ERROR;
	return VAR_DOUBLE;
    }
    else if (types & VAR_INT)
    {
	//Condense to integers
	if (to_int(a)!=0)return VAR_ERROR;
	if (to_int(b)!=0)return VAR_ERROR;
	return VAR_INT;
    }
    return VAR_ERROR;
}

variable interpreter::div(variable a,variable b)
{
    variable v=init_variable();
    variable ta=copy_variable(a);
    variable tb=copy_variable(b);
    switch (condense(&ta,&tb))
    {
	case VAR_ERROR:break;
	case VAR_INT:v=div_int(ta,tb);break;
	case VAR_DOUBLE:v=div_double(ta,tb);break;
	case VAR_STRING:break;
	default:break;
    } 
    free_variable(ta);
    free_variable(tb);
    return v;
}

variable interpreter::mul(variable a,variable b)
{
    variable v=init_variable();
    variable ta=copy_variable(a);
    variable tb=copy_variable(b);
    switch (condense(&ta,&tb))
    {
	case VAR_ERROR:break;
	case VAR_INT:v=mul_int(ta,tb);break;
	case VAR_DOUBLE:v=mul_double(ta,tb);break;
	case VAR_STRING:break;
	default:break;
    } 
    free_variable(ta);
    free_variable(tb);
    return v;
}

variable interpreter::sub(variable a,variable b)
{
    variable v=init_variable();
    variable ta=copy_variable(a);
    variable tb=copy_variable(b);
    switch (condense(&ta,&tb))
    {
	case VAR_ERROR:break;
	case VAR_INT:v=sub_int(ta,tb);break;
	case VAR_DOUBLE:v=sub_double(ta,tb);break;
	case VAR_STRING:break;
	default:break;
    } 
    free_variable(ta);
    free_variable(tb);
    return v;
}


variable interpreter::add(variable a,variable b)
{
    variable v=init_variable();
    variable ta=copy_variable(a);
    variable tb=copy_variable(b);
    switch (condense(&ta,&tb))
    {
	case VAR_ERROR:break;
	case VAR_INT:v=add_int(ta,tb);break;
	case VAR_DOUBLE:v=add_double(ta,tb);break;
	case VAR_STRING:v=add_string(ta,tb);break;
	default:break;
    } 
    free_variable(ta);
    free_variable(tb);
    return v;
}

variable init_variable(void)
{
    variable v;
    v.ptr=NULL;
    v.name=NULL;
    v.length=0;
    v.type=-1;
    v.freed=0;
    v.tied=0;
    v.constant=0;
    v.compartment=1;
    return v;
}

//This parses a term follower, e.g. "+6*7;"
variable interpreter::parse_follow_term(variable n,int prio)
{
    const char *op2=NULL;
    if ((op2=parse_op2(prio))!=NULL)
    {
	variable c=init_variable();
	switch (op2[0])
	{
	    case '+':c=parse_term(0);n=add(n,c);break;
	    case '-':c=parse_term(1);n=sub(n,c);break;
	    case '*':c=parse_single_term();n=mul(n,c);break;
        case '/':c=parse_single_term();n=div(n,c);break;
        case '>':c=parse_term(0);if (op2[1]=='=')n=greater_or_equal(n,c);else n=greater(n,c);break;
	    case '<':c=parse_term(0);if (op2[1]=='=')n=less_or_equal(n,c);else n=less(n,c);break;
	    case '=':c=parse_term(0);n=equal(n,c);break;
            case '!':c=parse_term(0);n=not_equal(n,c);break;
        }
	free_variable(c);
    }
    else
	return n;
    if (parse_char(';')==0 && parse_char(0)==0)
    {
	n=parse_follow_term(n,prio);
    }
    return n;
}


void new_flux_handler(int sig)
{
#ifdef FLUX_WINDOWS
    ExitThread(0);
#else
    signal(SIGABRT,new_flux_handler);
    signal(SIGRTMIN+2,new_flux_handler);
    signal(SIGRTMIN+1,new_flux_handler);
    if (sig==SIGABRT || sig==SIGKILL)
    {
        pthread_exit(NULL);
    }
    else if (sig==SIGRTMIN+2)
    {
        sigset_t sigs;
        int sig=0;
	sigaddset(&sigs,SIGRTMIN+1);
	sigwait(&sigs,&sig);
    }
#endif
}

//This evaluates a user-defined function
variable interpreter::eval_real(func_d f,opts o)
{
    char *temp;
    variable vp;
    variable v=init_variable();
    push_compartment();
    com=create_compartment(com.ID);
    if (o.size!=f.olen)
    {
        cout << "Error: Function \"" << f.name << "\" takes exactly " << f.olen << " arguments, not " << o.size << "!\n";
        return v; 
    }
    for(int x=0;x<f.olen;x++)
    {
	vp=copy_variable(o.options[x]);
	if (vp.name!=NULL)
		free(vp.name);
	vp.name=(char *)malloc((strlen(f.options[x])+1)*sizeof(char));
	strcpy(vp.name,f.options[x]);
	declare_variable(vp.name,vp.type);
        set_variable(vp);
//	cout << "Declared variable " << vp.type << " ";vprint(vp);cout << " with name \"" << vp.name << "\"\n";
    }
    eval_str(f.definition,0);
    delete_compartment(com);
    pop_compartment();
    if (retv.type!=-1){
	    return copy_variable(retv);}
    return init_variable();
}

void interpreter::set_output(void (*func)(char *))
{
    cout << "Setting new output...\n";
    output=func;
}

#ifdef FLUX_WINDOWS
DWORD WINAPI ThreadFunktion(LPVOID param)
{
    signal(SIGABRT,new_flux_handler);
    exopt opts=*((exopt *)param);
    exopt *ptr=(exopt *)param;
    int n=opts.thread_number;
//We wait for the thread to be registered.
    if (opts.multithread)
    {
      while (ptr->i->registered_thread(*ptr)==0)
      {
    	 Sleep(10);
      }
    }	
    if (opts.action==EX_FUNCTION)
	{
		if (opts.f.type==FUNC_CPP)
		{
			opts.o.calling_func=opts.f.name;
			ptr->r=opts.f.func(opts.o,opts.i);
		}
		else if (opts.f.type==FUNC_REAL)
		{
			opts.o.calling_func=opts.f.name;
			ptr->r=opts.i->eval_real(opts.f,opts.o);
		}
		ptr->done=1;
		if (opts.multithread)
		{
    		opts.i->finish_thread(n);
		}
	}
	else if (opts.action==EX_EVAL)
	{
		opts.i->eval_str(opts.eval_str,0);
		opts.i->finish_thread(n);
		ptr->done=1;
	}
    return 0;
}
#else
void *execute(void *p)
{
    exopt opts=*((exopt *)p);
    exopt *ptr=(exopt *)p;
    if (opts.multithread)
    {
      while (ptr->i->registered_thread(*ptr)==0)
      {
	     timespec t,tt;
	     t.tv_sec=0;
	     t.tv_nsec=100000;
	     nanosleep(&t,&tt);
      }
    }
//We wait for the thread to be registered.
    if (opts.multithread){
      signal(SIGABRT,new_flux_handler);
      signal(SIGRTMIN+2,new_flux_handler);
      signal(SIGRTMIN+1,new_flux_handler);
    }
    int n=opts.thread_number;
	if (opts.action==EX_FUNCTION)
	{
		if (opts.f.type==FUNC_CPP)
		{
			opts.o.calling_func=opts.f.name;
			ptr->r=opts.f.func(opts.o,opts.i);
		}
		else if (opts.f.type==FUNC_REAL)
		{
			opts.o.calling_func=opts.f.name;
			ptr->r=opts.i->eval_real(opts.f,opts.o);
		}
		ptr->done=1;
		if (opts.multithread)
		{
			opts.i->finish_thread(n);
			pthread_exit(NULL);
		}
		return NULL;
	}
	else if (opts.action==EX_EVAL)
	{
	        opts.i->eval_str(opts.eval_str,1);
		opts.i->finish_thread(n);
		ptr->done=1;
		pthread_exit(NULL);
	}
	pthread_exit(NULL);
}
#endif

int interpreter::get_thread_number(void)
{
    return thread_count++;
}

int interpreter::registered_thread(ex p)
{
	for(int x=0;x<nthreads;x++)
	{
		if (threads[x].thread_number==p.thread_number)
			return 1;
	}
	return 0;
}

int interpreter::register_thread(ex p)
{
    nthreads++;
    threads=(ex *)realloc((void *)threads,sizeof(ex)*nthreads);
    threads[nthreads-1]=p;
	return 1;
}

char *status_name(int s)
{
    switch(s)
    {
	case THREAD_RUNNING:return "RUNNING";
	case THREAD_SUSPENDED:return "SUSPENDED";
	case THREAD_FINISHED:return "FINISHED";
	case THREAD_KILLED:return "KILLED";
	default:return "UNDEFINED";
    };
    return NULL;
}

char *interpreter::thread_table(void)
{
    char buf[10000];
    int len=0;
    for(int x=0;x<nthreads;x++)
    {
        if (threads[x].status!=THREAD_FINISHED && threads[x].status!=THREAD_KILLED)
        {
			if (threads[x].action==EX_FUNCTION)
				sprintf(buf,"\n%.4d\t%.30s\t%-s\n",threads[x].thread_number,threads[x].f.name,status_name(threads[x].status));
			else
				sprintf(buf,"\n%.4d\t[code evaluation]\t%-s\n",threads[x].thread_number,status_name(threads[x].status));
			cout << buf;
		}
    }    
	return NULL;
}

int interpreter::finish_thread(int n)
{
    int x;
    for(x=0;x<nthreads;x++)
    {
        if (threads[x].thread_number==n)
        {
		threads[x].status=THREAD_FINISHED;
		break;
        }
    }
    int mover=0;
    for(x=0;x<nthreads;x++)
	if (threads[x].status==THREAD_FINISHED || threads[x].status==THREAD_KILLED)mover++;
    else
    {
        threads[x-mover]=threads[x];
    }
    nthreads-=mover;
    threads=(exopt *)realloc((void *)threads,sizeof(exopt)*nthreads);
    return 0;
}

int interpreter::suspend_thread(char *n)
{
    for(int x=0;x<nthreads;x++)
    {
        if (strcmp(threads[x].f.name,n)==0 &&  threads[x].status==THREAD_RUNNING)
        {
#ifdef FLUX_WINDOWS
                SuspendThread(threads[x].thread);
#else
       		pthread_kill(threads[x].thread,SIGRTMIN+2);             
#endif   
            threads[x].status=THREAD_SUSPENDED;
            return 1;
        }
    }
    return 0;   
}

int interpreter::suspend_thread(int n)
{
    for(int x=0;x<nthreads;x++)
    {
        if (threads[x].thread_number==n)
        {
#ifdef FLUX_WINDOWS
                SuspendThread(threads[x].thread);
#else
       		pthread_kill(threads[x].thread,SIGRTMIN+2);
#endif   
            threads[x].status=THREAD_SUSPENDED;
            return 1;
        }
    }
    return 0;   
}

int interpreter::revive_thread(int n)
{
    for(int x=0;x<nthreads;x++)
    {
        if (threads[x].thread_number==n)
        {
#ifdef FLUX_WINDOWS
                ResumeThread(threads[x].thread);           
#else
       		pthread_kill(threads[x].thread,SIGRTMIN+1);             
#endif   
            calculating++;
            threads[x].status=THREAD_RUNNING;
            return 1;
        }
    }
    return 0;	
}


int interpreter::revive_thread(char *n)
{
    for(int x=0;x<nthreads;x++)
    {
        if (strcmp(threads[x].f.name,n)==0 && threads[x].status==THREAD_SUSPENDED)
        {
#ifdef FLUX_WINDOWS
                ResumeThread(threads[x].thread);           
#else
       		pthread_kill(threads[x].thread,SIGRTMIN+1);             
#endif
            ::calculating++;
            threads[x].status=THREAD_RUNNING;
            return 1;
        }
    }
    return 0;	
}

int interpreter::kill_thread(char *n)
{
    for(int x=0;x<nthreads;x++)
    {
        if (strcmp(threads[x].f.name,n)==0 && threads[x].status!=THREAD_FINISHED && threads[x].status!=THREAD_KILLED)
        {
#ifdef FLUX_WINDOWS
                TerminateThread(threads[x].thread,SIGABRT);                    
#else
       		pthread_kill(threads[x].thread,SIGABRT);             
#endif   
            threads[x].status=THREAD_KILLED;
                ::calculating--; 
            return 1;
        }
    }
    return 0;
}


int interpreter::kill_all(void)
{
	int x;
int killed=0;
    for(x=0;x<nthreads;x++)
    {
		if (threads[x].status!=THREAD_KILLED)killed++;
	        threads[x].status=THREAD_KILLED;
		threads[x].done=1;
    }
    for(x=0;x<nthreads;x++)
    {
#ifdef FLUX_WINDOWS
		TerminateThread(threads[x].thread,SIGABRT);
#else
   		pthread_kill(threads[x].thread,SIGABRT);
#endif   
    }
    return killed;
}

void interpreter::wait_for_terminate()
{
	int found=0;
	while(1)
	{
		found=0;
		for(int x=0;x<nthreads;x++)
		{
			if (threads[x].status==THREAD_RUNNING)
			{
				found=1;
			}
		}
		if (found==0)return;
		#ifdef FLUX_WINDOWS
	        Sleep(100);
		#else
		timespec t,tt;
		t.tv_sec=0;
		t.tv_nsec=10000000;
		nanosleep(&t,&tt);
		#endif
	}
}


int interpreter::suspend_all(void)
{
    int suspended=0;
    for(int x=0;x<nthreads;x++)
    {
	    if (threads[x].status==THREAD_RUNNING)
		{
			suspended++;
			suspend_thread(threads[x].thread_number);
		}
    }
    return suspended;
}

int interpreter::resume_all(void)
{
	int resumed=0;
    for(int x=0;x<nthreads;x++)
    {
		if (threads[x].status==THREAD_SUSPENDED)
		{
			resumed++;
			revive_thread(threads[x].thread_number);
		}
    }
	return resumed;
}


int interpreter::kill_thread(int n)
{
    for(int x=0;x<nthreads;x++)
    {
        if (threads[x].thread_number==n)
        {
#ifdef FLUX_WINDOWS
                TerminateThread(threads[x].thread,SIGABRT);                    
#else
       		pthread_kill(threads[x].thread,SIGABRT);             
#endif   
            threads[x].status=THREAD_KILLED;
            return 1;
        }
    }
    return 0;
}

void interpreter::new_thread(exopt *ex,int wait)
{
#ifndef FLUX_WINDOWS
	pthread_t flux_thread;
	pthread_attr_t flux_attr;
	pthread_attr_init(&flux_attr);
        pthread_create(&flux_thread,&flux_attr,execute,(void *)ex);
	pthread_detach(flux_thread);
        ex->thread=flux_thread;
#else
	   unsigned long threadId;
	   HANDLE threadHandle = CreateThread(0, // keine Security
					      0, // default-Stack
	   ThreadFunktion,
    	(LPVOID)ex, // kein Parameter
     	0, // normal erzeugen
     	&threadId // threadId
					     );
	   ex->thread=threadHandle;
#endif
	   ex->status=THREAD_RUNNING;
	   register_thread(*ex);
       if (wait)
	   {
			#ifdef FLUX_WINDOWS
            Sleep(100);
			#else
			timespec t,tt;
			t.tv_sec=0;
			t.tv_nsec=100000;
			nanosleep(&t,&tt);
			#endif
			if (ex->done){return;}
	   }

}

void interpreter::thread_eval_str(char *str,int wait)
{
	exopt *ex=(exopt *)malloc(sizeof(exopt));
	ex->done=0;
	ex->eval_str=(char *)malloc(sizeof(char)*(strlen(str)+1));
	strcpy(ex->eval_str,str);
	ex->action=EX_EVAL;
	ex->thread_number=get_thread_number();
	ex->multithread=1;
	ex->i=this;
	new_thread(ex,wait);
}

#define THREAD_STACK 2097152 
variable interpreter::eval(func_d f,opts o)
{
    variable v=init_variable();
    if (f.type!=FUNC_ERROR)
    {
        exopt *ex=(exopt *)malloc(sizeof(exopt));
	ex->o=o;
	ex->f=f;
        ex->r=init_variable();
	ex->done=0;
	ex->i=this;
        ex->thread_number=get_thread_number(); 
#ifndef FLUX_WINDOWS
	timespec t,tt;
	t.tv_sec=0;
	t.tv_nsec=100000;
#endif
	ex->multithread=0;
	ex->action=EX_FUNCTION;
	if (ex->multithread!=0)
		new_thread(ex,0);
	else
       {
#ifdef FLUX_WINDOWS
		ThreadFunktion(ex);
#else
            execute(ex);
#endif
            variable ret=ex->r;
            free(ex);
            ::calculating--;
	    return ret;
       	}
            while(ex->done!=1)
            {
#ifdef FLUX_WINDOWS
            Sleep(100);
#else
	timespec t,tt;
	t.tv_sec=0;
	t.tv_nsec=100000;
	    nanosleep(&t,&tt);
#endif    
	    if (::interrupt==1)
		{
            suspend_thread(ex->thread_number);
            ::interrupt=0;
            return v;
		}
	    else if (::interrupt==2)
		{
			::interrupt=0;
			free((void *)ex);
			//cout << "\n[Creating a background process...]\n";
			::calculating--;
			return v;
		}
            }
            variable ret=ex->r;
            free(ex);
//	    free_variable(ret);
	    ::calculating--;
	return ret;
    }
    cout << "Error! Can't evaluate \"";cout << f.name;cout << "\"\n\n";
    //exit(0);
}

double interpreter::eval_double(variable v)
{
    if (v.type==VAR_DOUBLE && v.length)
	return *((double *)v.ptr);
    return 0.;
}

void interpreter::vprint(variable a)
{
    char buffer[1024];
    if (a.type==VAR_DOUBLE && a.ptr!=NULL)
    {
		cout << *((double *)a.ptr);
    }
    else if (a.type==VAR_INT && a.ptr!=NULL)
    {
		cout << *((int *)a.ptr);
    }
    else if (a.type==VAR_STRING && a.ptr!=NULL)
    {
        cout << "\"";cout << ((char *)a.ptr);cout << "\"";
    }
}

char *interpreter::get_string(variable a)
{
	char *output=(char *)malloc(sizeof(char)*10000);
	if (a.type==VAR_DOUBLE && a.ptr!=NULL)
	{
		sprintf(output,"%g",*((double *)a.ptr));
	}
	else if (a.type==VAR_INT && a.ptr!=NULL)
	{
		sprintf(output,"%d",*((int *)a.ptr));
	}
	else if (a.type==VAR_STRING && a.ptr!=NULL)
	{
		sprintf(output,"\"%s\"",(char *)a.ptr);
	}
	else
		output[0]=0;
	return output;	
}


variable interpreter::parse_string()
{
    variable v=init_variable();
    v.type=VAR_STRING;
    v.name=NULL;
    char *str=(char *)malloc(sizeof(char));
    int slen=0;
	char last_atom;
    if (atom()=='"')
    {
    last_atom=atom();
	next_atom();
	while(1)
	{
		if (atom()=='"')
			if (last_atom=='\\')
				slen--;
			else
				break;
		if (atom()==0)break;
	    slen++;
	    str=(char *)realloc((void *)str,sizeof(char)*(1+slen));
	    str[slen-1]=(char)atom();
	    last_atom=atom();
		next_atom();
	}
    }
    next_atom();
    str[slen]=0;
	char *new_str=(char *)malloc(sizeof(char)*(strlen(str)+1));
	sprintf(new_str,"%s",str);
	free(str);
    v.ptr=(void *)new_str;
    v.length=slen+1;
    return v;
}

void interpreter::clip_whitespaces(void)
{
    const char *newline="\n";
    while(atom()==32 || atom()==10 || atom()==13 || atom()==9 || atom()==' ' || atom()==newline[0] || atom()==newline[1])
        next_atom();
}

int interpreter::check_for_string()
{
    push_token();
    if (atom()=='"')
    {
	next_atom();
	while(atom()!='"' & atom()!=0)
	    next_atom();
	if (atom()=='"')
	{
	    pop_token();
	    return 1;
	}
    }
    pop_token();
    return 0;
}



//Term: Number | Number Op2 Term | '(' Term ')' Follow
variable interpreter::parse_single_term()
{
    variable v=init_variable();
    func_d f=init_fd();
    opts o;
    const char *op2=NULL;
    if (parse_char('(')==1)
    {
		variable r=init_variable();
		r=parse_term(0);
		parse_char(')');
		return r;
    }
    else if (check_for_number())
    {
		v=parse_number(NULL);
    }
    else if(check_for_variable())
    {
		v=parse_variable();
    }
    else if (check_for_function())
    {
		f=parse_function();
		o=parse_options();
		v=eval(f,o);
		free_options(o);
    }
    else if (check_for_string())
    {
		v=parse_string();
    }
    else
    {
		next_atom();
		return v;
    }
    return v;
}


//Term: Number | Number Op2 Term | '(' Term ')' Follow
variable interpreter::parse_term(int prio)
{
    variable v=init_variable();
    func_d f=init_fd();
    opts o;
    const char *op2=NULL;
    if (parse_char('(')==1)
    {
	variable r=init_variable();
	r=parse_term(0);
	parse_char(')');
	if (parse_char(';')==0)
	{
	    r=parse_follow_term(r,prio);
	}
	return r;
    }
    else if (check_for_number())
    {
	v=parse_number(NULL);
    }
    else if(check_for_variable())
    {
	v=parse_variable();
    }
    else if (check_for_function())
    {
		f=parse_function();
		o=parse_options();
		v=eval(f,o);
		free_options(o);
    }
    else if (check_for_string())
    {
		v=parse_string();
    }
    else
    {
		next_atom();
		return v;
    }
    if ((op2=parse_op2(prio))!=NULL)
	{
	    variable c=init_variable();
	    variable temp=v;
	    switch (op2[0])
	    {
		case '+':c=parse_term(0);v=add(v,c);break;
		case '-':c=parse_term(1);v=sub(v,c);break;
		case '*':c=parse_single_term();v=mul(v,c);break;
		case '/':c=parse_single_term();v=div(v,c);break;
		case '>':c=parse_term(0);if (op2[1]=='=')v=greater_or_equal(v,c);else v=greater(v,c);break;
		case '<':c=parse_term(0);if (op2[1]=='=')v=less_or_equal(v,c);else v=less(v,c);break;
		case '=':c=parse_term(0);v=equal(v,c);break;
        case '!':c=parse_term(0);v=not_equal(v,c);break;
	    }
	    free_variable(c);
	    free_variable(temp);
	}
    if (parse_char(';')==0 && parse_char(0)==0)
    {
		v=parse_follow_term(v,prio);
    }
    return v;
}

//This returns the current atom within the current token
char interpreter::atom()
{
    if (token!=NULL && strlen(token))
        return token[0];
    return (char)0;
}

//This selects the next atom within the current token
void interpreter::next_atom()
{
    if (token!=NULL)
    {
	token++;
    }
}

//This searches for a particular char c
int interpreter::parse_char(const char c)
{
    if (atom()==0)
	return -1;
    if (atom()==c)
    {
	next_atom();
	return 1;
    } 
    return 0;
}

char *interpreter::eat_number()
{
	char *buffer=(char *)malloc(sizeof(char));
	int blen=0;
	if (atom()=='+' || atom()=='-')
	{
		blen++;
		buffer=(char *)realloc((void *)buffer,sizeof(char)*(blen+1));
		buffer[blen-1]=atom();
		next_atom();
	}
	while (atom()>=48 && atom()<=57)
	{
		blen++;
		buffer=(char *)realloc((void *)buffer,sizeof(char)*(blen+1));
		buffer[blen-1]=atom();
		next_atom();
	}
	if (atom()=='.')
	{
		next_atom();
		blen++;
		buffer=(char *)realloc((void *)buffer,sizeof(char)*(blen+1));
		buffer[blen-1]='.';
		while (atom()>=48 && atom()<=57)
		{
			blen++;
			buffer=(char *)realloc((void *)buffer,sizeof(char)*(blen+1));
			buffer[blen-1]=atom();
			next_atom();
		}
	}
	buffer[blen]=0;
	return buffer;
}

//This parses a decimal number
variable interpreter::parse_number(int *dec)
{
    char *buffer1,*buffer2,*buffer;
	int blen=0;
	double number=0.0;
	buffer1=eat_number();
	if (atom()=='e' || atom()=='E')
	{
		next_atom();
		buffer2=eat_number();
		buffer=(char *)malloc(sizeof(char)*(strlen(buffer1)+strlen(buffer2)+2));
		sprintf(buffer,"%se%s",buffer1,buffer2);
	    	free(buffer2);
		number=atof(buffer);
		free(buffer);
	}
	else
	{
		number=atof(buffer1);
	}
	free(buffer1);
	variable v=init_variable();
//	if (((int)number)==number)
//		v=int_variable((int)number);
//	else
		v=double_variable(number);
    return v;
}

//This parses a two-element operator with priority >=prio
const char *interpreter::parse_op2(int prio)
{
    char *r=NULL;
    char **p=NULL;
    static char *prio_0[]={"+","-","*","/",">=","<=",">","<","==","!="};
    static char *prio_1[]={"*","/"};
    static char *prio_2[]={"*","/"};
    if (prio==0)
		p=prio_0;
    if (prio==1)
		p=prio_1;
    if (prio==2)
		p=prio_2;
    for(int x=0;x<(prio==0?10:2);x++)
    {
	if (strstr(get_token(),p[x])==get_token())
	{
	    r=p[x];
	    for(int y=0;y<strlen(p[x]);y++)
		next_atom();
	    return r;
	}
    }
    return NULL;
}

variable interpreter::string_variable(char *x)
{
    variable v=init_variable();
    v.type=VAR_STRING;
    v.ptr=(void *)x;
    v.length=strlen(x)+1;
    v.name=NULL;
    v.compartment=com.ID;
    return v;
}


variable interpreter::double_variable(double x)
{
    variable v=init_variable();
    v.type=VAR_DOUBLE;
    v.ptr=(void *)malloc(64);
    *((double *)v.ptr)=x;
    v.length=sizeof(double);
    v.name=NULL;
    v.compartment=com.ID;
    return v;
}

variable interpreter::truth_variable(int x)
{
    variable v=init_variable();
    v.type=VAR_TRUTH;
    v.ptr=(void *)malloc(INTSIZE);
    *((int *)v.ptr)=x;
    v.length=sizeof(int);
    v.name=NULL;
    v.compartment=com.ID;
    return v;
}


variable interpreter::int_variable(int x)
{
    variable v=init_variable();
    v.type=VAR_INT;
    v.ptr=malloc(sizeof(int));
	int *ptr=(int *)v.ptr;
    *ptr=x;
    v.name=NULL;
    v.length=sizeof(int);
    v.compartment=com.ID;
    return v;
}

//This returns a pointer to the current token.
char *interpreter::get_token()
{
    return token;
}

void interpreter::push_op(int op)
{
    opos++;
    opstack=(int *)realloc((void *)opstack,sizeof(int)*opos);
    opstack[opos-1]=op;
}

void interpreter::push_number(double n)
{
    npos++;
    nstack=(double *)realloc((void *)nstack,sizeof(double)*opos);
    nstack[npos-1]=n; 
}

void interpreter::push_string(char *c)
{
    cpos++;
    cstack=(char **)realloc((void *)cstack,sizeof(char *)*cpos);
    cstack[cpos]=(char *)malloc(sizeof(char)*strlen(c));
    strcpy(cstack[cpos],c);
}

int interpreter::pop_op()
{
    int op=0;
    if (opos>0)
    {
	opos--;
	op=opstack[opos];
	opstack=(int *)realloc((void *)opstack,sizeof(int)*opos);
    }
    return op;
}

char *interpreter::pop_string()
{
    char *str=NULL;
    if (cpos>0)
    {
	cpos--;
	str=cstack[cpos];
	cstack=(char **)realloc((void *)cstack,sizeof(char *)*cpos);
    }
    return str;
}

double interpreter::pop_number()
{
    double n=0.0;;
    if (npos>0)
    {
	npos--;
	n=nstack[npos];
	nstack=(double *)realloc((void *)nstack,sizeof(double)*npos);    
    }
    return n;
}

void interpreter::free_input(void)
{
    if (input!=NULL)
	free(input);
}

char *interpreter::get_history(void)
{
    return history;
}

void interpreter::flush_history(void)
{
    hlen=0;
    free(history);
    history=(char *)malloc(sizeof(char));    
}

void interpreter::add_to_history(const char *i)
{
    hlen+=strlen(i);
    history=(char *)realloc((void *)history,hlen+1);
    strcpy(history+hlen-strlen(i),i);
    history[hlen]=0;    
}
	
void interpreter::load_input(char *i,int flush)
{
    if (input!=NULL && flush)
    {
	free(input);
    }
    input=(char *)malloc(sizeof(char)*(strlen(i)+1));
    strcpy(input,i);
    token=input;
    if (flush)
        flush_tokens();
}

void interpreter::flush_tokens()
{
    tokens=NULL;
    n_token=0;
}

void interpreter::console(void)
{
    char buffer[2048];
    char *tmp;
    signal(SIGABRT,new_flux_handler);
    signal(SIGINT,handler);
    fflush(stdin);
	buffer[0]=0;
    while(1)
    {
	  
	    wait_for_terminate();
	    vprint(rv);
	    cout << "\nflux>";
	    fflush(stdout);
		if (cin.getline(buffer,2048))
		{
			tmp=(char *)malloc(sizeof(char)*(strlen(buffer)+1));
			strcpy(tmp,buffer);
			thread_eval_str(tmp,1);
		}
    }
}

void interpreter::print_token(void)
{
    if (token!=NULL)
    {
	cout << "\t>" ;
	cout << token;
	cout << "; \n";
    }
    else
    {
    cout << "\t>EndOfF\n";
    }
}

void interpreter::next_symbol(void)
{
    token++;
}

//This parses an expression/token of form e.g. a=5+4*c/7;
void interpreter::parse_token(void)
{
    switch (token[0])
    {
	case 2:cout << "a\n";
	default:;
    };
}

void handler(int sig)
{
    signal(SIGINT,handler);
    signal(SIGABRT,handler);
    signal(SIGINT,handler);
    if(i->suspend_all()==0)
	{
		i->kill_all();
		cout << "\n[Interrupt: Exiting...]\n";
#ifdef FLUX_WINDOWS
        ExitProcess(0);
#else
        exit(0);
#endif
	}
	cout << "\n[Interrupt: Suspending all processes...]\n";
	return;
}

variable constants(opts o,interpreter *i)
{
    variable v=init_variable();
    for(int x=0;x<i->var_length;x++)
    {
        if (i->vars[x].constant==1){
	    cout << i->vars[x].name;*i << " = ";i->vprint(i->vars[x]);*i << "\n";}
    }
    return v;
}

variable functions(opts o,interpreter *i)
{
    variable v=init_variable();
    for(int x=0;x<i->fd_length;x++)
    {
	cout <<  i->fd[x].name;*i << "\n\n";
    }
    return v;
}

variable variables(opts o,interpreter *i)
{
    variable v=init_variable();
    for(int x=0;x<i->var_length;x++)
    {
        if (i->vars[x].constant!=1)
        {
	    *i << i->vars[x].name;*i << " = ";i->vprint(i->vars[x]);*i << "\n";    
        }   
    }
    return v;
}

variable declare(opts o,interpreter *i)
{
    variable v=init_variable();
    cout << "Starting...\n";
    const char *usage="[Usage: declare(\"[name]\",\"[type]\")]\nSupported types:\ndouble\t\t Double-precision number\nstring\t\tA character string\n";
    if (o.size==2)
    {
	if (o.options[0].type==VAR_STRING && o.options[1].type==VAR_STRING)
	{
	    char *type=(char *)o.options[1].ptr;
	    char *name=(char *)o.options[0].ptr;
	    if (strcmp(type,"double")==0)
		i->declare_variable(name,VAR_DOUBLE);
	    else if (strcmp(type,"string")==0)
                i->declare_variable(name,VAR_STRING);
	    else *(i->os) << "Error! Type not supported!\n" << usage;
	}
	else
	    *(i->os) << "Error! Wrong type of parameters supplied!\n" << usage;
    }
    else
    {
	*(i->os) << "Error! Wrong number of parameters!\n" << usage;
    }
    return v;
}

variable interpreter::eval_code(const char *str,int log)
{
    char *temp=token;
    char *my=(char *)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(my,str);
    variable zz=rv;
	variable ret;
    token=my;
    rv=init_variable();
    parse_expression(0,log);
//    vprint(rv);
    token=temp;
    free(my);
    init_stacks();
    flush_tokens();
	ret=rv;
    rv=zz; 
	return ret;
}

void interpreter::eval_str(const char *str,int log)
{
    char *temp=token;
    char *my=(char *)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(my,str);
    variable zz=rv;
    token=my;
    free_variable(rv);
    rv=init_variable();
    parse_expression(0,log);
    token=temp;
    free(my);
    init_stacks();
    flush_tokens();
}

void interpreter::eval_str_compartment(const char *str,int log)
{
    push_compartment();
    com=create_compartment(com.ID);
    eval_str(str,log);
    delete_compartment(com);
    pop_compartment();
}

void interpreter::interpret(void)
{
    init_stacks();
    free_variable(rv);
    rv=init_variable();
    parse_expression(0,1);
}

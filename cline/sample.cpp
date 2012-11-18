#include<stdio.h>
#include<iostream>

#ifndef VISUAL_CPP
#include<unistd.h>
#else

#endif

#ifdef CIRCUIT_SIM
#include"../circuit.h"
#else
#include"../interface.h"
#endif
#include "../buffers_interface.h"
//Some global options here
//
int silent=0;//Controlled by the -s switch. Shuts down input,output and error channels.
int daemonize=0;//Daemonizes the program, i.e. makes it run in the background. Controlled by the -d switch.
int interpret_file=0;//Interpret a file???
int redirect=0;//Redirect stdout and stderr to a logfile???
char *filename=NULL;
char *redirect_file=NULL;
int do_eval=0;
char *eval_string=NULL;

void process_flag(char flag,char *optarg)
{
		switch (flag)
		{
			case 's':
				silent=1;
				break;
			case 'e':
				if (optarg!=NULL)
				{
					do_eval=1;
					eval_string=optarg;
				}
				break;
			case 'd':
				daemonize=1;
				break;
			case 'r':
				if (optarg!=NULL)
				{
					redirect=1;
					redirect_file=optarg;
				}
				break;
			case 'f':
				if (optarg!=NULL)
				{
					filename=optarg;
					interpret_file=1;
				}
				break;
			default:
				
				break;
		}
}

//Makes just what it says.
void parse_command_line(int argc,char *argv[])
{
	int c=0;
	extern char *optarg;
	extern int optind, optopt;
#ifndef VISUAL_CPP
	while ((c=getopt(argc,argv,"e:sdf:r:"))!=-1)
	{
		process_flag(c,optarg);
	}
#else
	int x;
	char flag[1024];
	for(x=0;x<argc;x++)
	{
		if(sscanf(argv[x],"-%s",flag))
		{
			process_flag(flag[0],x<argc-1?argv[x+1]:NULL);
		}
	}
#endif
}

int main(int argc,char *argv[])
{
#ifdef VISUAL_CPP
	//cout.rdbuf()->setbuf(0,0);
#endif
    parse_command_line(argc,argv);
    if(silent)			
    {
	    cout << "Going to silent mode...\n";
	    fclose(stdout);
	    fclose(stdin);
	    fclose(stderr);
    }
    if (redirect)
    {
	    cout << "Redirecting stdout/stderr to file \"" << redirect_file << "\"\n";
	    fflush(stdout);
	    fclose(stderr);
	    fclose(stdin);
	    freopen(redirect_file,"w+",stdout);
    }
    if (daemonize)
    {
	    cout << "Daemonizing myself...Boooo!\n";
    }
    interpreter i;
#ifdef CIRCUIT_SIM
    init_circuit_funcs(&i);
#else
    init_flux_funcs(&i);
#endif
	init_buffer_funcs(&i);
    set_interpreter(&i);
    if (do_eval)
    {
	    i.thread_eval_str(eval_string,1);
    }
    if (interpret_file)
    {
	    cout << "Interpreting file " << filename << "\n";
	    i.interpret_file(filename);
    }
    if (i.get_output())
    {
		cout << i.get_output() << "\n";
		i.flush_output();
    }
    i.console();
    exit(0);
	return 0;
}

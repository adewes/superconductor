#include "buffers_interface.h"

variable flux_new_buffer(opts o,interpreter *i)
{
    int buf=0;
	char *str;
	char *filename;
    variable v=init_variable();
	if (i->cast_string(&str,o,0))
	{
		if (i->cast_string(&filename,o,1))
			v=i->int_variable((buf=new_buffer(str,filename)));
		else
			v=i->int_variable((buf=new_buffer(str)));
	}
	else
		v=i->int_variable((buf=new_buffer()));
    return v;
}

variable math_exp(opts o,interpreter *i)
{
  double arg=0.0;
  variable v=init_variable();
  if (i->cast_double(&arg,o,0))
    {
      v=i->double_variable(exp(arg));
    }
  return v;
}

variable math_arctan(opts o,interpreter *i)
{
  double arg=0.0;
  variable v=init_variable();
  if (i->cast_double(&arg,o,0))
    {
      v=i->double_variable(atan(arg));
    }
  return v;
}


variable math_ln(opts o,interpreter *i)
{
  double arg=0.0;
  variable v=init_variable();
  if (i->cast_double(&arg,o,0))
    {
      v=i->double_variable(log(arg));
    }
  return v;
}

variable math_abs(opts o,interpreter *i)
{
    double arg=0.0;
    variable v=init_variable();
    if (i->cast_double(&arg,o,0))
    {
	v=i->double_variable(fabs(arg));
    }
    return v;
}

variable math_sin(opts o,interpreter *i)
{
  double arg=0.0;
  variable v=init_variable();
  if (i->cast_double(&arg,o,0))
    {
      v=i->double_variable(sin(arg));
    }
  return v;
}

variable math_cos(opts o,interpreter *i)
{
  double arg=0.0;
  variable v=init_variable();
  if (i->cast_double(&arg,o,0))
    {
      v=i->double_variable(cos(arg));
    }
  return v;
}

variable math_sqrt(opts o,interpreter *i)
{
  double arg=0.0;
  variable v=init_variable();
  if (i->cast_double(&arg,o,0))
    {
      v=i->double_variable(sqrt(arg));
    }
  return v;
}


//This plots a buffer using gnuplot
variable flux_plot_buffer(opts o,interpreter *i)
{
    variable v=init_variable();
    int buffer=0;
    char *filename=NULL;
    if (i->cast_int(&buffer,o,0) && i->cast_string(&filename,o,1))
    {
        plot_buffer(buffer,filename);
    }    
    return v;
}

//This plots a graph using gnuplot
variable flux_plot_graph(opts o,interpreter *i)
{
    variable v=init_variable();
    int buffer=0;
    char *filename=NULL;
    if (i->cast_int(&buffer,o,0) && i->cast_string(&filename,o,1))
    {
        plot_graph(buffer,filename);
    }    
    return v;
}

variable flux_flush_state(opts o,interpreter *i)
{
    variable v=init_variable();
    int buf=0;
    if (i->cast_int(&buf,o,0))
        v=i->double_variable(flush_state(buf));
    return v;    
}

variable flux_write_buffer(opts o,interpreter *i)
{
	variable v=init_variable();
	int buffer=0;
	char *ptr;
	if (i->cast_int(&buffer,o,0) && i->cast_string(&ptr,o,1))
	{
		write_buffer(ptr,buffer);
	}	
	return v;
}
variable flux_approx(opts o,interpreter *i)
{
	variable v=init_variable();
	int b=0;
	double x=0.0;
	if (i->cast_int(&b,o,0) && i->cast_double(&x,o,1))
	{
		return i->double_variable(approx(b,x));
	}
	return v;
}

variable flux_set_value(opts o,interpreter *i)
{
	variable v=init_variable();
	int b=0;
	int m=0;
	int x=0;
	double y;
	if (i->cast_int(&b,o,0) && i->cast_int(&m,o,1) && i->cast_int(&x,o,2) && i->cast_double(&y,o,3))
	{
		set_value(b,m,x,y);
	}
	return v;
}

variable flux_buffer_length(opts o,interpreter *i)
{
	variable v=init_variable();
	int b=0;
	if (i->cast_int(&b,o,0))
	{
		return i->int_variable(buffer_length(b));
	}
	return v;
}

variable flux_get_value(opts o,interpreter *i)
{
	variable v=init_variable();
	int b=0;
	int m=0;
	int x=0;
	if (i->cast_int(&b,o,0) && i->cast_int(&m,o,1) && i->cast_int(&x,o,2))
	{
		return i->double_variable(get_value(b,m,x));
	}
	return v;
}

variable flux_buffer_flip(opts o,interpreter *i)
{
	variable v=init_variable();
	int b=0;
	int x=0;
	int y=0;
	if (i->cast_int(&b,o,0) && i->cast_int(&x,o,1) && i->cast_int(&y,o,2))
	{
		flip_xy(b,x,y);
	}
	return v;
}


variable flux_set_flush(opts o,interpreter *i)
{
    variable v=init_variable();
    int buf=0;
    int state=0;
    if (i->cast_int(&buf,o,0) && i->cast_int(&state,o,1))
    {
        set_flush(buf,state);    
    }         
    return v;
}

variable flux_flush_buffer(opts o,interpreter *i)
{
    variable v=init_variable();
    int buf=0;
    if (i->cast_int(&buf,o,0))
        flush_buffer(buf);
    return v;    
}

variable flux_buffer_new_series(opts o,interpreter *i)
{
	variable v=init_variable();
	int b=0;
	if (i->cast_int(&b,o,0))
	{
		buffer_new_series(b);
	}
	return v;
}


variable flux_load_buffer(opts o,interpreter *i)
{
    variable v=init_variable();
    int buf=0;
    char *str;
    if (i->cast_int(&buf,o,0) && i->cast_string(&str,o,1))
	load_buffer(str,buf);
    return v;
}

variable flux_add_value(opts o,interpreter *i)
{
    variable v=init_variable();
    double x=0.0;
    double y=0.0;
    double z=0.0;
    int buf=-1;
    if (i->cast_int(&buf,o,0) && i->cast_double(&x,o,1) && i->cast_double(&y,o,2))
    {
    	if (i->cast_double(&z,o,3))
				buffer_add_value(buf,x,y,z);
		  else
		  	buffer_add_value(buf,x,y);
    }
    return v;
}

variable flux_set_style(opts o,interpreter *i)
{
	variable v=init_variable();
	int buffer=0;
	int style=0;
	if (i->cast_int(&buffer,o,0) && i->cast_int(&style,o,1))
		set_style(buffer,style);
	return v;
}

//This is a very important function. It writes all registered buffers, all variables and the command history to the directory specified in the first argument. Also, it generates eps-files from the switching histogram buffer and log-normalized escape rate buffer.
variable flux_write_all_data(opts o,interpreter *i)
{
    char *s;
    char *vars;
    FILE *f;
    char buffer[1024];
    variable v=init_variable();
    if (i->cast_string(&s,o,0))
    {
        sprintf(buffer,"mkdir \"./data/%s\"",s);
        system(buffer);
        sprintf(buffer,"./data/%s/history.ini",s);
        f=fopen(buffer,"w");
        if(f==NULL)
            return v;
        if (i->get_history()!=NULL)
            fwrite((void *)(i->get_history()),strlen((i->get_history()))+1,1,f);
        fclose(f);
        for(int x=0;x<n_buffers();x++)
        {
				if (buffer_filename(x)!=NULL && buffer_empty(x)==0)
				{
					sprintf(buffer,"write_buffer(%d,\"./data/%s/%s\");",x,s,buffer_filename(x));
					i->eval_str(buffer,1);
					sprintf(buffer,"plot_buffer(%d,\"./data/%s/%s.eps\");",x,s,buffer_filename(x));
					i->eval_str(buffer,1);
				}
        }
        sprintf(buffer,"./data/%s/vars.ini",s);
        f=fopen(buffer,"w");
        if(f==NULL)
            return v;
        vars=i->output_all_variables();
        fwrite((void *)vars,strlen(vars),1,f);
        fclose(f);
        free(vars);
    }
    return v;    
}


void init_buffer_funcs(interpreter *i)
{
	i->add_function(flux_new_buffer,"new_buffer");
    i->add_function(flux_add_value,"add_value");
	i->add_function(flux_plot_buffer,"plot_buffer");
	i->add_function(flux_plot_graph,"plot_graph");
	i->add_function(flux_set_flush,"set_flush");
    i->add_function(flux_write_buffer,"write_buffer");
    i->add_function(flux_flush_buffer,"flush_buffer");
    i->add_function(flux_flush_state,"flush_state");
    i->add_function(flux_buffer_flip,"flip");
    i->add_function(flux_approx,"approx");
    i->add_function(flux_get_value,"get_value");
    i->add_function(flux_set_value,"set_value");
    i->add_function(flux_buffer_length,"buffer_length");
    i->add_function(flux_load_buffer,"load_buffer");
    i->add_function(flux_write_all_data,"write_all");
	i->add_function(flux_set_style,"set_style");

//Some math functions...

    i->add_function(math_sqrt,"sqrt");
    i->add_function(math_ln,"ln");
    i->add_function(math_sin,"sin");
    i->add_function(math_abs,"abs");
    i->add_function(math_cos,"cos");
    i->add_function(math_exp,"exp");
    i->add_function(math_arctan,"arctan");

	i->add_int_constant(STYLE_LINE,"STYLE_LINE");
	i->add_int_constant(STYLE_POINTS,"STYLE_POINTS");
	i->add_int_constant(STYLE_HISTO,"STYLE_HISTO");
	i->add_int_constant(BUFFER_IC_H,"BUF_IC_H");
    i->add_int_constant(BUFFER_IVC,"BUF_IVC");
    i->add_int_constant(BUFFER_PHI,"BUF_PHI");
	i->add_int_constant(BUFFER_PHONONS,"BUF_PHONONS");
    i->add_int_constant(BUFFER_DPHI_DX,"BUF_DPHI_DX");
    i->add_int_constant(BUFFER_DPHI_DT,"BUF_DPHI_DT");
    i->add_int_constant(BUFFER_PHI_CRIT,"BUF_PHI_CRIT");
    i->add_int_constant(BUFFER_DPHI_DX_CRIT,"BUF_DPHI_DX_CRIT");
    i->add_int_constant(BUFFER_IC_SW,"BUF_IC_SW");
    i->add_int_constant(BUFFER_IC_BINS,"BUF_IC_BINS");
	i->add_function(flux_buffer_new_series,"new_series");
    i->add_int_constant(BUFFER_IC_LOG,"BUF_IC_LOG");
    i->add_int_constant(BUFFER_ESC_RATE,"BUF_ESC_RATE");
    i->add_int_constant(BUFFER_FFT,"BUF_FFT");           
	i->add_int_constant(BUFFER_ACT_ENERGY,"BUF_ACT_ENERGY");           
	i->add_double_constant(3.14159265,"PI");

}

#include"buffers.h"

int BUFFERS=0;
buffer *buffers=NULL;
GRAPH *graphs=NULL;
int n_graphs=0;

GRAPH get_graph(int x)
{
	GRAPH g;
	g.buffers=NULL;
	g.changed=NULL;
	g.n_buffers=-1;
	if (x<0 || x>=n_graphs)return g;
	return graphs[x];
}

void set_graph(GRAPH g,int x)
{
	if (x<0 || x>=n_graphs)return;
	graphs[x]=g;
}


int get_n_graphs()
{
	return n_graphs;
}

int new_graph(char *name)
{
	GRAPH g;
	if (graphs==NULL)
		graphs=(GRAPH *)malloc(sizeof(GRAPH));
	n_graphs++;
	graphs=(GRAPH *)realloc((void *)graphs,sizeof(GRAPH)*n_graphs);
	g.n_buffers=0;
	g.buffers=(int *)malloc(sizeof(int));
	g.changed=(int *)malloc(sizeof(int));
	g.name=name;
	g.WINDOW=NULL;
	g.has_window=0;
	graphs[n_graphs-1]=g;
	return n_graphs-1;
}

void graph_add_buffer(int graph,int buffer)
{
	if (graph>=n_graphs || graph<0 || buffer>=BUFFERS || buffer<0)
		return;
	graphs[graph].n_buffers++;
	graphs[graph].buffers=(int *)realloc((void *)graphs[graph].buffers,sizeof(int)*graphs[graph].n_buffers);
	graphs[graph].changed=(int *)realloc((void *)graphs[graph].changed,sizeof(int)*graphs[graph].n_buffers);
	graphs[graph].buffers[graphs[graph].n_buffers-1]=buffer;
	graphs[graph].changed[graphs[graph].n_buffers-1]=0;
}

int new_buffer()
{
	BUFFERS++;
	buffers=(buffer *)realloc((void *)buffers,sizeof(buffer)*BUFFERS);
	init_buffer(BUFFERS-1);
	buffers[BUFFERS-1].description=NULL;
	buffers[BUFFERS-1].name=NULL;
	buffers[BUFFERS-1].filename=NULL;
	buffers[BUFFERS-1].style=STYLE_LINE;
	return BUFFERS-1;
}

void set_style(int buffer,int style)
{
	if (buffer>=BUFFERS || buffer<0)return;
	buffers[buffer].style=style;
	buffers[buffer].changed++;
}

int new_buffer(char *name)
{
	int b=new_buffer();
	buffers[b].name=(char *)malloc(sizeof(char)*(strlen(name)+1));
	strcpy(buffers[b].name,name);
	return b;
}

int new_buffer(char *name,char *filename)
{
	int b=new_buffer();
	buffers[b].name=(char *)malloc(sizeof(char)*(strlen(name)+1));
	buffers[b].filename=(char *)malloc(sizeof(char)*(strlen(filename)+1));
	strcpy(buffers[b].name,name);
	strcpy(buffers[b].filename,filename);
	return b;
}


void set_description(int buffer,char *desc)
{
	if (buffer>=BUFFERS)return;
	if (buffers[buffer].description!=NULL)
		free(buffers[buffer].description);
	buffers[buffer].description=(char *)malloc(sizeof(char)*(strlen(desc)+1));
	strcpy(buffers[buffer].description,desc);
}


void set_name(int buffer,char *name)
{
	if (buffer>=BUFFERS)return;
	if (buffers[buffer].name!=NULL)
		free(buffers[buffer].name);
	buffers[buffer].name=(char *)malloc(sizeof(char)*(strlen(name)+1));
	strcpy(buffers[buffer].name,name);
}


char *read_file(char *filename)
{
	FILE *ff;
	char buffer[256];
	char *content;
	int len=0;
	int rlen=0;
	ff=fopen(filename,"r");
	if (ff==NULL)
	{
	  cout << "Can't open file " << filename << "\n";
		return NULL;
	}
	content=(char *)malloc(sizeof(char));
	while((rlen=fread((void *)buffer,1,255,ff)))
	{
		len+=rlen;
		content=(char *)realloc((void *)content,len+1);
		memcpy((void *)(content+len-rlen),(void *)buffer,rlen);
		content[len]=0;
	}
	fclose(ff);
	return content;
}

double buffer_min(int buf,int n)
{
	return buffers[buf].min.values[n];
}

double buffer_max(int buf,int n)
{
	return buffers[buf].max.values[n];
}


double buffer_min(buffer b,int n)
{
	return b.min.values[n];
}

int n_buffers(void)
{
	return BUFFERS;
}

double buffer_max(buffer b,int n)
{
	return b.max.values[n];
}

void buffer_invert(int buffer,int n)
{
	for(int x=0;x<buffers[buffer].length;x++)
	{
		buffers[buffer].elements[x].values[n]=-buffers[buffer].elements[x].values[n];
	}
}

int wait_for_unlock(int b)
{
	int cnt=0;
#ifndef FLUX_WINDOWS
	timespec t,tt;
	t.tv_sec=0;
	t.tv_nsec=1.0e4;
#endif
	while(buffers[b].locked && cnt++<100)
#ifdef FLUX_WINDOWS
		Sleep(10);	
#else
	nanosleep(&t,&tt);
#endif
	if (cnt==100)
	  return 0;
	else
	  return 1;
}

void buffer_erase_rect(int b,rect r)
{
  cout << "Erasing...\n";
	if(wait_for_unlock(b)==0)return;
	buffers[b].locked=1;
	int l=buffers[b].length;
	for(int x=0;x<buffers[b].length;x++)
	{
		if (buffers[b].elements[x].values[0]>=r.left && buffers[b].elements[x].values[0]<=r.right )
			if( buffers[b].elements[x].values[1]>=r.bottom && buffers[b].elements[x].values[1]<=r.top)
			{
			  cout << "Deleting " << x << "\n";
			  if (delete_element(b,x)!=-1)
				x--;
			}
	}
	buffers[b].locked=0;
}

void set_approx(int buffer,int x,int save)
{
  
}

double approx(int buffer,double v)
{
	return approx(buffer,v,1);
}

//Calculates an approximate value of buffer [buffer] at position [x].
double approx(int buffer,double v,int i)
{
	int min=0;
	int max=0;
	double dist_min=0xFFFFFF;
	double dist_max=0xFFFFFF;
	for(int x=0;x<buffers[buffer].length;x++)
	{
		if (fabs(v-buffers[buffer].elements[x].values[0])<=dist_min && v-buffers[buffer].elements[x].values[0]<=0)
		{
			dist_min=fabs(v-buffers[buffer].elements[x].values[0]);
			min=x;
		}
	       	if (fabs(v-buffers[buffer].elements[x].values[0])<=dist_max && v-buffers[buffer].elements[x].values[0]>=0)
		{
			dist_max=fabs(v-buffers[buffer].elements[x].values[0]);
			max=x;
			}
	}
	if (min>=0 && max>=0 && min!=max)
	{ 
		double frac=(v-buffers[buffer].elements[max].values[0])/(buffers[buffer].elements[min].values[0]-buffers[buffer].elements[max].values[0]);
		return 	buffers[buffer].elements[max].values[i]+(buffers[buffer].elements[min].values[i]-buffers[buffer].elements[max].values[i])*frac;
	}
	if (max>=0)
		return buffers[buffer].elements[max].values[i];
	if (min>=0)
		return buffers[buffer].elements[min].values[i];
	return 0.0;
}

int load_buffer(char *filename,int buf)
{
	char *content;
	char *file;
	char *endptr;
	char *ptr,*pjotr;
	double *values=NULL;
	int n_values=0;
	double x,y,z;
	cout << "Loading buffer from \"" << filename << "\"\n";
	file=read_file(filename);
	content=file; 
	if (content==NULL)
	{
	  cout << "Error while reading file.\n";
		return -1; 
	}
	buffer_new_series(buf);
	char *line=NULL;
	int n_pos=strcspn(content,"\n");
	line=content;
	line[n_pos]=0;
	while (line!=NULL)
	{
		ptr=strtok(line,"\t ");
		n_values=0;
		values=(double *)malloc(sizeof(double));
		while(ptr!=NULL)
		{
#ifndef VISUAL_CPP
			x=strtold(ptr,&endptr);
#else
			x=atof(ptr);
#endif
			n_values++;
			values=(double *)realloc((void *)values,sizeof(double)*n_values);
			values[n_values-1]=x;
			ptr=strtok(NULL,"\t ");
		}
		if (n_values>1)
			buffer_add_value(buf,values,n_values);
		line+=n_pos+1;
		n_pos=strcspn(line,"\n");
		if (n_pos==strlen(line))
			line=NULL;
		else
			line[n_pos]=0;
	}
	if (file!=NULL)
		free(file);
	cout << "Loaded " << buffer_length(buf) << " values...\n";
	return 0;
}


//This writes a buffer to a file and returns the number of files written
int write_buffer(char *filename,int buf)
{
	char fbuffer[1024];
	int series=1;
	buffer mybuf;
	char buffer[10024];
	char sbuffer[1024];
	FILE *ff;
	mybuf=get_buffer(buf);
	sprintf(fbuffer,"%s_%.2d.txt",filename,series);     
	ff=fopen(fbuffer,"w");
	if (ff==NULL)
	{
		cout << "Error in writing buffer: Can't open file:" << fbuffer << "\n";
		return 0;
	}
	sprintf(buffer,"#%s\n",mybuf.name);
	fwrite((void *)buffer,strlen(buffer),1,ff);
	for(int x=0;x<mybuf.length;x++)
	{
		if (mybuf.elements[x].type==ELEMENT_EMPTY)
		{
			fclose(ff);
			series++;
			sprintf(fbuffer,"%s_%.2d.txt",filename,series);     
			ff=fopen(fbuffer,"w");
		}
		else if (mybuf.elements[x].type==ELEMENT_XY)
		{
				sprintf(buffer,"%Lg\t%g\t%g\n",mybuf.elements[x].x,mybuf.elements[x].y,mybuf.elements[x].z);
		}
		else if (mybuf.elements[x].type==ELEMENT_MULTIVAL)
		{
			buffer[0]=0;
			for(int y=0;y<mybuf.elements[x].n_values;y++)
			{
				if (y!=mybuf.elements[x].n_values-1)
					sprintf(sbuffer,"%g\t",mybuf.elements[x].values[y]);
				else
					sprintf(sbuffer,"%g",mybuf.elements[x].values[y]);
				strcat(buffer,sbuffer);
			}
			sprintf(sbuffer,"\n");
			strcat(buffer,sbuffer);
		}
		if (strlen(buffer)>0)
			if(fwrite((void *)buffer,strlen(buffer),1,ff)==0)
				return 0;
	}
	fclose(ff);
	return series;
}

void plot_graph(int graph,char *filename)
{
	char buffer[1024];
	char title[1024];
	if (graph>=n_graphs || graph<0)return;
	int *series=(int *)malloc(sizeof(int)*graphs[graph].n_buffers);
	FILE *w;
	w=fopen("plot.gnu","w");
	sprintf(buffer,"set terminal postscript landscape color;set grid;set key off;set out \"%s\";plot ",filename);
	fwrite((void *)buffer,strlen(buffer),1,w);
	for(int x=0;x<graphs[graph].n_buffers;x++)
	{
		sprintf(buffer,"plot_tmp_%.2d",x);
		series[x]=write_buffer(buffer,graphs[graph].buffers[x]);
		for(int y=1;y<=series[x];y++)
		{
			sprintf(title,"%s - %.2d",buffer_name(graphs[graph].buffers[x]),y);
			sprintf(buffer,"\"plot_tmp_%.2d_%.2d.txt\" using 1:2 with lines title \"%s\"",x,y,title);
			fwrite((void *)buffer,strlen(buffer),1,w);
			if (y<series[x] || (x<graphs[graph].n_buffers-1))
				sprintf(buffer," , ");
			else
				sprintf(buffer," ; ");
			fwrite((void *)buffer,strlen(buffer),1,w);
		}
	}
	sprintf(buffer,"exit;");
	fwrite((void *)buffer,strlen(buffer),1,w);
	fclose(w);
#ifndef FLUX_WINDOWS
	sprintf(buffer,"gnuplot plot.gnu");
	system(buffer);
#else
	sprintf(buffer,"wgnuplot plot.gnu");
	system(buffer);
#endif
}


void plot_buffer(int buf,char *filename)
{
	char buffer[1024];
	char title[1024];
	cout << "Plotting buffer " << buffer_name(buf) << " to file \"" << filename << "\"\n";
	FILE *w;
	w=fopen("plot.gnu","w");
	int series=write_buffer("plot_tmp",buf);
	sprintf(buffer,"set terminal postscript landscape color;set grid;set out \"%s\";plot ",filename);
	fwrite((void *)buffer,strlen(buffer),1,w);
	for(int x=1;x<=series;x++)
	{
		sprintf(title,"%s - %.2d",buffer_name(buf),x);
		sprintf(buffer,"\"plot_tmp_%.2d.txt\" using 1:2 with lines title \"%s\"",x,title);
		fwrite((void *)buffer,strlen(buffer),1,w);
		if (x<series)
			sprintf(buffer," , ");
		else
			sprintf(buffer," ; ");
		fwrite((void *)buffer,strlen(buffer),1,w);
	}
	sprintf(buffer,"exit;");
	fwrite((void *)buffer,strlen(buffer),1,w);
	fclose(w);
#ifndef FLUX_WINDOWS
	sprintf(buffer,"gnuplot plot.gnu");
	system(buffer);
#else
	sprintf(buffer,"wgnuplot plot.gnu");
	system(buffer);
#endif
}

element get_element(int b,int x)
{
	element e;
	e.n_values=0;
	e.type=ELEMENT_EMPTY;
	if (b<0 || b>=BUFFERS)return e;
	if (x<0 || x>=buffers[b].length)return e;
	return buffers[b].elements[x];
}


char *buffer_name(int buf)
{
	if (buf<0 || buf>=BUFFERS)
		return NULL;
	if (buffers[buf].name!=NULL)
		return buffers[buf].name;
	switch (buf)
	{
		case BUFFER_IVC:return "V(I)";
		case BUFFER_PHI:return "Phase";
		case BUFFER_DPHI_DX:return "Magnetic field";
		case BUFFER_DPHI_DT:return "Voltage";
		case BUFFER_IC_BINS:return "Histogram";
		case BUFFER_IC_SW:return "Switching currents";
		case BUFFER_IC_H:return "Ic(H)";
		case BUFFER_IC_LOG:return "ln(T)^2/3";
		case BUFFER_ESC_RATE:return "Escape rate";
		case BUFFER_Q:return "charge";
		case BUFFER_DQ_DX:return "dQ/dx";
		case BUFFER_DQ_DT:return "dQ/dt";		
		case BUFFER_FFT:return "log(power)";
		case BUFFER_PHONONS:return "phonons";
		default:return "undefined";
	};
	return "undefined";
}

char *buffer_filename(int buf)
{
	if (buffers[buf].filename!=NULL)
		return buffers[buf].filename;
	if (buffers[buf].name!=NULL)
		return buffers[buf].name;
	switch (buf)
	{
		case BUFFER_IVC:return "ivc";
		case BUFFER_PHI:return "phi";
		case BUFFER_DPHI_DX:return "dphi_dx";
		case BUFFER_DPHI_DT:return "dphi_dt";
		case BUFFER_IC_BINS:return "ic_bins";
		case BUFFER_IC_SW:return "ic_sw";
		case BUFFER_IC_H:return "ic";
		case BUFFER_IC_LOG:return "ic_log";
		case BUFFER_ESC_RATE:return "esc_rate";
		case BUFFER_Q:return "q";
		case BUFFER_DQ_DX:return "dq_dx";
		case BUFFER_DQ_DT:return "dq_dt";
		case BUFFER_FFT:return "fft";
		case BUFFER_PHONONS:return "phonons";
		default:return "undefined";
	};
	return NULL;
}


int flush_state(int buf)
{
	if (buf<BUFFERS)
		return buffers[buf].flush;
}

void set_flush(int buf,int f)
{
	if (buf<BUFFERS)
		buffers[buf].flush=f;    
}

void buffer_new_series(int buf)
{
	if (flush_state(buf))
		flush_buffer(buf);
	else if ((get_buffer(buf)).length>0)
		buffer_add_empty(buf);
}

buffer copy_buffer(int b)
{
	buffer buf;
	buf.length=0;
	if (b<0 || b>=BUFFERS)return buf;
	int len;
	buf=buffers[b];
	lock_buffer(b);
	len=buf.alloc;
	buf.elements=(element *)malloc(sizeof(element)*len);
	memmove((void *)buf.elements,(void *)buffers[b].elements,sizeof(element)*len);
	unlock_buffer(b);
	return buf;
}


void free_buffer(buffer b)
{
	free(b.elements);
}


buffer get_last_buffer(int b)
{
	buffer bf;
	if (b>=0 && b<BUFFERS)
	{
		bf=get_buffer(b);
		bf.elements+=bf.last;
		bf.length-=bf.last;
		return bf;
	}
	bf.type=BUFFER_ERROR;
	return bf;
}

buffer get_buffer(int b)
{
	buffer bf;
	if (b>=0 && b<BUFFERS)
		return buffers[b];
	bf.type=BUFFER_ERROR;
	return bf;
}

int delete_element(int b,int n)
{
	if (b>=BUFFERS || b<0)
		return -1;
	if (n>buffers[b].length)
		return -1;
	if (buffers[b].elements[n].type==ELEMENT_EMPTY)
	  return -1;
	if (wait_for_unlock(b)==0)return -1;
	buffers[b].locked=1;
	cout << "Deleting element " << n << "\n";
	if (n<buffers[b].length-1)
		memcpy(buffers[b].elements+n,buffers[b].elements+n+1,sizeof(element)*(buffers[b].length-n));
	buffers[b].length--;
	//	buffers[b].elements=(element *)realloc((void *)buffers[b].elements,sizeof(element)*buffers[b].length);
	buffers[b].changed++;
	buffers[b].alloc--;
	buffers[b].locked=0;
}

int set_value(int b,int n,int m,double val)
{
	int old_length;
	if (b>=BUFFERS || b<0)
		return -1;
	if (n>=buffers[b].length)
	{
		old_length=buffers[b].length;
		buffers[b].length=n+1;
		buffers[b].elements=(element *)realloc((void *)buffers[b].elements,sizeof(element)*buffers[b].length);
		for(int x=old_length;x<buffers[b].length;x++)
		{
			buffers[b].elements[x].type=ELEMENT_MULTIVAL;
			buffers[b].elements[x].n_values=2;
			buffers[b].elements[x].values[0]=0.0;
			buffers[b].elements[x].values[1]=0.0;
		}
	}
	if (buffers[b].elements[n].type!=ELEMENT_MULTIVAL)
		return -1;
	if (m>=buffers[b].elements[n].n_values)
	{
		buffers[b].elements[n].n_values=m+1;
	}
	buffers[b].elements[n].values[m]=val;
	buffers[b].changed++;
	return 0;
}

//Flips the coordinates x and y.
void flip_xy(int b,int x,int y)
{
	if (b>=BUFFERS || b<0)
		return;
//	cout << "Flipping...\n";
	for(int z=0;z<buffers[b].length;z++)
	{
		if (buffers[b].elements[z].n_values>x && buffers[b].elements[z].n_values>y)
		{
			double t=buffers[b].elements[z].values[x];
			buffers[b].elements[z].values[x]=buffers[b].elements[z].values[y];
			buffers[b].elements[z].values[y]=t;
		}
	}
	buffers[b].changed++;
	double min,max;
	max=buffers[b].max.values[x];
	buffers[b].max.values[x]=buffers[b].max.values[y];
	buffers[b].max.values[y]=max;
	min=buffers[b].min.values[x];
	buffers[b].min.values[x]=buffers[b].min.values[y];
	buffers[b].min.values[y]=min;
}

int buffer_length(int b)
{
	if (b>=BUFFERS || b<0)
		return -1;
	return buffers[b].length;
}

//Takes the derivative dy/dx of buffer b and stores the result in t.
void differentiate(int b,int x,int y,int t)
{
	if (b>=BUFFERS || b<0)
		return;
	buffer_new_series(t);
	double deriv;
	cout << "Differentiating...\n";
	for(int z=0;z<buffers[b].length-1;z++)
	{
		deriv=(buffers[b].elements[z+1].values[y]-buffers[b].elements[z].values[y])/(buffers[b].elements[z+1].values[x]-buffers[b].elements[z].values[x]);
		buffer_add_value(t,(buffers[b].elements[z+1].values[x]+buffers[b].elements[z].values[x])/2.0,deriv);
	}
}

double get_value(int b,int n,int m)
{
	if (b>=BUFFERS || b<0)
		return 0.0;
	if (n>buffers[b].length)
		return 0.0;
	if (m>buffers[b].elements[n].n_values || buffers[b].elements[n].type!=ELEMENT_MULTIVAL)
		return 0.0;
	return buffers[b].elements[n].values[m];
}

void init_buffer(int b)
{
	if (b>=BUFFERS)
		return;
	buffers[b].elements=(element *)malloc(TRAIN*sizeof(element));
	buffers[b].alloc=TRAIN;
	buffers[b].length=0;
	buffers[b].flush=1;
	buffers[b].last=0;
	buffers[b].locked=0;
	for(int x=0;x<=10;x++)
	{
		buffers[b].max.n_values=0;
		buffers[b].min.n_values=0;
		buffers[b].max.values[x]=0.0;
		buffers[b].min.values[x]=0.0;
	}
	buffers[b].series=1;
	buffers[b].changed++;
}

void lock_buffer(int b)
{
	buffers[b].locked=1;
}

void unlock_buffer(int b)
{
	buffers[b].locked=0;
}


void flush_buffer(int b)
{
	if (b>=BUFFERS)
		return;
	lock_buffer(b);
	if (buffers[b].elements!=NULL)
		free((void *)(buffers[b].elements)); 
	buffers[b].elements=(element *)malloc(sizeof(element)*TRAIN);
	buffers[b].alloc=TRAIN;
	buffers[b].length=0;
	buffers[b].last=0;
	buffers[b].series=1;
	buffers[b].changed++;
	buffers[b].max.n_values=0;
	buffers[b].min.n_values=0;
	for(int x=0;x<MAX_VALS;x++)
	{
		buffers[b].max.values[x]=0.0;
		buffers[b].min.values[x]=0.0;
	}
	unlock_buffer(b);
}

void buffer_add_empty(int b)
{
	element e;
	e.x=0.;
	e.y=0.;
	e.z=0.;
	e.type=ELEMENT_EMPTY;
	buffer_add_element(b,e);
	buffers[b].last=buffers[b].length;
	buffers[b].series++;
}

int buffer_empty(int b)
{
	if (buffers[b].length==0)
		return 1;
	return 0;
}

void buffer_add_element(int b,element e)
{
	if (b>=BUFFERS)
		return;
	if (wait_for_unlock(b)==0)return;
	buffers[b].length++;
	lock_buffer(b);
	if (buffers[b].length>=buffers[b].alloc)
	{
		buffers[b].alloc+=TRAIN;
		buffers[b].elements=(element *)realloc((void *)buffers[b].elements,sizeof(element)*buffers[b].alloc);
	}
	buffers[b].elements[buffers[b].length-1]=e;
	buffers[b].changed++;
	if (e.type==ELEMENT_MULTIVAL)
		if (buffers[b].length==1)
		{
			buffers[b].max.n_values=e.n_values;
			buffers[b].min.n_values=e.n_values;
			buffers[b].max=e;
			buffers[b].min=e;
		}
		else 
		{
			for(int x=0;x<e.n_values;x++)
			{
				if (e.values[x]>buffers[b].max.values[x])
					buffers[b].max.values[x]=e.values[x];
				if (e.values[x]<buffers[b].min.values[x])
					buffers[b].min.values[x]=e.values[x];
			}
		}
		
		unlock_buffer(b);
}

void buffer_set_minmax(int b,double min,double max)
{
	if (b>=BUFFERS)
		return;
}

void buffer_add_value(int b,double *values,int n_values)
{
	if (b>=BUFFERS || b<0)
	{
		return;
	}
	element e;
	for(int x=0;x<n_values;x++)
		e.values[x]=values[x];
	e.n_values=n_values;
	e.type=ELEMENT_MULTIVAL;
	buffer_add_element(b,e);
}

void buffer_add_value(int b,double x,double y)
{
	buffer_add_value(b,x,y,0.);
}

int num_buffers()
{
	return BUFFERS;
}

void buffer_add_value(int b,double x,double y,double z)
{
	element e;
	e.type=ELEMENT_MULTIVAL;
	e.n_values=3;
	e.values[0]=x;
	e.values[1]=y;
	e.values[2]=z;
	buffer_add_element(b,e);
}

void change_all(void)
{
	for(int x=0;x<BUFFERS;x++)
		buffers[x].changed++;
}

int buffer_changed(int b)
{
	if (b<0 || b>BUFFERS)
		return 0;
	return buffers[b].changed;
}

#include"circuit.h"

int meshcnt=0;

CIRCUIT c;

STACK_ELEMENT *STACK;
int STACK_LENGTH=-1;

double *sa;
unsigned long *ija;

variable node_name(opts o,interpreter *i)
{
	variable v=init_variable();
	int N=0;
	char *name=NULL;
	if (i->cast_int(&N,o,0) && i->cast_string(&name,o,1))
	{
		node_name(N,c.BLOCK,name);
	}
	return v;
}

void node_name(int NODE,char *block,char *name)
{
//	cout << "Setting node name for node " << NODE << "\n";
	for(int x=0;x<c.n_nodes;x++)
	{
		if (c.nodes[x].N==NODE && strcmp(c.nodes[x].BLOCK,block)==0)
		{
//			cout << "Found node!\n";
			c.nodes[x].NAME=(char *)malloc(sizeof(char)*(strlen(name)+1));
			strcpy(c.nodes[x].NAME,name);
		}
	}
}

//Searches the node tree for a particular node number. Returns -1 if node number can't be found.
int check_for_node(int N,char *block)
{
	for(int x=0;x<c.n_nodes;x++)				
	{
		if (c.nodes[x].N==N && strcmp(c.nodes[x].BLOCK,block)==0)
			return x;
	}
	return -1;
}

//Searches the mesh tree for a particular mesh number. Returns -1 if mesh number can't be found.
int check_for_mesh(int N)
{
	for(int x=0;x<c.n_meshes;x++)
	{
		if (c.meshes[x].N==N)
			return x;
	}
	return -1;
}

variable CIRC_add_monitor(opts o,interpreter *i)
{
	variable ret=init_variable();
	int type;
	int NODE_1=-1;
	int NODE_2=-1;
	char *element;
	char *BLOCK;
	int buffer=-1;
	type=0;
	element=0;
	if (i->cast_int(&type,o,0) && i->cast_string(&element,o,1))
	{
		if (!i->cast_string(&BLOCK,o,2))
			BLOCK=c.BLOCK;
		if (find_element(element,BLOCK)!=-1)
			buffer=CIRC_add_monitor(type,find_element(element,BLOCK));
		else if (find_element(element)!=-1)
			buffer=CIRC_add_monitor(type,find_element(element));
	}
	else if (i->cast_int(&type,o,0) && i->cast_int(&NODE_1,o,1) && i->cast_int(&NODE_2,o,2))
	{
		buffer=CIRC_add_monitor(type,NODE_1,NODE_2);
	}
	ret=i->int_variable(buffer);
	return ret;
}

int find_element(char *element,char *block)
{
	for(int x=1;x<=c.n_elements;x++)
		if (!strcmp(c.elements[x].name,element) && !strcmp(block,c.elements[x].BLOCK))
			return x;
	return -1;	
}

int find_element(char *element)
{
	for(int x=1;x<=c.n_elements;x++)
		if (!strcmp(c.elements[x].name,element))
			return x;
	return -1;	
}

int CIRC_get_node(int N,char *block)
{
	int x;
	for(x=0;x<c.n_nodes;x++)
	{
		if (c.nodes[x].N==N && strcmp(block,c.nodes[x].BLOCK)==0)
			return x;
	}
	return -1;
}

//Returns the internal node number of a design node number.
int CIRC_get_node(char *name,char *block)
{
	int x;
	for(x=0;x<c.n_nodes;x++)
	{
		if (strcmp(c.nodes[x].NAME,name)==0 && strcmp(block,c.nodes[x].BLOCK)==0)
			return x;
	}
	return -1;
}


//This is a monitor between two nodes (only current and phase!).
int CIRC_add_monitor(int type,int NODE_1,int NODE_2)
{
	char buffer[10000];
	char filename[1024];
	const char *legend=NULL;
	if (type==MONITOR_CURRENT)
		return -1;
	c.MONITORS=(MONITOR *)realloc((void *)c.MONITORS,sizeof(MONITOR)*(c.n_monitors+++1));
	c.MONITORS[c.n_monitors-1].type=type;
	c.MONITORS[c.n_monitors-1].element=-1;
	c.MONITORS[c.n_monitors-1].NODE_1=CIRC_get_node(NODE_1,c.BLOCK);//replace this!!!
	c.MONITORS[c.n_monitors-1].NODE_2=CIRC_get_node(NODE_2,c.BLOCK);//replace this!!!
	switch (type)
	{
		case MONITOR_VOLTAGE:legend="V";break;
		case MONITOR_PHASE:legend="Phi";break;
		default:legend="undef.";break;
	}
	sprintf(buffer,"%s(%d->%d)",legend,NODE_1,NODE_2);
	c.MONITORS[c.n_monitors-1].buffer=new_buffer(buffer,buffer);
	return c.MONITORS[c.n_monitors-1].buffer;
}

//This is a voltage monitor.
int CIRC_add_monitor(int type,int element)
{
	char buffer[10000];
	const char *legend=NULL;
	c.MONITORS=(MONITOR *)realloc((void *)c.MONITORS,sizeof(MONITOR)*(c.n_monitors+++1));
	c.MONITORS[c.n_monitors-1].type=type;
	c.MONITORS[c.n_monitors-1].element=element;
	switch (type)
	{
	case MONITOR_VOLTAGE:legend="V";break;
	case MONITOR_CURRENT:legend="I";break;
	case MONITOR_PHASE:legend="Phi";break;
	default:legend="undef.";break;
	}
	sprintf(buffer,"%s(%s)",legend,c.elements[element].name);
	c.MONITORS[c.n_monitors-1].buffer=new_buffer(buffer);
	return c.MONITORS[c.n_monitors-1].buffer;
}

variable build_trees(opts o,interpreter *i)
{
	variable ret=init_variable();
	build_trees();
	return ret;
}

variable CIRC_solve(opts o,interpreter *i)
{
	variable ret=init_variable();
	CIRC_solve();
	return ret;
}

variable CIRC_evolve(opts o,interpreter *i)
{
	variable ret=init_variable();
	CIRC_evolve();
	return ret;
}

void CIRC_delete_node(int N)
{
	int x;
	int copy=0;
	for(x=0;x<c.n_nodes-1;x++)
	{
		if (c.nodes[x].N==N)
		{
			if (c.nodes[x].NAME!=NULL)
				free(c.nodes[x].NAME);
			copy=1;
		}
		if (copy)
			c.nodes[x]=c.nodes[x+1];
	}
	if (copy)
		c.n_nodes--;
}

void CIRC_delete_element(int N)
{
	int x;
	int copy=0;
	for(x=N;x<c.n_elements;x++)
	{
		c.elements[x]=c.elements[x+1];
	}
	c.n_elements--;
}

void clean_from_node(int a,int e)
{
	int x;
	int copy=0;
	for(x=0;x<c.nodes[a].n_elements-1;x++)
	{
		if (c.nodes[a].elements[x]==e)
			copy=1;
		if (copy)
			c.nodes[a].elements[x]=c.nodes[a].elements[x+1];
	}
	if (copy)
		c.nodes[a].n_elements--;
}

//Does exactly what it says.
void eliminate_connections(void)
{
	int x,y,z;
	int a,b;
	for(x=1;x<=c.n_elements;x++)
	{
		if (c.elements[x].TYPE==TYPE_CON)
		{	
			a=c.elements[x].NODE_1;
			b=c.elements[x].NODE_2;
			clean_from_node(a,x);
			clean_from_node(b,x);
//			cout << "Eliminating connection from " << a << " to " << b << "\n";
			for(y=1;y<=c.n_elements;y++)
			{
				if (y!=x)
				{
					if (c.elements[y].NODE_1==a)
					{
						c.elements[y].NODE_1=b;
					}
					if (c.elements[y].NODE_2==a)
					{
						c.elements[y].NODE_2=b;
					}
				}
			}
//			cout << "Eliminating " << c.elements[x].name << "\n";
			CIRC_delete_element(x);
			CIRC_delete_node(a);
			x--;
		}
//		c.elements[x].BLOCK="MAIN";
	}
}

void build_trees()
{
	char str[1024];
	int x,y,z,n_count;
	double d=0.0;
	int *indx;
	double **temp;
	int solver=c.SOLVER;
	eliminate_connections();
	build_flat_node_tree();
//	print_node_tree();
	build_mesh_tree();
	int n1=0;
	int n=0;
//	gauss_elimination(c.mesh_matrix,c.n_elements);
//	gauss_elimination(c.node_matrix,c.n_elements);
	for(x=1;x<=c.n_elements;x++)
	{
		for(y=1;y<=c.n_elements;y++)
		{
			if (c.mesh_matrix[x][y]!=0)
				n1++;
		}
	}
	for(x=1;x<=c.n_elements;x++)
	{
		for(y=1;y<=c.n_elements;y++)
		{
			if (c.node_matrix[x][y]!=0)
				n++;
		}
	}
	cout << "saved nodes:" << (c.n_node_elements-n) << "\n";
	cout << "saved meshs:" << (c.n_mesh_elements-n1) << "\n";
//	print_mesh_tree();
	c.I=(double *)realloc((void *)c.I,sizeof(double)*(c.n_elements+1));
	c.V=(double *)realloc((void *)c.V,sizeof(double)*(c.n_elements+1));
	c.phi=(double *)realloc((void *)c.phi,sizeof(double)*(c.n_elements+1));
	c.I_int=(double *)realloc((void *)c.I_int,sizeof(double)*(c.n_elements+1));
	c.V_int=(double *)realloc((void *)c.V_int,sizeof(double)*(c.n_elements+1));
	c.I_p=(double *)realloc((void *)c.I_p,sizeof(double)*(c.n_elements+1));
	c.V_p=(double *)realloc((void *)c.V_p,sizeof(double)*(c.n_elements+1));
	c.phi_p=(double *)realloc((void *)c.phi_p,sizeof(double)*(c.n_elements+1));
	c.I_pp=(double *)realloc((void *)c.I_pp,sizeof(double)*(c.n_elements+1));
	c.V_pp=(double *)realloc((void *)c.V_pp,sizeof(double)*(c.n_elements+1));
	c.phi_pp=(double *)realloc((void *)c.phi_pp,sizeof(double)*(c.n_elements+1));
	//free_matrix(c.fjac,1,2*c.n_elements,1,2*c.n_elements);
	c.fjac=matrix(1,2*c.n_elements,1,2*c.n_elements);
	c.voltage_matrix=matrix(1,c.n_elements,1,c.n_elements);
	c.current_matrix=matrix(1,c.n_elements,1,c.n_elements);
	c.inv_voltage_matrix=matrix(1,c.n_elements,1,c.n_elements);
	c.inv_current_matrix=matrix(1,c.n_elements,1,c.n_elements);
	c.lin=matrix(1,2*c.n_elements,1,2*c.n_elements);
	temp=matrix(1,2*c.n_elements,1,2*c.n_elements);
	c.inverse=matrix(1,2*c.n_elements,1,2*c.n_elements);
	c.map=imatrix(1,2*c.n_elements,1,2*c.n_elements);
	c.lin_save=matrix(1,2*c.n_elements,1,2*c.n_elements);
	c.vec=vector(1,2*c.n_elements);
	c.solution=vector(1,2*c.n_elements);
	c.vec_save=vector(1,2*c.n_elements);
	
	c.indx=ivector(1,2*c.n_elements);



	c.sparse_n=generate_linear_matrix(c.lin,c.vec,0.0)*2;
	double sum=0.0;
	for(x=1;x<=2*c.n_elements;x++)
		for(y=1;y<=2*c.n_elements;y++)
		{
			c.map[x][y]=0;
			c.inverse[x][y]=x==y?1.0:0.0;
		}
	c.sparse_vec=vector(1,c.sparse_n);
	c.sparse_ind=lvector(1,c.sparse_n);
	
	sa=c.sparse_vec;
	ija=c.sparse_ind;

	
	sprsin(c.lin,c.map,2*c.n_elements,THLIN,c.sparse_n,sa,ija);

	for(x=1;x<=2*c.n_elements;x++)
		memcpy(temp[x],c.lin[x],sizeof(double)*(c.n_elements*2+1));


	sprsin(c.lin,c.map,2*c.n_elements,THLIN,c.sparse_n,sa,ija);

//	linbcg(2*c.n_elements,c.vec,c.solution,2,1e-6,0,&iter,&err,c.sparse_vec,c.sparse_ind,c.inverse_vec,c.inverse_ind);

	indx=ivector(1,2*c.n_elements);
	
	clock_t t1=clock();
//	full_gauss_elimination(c.lin,c.inverse,2*c.n_elements);

/*
	ludcmp(c.lin,2*c.n_elements,indx,&d);

	double *col=vector(1,2*c.n_elements);
	int i,j;
	n_count=0;
	cout << "Done with LU...\n";
	for(j=1;j<=2*c.n_elements;j++)
	{
		for(i=1;i<=2*c.n_elements;i++)
			col[i]=0.0;
		col[j]=1.0;
		cout << j << "\n";
		lubksb(c.lin,2*c.n_elements,indx,col);
		for(i=1;i<=2*c.n_elements;i++)
		{
			c.inverse[i][j]=col[i];
			if (fabs(c.inverse[i][j])>=TH/10.0)
				n_count++;
		}

	}*/
    gaussj(c.lin,2*c.n_elements,c.inverse,2*c.n_elements);
    clock_t t2=clock();
	sprintf(str,"%.4lf seconds for inverting A\n", (t2-t1)/(double)CLOCKS_PER_SEC);
	cout << str;
	n_count=4.0*c.n_elements*c.n_elements;
//	full_gauss_elimination(c.lin,2*c.n_elements);
	c.inverse_vec=vector(1,n_count+1);
	c.inverse_ind=lvector(1,n_count+1);
	cout << "N:" << n_count << "\n";
	sprsin(c.inverse,NULL,2*c.n_elements,TH,n_count,c.inverse_vec,c.inverse_ind);
	//free_matrix(c.lin,1,2*c.n_elements,1,2*c.n_elements);
	for(x=1;x<=2*c.n_elements;x++)
		c.solution[x]=0.0;

	memcpy(&(c.vec_save[1]),&(c.vec[1]),(2*(c.n_elements))*sizeof(double));
//	init_jacobian(c.fjac);

	for(x=1;x<=2*c.n_elements;x++)
		memcpy(c.lin[x],temp[x],sizeof(double)*(c.n_elements*2+1));

	for(x=1;x<=2*c.n_elements;x++)
		memcpy(&(c.lin_save[x][1]),&(c.lin[x][1]),(2*(c.n_elements))*sizeof(double));

	free_matrix(temp,1,2*c.n_elements,1,2*c.n_elements);
//	free_matrix(c.lin,1,2*c.n_elements,1,2*c.n_elements);
	free_matrix(c.inverse,1,2*c.n_elements,1,2*c.n_elements);
	
	for(x=1;x<=c.n_elements;x++)
	{
		c.I[x]=0.0;
		c.V[x]=0.0;
		c.phi[x]=0.0;
		c.I_p[x]=0.0;
		c.V_p[x]=0.0;
		c.phi_p[x]=0.0;
		c.I_pp[x]=0.0;
		c.V_pp[x]=0.0;
		c.phi_pp[x]=0.0;
		c.I_int[x]=0.0;
		c.V_int[x]=0.0;
		switch (c.elements[x].TYPE)
		{
			case TYPE_CS:
				sprintf(str,"%s_%s_I0",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].I),str);
				break;
			case TYPE_PS:
				sprintf(str,"%s_%s_Phi0",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].Phi),str);
				break;
			case TYPE_R:
				sprintf(str,"%_s%s_R",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].R),str);
				break;
			case TYPE_JJ:
				sprintf(str,"%s_%s_Ic",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].Ic),str);
				break;
			case TYPE_KAPPA_JJ:
				sprintf(str,"%s_%s_Ic",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].Ic),str);
				break;
			case TYPE_C:
				sprintf(str,"%s_%s_C",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].C),str);
				break;
			case TYPE_L:
				sprintf(str,"%s_%s_L",c.elements[x].BLOCK,c.elements[x].name);
				c.INT->tie_to_double(&(c.elements[x].L),str);
				break;
		}
		if (c.elements[x].TYPE!=TYPE_CON)
		{
			sprintf(str,"%s_%s_V",c.elements[x].BLOCK,c.elements[x].name);
			c.INT->tie_to_double(&(c.V[x]),str);
			sprintf(str,"%s_%s_phi",c.elements[x].BLOCK,c.elements[x].name);
			c.INT->tie_to_double(&(c.phi[x]),str);
			sprintf(str,"%s_%s_I",c.elements[x].BLOCK,c.elements[x].name);
			c.INT->tie_to_double(&(c.I[x]),str);
		}

	}	
	cout << "Circuit summary:\n";
	cout << "Number of elements:       \t" << c.n_elements << "\n";
	cout << "Number of nodes (-ground):\t" << (c.n_nodes-1) << "\n";
	cout << "Number of meshes:         \t" << c.n_meshes << "\n";
	cout << "Number of equations:      \t" << (c.n_elements+c.n_meshes+c.n_nodes-1) << "\n";
	cout << "Matrix elements:          \t" << ((c.n_elements+c.n_meshes+c.n_nodes-1)*(c.n_elements+c.n_meshes+c.n_nodes-1)) << "\n";
	cout << "Nonzero matrix elements   \t" << c.sparse_n << "\n";
	/*	update_linear_matrix(c.lin,c.vec);
	ludcmp(c.lin,2*c.n_elements,c.indx,&d);
	lubksb(c.lin,2*c.n_elements,c.indx,c.vec);
	memcpy(&(c.phi[1]),&(c.vec[1+c.n_elements]),c.n_elements*sizeof(double));
	memcpy(&(c.I[1]),&(c.vec[1]),c.n_elements*sizeof(double));
	for(x=1;x<=2*c.n_elements;x++)
		c.solution[x]=c.vec[x];
	c.SOLVER=solver;*/
}

void CIRC_solve()
{
	static int cnt=0;
//	static double **temp=matrix(1,2*c.n_elements,1,2*c.n_elements);
	int n=2*c.n_elements;
	double d=0.0;
	double t_sol=0.0;
	//The following algorithm uses the linearized version of the equations of motions and solves them directly by LU-decomposition.
	//Possible speedup through matrix manipulation???
	int iter=0;
	double err=0;
	int x,y,z;
	update_linear_matrix(c.lin,c.vec);
	if (c.SOLVER==SOLVER_LU)
	{
		ludcmp(c.lin,n,c.indx,&d);
		lubksb(c.lin,n,c.indx,c.vec);
		memcpy(&(c.phi[1]),&(c.vec[1+c.n_elements]),c.n_elements*sizeof(double));
		memcpy(&(c.I[1]),&(c.vec[1]),c.n_elements*sizeof(double));
	}
	else if (c.SOLVER==SOLVER_BILIN)
	{
		linbcg_single(2*c.n_elements,c.vec,c.solution,1,1e-6,0,&iter,&err,c.sparse_vec,c.sparse_ind,c.inverse_vec,c.inverse_ind);
//		dsprsax(c.inverse_vec,c.inverse_ind,c.vec,c.solution,2*c.n_elements);
		memcpy(&(c.phi[1]),&(c.solution[1+c.n_elements]),c.n_elements*sizeof(double));
		memcpy(&(c.I[1]),&(c.solution[1]),c.n_elements*sizeof(double));
	}
	else if (c.SOLVER==SOLVER_NEWTON)
	{
		mnewt(100,c.vec,c.n_elements*2,1e-6,MAX_ERROR,usrfun);
		memcpy(&(c.phi[1]),&(c.vec[1+c.n_elements]),c.n_elements*sizeof(double));
		memcpy(&(c.I[1]),&(c.vec[1]),c.n_elements*sizeof(double));
		
	}
}

//Currents first
double element_deriv(int element,int var)
{
	switch (c.elements[element].TYPE)
	{
		case TYPE_R:
			if (var<=c.n_elements)//This is a current
				if (element==var)
					return -c.elements[element].R;
			if (var>c.n_elements)//This is a phase
				if (element==var-c.n_elements)
					return 1.0/c.STEPT;
			break;
		case TYPE_L:
			if (var<=c.n_elements)
				if (element==var)
					return c.elements[element].L;
			if (var>c.n_elements)
				if (element==var-c.n_elements)
					return -1.0;
			break;
		case TYPE_C:
			//This is a capacitor with the relation C_n=Q_n/U_n and Q=\Delta t\cdot\sum\limits_{i=0}^t I_n^t. Therefore, we have
			if (var<=c.n_elements)//This is a current
				if (element==var)
					return 1.0;
			if (var>c.n_elements)//This is a phase.
				if (element==var-c.n_elements)
					return -c.elements[element].C/c.STEPT/c.STEPT;
			break;
		case TYPE_JJ://Josephson Junction.
			if (var<=c.n_elements)//This is a current
				if (element==var)
					return -1.0;
			if (var>c.n_elements)//This is a phase
				if (element==var-c.n_elements)
					return c.elements[element].Ic*cos(c.phi[element])+c.elements[element].alpha/c.STEPT+1.0/c.STEPT/c.STEPT;
			break;
		case TYPE_KAPPA_JJ://kappa-Josephson Junction.
			if (var<=c.n_elements)//This is a current
				if (element==var)
					return -1.0;
			if (var>c.n_elements)//This is a phase
				if (element==var-c.n_elements)
					return c.elements[element].Ic*cos(c.phi[element]+c.elements[element].kappa*PI)+c.elements[element].alpha/c.STEPT+1.0/c.STEPT/c.STEPT;
			break;
		case TYPE_CS:
			if (var<=c.n_elements)//this is a current source
				if (element==var)
					return 1.0;
			break;
		case TYPE_NC:
			if (var<=c.n_elements)//this is a current source
				if (element==var)
					return 1.0;
			break;
		case TYPE_CSG:
			if (var<=c.n_elements)//this is a current signal generator
				if (element==var)
					return 1.0;
			break;
		case TYPE_VSG:
			if (var>c.n_elements)//this is a voltage signal generator
				if (element==var-c.n_elements)
					return 1.0/c.STEPT;
			break;
		case TYPE_PSG:
			if (var>c.n_elements)//this is a voltage signal generator
				if (element==var-c.n_elements)
					return 1.0;
			break;
		case TYPE_PS://This is a phase source.
			if (var>c.n_elements)
				if (element==var-c.n_elements)
					return 1.0;
			break;
		case TYPE_VS:
			if (var>c.n_elements)//this is a voltage source
				if (element==var-c.n_elements)
					return 1.0/c.STEPT;
			break;
	}
	return 0;
}


//Currents first, then phases.
double mesh_deriv(int mesh,int var)
{
	for(int x=0;x<c.meshes[mesh].n_elements;x++)
		if (abs(c.meshes[mesh].elements[x])==var-c.n_elements)
			return c.meshes[mesh].elements[x]>0.0?1.0:-1.0;
	return 0;
}

double node_deriv(int node,int var)
{
	for(int x=0;x<c.nodes[node].n_elements;x++)
		if (abs(c.nodes[node].elements[x])==var)
			return c.nodes[node].elements[x]>0.0?1.0:-1.0;
	return 0;
}

//Returns the right-handed side of an element equation, i.e. the constant part of it.
double rhs(int element)
{
	switch(c.elements[element].TYPE)
	{
		case TYPE_R:
			return c.phi_p[element]/c.STEPT;
			break;
		case TYPE_CON:
			return 0;
			break;
		case TYPE_L:
			return 0.0;
			break;
		case TYPE_C:
			return c.elements[element].C*(2.*c.phi_p[element]-c.phi_pp[element])/c.STEPT/c.STEPT;
			break;
		case TYPE_JJ://Josephson Junction.
			return -c.elements[element].Ic*sin(c.phi_p[element])+c.elements[element].Ic*c.phi_p[element]*cos(c.phi_p[element])+c.elements[element].alpha/c.STEPT*c.phi_p[element]+2.0/c.STEPT/c.STEPT*c.phi_p[element]-c.phi_pp[element]/c.STEPT/c.STEPT;
			break;
		case TYPE_KAPPA_JJ://kappa-Josephson Junction.
			return -c.elements[element].Ic*sin(c.phi_p[element]+c.elements[element].kappa*PI)+c.elements[element].Ic*c.phi_p[element]*cos(c.phi_p[element]+c.elements[element].kappa*PI)+c.elements[element].alpha/c.STEPT*c.phi_p[element]+2.0/c.STEPT/c.STEPT*c.phi_p[element]-c.phi_pp[element]/c.STEPT/c.STEPT;
			break;
		case TYPE_CS:
			return c.elements[element].I;
			break;
		case TYPE_NC:
			return c.elements[element].I;
			break;
		case TYPE_CSG:
			return SG(c.elements[element]);
			break;
		case TYPE_VSG:
			return SG(c.elements[element])+c.phi_p[element]/c.STEPT;
			break;
		case TYPE_PSG:
			return SG(c.elements[element]);
			break;
		case TYPE_PS://This is a phase source.
			return c.elements[element].Phi;
			break;
		case TYPE_VS:
			return c.elements[element].V+c.phi_p[element]/c.STEPT;
			break;
	}
	return 0;
		
}

//Returns the linear coefficient for a certain element, i.e. the left-handed coefficient in the linearized element model.
double linear_coeff(int element,int var,double lin)
{
	switch (c.elements[element].TYPE)
	{
		case TYPE_R:
			if (var<=c.n_elements)//This is a current
				if (element==var)
					return -c.elements[element].R;
			if (var>c.n_elements)//This is a phase
				if (element==var-c.n_elements)
					return 1.0/c.STEPT;
			break;
		case TYPE_L:
			if (var<=c.n_elements)
				if (element==var)
					return c.elements[element].L;
			if (var>c.n_elements)
				if (element==var-c.n_elements)
					return -1.0;
			break;
		case TYPE_C:
			//This is a capacitor with the relation C_n=Q_n/U_n and Q=\Delta t\cdot\sum\limits_{i=0}^t I_n^t. Therefore, we have
			if (var<=c.n_elements)//This is a current
				if (element==var)
					return -1.0;
			if (var>c.n_elements)//This is a phase.
				if (element==var-c.n_elements)
					return c.elements[element].C/c.STEPT/c.STEPT;
			break;
			case TYPE_JJ://Josephson Junction.
				if (var<=c.n_elements)//This is a current
					if (element==var)
						return -1.0;
				if (var>c.n_elements)//This is a phase
					if (element==var-c.n_elements)
						return c.elements[element].Ic*cos(c.phi_p[element])*lin+c.elements[element].alpha/c.STEPT+1.0/c.STEPT/c.STEPT;
				break;
			case TYPE_KAPPA_JJ://Josephson Junction.
				if (var<=c.n_elements)//This is a current
					if (element==var)
						return -1.0;
				if (var>c.n_elements)//This is a phase
					if (element==var-c.n_elements)
						return c.elements[element].Ic*cos(c.phi_p[element]+c.elements[element].kappa*PI)*lin+c.elements[element].alpha/c.STEPT+1.0/c.STEPT/c.STEPT;
				break;
		case TYPE_CS:
			if (var<=c.n_elements)//this is a current source
				if (element==var)
					return 1.0;
			break;
		case TYPE_NC:
			if (var<=c.n_elements)//this is a current source
				if (element==var)
					return 1.0;
			break;
		case TYPE_CSG:
			if (var<=c.n_elements)//this is a current signal generator
				if (element==var)
					return 1.0;
			break;
		case TYPE_VSG:
			if (var>c.n_elements)//this is a voltage signal generator
				if (element==var-c.n_elements)
					return 1.0/c.STEPT;
			break;
		case TYPE_PSG:
			if (var>c.n_elements)//this is a voltage signal generator
				if (element==var-c.n_elements)
					return 1.0;
			break;
			case TYPE_PS://This is a phase source.
				if (var>c.n_elements)
					if (element==var-c.n_elements)
						return 1.0;
				break;
		case TYPE_VS:
			if (var>c.n_elements)//this is a voltage source
				if (element==var-c.n_elements)
					return 1.0/c.STEPT;
			break;
	}
	return 0;
}

//Updates the linear matrix obtained by "generate_linear_matrix". Only dynamic coefficients are changed.
void update_linear_matrix(double **lin,double *vec)
{
	int x,element;
	//First, we restore the original version of the matrix...
	if (c.SOLVER!=SOLVER_BILIN)
	{
		for(x=1;x<=2*c.n_elements;x++)
			memcpy(&(c.lin[x][1]),&(c.lin_save[x][1]),(2*(c.n_elements))*sizeof(double));
		memcpy(&(c.vec[1]),&(c.vec_save[1]),(2*c.n_elements)*sizeof(double));
	}
	//Then, we repopulate the element equations in the matrix and the solution vector.
	for(x=c.n_elements+1;x<=2*c.n_elements;x++)
	{
		if (c.SOLVER!=SOLVER_BILIN)
		{
			lin[x][x]=linear_coeff(x-c.n_elements,x,1.0);
		}
		vec[x]=rhs(x-c.n_elements);
		c.sparse_vec[c.map[x][x]]=linear_coeff(x-c.n_elements,x,1.0);
	}
}

//Returns the linearized matrix of the system, together with the rhs vector.
int generate_linear_matrix(double **lin,double *vec,double set_lin)
{
	int n=1;
	for(int x=1;x<=2*c.n_elements;x++)
	{
		for(int y=1;y<=2*c.n_elements;y++)
		{
			
			if (x>c.n_elements)
			{
				if ((x-c.n_elements==y) || (x==y))//This is a small shortcut to improve calculation time...
				{
					lin[x][y]=linear_coeff(x-c.n_elements,y,set_lin);
				}
				else
					lin[x][y]=0.0;
					if ((x-c.n_elements)==y)
					{
						c.current_matrix[x-c.n_elements][y]=lin[x][y];
					}
					else if (x==y)
					{
						c.voltage_matrix[x-c.n_elements][y-c.n_elements]=lin[x][y];
					}
					else {if (y>c.n_elements)
					{
						c.voltage_matrix[x-c.n_elements][y-c.n_elements]=0.0;
						c.inv_voltage_matrix[x-c.n_elements][y-c.n_elements]=0.0;
					}
					else
					{
							c.inv_current_matrix[x-c.n_elements][y]=0.0;
							c.current_matrix[x-c.n_elements][y]=0.0;
					} }

			//This is an element equation.
			}
			else if (x>c.n_meshes)
			{
				if (y<=c.n_elements)
					lin[x][y]=c.node_matrix[x-c.n_meshes][y];//mesh_deriv(x-1,y);
				else
					lin[x][y]=0.0;
//				lin[x][y]=node_deriv(x-c.n_meshes-1,y);
		//This is a node equation.
			}
			else
			{
				if (y>c.n_elements)
				{
					lin[x][y]=c.mesh_matrix[x][y-c.n_elements];//mesh_deriv(x-1,y);
				}
				else
				{
					lin[x][y]=0.0;
				}
		//This is a mesh equation.
			}
			if (fabs(lin[x][y])>=THLIN/10.0)
				n++;
	
		}
		if (x>c.n_elements)
		{
			//This is an element vector
			vec[x]=rhs(x-c.n_elements);
		}
		else
		{
			vec[x]=0.0;
		}
	}
	cout << "N:" << n << "\n";
	return n;
}

void init_jacobian(double **fjac)
{
	for(int x=1;x<=2*c.n_elements;x++)
		for(int y=1;y<=2*c.n_elements;y++)
		{
			if (x>=c.n_meshes+c.n_nodes)
			{
				if ((x-c.n_meshes-c.n_nodes+1==y) || (x-c.n_meshes-c.n_nodes+1+c.n_elements==y))//This is a small shortcut to improve calculation time...
					fjac[x][y]=element_deriv(x-c.n_meshes-c.n_nodes+1,y);
				else
					fjac[x][y]=0.0;
		//This is an element equation.
			}
			else if (x>c.n_meshes)
			{
				fjac[x][y]=node_deriv(x-c.n_meshes-1,y);
		//This is a node equation.
			}
			else
			{
				fjac[x][y]=mesh_deriv(x-1,y);
		//This is a mesh equation.
			}
		
		}
}

void generate_jacobian(double **fjac)
{
	for(int x=c.n_meshes+c.n_nodes;x<=2*c.n_elements;x++)
		{
			fjac[x][x-c.n_meshes-c.n_nodes+1]=element_deriv(x-c.n_meshes-c.n_nodes+1,x-c.n_meshes-c.n_nodes+1);
			fjac[x][x-c.n_meshes-c.n_nodes+1+c.n_elements]=element_deriv(x-c.n_meshes-c.n_nodes+1,x-c.n_meshes-c.n_nodes+1+c.n_elements);
		}
}


//Returns the element (x,y) of the Jacobian matrix of the system.
double JJ(int x,int y)
{
	//Row select
	if (x>=c.n_meshes+c.n_nodes)
	{
		if ((x-c.n_meshes-c.n_nodes+1==y) || (x-c.n_meshes-c.n_nodes+1+c.n_elements==y))//This is a small shortcut to improve calculation time...
			return element_deriv(x-c.n_meshes-c.n_nodes+1,y);
		else
			return 0.0;
		//This is an element equation.
	}
	else if (x>c.n_meshes)
	{
		return node_deriv(x-c.n_meshes-1,y);
		//This is a node equation.
	}
	else
	{
		return mesh_deriv(x-1,y);
		//This is a mesh equation.
	}
	return 0.0;
}

double error_func()
{
	double error=0.0;
	for(int x=1;x<=2*c.n_elements;x++)
	{
		error+=fabs(func(x));
	}
	return error;
}

//Pushes a solution to the stack.
void push_solution(void)
{
	if (STACK_LENGTH==-1)
	{
		STACK=(STACK_ELEMENT *)malloc(sizeof(STACK_ELEMENT));
		STACK_LENGTH=0;
	}
	STACK_LENGTH++;
	STACK=(STACK_ELEMENT *)realloc((void *)STACK,sizeof(STACK_ELEMENT)*STACK_LENGTH);
	STACK[STACK_LENGTH-1].I=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].V=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].phi=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].I_int=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].V_int=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].I_p=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].V_p=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].phi_p=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].I_pp=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].V_pp=(double *)malloc(sizeof(double)*(c.n_elements+1));
	STACK[STACK_LENGTH-1].phi_pp=(double *)malloc(sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].I_int,(void *)c.I_int,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].V_int,(void *)c.V_int,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].I,(void *)c.I,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].I_p,(void *)c.I_p,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].I_pp,(void *)c.I_pp,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].V,(void *)c.V,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].V_p,(void *)c.V_p,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].V_pp,(void *)c.V_pp,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].phi,(void *)c.phi,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].phi_p,(void *)c.phi_p,sizeof(double)*(c.n_elements+1));
	memcpy((void *)STACK[STACK_LENGTH-1].phi_pp,(void *)c.phi_pp,sizeof(double)*(c.n_elements+1));
}

//Pops a solution from the stack.
void pop_solution(void)
{
	if (STACK_LENGTH>0)
	{
		STACK_LENGTH--;
		free(c.phi);
		free(c.phi_p);
		free(c.phi_pp);
		free(c.I_int);
		free(c.V_int);
		free(c.I_p);
		free(c.V_p);
		free(c.I_pp);
		free(c.V_pp);
		c.phi=STACK[STACK_LENGTH].phi;
		c.phi_p=STACK[STACK_LENGTH].phi_p;
		c.phi_pp=STACK[STACK_LENGTH].phi_pp;
		c.I_int=STACK[STACK_LENGTH].I_int;
		c.V_int=STACK[STACK_LENGTH].V_int;
		c.V_p=STACK[STACK_LENGTH].V_p;
		c.I_p=STACK[STACK_LENGTH].I_p;
		c.V_pp=STACK[STACK_LENGTH].V_pp;
		c.I_pp=STACK[STACK_LENGTH].I_pp;
		STACK=(STACK_ELEMENT *)realloc((void *)STACK,sizeof(STACK_ELEMENT)*(STACK_LENGTH>0?STACK_LENGTH:1));
	}
}

//Returns the function value for row "x".
double func(int x)
{
	//Row select
	if (x>=c.n_meshes+c.n_nodes)
	{
//		cout << "Returning element function " << (x-c.n_meshes-c.n_nodes+1) << "\n";
		return element_func(x-c.n_meshes-c.n_nodes+1);
		//This is an element equation.
	}
	else if (x>c.n_meshes)
	{
//		cout << "Returning node function " << (x-c.n_meshes+1) << "\n";
		return node_func(x-c.n_meshes-1);
		//This is a node equation.
	}
	else
	{
		return mesh_func(x-1);
		//This is a mesh equation.
	}
}
//Generates the jacobian of the system.
//Column order is: current, voltages.
//Row order is: meshes, nodes, element equations.


//Returns the value of the current flowing through an element
double element_current(int element)
{
	if (c.elements[element].TYPE==TYPE_CS || c.elements[element].TYPE==TYPE_NC)
		return c.elements[element].I;
	else 
		return c.I[element];
}

//Returns the value of the voltage between the nodes of an element
double element_voltage(int element)
{
	if (c.elements[element].TYPE==TYPE_VS)
		return c.elements[element].V;
	else 
	{
		return c.V[element];	
	}
}

//Returns the value of the voltage between the nodes of an element
double element_phase(int element)
{
	if (c.elements[element].TYPE==TYPE_PS)
	{
		return c.elements[element].Phi;
	}
	else 
	{
		return c.phi[element];	
	}
}

//This is the function for the nnewt-function of the NR-package. Returns the jacobian of the system and the current function vector for the solution 
void usrfun(double *vec,int n,double *fvec,double **fjac)
{
	int x;
	memcpy(&(c.phi[1]),&(vec[1+c.n_elements]),c.n_elements*sizeof(double));
	memcpy(&(c.I[1]),&(vec[1]),c.n_elements*sizeof(double));
	for(x=1;x<=2*c.n_elements;x++)
	{
		fvec[x]=func(x);
	}
	generate_jacobian(c.fjac);
	for(x=1;x<=2*c.n_elements;x++)
		memcpy(&(fjac[x][1]),&(c.fjac[x][1]),(2*(c.n_elements))*sizeof(double));
}

//Returns the value of the selected node equation
double node_func(int node)
{
	double I=0.0;
	for(int x=0;x<c.nodes[node].n_elements;x++)
	{
		double v=c.nodes[node].elements[x];
		I+=element_current(abs(v))*(v<0.0?-1.0:1.0);
	}
	return I;	
}

//Return the value of the selected mesh equation.
double mesh_func(int mesh)
{
	double phi=0.0;
	for(int x=0;x<c.meshes[mesh].n_elements;x++)
		phi+=element_phase(abs(c.meshes[mesh].elements[x]))*(c.meshes[mesh].elements[x]<0?-1.0:1.0);
	return sin(phi);
}


double SG(ELEMENT e)
{
	double A,t;
	switch(e.SIG)
	{
		case SIG_SINE:
			A=sin(c.t*2.*PI*e.f-e.off)*e.A;
			return A;
			break;
		case SIG_PULSE://This is a pulse generator.
			t=c.t+e.off-(int((c.t+e.off)/e.f))*e.f;
			A=t>=e.f/2.0?e.A:0.0;
			return A;
			break;
		case SIG_INC://This is a increment generator.
			t=int(c.t/e.f);
			A=t*e.A;
			return A;
			break;
	}
	return 0.0;
}


double element_func(int element)
{
	switch(c.elements[element].TYPE)
	{
		case TYPE_R:
			return (c.phi[element]-c.phi_p[element])/c.STEPT-c.I[element]*c.elements[element].R;
			break;
		case TYPE_L:
			return c.I[element]*c.elements[element].L-c.phi[element];
		case TYPE_C:
			return -c.elements[element].C*(c.phi[element]+c.phi_pp[element]-2.*c.phi_p[element])/c.STEPT/c.STEPT+c.I[element];
			break;
		case TYPE_CS:
			return c.I[element]-c.elements[element].I;
			break;
		case TYPE_NC:
			return c.I[element]-c.elements[element].I;
			break;
		case TYPE_VS:
			return (c.phi[element]-c.phi_p[element])/c.STEPT-c.elements[element].V;
			break;
		case TYPE_PS:
			return c.phi[element]-c.elements[element].Phi;
			break;
		case TYPE_CSG://Current signal generator
			return c.I[element]-SG(c.elements[element]);
			break;
		case TYPE_VSG://Voltage signal generator
			return (c.phi[element]-c.phi_p[element])/c.STEPT-SG(c.elements[element]);
			break;
		case TYPE_PSG://phase signal generator
			return c.phi[element]-SG(c.elements[element]);
			break;
		case TYPE_JJ://Josephson junction, I=I_c\cdot\sin{\phi}, \frac{d\phi}{dt}\cdot\frac{\Phi_0}{2\pi}=U
			return c.elements[element].Ic*sin(c.phi[element])+c.elements[element].alpha*(c.phi[element]-c.phi_p[element])/c.STEPT+(c.phi[element]+c.phi_pp[element]-2.*c.phi_p[element])/c.STEPT/c.STEPT-c.I[element];
		case TYPE_KAPPA_JJ://Josephson junction, I=I_c\cdot\sin{\phi}, \frac{d\phi}{dt}\cdot\frac{\Phi_0}{2\pi}=U
			return c.elements[element].Ic*sin(c.phi[element]+c.elements[element].kappa*PI)+c.elements[element].alpha*(c.phi[element]-c.phi_p[element])/c.STEPT+(c.phi[element]+c.phi_pp[element]-2.*c.phi_p[element])/c.STEPT/c.STEPT-c.I[element];
	};
	return 0.0;
}

variable set_block(opts o,interpreter *i)
{
	variable v=init_variable();
	char *BLOCK=NULL;
	if (i->cast_string(&BLOCK,o,0))
	{
		set_block(BLOCK);
	}
	return v;
}

void set_block(char *block)
{
	cout << "Setting block to " << block << "\n";
	c.BLOCK=(char *)malloc(sizeof(char)*(strlen(block)+1));
	strcpy(c.BLOCK,block);
}

int new_node(int N,char *block)
{
	c.n_nodes++;
	c.nodes=(NODE *)realloc((void *)c.nodes,sizeof(NODE)*c.n_nodes);
//	c.nodes[c.n_nodes-1].BLOCK=(char *)malloc(sizeof(char)*(strlen(block)+1));
//	strcpy(c.nodes[c.n_nodes-1].BLOCK,block);
	c.nodes[c.n_nodes-1].BLOCK=block;
	c.nodes[c.n_nodes-1].n_elements=0;
	c.nodes[c.n_nodes-1].N=N;
	c.nodes[c.n_nodes-1].NAME=NULL;
	return c.n_nodes-1;
}

//Creates a new mesh element.
int new_mesh(void)
{
	c.n_meshes++;
	c.meshes=(MESH *)realloc((void *)c.meshes,sizeof(MESH)*c.n_meshes);
	c.meshes[c.n_meshes-1].n_elements=0;
	c.meshes[c.n_meshes-1].N=meshcnt++;
	c.meshes[c.n_meshes-1].broken=0;
	return c.n_meshes-1;	
}

//Finds the corresponding node for an element number (positive for first connection, negative for second)
int find_element_node(int N)
{
	for(int x=0;x<c.n_nodes;x++)
	{
		for(int y=0;y<c.nodes[x].n_elements;y++)
		{
			if (c.nodes[x].elements[y]==N)
				return x;
		}
	}
	return -1;
}

int copy_mesh(int from)
{
	int n;
	n=new_mesh();
//	c.meshes[n].elements=c.meshes[from].elements;
	for(int x=0;x<c.meshes[from].n_elements;x++)
		c.meshes[n].elements[x]=c.meshes[from].elements[x];
	c.meshes[n].n_elements=c.meshes[from].n_elements;
	return n;
}

int is_in_mesh(int mesh,int element)
{
	for(int x=0;x<c.meshes[mesh].n_elements;x++)
		if (fabs((double)c.meshes[mesh].elements[x])==fabs((double)element))
			return 1;
	return 0;
}

//If make_new ist set, the function generates a new mesh and copies "make_new" elements from mesh "mesh" to the new mesh.
void travel_nodes(int mesh,int E,int make_new)
{
	int y,x;
	if (make_new)
	{
		y=copy_mesh(mesh);
	}
	else
		y=check_for_mesh(mesh);
	if (y==-1)
		y=new_mesh();
	int node_1=find_element_node(E);
	int node_2=find_element_node(-E);
	c.nodes[node_1].marked=1;
	cout << "adding element " << E << "\n";
	c.meshes[y].elements[c.meshes[y].n_elements]=E;
	c.meshes[y].n_elements++;
	if (node_2==0)
		return;
	if (node_1<0 || node_2<0)
		return;
	if (c.nodes[node_2].marked==0 || c.nodes[node_2].N==0)
	{
		int cnt=0;
		int new_meshes[1000];
		int new_elements[1000];
		for(x=0;x<c.nodes[node_2].n_elements;x++)
		{
			if (fabs((double)c.nodes[node_2].elements[x])!=fabs((double)E))
			{
				new_meshes[cnt]=cnt==0?y:copy_mesh(y);
				new_elements[cnt]=c.nodes[node_2].elements[x];
				cnt++;
			}
		}
		for (x=0;x<cnt;x++)
			travel_nodes(new_meshes[x],new_elements[x],0);
	}
	else
	{
		cout << "Mesh is broken!\n";
		c.meshes[y].broken=1;
	}
}

int pathfinder(int mesh,int from,int to)
{
	int next;
//	cout << "from " << from << " to " << to << "\n";
	if (from==to)
	{
//		cout << "found...\n";
		return 1;
	}
	for(int x=0;x<c.nodes[from].n_elements;x++)
	{
		if (c.elements[abs(c.nodes[from].elements[x])].marked==0)
		{
//			cout << "Going to " << c.elements[abs(c.nodes[from].elements[x])].name << "\n";
			if (c.elements[abs(c.nodes[from].elements[x])].NODE_1==from)
				next=c.elements[abs(c.nodes[from].elements[x])].NODE_2;
			else
				next=c.elements[abs(c.nodes[from].elements[x])].NODE_1;
			if (c.elements[abs(c.nodes[from].elements[x])].NODE_1==from)
				c.meshes[mesh].elements[c.meshes[mesh].n_elements++]=-abs(c.nodes[from].elements[x]);
			else
				c.meshes[mesh].elements[c.meshes[mesh].n_elements++]=abs(c.nodes[from].elements[x]);
//			cout <<  "added " << c.elements[abs(c.nodes[from].elements[x])].name << "\n";
			c.elements[abs(c.nodes[from].elements[x])].marked=1;
			if (pathfinder(mesh,next,to))
			{
				return 1;
			}
			else
			{
				c.meshes[mesh].n_elements--;
			}
		}
	}
	return 0;
}

//Finds a mesh consisting only of the master branches and one of the optional branches specified by "opt_branch".
void generate_mesh(int b)
{
	int x;
//	cout << "Generating mesh from " << c.opt[b].from << " to " << c.opt[b].to << " via " << c.elements[abs(c.opt[b].via)].name << "\n";
	int mesh=new_mesh();
	for(x=1;x<=c.n_elements;x++)//Unmark all nodes.
		c.elements[x].marked=0;
	for(x=0;x<c.n_opt;x++)
	{
//		cout << "Marking " << c.elements[abs(c.opt[x].via)].name << "\n";
		c.elements[abs(c.opt[x].via)].marked=1;
	}
	c.meshes[mesh].elements[c.meshes[mesh].n_elements++]=abs(c.opt[b].via);
	if (pathfinder(mesh,c.elements[abs(c.opt[b].via)].NODE_1,c.elements[abs(c.opt[b].via)].NODE_2)==0)
	{
		c.meshes[mesh].broken=1;
		cout << "Serious error: Meshing-algorithm doesn't find path...\n";
//		exit(0);
	}
//	cout << "\n";
}

void add_to_master(BRANCH b)
{
	int found=0;
	int x;
	for(x=0;x<c.n_opt;x++)
	{
		if (b.from==c.opt[x].from && b.to==c.opt[x].to && fabs((double)b.via)==fabs((double)c.opt[x].via))
			found=1;
	}
	for(x=0;x<c.n_master;x++)
	{
		if (b.from==c.master[x].from && b.to==c.master[x].to && fabs((double)b.via)==fabs((double)c.master[x].via))
			found=1;
	}
	if (found==0)
	{
//		cout << "Adding master " << b.from << "-> " << b.to << " via " << c.elements[abs(b.via)].name << "\n";
		c.n_master++;
		c.master=(BRANCH *)realloc((void *)c.master,sizeof(BRANCH)*c.n_master);
		c.master[c.n_master-1]=b;	
	}
}

void add_to_opt(BRANCH b)
{
	int found=0;
	int x;
	for(x=0;x<c.n_opt;x++)
	{
		if (b.from==c.opt[x].from && b.to==c.opt[x].to && fabs((double)b.via)==fabs((double)c.opt[x].via))
			found=1;
	}
	for(x=0;x<c.n_master;x++)
	{
		if (b.from==c.master[x].from && b.to==c.master[x].to && fabs((double)b.via)==fabs((double)c.master[x].via))
			  found=1;
	}
	if (found==0)
	{
//		cout << "Adding opt " << b.from << "-> " << b.to << " via " << c.elements[abs(b.via)].name << "\n";
		c.n_opt++;
		c.opt=(BRANCH *)realloc((void *)c.opt,sizeof(BRANCH)*c.n_opt);
		c.opt[c.n_opt-1]=b;	
	}
}

void push_block(void)
{
	c.n_block_stack++;
	c.block_stack=(char **)realloc((void *)c.block_stack,sizeof(char **)*c.n_block_stack);
	c.block_stack[c.n_block_stack-1]=c.BLOCK;
}

void pop_block(void)
{
	if (c.n_block_stack>0)
		c.n_block_stack--;
	c.BLOCK=c.block_stack[c.n_block_stack];
	cout << "Setting block to " << c.BLOCK << "\n";
}

variable push_block(opts o,interpreter *i)
{
	variable v=init_variable();
	push_block();
	return v;
}

variable pop_block(opts o,interpreter *i)
{
	variable v=init_variable();
	pop_block();
	return v;
}

void build_master_tree(void)
{
	BRANCH b;
	for(int x=0;x<c.n_nodes;x++)
	{
		for(int y=0;y<c.nodes[x].n_elements;y++)
		{
			int e=fabs((double)c.nodes[x].elements[y]);
			int node;
//			cout << "Element " << e << " connected to " << x << "\n";
			if (c.elements[e].NODE_1==x)
				node=c.elements[e].NODE_2;
			else
				node=c.elements[e].NODE_1;
//			cout << "Node:" << node << "\n";
//			cout << "node:" << c.nodes[node].marked << ", element:" << c.elements[abs(c.nodes[x].elements[y])].marked << "\n";
			if (c.nodes[node].marked==0 && c.elements[abs(c.nodes[x].elements[y])].marked==0)
			{
				b.from=node>x?x:node;
				b.to=node<=x?x:node;
				c.elements[abs(c.nodes[x].elements[y])].marked=1;
				b.via=c.nodes[x].elements[y];
				add_to_master(b);
				c.nodes[node].marked=1;
			}
			else if (c.elements[abs(c.nodes[x].elements[y])].marked==0)
			{
				b.from=node>x?x:node;
				b.to=node<=x?x:node;
				b.via=c.nodes[x].elements[y];
				c.elements[abs(c.nodes[x].elements[y])].marked=1;
				add_to_opt(b);
			}
		}
		c.nodes[x].marked=1;
	}
//	cout << "Master branches:" << c.n_master << "\n";
//	cout << "Optional branches:" << c.n_opt << "\n";
}
//Builds the node-tree needed for simulation
void build_mesh_tree(void)
{
	int x,y;
	cout << "Building the mesh tree...\n";
	c.meshes=(MESH *)realloc((void *)c.meshes,sizeof(MESH));
	c.n_meshes=0;
	for(x=0;x<c.n_nodes;x++)
		c.nodes[x].marked=0;
	for(x=1;x<=c.n_elements;x++)
		c.elements[x].marked=0;
	build_master_tree();
	for(x=0;x<c.n_opt;x++)
		generate_mesh(x);
	int offset=0;
	for(x=0;x<c.n_meshes;x++)
	{
		c.meshes[x-offset]=c.meshes[x];
		if(c.meshes[x].broken)
			offset++;
	}
	c.n_meshes-=offset;
	c.mesh_matrix=matrix(1,c.n_elements,1,c.n_elements);
	for(x=1;x<=c.n_elements;x++)
		for(y=1;y<=c.n_elements;y++)
			c.mesh_matrix[x][y]=0.0;
	c.n_mesh_elements=0;
	for(x=0;x<c.n_meshes;x++)
		for(y=0;y<c.meshes[x].n_elements;y++)
		{
			c.n_mesh_elements++;
			c.mesh_matrix[x+1][abs(c.meshes[x].elements[y])]=c.meshes[x].elements[y]>0?1.0:-1.0;
		}
}

//Builds the node-tree needed for simulation
ELEMENT build_element_nodes(ELEMENT e,int x)
{
	int y;
	if ((y=check_for_node(e.NODE_1,e.BLOCK))==-1)
		y=new_node(e.NODE_1,e.BLOCK);
	e.NODE_1=y;
	c.nodes[y].n_elements++;
	c.nodes[y].elements[c.nodes[y].n_elements-1]=x;
	if ((y=check_for_node(e.NODE_2,e.TYPE!=TYPE_CON?e.BLOCK:e.SBLOCK))==-1)
		y=new_node(e.NODE_2,e.TYPE!=TYPE_CON?e.BLOCK:e.SBLOCK);
	e.NODE_2=y;
	c.nodes[y].n_elements++;
	c.nodes[y].elements[c.nodes[y].n_elements-1]=-x;
	return e;
}


//Builds the node-tree needed for simulation
void build_node_tree(void)
{
	int x=0;
	int y=0;
	int z=0;
	c.nodes=(NODE *)realloc((void *)c.nodes,sizeof(NODE));
	c.n_nodes=0;
	for(x=1;x<=c.n_elements;x++)
	{
		y=0;
		if ((y=check_for_node(c.elements[x].NODE_1,c.elements[x].BLOCK))==-1)
			y=new_node(c.elements[x].NODE_1,c.elements[x].BLOCK);
		//Added next line!
		c.elements[x].NODE_1=y;
		c.nodes[y].n_elements++;
		c.nodes[y].elements[c.nodes[y].n_elements-1]=x;
		if ((y=check_for_node(c.elements[x].NODE_2,c.elements[x].TYPE!=TYPE_CON?c.elements[x].BLOCK:c.elements[x].SBLOCK))==-1)
			y=new_node(c.elements[x].NODE_2,c.elements[x].TYPE!=TYPE_CON?c.elements[x].BLOCK:c.elements[x].SBLOCK);
		//Added next line!
		c.elements[x].NODE_2=y;
		c.nodes[y].n_elements++;
		c.nodes[y].elements[c.nodes[y].n_elements-1]=-x;
	}
}

//Builds the node-tree needed for simulation
void build_flat_node_tree(void)
{
	int x=0;
	int y=0;
	int z=0;
	c.nodes=(NODE *)realloc((void *)c.nodes,sizeof(NODE));
	c.n_nodes=0;
	for(x=1;x<=c.n_elements;x++)
	{
		y=0;
		if ((y=check_for_node(c.elements[x].NODE_1,"MAIN"))==-1)
			y=new_node(c.elements[x].NODE_1,"MAIN");
		//Added next line!
		c.elements[x].NODE_1=y;
		c.nodes[y].n_elements++;
		c.nodes[y].elements[c.nodes[y].n_elements-1]=x;
		if ((y=check_for_node(c.elements[x].NODE_2,"MAIN"))==-1)
			y=new_node(c.elements[x].NODE_2,"MAIN");
		//Added next line!
		c.elements[x].NODE_2=y;
		c.nodes[y].n_elements++;
		c.nodes[y].elements[c.nodes[y].n_elements-1]=-x;
	}
	c.node_matrix=matrix(1,c.n_elements,1,c.n_elements);
	for(x=1;x<=c.n_elements;x++)
		for(y=1;y<=c.n_elements;y++)
			c.node_matrix[x][y]=0.0;
	c.n_node_elements=0;
	for(x=0;x<c.n_nodes;x++)
		for(y=0;y<c.nodes[x].n_elements;y++)
		{
			c.n_node_elements++;
			c.node_matrix[x+1][abs(c.nodes[x].elements[y])]=c.nodes[x].elements[y]>0?1.0:-1.0;
		}
}


void print_node_tree()
{
	for(int x=0;x<c.n_nodes;x++)
	{
		cout << "Node Nr. " <<c.nodes[x].N << "(" << c.nodes[x].BLOCK  << "):\n";
		for(int y=0;y<c.nodes[x].n_elements;y++)
		{
			cout << "Connected to " << c.elements[abs(c.nodes[x].elements[y])].name << "\n";
		}
	}
}

//Checks the value of the phases along the superconductive elements in all the closed loops (should be constant and =2\pi n)
void CIRC_check_phases(void)
{
	for(int x=0;x<c.n_meshes;x++)
	{
		double phase=0.0;
		double V=0.0;
		if (c.meshes[x].broken!=1)
		{
			for(int y=0;y<c.meshes[x].n_elements;y++)
			{
				phase+=element_phase(abs(c.meshes[x].elements[y]))*(c.meshes[x].elements[y]<0?-1.0:1.0);
				V+=element_voltage(abs(c.meshes[x].elements[y]))*(c.meshes[x].elements[y]<0?-1.0:1.0);
			}
			if (fabs(V)>0.1 || fabs(phase)>0.1)
			{
				cout << "Serious error: Phase/Voltage is not conserved!\n";
				cout << "Phase gain:" << sin(phase) << "\n";
				cout << "Voltage gain:" << V << "\n";
/*				for(int x=0;x<c.n_meshes;x++)
				{
					cout << mesh_func(x) << "\n";
				}*/
//				exit(-1);
			}
		}
	}
}

void print_mesh_tree()
{
	for(int x=0;x<c.n_meshes;x++)
	{
		if (c.meshes[x].broken!=1)
		{	
			cout << "Mesh Nr. " << x << ":\n";
			for(int y=0;y<c.meshes[x].n_elements;y++)
			{
				cout  << c.elements[abs(c.meshes[x].elements[y])].name << "(" << c.meshes[x].elements[y] << ")\t";
			}
			cout << "\n";
		}
	}
}

void init_circuit_funcs(interpreter *i)
{
   i->tie_to_double(CIRC_STEPT_ptr(),"CIRC_STEPT");
   i->tie_to_double(&c.t,"CIRC_T");
   i->add_function(pop_block,"pop_block");
   i->add_function(push_block,"push_block");
   i->add_function(node_name,"node_name");
   i->add_function(set_block,"block");
   i->add_function(initialize,"CIRC_initialize");
   i->add_function(add_element,"add_C");
   i->add_function(add_element,"add_L");
   i->add_function(CIRC_IV,"IV");
   i->add_function(CIRC_IC,"IC");
   i->add_function(add_element,"add_R");
   i->add_function(CIRC_add_monitor,"add_monitor");
   i->add_function(add_CON,"connect");
   i->add_function(add_element,"add_JJ");
   i->add_function(add_element,"add_kappa_JJ");
   i->add_function(add_element,"add_CS");
   i->add_function(add_element,"add_NC");
   i->add_function(add_element,"add_VS");
   i->add_function(add_element,"add_PS");
   i->add_function(add_element,"add_CSG");
   i->add_function(add_element,"add_VSG");
   i->add_function(add_element,"add_PSG");
   i->add_function(CIRC_solve,"CIRC_solve");
   i->add_function(CIRC_run,"CIRC_run");
   i->add_function(CIRC_evolve,"CIRC_evolve");
   i->add_function(build_trees,"CIRC_build_trees");
   i->add_int_constant(MONITOR_CURRENT,"CURRENT");
   i->add_int_constant(MONITOR_VOLTAGE,"VOLTAGE");
   i->add_int_constant(MONITOR_PHASE,"PHASE");
   i->add_int_constant(SIG_SINE,"SIG_SINE");
   i->add_int_constant(SIG_PULSE,"SIG_PULSE");
   i->add_int_constant(SIG_INC,"SIG_INC");
   i->add_int_constant(SOLVER_LU,"SOLVER_LU");
   i->add_int_constant(SOLVER_BILIN,"SOLVER_BILIN");
   i->add_int_constant(SOLVER_NEWTON,"SOLVER_NEWTON");
   c.INT=i;
   c.INTT=200.0;
   c.SOLVER=SOLVER_BILIN;
   c.IVAMP=1.1;
   c.ICAMP=3.0;
   c.IVPERIOD=1.0;
   c.IVSTEPS=400;
   c.ICSTEPS=400;
   i->tie_to_int(&c.SOLVER,"CIRC_SOLVER");
   i->tie_to_double(&c.IVAMP,"IVAMP");
   i->tie_to_double(&c.IVPERIOD,"IVPERIOD");
   i->tie_to_double(&c.INTT,"INTT");
   i->tie_to_int(&c.IVSTEPS,"IVSTEPS");
   i->tie_to_double(&c.ICAMP,"ICAMP");
   i->tie_to_int(&c.ICSTEPS,"ICSTEPS");
}

variable initialize(opts o,interpreter *i)
{
   variable ret=init_variable();
   initialize();
   return ret;
}

variable add_element(opts o,interpreter *i)
{
	variable ret=init_variable();
	int NODE_1,NODE_2;
	double par=0.0;
	double f=0.0;
	double off=0.0;
	double alpha=0.0;
	double kappa=0.0;
	int SIG=0;
	char *str;
	char *name;
	if (i->cast_string(&name,o,0) && i->cast_int(&NODE_1,o,1) && i->cast_int(&NODE_2,o,2) && i->cast_double(&par,o,3))
	{
		str=(char *)malloc(strlen(name)+1);
		strcpy(str,name);
		if (!strcmp(o.calling_func, "add_R"))
			add_R(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_L"))
			add_L(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_C"))
			add_C(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_CSG"))
		{
			if (i->cast_double(&f,o,4) && i->cast_double(&off,o,5) && i->cast_int(&SIG,o,6))
				add_CSG(str,NODE_1,NODE_2,par,f,off,SIG);
		}
		else if (!strcmp(o.calling_func, "add_VSG"))
		{
			if (i->cast_double(&f,o,4) && i->cast_double(&off,o,5) && i->cast_int(&SIG,o,6))
				add_VSG(str,NODE_1,NODE_2,par,f,off,SIG);
		}
		else if (!strcmp(o.calling_func, "add_PSG"))
		{
			if (i->cast_double(&f,o,4) && i->cast_double(&off,o,5) && i->cast_int(&SIG,o,6))
				add_PSG(str,NODE_1,NODE_2,par,f,off,SIG);
		}
		else if (!strcmp(o.calling_func, "add_VS"))
			add_VS(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_PS"))
			add_PS(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_CS"))
			add_CS(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_NC"))
			add_NC(str,NODE_1,NODE_2,par);
		else if (!strcmp(o.calling_func, "add_JJ"))
		{
			if (i->cast_double(&alpha,o,4))
				add_JJ(str,NODE_1,NODE_2,par,alpha);
		}
		else if (!strcmp(o.calling_func, "add_kappa_JJ"))
		{
			if (i->cast_double(&alpha,o,4) && i->cast_double(&kappa,o,5))
				add_kappa_JJ(str,NODE_1,NODE_2,par,alpha,kappa);
		}

	}
	return ret;
}

variable CIRC_IV(opts o,interpreter *i)
{
	variable ret=init_variable();
	int buffer=0;
	char *source;
	char *element;
	if (i->cast_string(&element,o,0) && i->cast_string(&source,o,1) && i->cast_int(&buffer,o,2))
	{
		IV(element,source,buffer);
	}
	return ret;
}

variable CIRC_IC(opts o,interpreter *i)
{
	variable ret=init_variable();
	int buffer=0;
	char *source;
	char *current;
	char *element;
	if (i->cast_string(&element,o,0) && i->cast_string(&source,o,1) && i->cast_string(&current,o,2) && i->cast_int(&buffer,o,3))
	{
		IC(element,source,current,buffer);
	}
	return ret;
}

//Determines the average voltage in an element.
double CIRC_get_V(int el)
{
	double V_av=0.0;
	int AV=(int)(c.INTT/c.STEPT);
	V_av=0.0;
	for(int y=0;y<AV;y++)
	{
		CIRC_solve();
		CIRC_evolve();
		V_av+=c.V[el];
	}
	V_av/=(double)AV;
	return V_av;
}


//Generates an IV-curve, tracking the voltage of element *element as a function of the current through source *source. Stores the result in buffer buf
void IV(char *element,char *source,int buf)
{
	double I=0.0;
	double V=0.0;
	int el=find_element(element,c.BLOCK);
	int sc=find_element(source,c.BLOCK);
	if (el<0 || sc<0)
		return;
	buffer_new_series(buf);
	for(int x=0;x<=c.IVSTEPS;x++)
	{
		I=sin((double)x/(double)c.IVSTEPS*c.IVPERIOD*PI-PI/2.)*c.IVAMP;
		switch(c.elements[sc].TYPE)
		{
			case TYPE_PS:
				c.elements[sc].Phi=I;
				break;
			case TYPE_CS:
				c.elements[sc].I=I;
				break;
			case TYPE_VS:
				c.elements[sc].V=I;
				break;
			default:
				cout << "Error: Can't modify characteristic value in source " << source << "!\n";
				return;
				break;
		}
		V=CIRC_get_V(el);
		cout << "I:" << I << ", V:" << V << "\n";
		if (x>0)
			buffer_add_value(buf,V,I);		
	}
}

//Generates an IC-curve, tracking the critical current of element *element as a function of the value in element *source and by ramping current current*. Stores the result in buffer buf
void IC(char *element,char *source,char *current,int buf)
{
	double f=0.0;
	double V_av=0.0;
	int AV=(int)(400.0/c.STEPT);
	int el=find_element(element,c.BLOCK);
	int sc=find_element(source,c.BLOCK);
	int cu=find_element(current,c.BLOCK);
	if (el<0 || sc<0)
		return;
	buffer_new_series(buf);
	for(int x=0;x<=c.ICSTEPS;x++)
	{
		f=2.0*c.ICAMP*(double)(x-c.ICSTEPS/2)/(double)c.ICSTEPS;
		switch(c.elements[sc].TYPE)
		{
			case TYPE_PS:
				c.elements[sc].Phi=f;
				break;
			case TYPE_CS:
				c.elements[sc].I=f;
				break;
			case TYPE_VS:
				c.elements[sc].V=f;
				break;
			default:
				cout << "Error: Can't modify characteristic value in source " << source << "!\n";
				return;
				break;
		}
		double ic=CIRC_get_Ic(el,cu);
		buffer_add_value(buf,f,ic);
		printf("f = %.6f\t\tIc = %.6f%c",f,ic,char(0x0D));	
		fflush(stdout);
	}
}

#define IC_ACC 10000
#define IC_STEPS 20.
#define TH 0.1
#define MAX 10.0
#define VCRIT 0.05
double CIRC_get_Ic(int el,int cu)
{
	int polarity=0;
	double int_phi=0.;
	double steps=IC_STEPS;
	double y=0.0;
	double a=0.0;
	double V=0.0;
	double gauge_span=0.1;
	c.elements[cu].I=0.0;
	int cnt=0;
	while(fabs(CIRC_get_V(el))>VCRIT && cnt++<100);
	if (cnt==100)
	{
		cout << "Error: Can't init element to subcritical state! Bailing out...\n";
		return -1.0;
	}
	push_solution();
	while(1)
	{
//		cout << "Checking at " << y << "\n";
		y+=((double)MAX)/steps;
		if(a++>10)
		{
			a=0;
			steps/=10.0;
		}  
		c.elements[cu].I=y;
		push_solution();
		V=CIRC_get_V(el);
//		cout << "Voltage:" << V << "\n";
		if(fabs(V)>VCRIT)
		{
			if (steps>=IC_ACC)
			{
				return y;
			}
			else
			{
		//Bring the junction back to zero voltage state...
				a=0;
				pop_solution();
				y-=((double)MAX)/steps;
				push_solution();
				steps*=10.;
			}
		}
//		if (y>=MAX)
//			return MAX;
	}
}


void add_JJ(char *name,int NODE_1,int NODE_2,double Ic,double alpha)
{
	ELEMENT e;
	e.TYPE=TYPE_JJ;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.Ic=Ic;
	e.alpha=alpha;
	e.name=name;
	add_element(e);
}

void add_kappa_JJ(char *name,int NODE_1,int NODE_2,double Ic,double alpha,double kappa)
{
	ELEMENT e;
	e.TYPE=TYPE_KAPPA_JJ;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.Ic=Ic;
	e.alpha=alpha;
	e.kappa=kappa;
	e.name=name;
	add_element(e);
}


void add_C(char *name,int NODE_1,int NODE_2,double C)
{
	ELEMENT e;
	e.TYPE=TYPE_C;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.BLOCK=c.BLOCK;
	e.MODEL=NULL;
	e.C=C;
	e.name=name;
	add_element(e);
}


void add_L(char *name,int NODE_1,int NODE_2,void *MODEL)
{
	ELEMENT e;
	e.TYPE=TYPE_L;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.BLOCK=c.BLOCK;
	e.MODEL=MODEL;
	e.name=name;
	add_element(e);
}

void add_R(char *name,int NODE_1,int NODE_2,double R)
{
	ELEMENT e;
	e.TYPE=TYPE_R;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.R=R;
	e.name=name;
	add_element(e);
}

void add_L(char *name,int NODE_1,int NODE_2,double L)
{
	ELEMENT e;
	e.TYPE=TYPE_L;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.L=L;
	e.name=name;
	add_element(e);
}

void add_CON(int NODE_1,char *BLOCK1,int NODE_2,char *BLOCK2)
{
	static int con_number=0;
	ELEMENT e;
	e.TYPE=TYPE_CON;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.BLOCK=(char *)malloc(sizeof(char)*(strlen(BLOCK1)+1));
	e.SBLOCK=(char *)malloc(sizeof(char)*(strlen(BLOCK2)+1));
	strcpy(e.BLOCK,BLOCK1);
	strcpy(e.SBLOCK,BLOCK2);
//	cout << "Connecting node " << NODE_1 << " in block " << BLOCK1 << " to node " << NODE_2 << " in block " << BLOCK2 << "\n";
	e.name=(char *)malloc(sizeof(char)*1024);
	sprintf(e.name,"connection-%.2d",con_number++);
	add_element(e);
}

int get_node_N(char *name,char *block)
{
	for (int x=0;x<c.n_nodes;x++)
	{
		if (strcmp(block,c.nodes[x].BLOCK)==0)
		{
			if (c.nodes[x].NAME!=NULL)
				if (strcmp(c.nodes[x].NAME,name)==0)
					return c.nodes[x].N;
		}
	}
	return -1;
}

variable add_CON(opts o,interpreter *i)
{
	variable v=init_variable();
	int NODE_1=0;
	int NODE_2=0;
	char *NODE_1_STR=NULL;
	char *NODE_2_STR=NULL;
	char *BLOCK1=NULL;
	char *BLOCK2=NULL;
	if (i->cast_string(&BLOCK1,o,1) && i->cast_string(&BLOCK2,o,3))
	{
		if (!i->cast_int(&NODE_1,o,0))
		{
			if (i->cast_string(&NODE_1_STR,o,0))
				NODE_1=get_node_N(NODE_1_STR,BLOCK1);
		}
		if (!i->cast_int(&NODE_2,o,2))
		{
			if (i->cast_string(&NODE_2_STR,o,2))
				NODE_2=get_node_N(NODE_2_STR,BLOCK2);
//			cout << "Looking for node " << NODE_2_STR << "\n";
		}
		add_CON(NODE_1,BLOCK1,NODE_2,BLOCK2);
	}
	return v;
}

void add_CS(char *name,int NODE_1,int NODE_2,double I)
{
	ELEMENT e;
	e.TYPE=TYPE_CS;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.I=I;
	e.name=name;
	add_element(e);
}


void add_NC(char *name,int NODE_1,int NODE_2,double theta)
{
	ELEMENT e;
	e.TYPE=TYPE_NC;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.theta=theta;
	e.I=noise(theta);
	e.name=name;
	add_element(e);
}

void add_CSG(char *name,int NODE_1,int NODE_2,double A,double f,double off,int SIG)
{
	ELEMENT e;
	e.TYPE=TYPE_CSG;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.BLOCK=c.BLOCK;
	e.A=A;
	e.f=f;
	e.SIG=SIG;
	e.off=off;
	e.name=name;
	add_element(e);
}

void add_VSG(char *name,int NODE_1,int NODE_2,double A,double f,double off,int SIG)
{
	ELEMENT e;
	e.TYPE=TYPE_VSG;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.BLOCK=c.BLOCK;
	e.MODEL=NULL;
	e.A=A;
	e.f=f;
	e.SIG=SIG;
	e.off=off;
	e.name=name;
	add_element(e);
}

void add_PSG(char *name,int NODE_1,int NODE_2,double A,double f,double off,int SIG)
{
	ELEMENT e;
	e.TYPE=TYPE_PSG;
	e.NODE_1=NODE_1;
	e.BLOCK=c.BLOCK;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.A=A;
	e.f=f;
	e.SIG=SIG;
	e.off=off;
	e.name=name;
	add_element(e);
}

void add_VS(char *name,int NODE_1,int NODE_2,double V)
{
	ELEMENT e;
	e.TYPE=TYPE_VS;
	e.NODE_1=NODE_1;
	e.BLOCK=c.BLOCK;
	e.NODE_2=NODE_2;
	e.MODEL=NULL;
	e.V=V;
	e.name=name;
	add_element(e);
}


void add_PS(char *name,int NODE_1,int NODE_2,double Phi)
{
	ELEMENT e;
	e.TYPE=TYPE_PS;
	e.NODE_1=NODE_1;
	e.NODE_2=NODE_2;
	e.BLOCK=c.BLOCK;
	e.MODEL=NULL;
	e.Phi=Phi;
	e.name=name;
	add_element(e);
}

void add_element(ELEMENT e)
{
	char str[1024];
	c.n_elements++;
	char *name=e.name;
	cout << "Adding element " << e.name << " at nodes " << e.NODE_1 << " (" << e.BLOCK << "), " << e.NODE_2 << " (" << (e.TYPE==TYPE_CON?e.SBLOCK:e.BLOCK) << ")\n";
	c.elements=(ELEMENT *)realloc((void *)c.elements,sizeof(ELEMENT)*(c.n_elements+1));
	c.elements[c.n_elements]=build_element_nodes(e,c.n_elements);
}

char *type_name(int type)
{
	switch(type)
	{
		case TYPE_JJ:return "JJ";break;
		case TYPE_KAPPA_JJ:return "kappa_JJ";break;
		case TYPE_R:return "R";break;
		case TYPE_L:return "L";break;
		case TYPE_C:return "C";break;
		case TYPE_CS:return "CS";break;
		case TYPE_NC:return "NC";break;
		case TYPE_VS:return "VS";break;
		case TYPE_PS:return "PS";break;
		default:return "unknown";break;
	}
}

double *CIRC_STEPT_ptr()
{
	return &c.STEPT;
}

variable CIRC_run(opts o,interpreter *i)
{
	double T=0.0;
	variable ret=init_variable();
	if (i->cast_double(&T,o,0))
	{
		CIRC_run(T);
	}
	return ret;
}

//Runs the simulator for time time
void CIRC_run(double T)
{
	int x;
	char buffer[2048];
	 clock_t t1=clock();
#ifndef FLUX_WINDOWS
	struct timeval tv1,tv2;
	struct timezone tz1,tz2;
	gettimeofday(&tv1,&tz1);
#endif
	generate_linear_matrix(c.lin,c.vec,0.0);
	sprsin(c.lin,c.map,2*c.n_elements,THLIN,c.sparse_n,sa,ija);
	for(x=1;x<=2*c.n_elements;x++)
		memcpy(&(c.lin_save[x][1]),&(c.lin[x][1]),(2*(c.n_elements))*sizeof(double));
	cout << "Running circuit for " << T << " time units...\n";
	for(x=0;x<T/c.STEPT;x++)
	{
		CIRC_solve();
		CIRC_evolve();
	}
	cout << "Done.\n";
#ifndef FLUX_WINDOWS
	gettimeofday(&tv2,&tz2);
	cout << "Simulation time:" << (tv2.tv_sec-tv1.tv_sec+(tv2.tv_usec-tv1.tv_usec)/1.0e6) << " seconds.\n";	
#endif
             clock_t t2=clock();
    sprintf(buffer,"%.4lf seconds of processing\n", (t2-t1)/(double)CLOCKS_PER_SEC);
	cout << buffer;
}

long int c_idnum=345535345;
double noise(double theta)
{
	double n1;
	n1=gasdev(&c_idnum);
	return n1*sqrt(theta);//2*ALPHA(?)
}

double CIRC_gnd_voltage(int node)
{
	return CIRC_gnd_path(node,c.V);
}

double CIRC_gnd_phase(int node)
{
	return CIRC_gnd_path(node,c.phi);
}

double CIRC_gnd_path(int node,double *array)
{
	int x,y;
	int found_node;
	int found_gnd;
	int elements=0xFFFFFF;
	double voltage=0.0;
	int mesh=-1;
	if (node==0)
		return 0.0;
	for(x=0;x<c.n_meshes;x++)
	{
		found_node=0;
		found_gnd=0;
		for(y=0;y<c.meshes[x].n_elements;y++)
		{
			if (c.elements[abs(c.meshes[x].elements[y])].NODE_2==node || c.elements[abs(c.meshes[x].elements[y])].NODE_1==node)
				found_node=1;
			if (c.elements[abs(c.meshes[x].elements[y])].NODE_1==0 || c.elements[abs(c.meshes[x].elements[y])].NODE_2==0)
				found_gnd=1;
		}
		if (found_gnd==1 && found_node==1)
		{
			if (c.meshes[x].n_elements<elements)
			{
				elements=c.meshes[x].n_elements;
				mesh=x;
			}
		}
	}
	int done=0;
	x=0;
	found_node=0;
	found_gnd=0;
	int cur_node=node;
	double sign=1.0;
	if (mesh>=0)
	{
		while(cur_node!=0)
		{
			if (c.elements[abs(c.meshes[mesh].elements[x])].NODE_1==cur_node)
			{
				cur_node=c.elements[abs(c.meshes[mesh].elements[x])].NODE_2;
				voltage+=array[abs(c.meshes[mesh].elements[x])]*(c.meshes[mesh].elements[x]>0?1.0:-1.0);
			}
			else if (c.elements[abs(c.meshes[mesh].elements[x])].NODE_2==cur_node)
			{
				cur_node=c.elements[abs(c.meshes[mesh].elements[x])].NODE_1;
				voltage+=array[abs(c.meshes[mesh].elements[x])]*(c.meshes[mesh].elements[x]>0?1.0:-1.0);
			}
			x++;
			x%=c.meshes[mesh].n_elements;
		}
	}
	return voltage;
}


void CIRC_evolve()
{
	int x;
	double *tV,*tI,*tphi;
//	CIRC_check_phases();
	for(x=1;x<=c.n_elements;x++)
	{
		if (c.elements[x].TYPE==TYPE_NC)
			c.elements[x].I=noise(c.elements[x].theta);
		c.V[x]=(c.phi[x]-c.phi_p[x])/c.STEPT;
//		c.I_int[x]+=c.STEPT*c.I[x];
//		c.V_int[x]+=c.STEPT*c.V[x];
	}
	for(x=0;x<c.n_monitors;x++)
	{
		if (c.MONITORS[x].element>=0)
		{
			if (c.MONITORS[x].type==MONITOR_VOLTAGE)
			{
				buffer_add_value(c.MONITORS[x].buffer,c.t,c.V[c.MONITORS[x].element]);
			}
			else if (c.MONITORS[x].type==MONITOR_CURRENT)
			{
				buffer_add_value(c.MONITORS[x].buffer,c.t,c.I[c.MONITORS[x].element]);
			}
			else if (c.MONITORS[x].type==MONITOR_PHASE)
			{
				buffer_add_value(c.MONITORS[x].buffer,c.t,c.phi[c.MONITORS[x].element]);
			}
		}
		else
		{
			if (c.MONITORS[x].NODE_1>=0 && c.MONITORS[x].NODE_2>=0)
			{
				if (c.MONITORS[x].type==MONITOR_VOLTAGE)
				{
					buffer_add_value(c.MONITORS[x].buffer,c.t,CIRC_gnd_voltage(c.MONITORS[x].NODE_1)-CIRC_gnd_voltage(c.MONITORS[x].NODE_2));
				}
				else if (c.MONITORS[x].type==MONITOR_PHASE)
				{
					buffer_add_value(c.MONITORS[x].buffer,c.t,CIRC_gnd_phase(c.MONITORS[x].NODE_1)-CIRC_gnd_phase(c.MONITORS[x].NODE_2));
				}
			}
		}
	}
	c.t+=c.STEPT;
	tV=c.V_pp;
	tI=c.I_pp;
	tphi=c.phi_pp;
	c.V_pp=c.V_p;
	c.I_pp=c.I_p;
	c.phi_pp=c.phi_p;
	c.V_p=c.V;
	c.I_p=c.I;
	c.phi_p=c.phi;
	c.V=tV;
	c.I=tI;
	c.phi=tphi;
}

void initialize(void)
{
	c.n_elements=0;
	c.elements=(ELEMENT *)malloc(sizeof(ELEMENT));
	c.n_nodes=0;
	c.t=0.0;
	c.n_block_stack=0;
	c.block_stack=(char **)malloc(sizeof(char **));
	c.n_master=0;
	c.n_opt=0;
	c.master=(BRANCH *)malloc(sizeof(BRANCH));
	c.opt=(BRANCH *)malloc(sizeof(BRANCH));
	c.STEPT=0.01;
	c.nodes=(NODE *)malloc(sizeof(NODE));
	c.n_monitors=0;
	c.MONITORS=(MONITOR *)malloc(sizeof(MONITOR));
	c.I=(double *)malloc(sizeof(double));
	c.V=(double *)malloc(sizeof(double));
	c.phi=(double *)malloc(sizeof(double));
	c.I_p=(double *)malloc(sizeof(double));
	c.V_p=(double *)malloc(sizeof(double));
	c.phi_p=(double *)malloc(sizeof(double));
	c.I_pp=(double *)malloc(sizeof(double));
	c.V_pp=(double *)malloc(sizeof(double));
	c.phi_pp=(double *)malloc(sizeof(double));
	c.I_int=(double *)malloc(sizeof(double));
	c.V_int=(double *)malloc(sizeof(double));
	c.JJ=NULL;
//	if (check_for_node(0,"MAIN")==-1)
	set_block("MAIN");
}


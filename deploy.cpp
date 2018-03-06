#include "deploy.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include "math.h"
#include "time.h"
#include <sys/timeb.h>
#include <cstring>

#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

#define popsize 5
#define maxgens 50000
#define pxover 0.9
#define pmutation 0.9
#define pmutation_out 0.9
#define nvars_long 800
int nvars = 0;
int Cmax = 0; 


struct genotype
{
	int gene[nvars_long];
	int course;
	double fitness;
	double rfitness;
	double cfitness;
};



struct genotype population[popsize];
struct genotype newpopulation[popsize];
struct genotype better;

using namespace std;

/*����һ����Ȩֵ������ͼ*/
typedef int vertextype;     //���嶥������Ϊ����
typedef int  edgetype;    //�ߵ�����
#define MAXVEX 1000     //��󶥵���
#define INFINITY 0x0fffffff     //���ֵ 

class MGraph
{
	public:
		MGraph(int numVertexes_,int numEdges_)
			:numVertexes(numVertexes_),numEdges(numEdges_){};
		vertextype vexs[MAXVEX];  //�����
		edgetype net[MAXVEX][MAXVEX]; //�ڽӾ��󣬴洢�ߴ��� 
		edgetype cost[MAXVEX][MAXVEX];//�ڽӾ��󣬴洢�ߵ�λ����۸� 
		int numVertexes, numEdges;   //ͼ�е�ǰ�Ķ������ͱ���
		~MGraph(){};
	private:

};

int cons[MAXVEX]={0};//�洢���ѽڵ㼰�����ӵĽڵ� 
int consnet[MAXVEX]={0};
int consneed[MAXVEX]={0};
MGraph *G = new MGraph(0,0);   //new һ��ͼ 
MGraph *G1 = new MGraph(0,0);

string res;
char a[20];

//�����ĸ�ȫ�ֱ�������SPFA���·�㷨����С����������� 

typedef int arrT[MAXVEX];
typedef bool arrB[MAXVEX];
//int *path = new arrT; 
int path[MAXVEX];
//int *dist = new arrT;
//int *q = new arrT;
//bool *visit = new arrB;

int s;//��Դ�� 
int t;//����� 

int maxflow_need = 0;
int servercost = 0;
unsigned long out_s=0;
unsigned long out_s_accumulate=0;

//SPFA�����·�㷨���ٶȿ� 
bool SPFA()
{
    int dist[MAXVEX];
    bool visit[MAXVEX];
    int q[MAXVEX];

	int i, j, f=0, r=1;
	memset(path, -1, sizeof(path));
	memset(visit, 0, sizeof(visit));
	fill(dist, dist+MAXVEX,INFINITY);
	dist[s] = 0;
	q[r] = s;
	visit[s] = true;
	while (f < r)
	{
		f++;
		i = q[f];	visit[i] = false;
		for (j = 0; j < G->numVertexes; j++)
			if ((G->net[i][j] > 0) && (dist[j] > dist[i] + G->cost[i][j]))
			{
				if (!visit[j])
				{
					r++;
					q[r] = j;
					visit[j] = true;
				}
				dist[j] = dist[i] + G->cost[i][j];
				path[j] = i;
			}
		//	cout<<i<<endl; 
	}

	return dist[t] != INFINITY;
}


//��С����������㷨 
int min_cost_max_flow()
{
    int mincost = 0, maxflow = 0;
    while( SPFA() )			//�����ڴ�s��t�����·������·��
	{
		int now = t;
        int neck = INFINITY;
        while(now != s) 			//�������·���ϵ���С��������
		{
            int pre = path[now];
            neck = min(neck, G->net[pre][now]);
            now = pre;
        }
        maxflow += neck;			//���������
        now = t;
        while(now != s) 			//����
		{
             int pre = path[now];
             G->net[pre][now] -= neck;
             mincost += G->cost[pre][now] * neck;	//������С����
             now = pre;
        }
   
    }
    // cout<<"success"<<endl;
    if(maxflow == maxflow_need)
        return mincost;
    else
        return INFINITY;
}

//��С����������㷨�����һ�δ�ӡ�� 
int min_cost_max_flow_printf()
{
	int mincost = 0, maxflow = 0, totalpath = 0;
    while( SPFA() )			//�����ڴ�s��t�����·������·��
	{
        int now = t;
        int neck = INFINITY;
        while(now != s) 			//�������·���ϵ���С��������
		{
            int pre = path[now];
            neck = min(neck, G->net[pre][now]);
            now = pre;
        }
        maxflow += neck;			//���������
        now = t;

        int consumerprint = path[now];
        sprintf(a, "%d %d",cons[consumerprint],neck);
        res = a+res;
        totalpath++;
        while(now != s) 			//����
		{
             int pre = path[now];
             G->net[pre][now] -= neck;	
			 		
             //G->net[now][pre] += neck;		//������
             //G->cost[now][pre] = - G->cost[pre][now];	//���·���ߵĴ���

             mincost += G->cost[pre][now] * neck;	//������С����
             now = pre;
             if(pre!=(G->numVertexes-1))
             {
             	sprintf(a, "%d ",pre);
                res = a+res;
			 }
        }
        
        res = "\n"+res;
    }
    
    sprintf(a, "%d\n",totalpath);
	res = a+res;
    return maxflow;
}

void initialize(int consumerNum1)		// ��ʼ���ṹ��
{	
	int i,j,k;
	int p;
	for (i=0;i<popsize;i++)
	{
		for (j=0;j<nvars_long;j++ )
		{
			population[i].gene[j]=0;		 
		}
		population[i].course=0;
		population[i].fitness=0;
		population[i].rfitness=0;
		population[i].cfitness=0;
	}										// ��ʼ��
	printf("��ʼ����Ⱥ�У�");
	for (j = 0; j < consumerNum1; j++)
    {
	    population[0].gene[consnet[j]] = 1;
    }

	for (i=1;i<popsize;i++ )
	{  
	    p=1;								// ����Ȩ��ʼ��
       // k = rand()%nvars;
		while (p<=nvars/3)
		{
			j = rand()%nvars;				// �漴��������Ȩ
			if (population[i].gene[j]==0)
			{
				population[i].gene[j]=1;
				p++;
			}
			
		}
		if((i+1)%10==0)	printf(".");

	}
	printf("\n");
}

void evaluate(int netnodeNum1)
{
	int i,j,k,n,m;						
	//double abc;				// ������һ����ת���� 
	for (n=0;n<popsize ;n++ )
	{
        *G= *G1;
        m = 0;

        for (j=0;j<nvars;j++ )
		{ 
		  if(population[n].gene[j]==1)	 
		  {
		  	G->net[netnodeNum1+1][j]=INFINITY;
            G->cost[netnodeNum1+1][j]=0;
            m++;
		  }
		}
		s=netnodeNum1+1;
        t=netnodeNum1;
  
        population[n].course=min_cost_max_flow()+ servercost*m;	
		if((population[n].course>Cmax)&&(population[n].course<INFINITY))
		   	Cmax=population[n].course+1;	 	
//     	abc=population[n].course;		// ����ֵ����ת����INT��תΪDOUBLE��
//		population[n].fitness=10000/abc;	// ������Ӧ��
		//printf("%f\n",1/abc);  	
	}
	//cout<<Cmax<<" ";
	for (n=0;n<popsize ;n++ )
	{
	   population[n].fitness=Cmax-population[n].course;
	   if(population[n].fitness<0) 
	          population[n].fitness = 0;
	}

		
	
}

void best(void)
{
	int i;
	better.fitness=0;
	for (i=0;i<popsize ;i++ )
	{
		if (population[i].fitness>better.fitness)
		{
			better=population[i];
		}
	}
}

void keep_the_best(void)
{
	int i;
	for (i=0;i<popsize ;i++ )
	{
		if (population[i].fitness>better.fitness)
		{
			better=population[i];
		}
	}
}

void elitist() 
{ 
	int i; 
	double best, worst; /* best and worst fitness values */ 
	int best_mem, worst_mem; /* indexes of the best and worst member */ 
	
	best = population[0].fitness; 
	worst = population[0].fitness; 
	for (i = 0; i < popsize - 1; ++i) 
	{ 
		if(population[i].fitness > population[i+1].fitness) 
		{ 
			if (population[i].fitness >= best) 
			{ 
			best = population[i].fitness; 
			best_mem = i; 
			} 
			if (population[i+1].fitness <= worst) 
			{ 
			worst = population[i+1].fitness; 
			worst_mem = i + 1; 
			} 
		} 
	    else 
	    { 
			if (population[i].fitness <= worst) 
			{ 
			worst = population[i].fitness; 
			worst_mem = i; 
			} 
			if (population[i+1].fitness >= best) 
			{ 
			best = population[i+1].fitness; 
			best_mem = i + 1; 
			} 
	    } 
	} 
	/* if best individual from the new population is better than */ 
	/* the best individual from the previous population, then */ 
	/* copy the best from the new population; else replace the */ 
	/* worst individual from the current population with the */ 
	/* best one from the previous generation */ 
	if (best >= better.fitness) 
	{ 
		better = population[best_mem];
	} 
	else 
	{ 
		population[worst_mem] = better; 
	} 
} 

void select(void)
{
	int i,j;
	double sum=0;
	double p;
	for (i=0;i<popsize ;i++ )
	{
		sum+=population[i].fitness;
	}
	for (i=0;i<popsize ;i++ )
	{
		population[i].rfitness =  population[i].fitness/sum;
	//	cout<<population[i].rfitness<<" ";
	}
	population[0].cfitness = population[0].rfitness;
	for (i=1;i<popsize ;i++ )
	{
		population[i].cfitness =  population[i-1].cfitness + population[i].rfitness;
	}
	for (i = 0; i < popsize; i++)
    { 
		p = rand()%1000/1000.0;
		if (p < population[0].cfitness)
			newpopulation[i] = population[0];      
		else
		{
			for (j = 0; j <popsize ;j++)      
				if (p >= population[j].cfitness && p<population[j+1].cfitness)
                        newpopulation[i] = population[j+1];
		}
	}
	for (i=0;i<popsize ;i++ )
	{
		population[i] = newpopulation[i];
	}
}

void xover(int cc,int d)
{
	int a,b;
	int i,j,k;
	int temp;
	a=rand()%nvars;
	b=rand()%nvars;
	if(a<b)
	{
		temp=a;a=b;b=temp;
	}
	for(i=a;i<=b;i++)
	{
		temp=population[cc].gene[i];
		population[cc].gene[i]=population[d].gene[i];
		population[d].gene[i]=temp;
	}
	for(i=0;i<=(b-a);i++)
	{
		for(j=0;j<a;j++)
			for(k=a;k<=b;k++)
				if(population[cc].gene[j]==population[cc].gene[k])
					population[cc].gene[j]=population[d].gene[k];
		for(j=b+1;j<nvars;j++)
			for(k=a;k<=b;k++)
				if(population[cc].gene[j]==population[cc].gene[k])
					population[cc].gene[j]=population[d].gene[k];
	}
	for(i=0;i<=(b-a);i++)
	{
		for(j=0;j<a;j++)
			for(k=a;k<=b;k++)
				if(population[d].gene[j]==population[d].gene[k])
					population[d].gene[j]=population[cc].gene[k];
		for(j=b+1;j<nvars;j++)
			for(k=a;k<=b;k++)
				if(population[d].gene[j]==population[d].gene[k])
					population[d].gene[j]=population[cc].gene[k];
	}
}

void crossover(void)
{
	int one,two;
	double x;
	int temp=1;
	for (one=0;one<popsize ;one++ )
	{
		x=rand()%1000/1000.0;
		if (x<pxover)
		{
			if (temp%2==0)
				xover(one,two);
			else
				two=one;
		temp++;
		}
	}
}

void mutate(void)
{
	int i,j,k;
	double x;
	int temp;
	for (k=0;k<popsize ;k++ )
	{
		x=rand()%1000/1000.0;
		if (x<pmutation)
		{
			i=rand()%nvars;
			j=rand()%nvars;
			while (j==i)
			{
				j=rand()%nvars;
			}
			temp=population[k].gene[i];
			population[k].gene[i]=population[k].gene[j];
			population[k].gene[j]=temp;
		}
	}	
}

void mutate_out(void)
{
	int i,k;
	double x;
	int temp;
	for (k=0;k<popsize ;k++ )
	{
		x=rand()%1000/1000.0;
		if (x<pmutation_out)
		{
			i=rand()%nvars;
			if(population[k].gene[i]==0)
			    population[k].gene[i]=1;
			else if(population[k].gene[i]==1)
			    population[k].gene[i]=0;
		}
	}
	
}

void report(void)
{
	int i;
	printf("\nthe best course is:%d",better.course);
}

void Out_time(unsigned long &out_s)
{
    struct timeb rawtime;
    struct tm * timeinfo;
    ftime(&rawtime);
    timeinfo = localtime(&rawtime.time);

    static unsigned long st = rawtime.time;
    out_s = rawtime.time - st;
    st = rawtime.time;
}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	int consumerNum = 0;
    int netnodeNum = 0;
    int linkNum = 0;
    char *c;
    char *c1;
    int spaceCount = 0;
    c = topo[0];
    Out_time(out_s);
    srand((unsigned)time(NULL));

    while (*c != '\0' && *c != '\n' && *c != '\r')
    {

        if (*c == ' ')
        {
            c++;
            spaceCount++;
            continue;
        }
        if (spaceCount == 2)
        {
            consumerNum = *c - '0' + consumerNum * 10;
        }
        if (spaceCount == 1)
        {
            linkNum = *c - '0' + linkNum * 10;
        }
        if (spaceCount == 0)
        {
            netnodeNum = *c - '0' + netnodeNum * 10;
        }
        c++;
    }
   // cout<< netnodeNum<<" "<<linkNum<<" "<<consumerNum<<endl;
	G->numVertexes=netnodeNum+2;
    G->numEdges=linkNum;

    for (int i = 0; i < G->numVertexes; i++)/* ��ʼ��ͼ */
	{
		G->vexs[i]=i;
	}

	for (int i = 0; i < G->numVertexes; i++)/* ��ʼ��ͼ */
	{
		for ( int j = 0; j < G->numVertexes; j++)
		{
			if (i==j)
			{
				G->net[i][j] = 0;
			    G->cost[i][j] = INFINITY;
			}
			else
			{
				G->net[i][j] = G->net[j][i] = 0;
				G->cost[i][j]= G->cost[j][i]= INFINITY;
			}
		}
	}

	spaceCount = 0;
    c = topo[2];
    while (*c != '\0' && *c != '\n' && *c != '\r')
    {
    	if (spaceCount == 0)
        {
            servercost = *c - '0' + servercost * 10;
        }
        c++;
    }
    //cout<<servercost;

    int linkstart = 0,linkend = 0,total_bandwidth = 0,unitcost = 0;
    for(int i = 0;i < G->numEdges; i++)
    {

       spaceCount = 0;
       c = topo[i+4];
       linkstart = 0,linkend = 0,total_bandwidth = 0,unitcost = 0;
        while (*c != '\0' && *c != '\n' && *c != '\r')
       {

        if (*c == ' ')
        {
            c++;
            spaceCount++;
            continue;
        }
        if (spaceCount == 3)
        {
            unitcost = *c - '0' + unitcost * 10;
        }
        if (spaceCount == 2)
        {
            total_bandwidth = *c - '0' + total_bandwidth * 10;
        }
        if (spaceCount == 1)
        {
           linkend = *c - '0' + linkend * 10;
        }
        if (spaceCount == 0)
        {
           linkstart = *c - '0' + linkstart * 10;
        }
        c++;
       }
     //  cout<<linkstart<<" "<<linkend<<" "<<total_bandwidth<<" "<<unitcost<<endl;
       	G->cost[linkstart][linkend] = unitcost;
       	G->cost[linkend][linkstart] = unitcost;
        G->net[linkstart][linkend]= total_bandwidth;
       	G->net[linkend][linkstart]= total_bandwidth;
       	
    }

    int netnode=0, need=0;
    for (int i = 0; i < consumerNum; i++)
    {
        c = topo[i+5+G->numEdges];
        netnode = need = spaceCount = 0;
        while (*c != '\0' && *c != '\n' && *c != '\r')
        {
            if (*c == ' ')
            {
                c++;
                spaceCount++;
                continue;
            }
            if (spaceCount == 1)
            {
                netnode = *c - '0' + netnode * 10;
            }
            else if (spaceCount == 2)
            {
                need = *c - '0' + need * 10;
            }
            c++;
        }
        G->net[netnode][netnodeNum]=need;//����� netnodeNum��Ϊ����㣬�� netnodeNum+1Ϊ��Դ�㣬��Դ�����ӵ���������ÿ�ε�������һ�� 
        G->cost[netnode][netnodeNum]=0;
        cons[netnode] = i;
        maxflow_need = maxflow_need + need;
        consnet[i] = netnode;
        consneed[i] = need;
    }
    *G1 = *G;
    cout<<maxflow_need<<endl;

	int generation=1;
	int better_repetition = 0;
	int pre_better = 0;
	nvars = netnodeNum;

	initialize(consumerNum);
	evaluate(netnodeNum);
	best();
	printf("�Ŵ�������\n");
	while (generation<=maxgens)
	{		
		Out_time(out_s);
        out_s_accumulate+=out_s;
	    if(out_s_accumulate>86) break;
		
		select();
		crossover();
		if(better_repetition>10)
		{
		crossover();
		}
	    mutate();
		mutate_out();
		
		evaluate(netnodeNum);
		//keep_the_best();
		elitist();
		cout<<better.course<<endl;
		if(better.course==pre_better)
			better_repetition++;
		else better_repetition=0;
		//cout<<better_repetition<<endl;
		pre_better = better.course;
		
		generation++;

	}
	report();
	
	*G = *G1;
    for (int j=0;j<nvars;j++ )
	{ 
		if(better.gene[j]==1)	 
		{
		  	G->net[netnodeNum+1][j]=INFINITY;
            G->cost[netnodeNum+1][j]=0;
		}
	}
	s=netnodeNum+1;
    t=netnodeNum;
    
    cout<<endl;
    int present_min_cost = 0;//Ŀǰѡ���������µ���ͼ۸� 
    present_min_cost=min_cost_max_flow_printf();
    cout<<present_min_cost<<endl;
    

	delete G;
	delete G1;
//	delete [] path;
//	delete [] dist;
//  delete [] visit;
//	delete [] q;
    char * topo_file = (char *)res.c_str();
    write_result(topo_file, filename);
}

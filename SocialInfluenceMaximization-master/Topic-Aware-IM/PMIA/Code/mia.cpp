#include "MIA.h"
#include "graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <vector>
#include <cassert>
int MIA::n = 0;
int MIA::top = 0;
double MIA::d[MAX_K];
int MIA::list[MAX_K];
char MIA::file[] = "MIA_0000.txt";

int MIA::k=1;
vector<int> MIA::dd(MAX_NODE,0);
double MIA::longest = log(100.0);
vector<double> MIA::dp(MAX_NODE,1.0);
vector<bool> MIA::used(MAX_NODE);
vector<double *>MIA::self(MAX_NODE);
vector<int> MIA::lastupdate(MAX_NODE,-1);
vector<double *>MIA::delta(MAX_NODE);
vector<int *>MIA::children(MAX_NODE,NULL);
vector<int *>MIA::path(MAX_NODE,NULL);

int *MIA::S;
double *MIA::distance, *MIA::b;
int *MIA::numchild;
int *MIA::queue;
int *MIA::heap;
int *MIA::childlist, *MIA::oldchildlist, *MIA::parent;
bool *MIA::validlist[MAX_K]={NULL};
vector<int> *MIA::childnum;
vector<double> *MIA::allb;
//double ratio=0.01;

int MIA::GetMax(int round)
{
		double max = -1000000.0;
		int mp = -1;
		for (int j=0; j<n; j++)
			if (!used[j] && lastupdate[j]!=round)
			{
				double tmp = dp[j];
				if (tmp >max)
				{
					max = tmp;
					mp = j;
				}
			}
		return mp;


}

int MIA::GetMax0(int round)
{
		double max = -1000000.0;
		int mp = -1;
		return mp;


}

int MIA::generateMIAfrom(int round, int node){
	int top=0, bottom=0;
	distance[node]=0;
	heap[0]=node;
	top++;
	while (true){
		//pop out of heap
		if (distance[heap[0]]<longest) S[heap[0]]=-1; 
		else break;
		childlist[bottom++]=heap[0];
		for (int i=0;i<Graph::GetNeighbor(heap[0]);i++){
			Edge e=Graph::GetEdge(heap[0],i);
			if (S[e.v]<0) continue;
			if (distance[e.v]>distance[heap[0]]+e.w1+EPS) {
				if (S[e.v]>=n){
					distance[e.v]=distance[heap[0]]+e.w1;
					heap[top]=e.v;
					int j=top++, x=(j-1)/2;
					double temp=distance[heap[j]];
					while (j>0) {
						if (distance[heap[x]]>temp){
							heap[j]=heap[x];
							if (S[heap[j]]<n) S[heap[j]]=j;
							j=x;
							x=(j-1)/2;
						}
						else break;
					}
					heap[j]=e.v;
					S[e.v]=j;
				}
				else{
					distance[e.v]=distance[heap[0]]+e.w1;
					int j=S[e.v], x=(j-1)/2;
					double temp=distance[heap[j]];
					while (j>0) {
						if (distance[heap[x]]>temp){
							heap[j]=heap[x];
							if (S[heap[j]]<n) S[heap[j]]=j;
							j=x;
							x=(j-1)/2;
						}
						else break;
					}
					heap[j]=e.v;
					S[e.v]=j;
				}
			}//endif
		
		}// end for
		heap[0]=heap[--top];
		if (!top) break;
		//siftdown
		int j=0, x=j*2+1;
		double temp=distance[heap[j]];
		while (x<top){
			if (x+1<top && distance[heap[x+1]]<distance[heap[x]]) x++;
			if (distance[heap[x]]<temp){
				heap[j]=heap[x];
				S[heap[j]]=j;
				j=x; x=j*2+1;
			}
			else break;
		}
		heap[j]=heap[top];
		if (S[heap[j]]<n) S[heap[j]]=j;
	}
	for (int i=0;i<bottom;i++)
	{
		int child=childlist[i];
		distance[child]=longest;
		S[child]=n;
		parent[child]=-1;
	}
	//update tree node set and heuristic
	for (int i=0;i<bottom;i++){
		int child=childlist[i];
		oldchildlist[i]=child;
		if (dd[child]>0) {
			for (int j=0;j<dd[child];j++) dp[children[child][j]]-=delta[child][j]*(1-self[child][j]);
		}
	}
	for (int i=1;i<bottom;i++) 			generateMIAto0(oldchildlist[i]);
	return bottom;
}

int MIA::generateMIAto(int node){
	int top=0, bottom=0;
	if (used[node]) {
		dd[node]=1;
		path[node][0]=0;
		self[node][0]=1;
		delta[node][0]=0;
		return 1;
	}
	distance[node]=0;
	heap[0]=node;
	top++;
	parent[node]=node;
	b[node]=1;
	while (true){
		//stack out of heap
		if (distance[heap[0]]<longest) S[heap[0]]=-bottom-1; 
		else break;
		childlist[bottom++]=heap[0];
		if (parent[heap[0]]!=heap[0]) numchild[parent[heap[0]]]++;
		if (!used[heap[0]])
		for (int i=0;i<Graph::GetNeighbor(heap[0]);i++){
			Edge e=Graph::GetEdge(heap[0],i);
			if (used[e.v] && !validlist[lastupdate[e.v]][node] || S[e.v]<0) continue;
			if (distance[e.v]>distance[heap[0]]+e.w2+EPS) {
				parent[e.v]=heap[0];
				b[e.v]=exp(-e.w2);
				if (S[e.v]>=n){
					distance[e.v]=distance[heap[0]]+e.w2;
					heap[top]=e.v;
					int j=top++, x=(j-1)/2;
					double temp=distance[heap[j]];
					while (j>0) {
						if (distance[heap[x]]>temp){
							heap[j]=heap[x];
							if (S[heap[j]]<n) S[heap[j]]=j;
							j=x;
							x=(j-1)/2;
						}
						else break;
					}
					heap[j]=e.v;
					S[heap[j]]=j;
				}
				else{
					distance[e.v]=distance[heap[0]]+e.w2;
					int j=S[e.v], x=(j-1)/2;
					double temp=distance[heap[j]];
					while (j>0) {
						if (distance[heap[x]]>temp){
							heap[j]=heap[x];
							if (S[heap[j]]<n) S[heap[j]]=j;
							j=x;
							x=(j-1)/2;
						}
						else break;
					}
					heap[j]=e.v;
					S[e.v]=j;
				}
			}//endif
			
		}// end for
		heap[0]=heap[--top];
		if (!top) break;
		//siftdown
		int j=0, x=j*2+1;
		double temp=distance[heap[j]];
		while (x<top){
			if (x+1<top && distance[heap[x+1]]<distance[heap[x]]) x++;
			if (distance[heap[x]]<temp){
				heap[j]=heap[x];
				S[heap[j]]=j;
				j=x; x=j*2+1;
			}
			else break;
		}
		heap[j]=heap[top];
		if (S[heap[j]]<n) S[heap[j]]=j;
	}
	//update tree node set and heuristic
	if (!dd[node]){
		children[node]=new int[bottom];
		delta[node]=new double[bottom];
		self[node]=new double[bottom];
		path[node]=new int[bottom];
	}
	dd[node]=bottom;
	int head=0, tail=0;
	for (int i=0;i<bottom;i++){
		children[node][i]=childlist[i];
		allb[node].push_back(b[childlist[i]]);
		childnum[node].push_back(numchild[childlist[i]]);
		if (numchild[childlist[i]])	self[node][i]=1;
		else {
			self[node][i]=used[childlist[i]]?1:0;
			queue[tail++]=i;
		}
		path[node][i]=-S[parent[childlist[i]]]-1;
	}
	for (int i=0;i<bottom;i++)
	{
		int child=childlist[i];
		distance[child]=longest;
		S[child]=n;
		parent[child]=-1;
	}
	
	int x,u;
	while (head<tail) {
		x=queue[head++];
		u=path[node][x];
		self[node][u]*=(1-self[node][x]*b[childlist[x]]);
		if (!--numchild[childlist[u]]) {
			self[node][u]=1-self[node][u];
			queue[tail++]=u;
		}		
	}
	numchild[node]=0;
	delta[node][queue[--head]]=1;
	dp[node]+=1-self[node][x];
	for (head--;head>=0;head--) {
		x=queue[head], u=path[node][x];
		delta[node][x]=(1-self[node][u])/(1-self[node][x]*b[childlist[x]])*b[childlist[x]]*delta[node][u];
		dp[childlist[x]]+=delta[node][x]*(1-self[node][x]);
		//if (delta[node][x]*(1-self[node][x])>1) printf(">1 %d\n", node);
	}

	return bottom;
}

int MIA::generateMIAto0(int node){
	int top=0, bottom=0;
	distance[node]=0;
	heap[0]=node;
	top++;
	bottom=dd[node];
	int head=0, tail=0;
	for (int i=0;i<bottom;i++){
		childlist[i]=children[node][i];
		numchild[i]=childnum[node][i];
		b[i]=allb[node][i];
		
		if (numchild[i])	self[node][i]=1;
		else {
			self[node][i]=used[childlist[i]]?1:0;
			queue[tail++]=i;
		}
	}
	for (int i=0;i<bottom;i++)
	{
		int child=childlist[i];
		distance[child]=longest;
		S[child]=n;
		parent[child]=-1;
	}
	int x,u;
	while (head<tail) {
		x=queue[head++];
		u=path[node][x];
		self[node][u]*=(1-self[node][x]*b[x]);
		if (!--numchild[u]) {
			self[node][u]=1-self[node][u];
			queue[tail++]=u;
		}		
	}
	delta[node][queue[--head]]=1;
	dp[node]+=1-self[node][x];
	for (head--;head>=0;head--) {
		x=queue[head], u=path[node][x];
		delta[node][x]=(1-self[node][u])/(1-self[node][x]*b[x])*b[x]*delta[node][u];
		dp[childlist[x]]+=delta[node][x]*(1-self[node][x]);
	}

	return bottom;
}
int MIA::count(int node){
	int top=0, bottom=0;
	distance[node]=0;
	heap[0]=node;
	top++;
	parent[node]=node;
	b[node]=1;
	int count=0;
	while (true){
		//stack out of heap
		if (distance[heap[0]]<longest) S[heap[0]]=-bottom-1; 
		else break;
		childlist[bottom++]=heap[0];
		if (parent[heap[0]]!=heap[0]) numchild[parent[heap[0]]]++;
		if (used[heap[0]])
			if (count++>0) return count;
		for (int i=0;i<Graph::GetNeighbor(heap[0]);i++){
			Edge e=Graph::GetEdge(heap[0],i);
			if (S[e.v]<0) continue;
			if (distance[e.v]>distance[heap[0]]+e.w2+EPS) {
				parent[e.v]=heap[0];
				b[e.v]=exp(-e.w2);
				if (S[e.v]>=n){
					distance[e.v]=distance[heap[0]]+e.w2;
					heap[top]=e.v;
					int j=top++, x=(j-1)/2;
					double temp=distance[heap[j]];
					while (j>0) {
						if (distance[heap[x]]>temp){
							heap[j]=heap[x];
							if (S[heap[j]]<n) S[heap[j]]=j;
							j=x;
							x=(j-1)/2;
						}
						else break;
					}
					heap[j]=e.v;
					S[heap[j]]=j;
				}
				else{
					distance[e.v]=distance[heap[0]]+e.w2;
					int j=S[e.v], x=(j-1)/2;
					double temp=distance[heap[j]];
					while (j>0) {
						if (distance[heap[x]]>temp){
							heap[j]=heap[x];
							if (S[heap[j]]<n) S[heap[j]]=j;
							j=x;
							x=(j-1)/2;
						}
						else break;
					}
					heap[j]=e.v;
					S[e.v]=j;
				}
			}//endif
			
		}// end for
		heap[0]=heap[--top];
		if (!top) break;
		//siftdown
		int j=0, x=j*2+1;
		double temp=distance[heap[j]];
		while (x<top){
			if (x+1<top && distance[heap[x+1]]<distance[heap[x]]) x++;
			if (distance[heap[x]]<temp){
				heap[j]=heap[x];
				S[heap[j]]=j;
				j=x; x=j*2+1;
			}
			else break;
		}
		heap[j]=heap[top];
		if (S[heap[j]]<n) S[heap[j]]=j;
	}
	//update tree node set and heuristic
	return count;
	if (!dd[node]){
		children[node]=new int[bottom];
		delta[node]=new double[bottom];
		self[node]=new double[bottom];
		path[node]=new int[bottom];
	}
	dd[node]=bottom;
	int head=0, tail=0;
	for (int i=0;i<bottom;i++){
		children[node][i]=childlist[i];
		if (numchild[childlist[i]])	self[node][i]=1;
		else {
			self[node][i]=used[childlist[i]]?1:0;
			queue[tail++]=i;
		}
		path[node][i]=-S[parent[childlist[i]]]-1;
	}
	for (int i=0;i<bottom;i++)
	{
		int child=childlist[i];
		distance[child]=longest;
		S[child]=n;
		parent[child]=-1;
	}
	
	int x,u;
	while (head<tail) {
		x=queue[head++];
		u=path[node][x];
		self[node][u]*=(1-self[node][x]*b[childlist[x]]);
		if (!--numchild[childlist[u]]) {
			self[node][u]=1-self[node][u];
			queue[tail++]=u;
			//printf("%d\t",u);
		}		
	}
	numchild[node]=0;
	delta[node][queue[--head]]=1;
	dp[node]+=1-self[node][x];
	for (head--;head>=0;head--) {
		x=queue[head], u=path[node][x];
		delta[node][x]=(1-self[node][u])/(1-self[node][x]*b[childlist[x]])*b[childlist[x]]*delta[node][u];
		dp[childlist[x]]+=delta[node][x]*(1-self[node][x]);
	}

	return bottom;
}

double MIA::Build(int num, int bound)
{
	clock_t start, end;
	start = clock();
	n = Graph::GetN();
	longest=log(double(bound));
	top = num;
	double treesize=0;
	S = new int[n];
	distance = new double[n];
	b = new double[n];
	heap = new int[n];
	childlist = new int[n];
	oldchildlist = new int[n];
	parent = new int[n];
	numchild = new int[n];
	queue = new int[n];
	childnum=new vector<int>[n];
	allb=new vector<double>[n];

	used.resize(n);
	lastupdate.resize(n);
	children.resize(n);
	dp.resize(n);
	self.resize(n);
	dd.resize(n);
	delta.resize(n);
	path.resize(n);
	int set[SET_SIZE];

	double old = 0.0;

	int i=0;
	for (i=0; i<n; i++)
	{
		lastupdate[i] = -1;
		children[i]=NULL;
		dp[i]=0.0;
		self[i]=NULL;
		used[i]=false;
	}
	for (i=0; i<n; i++)
		dd[i] = 0;
	for (int i=0;i<n;i++) distance[i]=longest;
	for (int i=0;i<n;i++) S[i]=n;
	for (int i=0;i<n;i++) parent[i]=-1;
	for (int i=0;i<n;i++) numchild[i]=0;
	

	for (i=0;i<n;i++)
	{
		double size=generateMIAto(i);
		treesize+=size*size;
	}
	end = clock();
	double	timer = (double)(end - start);
		printf("%lg ", timer);


	i=0;
	double max = -1000000.0;
	int mp;
	{
		int x=GetMax(i);
		set[i] = x;
		lastupdate[x] = i;			
		double improve=dp[x];
		if (improve > max) {
			max=improve;
			mp=x;
		}
	}
	used[mp] = true;
	set[i] = mp;
	list[i] = mp;
	d[i] = max;
	old+=d[i];
	generateMIAfrom(i, mp);

	for (i=1; i<top; i++)
	{
		max = -1000000.0;
		int x=GetMax(i);
		set[i] = x;
		lastupdate[x] = i;			
		double improve=dp[x];
		if (improve > max) {
			max=improve;
			mp=x;
		}
		used[mp] = true;
		set[i] = mp;
		list[i] = mp;
		d[i] = max;
		old+=d[i];
		generateMIAfrom(i, mp);
	}
	int ct=0;
	delete[] childlist;
	delete[] oldchildlist;
	delete[] distance;
	delete[] S;
	delete[] heap;
	delete[] b;
	delete[] parent;
	delete[] numchild;
	delete[] queue;

	for (i=0;i<n;i++)
		if (dd[i]) {
			delete[] children[i];
			delete[] delta[i];
			delete[] self[i];
			delete[] path[i];
			dd[i]=0;
		}
	for (i=0;i<top;i++)
		delete[] validlist[i];

	sprintf(file,"MIA_%04d.txt", bound);
	FILE *out = fopen(file, "w");
	fprintf(out, "%d\n", top);
	for (i=0; i<top; i++)
		fprintf(out, "%d\t%Lg\n", list[i], d[i]);
	fclose(out);
	printf("%lg ",treesize/n);
	return ct;
}

double MIA::Build(int num, int k0, int bound, double (*Run)(int num_iter, int size, int set[]), double (*RunFast)(int num_iter, int size, int set[]))
{
	n = Graph::GetN();
	longest=log(double(bound));
	k=k0;
	top = num;
	double treesize=0;
	S = new int[n];
	distance = new double[n];
	b = new double[n];
	heap = new int[n];
	childlist = new int[n];
	oldchildlist = new int[n];
	parent = new int[n];

	used.resize(n);
	lastupdate.resize(n);
	children.resize(n);
	dp.resize(n);
	self.resize(n);
	dd.resize(n);
	int set[SET_SIZE];

	double old = 0.0;

int i=0;
	for (i=0; i<n; i++)
	{
		lastupdate[i] = -1;
		children[i]=NULL;
		dp[i]=0.0;
		self[i]=NULL;
	}
	for (i=0; i<n; i++)
		dd[i] = 0;
	for (i=0;i<n;i++)
	{
		treesize+=generateMIAto(i);
	}


	double max = -1000000.0;
	int mp;
	for (int j=0;j<k;j++){
		int x=GetMax(i);
		set[i] = x;
		lastupdate[x] = i;			
		double improve = RunFast(NUM_ITER, i+1, set);
		if (improve > max) {
			max=improve;
			mp=x;
		}
	}
	used[mp] = true;
	set[i] = mp;
	list[i] = mp;
	d[i] = max;
	old+=d[i];
	generateMIAfrom(i, mp);

	for (i=1; i<top; i++)
	{
		max = -1000000.0;
		for (int j=0;j<k;j++){
			int x=GetMax(i);
			set[i] = x;
			lastupdate[x] = i;			
			double improve = i>top? Run(NUM_ITER/100, i+1, set) - old : RunFast(NUM_ITER/100, i+1, set) - old;
			if (improve > max) {
				max=improve;
				mp=x;
			}
		}
		used[mp] = true;
		set[i] = mp;
		list[i] = mp;
		d[i] = max;
		old+=d[i];
		generateMIAfrom(i, mp);
	}
	delete[] childlist;
	delete[] oldchildlist;
	delete[] distance;
	delete[] S;
	delete[] heap;
	delete[] b;
	delete[] parent;

	for (i=0;i<n;i++)
		if (dd[i]) {
			delete[] children[i];
			delete[] delta[i];
			delete[] self[i];
			delete[] path[i];
		}
	for (i=0;i<top;i++)
		delete[] validlist[i];

	sprintf(file,"MIA_%04d.txt", bound);
	FILE *out = fopen(file, "w");
	fprintf(out, "%d\n", top);
	for (i=0; i<top; i++)
		fprintf(out, "%d\t%Lg\n", list[i], d[i]);
	fclose(out);
	return treesize/n;
}

void MIA::BuildFromFile(int bound)
{
	n = Graph::GetN();
	sprintf(file,"MIA_%04d.txt", bound);
	FILE* in = fopen(file, "r");
	fscanf(in, "%ld", &top);
	for (int i=0; i<top; i++)
		fscanf(in, "%ld %Lg", &list[i], &d[i]);
	fclose(in);
}

int  MIA::GetNode(int i)
{
	if (i<0)
		return -1;
	if (i>=top) 
		return -1;
	return list[i];
}


char* MIA::filename(int bound)
{
	sprintf(file,"MIA_%04d.txt", bound);
	return file;
}

#pragma once
#ifndef SKYLINESEARCH_H_
#define SKYLINESEARCH_H_

#include <string>
#include <queue>
#include <list>
#include <vector>
#include <set>
#include <ctime>
using namespace std;

const int d_act = 11; // should be larger than dimH e.g. dimH + 1
const int Adim = 11;

class rSnode {
public:
	int nodeId;
	rSnode *next;
public:
	rSnode();
};

class rnode {
public:
	int d;
	int nodeId;
	double attributes[d_act];
};

class rskyline {
public:
	double IF[d_act]; //influence value
public:
	rskyline();
};

class Constraint {
public:
	int d;
	double corner[d_act];
	int keyNodes[d_act];
public:
	Constraint();
};

typedef list<Constraint> LC;

class SkylineComm_Search {
private:
	clock_t st;
	//int Adim; // actual highest dimension
  	int coresize;
	int graph_size; // the total size of the whole graph (the origin graph)
	int c_size; // the current graph size after some nodes removed (the size of the k core)
	int dim_size[d_act + 1]; // the number of nodes in each dimension
	int k; // the coreness of the skyline community
	int *key_node;
	//int **timestamp; 
	rnode **vert; // six dimension with the last dimension representing the current graph under consideration
	int *visit;
	int *core;
	int *deg;
	LC LCP[d_act]; // list of corner points at each dimension
	vector< rskyline > RES[d_act]; // store skyline community at each dimension
	rSnode *neighbour; // graph denoted by adjacency list
public:
	SkylineComm_Search();

	void initialize(int kcore, int gsize);

	void load_data(string path, string M, string OO, int k, int dL, int dH);

	void sort_by_attribute(int st, int ed, int d); // sort by the first attribute

	void recover(int d); 

	void LD_degree_calculation(int d); // last dimension degree calculation

	void degree_calculation(int d, int size); //

	int find_next_node(int st, int ed, int d); // find the next node with smallest attribute value

	int DFS(int u, int dL, int dH, Constraint C);

	void delDFS(int u);

	void FindSubgraph(int dL, int dH, Constraint C);

	double DimMax(int dL, int dH, Constraint C);

	void UpdateCornerPoints(rskyline sl, int d);

	double Skyline2D(int dH, Constraint C);

	void Skyline3D(int dL, int dH, Constraint C);

	void SkylineHighD(int dL, int dH, Constraint C);

	int skycomm_search(string path, string M, string OO, int kcore, int dimL, int dimH, int gsize);

	void clr();
};


#endif // !SKYLINESEARCH_H_

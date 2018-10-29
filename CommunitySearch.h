#pragma once
#ifndef COMMUNITYSEARCH_H_
#define COMMUNITYSEARCH_H_
#define upth 1000000000
#define ttsh 100000.0

#include <vector>
#include <string>
using namespace std;

class Snode {
public:
	int nodeId;
	Snode *next;
public:
	Snode();
};

class node {
public:
	int nodeId;
	double attributes[5];
};

class skyline {
public:
	double IF[5]; //influence value
public:
	skyline();
};

 //static int **timestamp; // 5 dimensions
class community_search {
private:
	int coresize;
	int graph_size; // the number of nodes in the original graph
	int node_num; // the number of nodes in the k-core
	int sub_num; // the number of nodes in the second round
	int snum; // the number of skyline community
	int answer_size;
	double xthreshold; //
	double ythreshold; // 
	double ststime;
	node *vertx; // 1st dimension
	node *verty; // 2nd dimension
	node *vertz; // 3rd dimension
	node *vertu; // 4th dimension
	node *vertv; // 5th dimension
	int *del; // deletion
	int *visit; // mark whether one node has been checked
	int *core; // coreness
	int *deg; // degree
	int *ddeg; // the copy of deg, used in delDFS()                                                        
	int **timestamp; // 5 dimensions
	node **keyNodes; // 5 dimensions
	Snode *neighbour; // graph denoted by adjacency list
	vector< skyline > res; // results
public:
	int initkcore(string path, string M, string OO, int k, int d, int dH); // find the maximal k-core (load data meanwhile)
public:
	community_search();

	void initialization(int gsize);

	void sort_by_attribute(int st, int ed, int d); // sort by the first attribute

	int find_next_node(int st, int ed, node *arr); // find the next node with smallest attribute value

	void degree_calculation(int type, int size, node *arr); // degree

	int DFS(int u, int k, int d, int tp, node *key_node);

	void delDFS(int u, int k); 
	
	void find_initial_graph_for_second_round(int tp); 
	
	void delete_nodes_by_dfs(double threshold, int k);

	void shrink_kcore();

	double save_results(string path, int k, int idx, int tp);

	void clear();

	int cptTreeNodes(int k, int d, int size, node *arr, node *key_node); //calculate the tree nodes

	void two_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl); // We provide the graph size "gsize" directly.

	void three_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl); // We provide the graph size "gsize" directly.

	void four_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl); // We provide the graph size "gsize" directly.

	void five_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl); // We provide the graph size "gsize" directly.

	int skyline_search(string path, string M, string OO, int k, int dimL, int dimH, int gsize); // the entry of search process
};

#endif // !COMMUNITYSEARCH_H_

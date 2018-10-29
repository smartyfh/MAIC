//#include "stdafx.h"
#include "SkylineSearch.h"
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ctime>
using namespace std;

bool cmp(const rnode & unode, const rnode & vnode) // sort by attributes asc
{
	int dim = unode.d;
	return unode.attributes[dim] < vnode.attributes[dim];
}

bool operator < (const Constraint & C1, const Constraint & C2) { // des
	int dim = C1.d;
	if (dim != 3)
		return C1.corner[dim] < C2.corner[dim];
	else {
		if (C1.corner[3] == C2.corner[3])
			return C1.corner[1] < C2.corner[1];
		else
			return C1.corner[3] < C2.corner[3];
	}
}

SkylineComm_Search::SkylineComm_Search()
{
	key_node = NULL;
	vert = NULL;
	visit = NULL;
	core = NULL;
	deg = NULL;
	neighbour = NULL;
	for (int i = 0; i < Adim; i++) {
		LCP[i].clear();
		RES[i].clear();
	}
}

void SkylineComm_Search::initialize(int kcore, int gsize)
{
	k = kcore;
	graph_size = gsize;
	c_size = gsize;

	key_node = new int[Adim];
	visit = new int[graph_size + 2];
	core = new int[graph_size + 2];
	deg = new int[graph_size + 2];
	neighbour = new rSnode[graph_size + 2];
	vert = new rnode*[Adim + 1];
	for (int i = 0; i < Adim + 1; i++) {
		vert[i] = new rnode[graph_size + 2];
	}
}

void SkylineComm_Search::load_data(string path, string M, string OO, int k, int dL, int dH)
{
	int unode, vnode, ucore;
	int idx = 0;
	double *data = new double[dH];
	string core_path;
	string list_path;
	string attribute_path;
  
  if (OO.compare("o") == 0)
	  core_path = path + "2-core_number.txt";
  else 
    core_path = path + "2-core_number-tmp.txt";
	list_path = path + "1-ad_list.txt";
	attribute_path = path + "0-attributes-" + M + ".txt";

	ifstream core_fin;
    coresize = 0;
	core_fin.open(core_path.c_str()); // load core number
	if (core_fin) {
		while (!core_fin.eof()) {
			core_fin >> unode >> ucore;
			if (unode == -1)
				break;
			core[unode] = ucore;
			deg[unode] = 0;
			visit[unode] = 0;
			if (ucore < k)
				visit[unode] = 1;
      else
        coresize++;
			unode = -1;
		}
	}
	else {
		cout << "Failed to open file: " + core_path << endl;
		exit(1);
	}
	core_fin.close();

	ifstream list_fin;
	list_fin.open(list_path.c_str()); // load adjacency list
	if (list_fin) {
		while (!list_fin.eof()) {
			list_fin >> unode >> vnode;
			if (unode == -1)
				break;
			else if ((core[unode] >= k) && (core[vnode] >= k)) {
				rSnode *sn = new rSnode();
				sn->nodeId = vnode;
				if (neighbour[unode].next == NULL) {
					neighbour[unode].next = sn;
				}
				else {
					rSnode *msn;
					msn = neighbour[unode].next;
					neighbour[unode].next = sn;
					sn->next = msn;
					msn = NULL;
				}
				sn = NULL;
			}
			unode = -1;
		}
	}
	else {
		cout << "Failed to open file: " + list_path << endl;
		exit(1);
	}
	list_fin.close();

	ifstream attribute_fin;
	attribute_fin.open(attribute_path.c_str()); // load attributes
	if (attribute_fin) {
		while (!attribute_fin.eof()) {
			attribute_fin >> unode;
			for (int i = 0; i < dH; i++)
				attribute_fin >> data[i];
			if (unode == -1)
				break;
			else if (core[unode] >= k) {
				vert[dL + 1][idx].nodeId = unode;
				for (int j = 0; j < dL; j++) {
					vert[dL + 1][idx].attributes[j + 1] = data[j];
				}
				idx++;
			}
			unode = -1;
		}
	}
	else {
		cout << "Failed to open file: " + attribute_path << endl;
		exit(1);
	}
	attribute_fin.close();
	delete[] data;
	c_size = idx;
	dim_size[dL + 1] = c_size;
}

void SkylineComm_Search::sort_by_attribute(int st, int ed, int d)
{
	sort(vert[d] + st, vert[d] + ed, cmp);
}

void SkylineComm_Search::recover(int d)
{
	for (int i = 0; i < dim_size[d]; i++)
		visit[vert[d][i].nodeId] = 0;
}

void SkylineComm_Search::LD_degree_calculation(int d)
{
	int u;
	int dg;
	rSnode *sn;
	for (int i = 0; i < dim_size[d]; i++) {
		u = vert[d][i].nodeId;
		dg = 0;
		sn = &neighbour[u];
		while (sn->next != NULL) {
			sn = sn->next;
			int v = sn->nodeId;
			if (visit[v] == 0)
				dg++;
		}
		deg[u] = dg;
	}
	sn = NULL;
}

void SkylineComm_Search::degree_calculation(int d, int size)
{
	int u;
	rSnode *sn;
	for (int i = 0; i < size; i++) {
		u = vert[d][i].nodeId;
		int dg = 0;
		sn = &neighbour[u];
		while (sn->next != NULL) {
			sn = sn->next;
			int v = sn->nodeId;
			if (visit[v] == 0)
				dg++;
		}
		deg[u] = dg;
	}
	sn = NULL;
}

int SkylineComm_Search::find_next_node(int st, int ed, int d)
{// find the next unvisited node in node array--vert[d]
	while (st < ed) {
		if (visit[vert[d][st].nodeId] == 0)
			return st;
		else
			st++;
	}
	return -1;
}

int SkylineComm_Search::DFS(int u, int dL, int dH, Constraint C)
{// when dealing with key_node, end dfs process, this operation may save some time
	int mrk = 1;
	for (int i = dL + 1; i <= dH; i++) {
		if (u == C.keyNodes[i]) {
			mrk = 0;
			break; // key nodes cannot be deleted.
		}
	}
	if (mrk == 0)
		return 0;

	int v;
	int fg = 1;
	visit[u] = 1;
	rSnode *sn;
	sn = &neighbour[u];
	while (sn->next != NULL) {
		sn = sn->next;
		v = sn->nodeId;
		if (visit[v] == 0) {
			deg[v]--;
			if (deg[v] < k) {
				fg = DFS(v, dL, dH, C);
				if (fg == 0)
					break;
			}
		}
	}
	sn = NULL;
	return fg;
}

void SkylineComm_Search::delDFS(int u)
{
	int v;
	visit[u] = 1;
	rSnode *sn;
	sn = &neighbour[u];
	while (sn->next != NULL) {
		sn = sn->next;
		v = sn->nodeId;
		if (visit[v] == 0) {
			deg[v]--;
			if (deg[v] < k)
				delDFS(v);
		}
	}
	sn = NULL;
}

void SkylineComm_Search::FindSubgraph(int dL, int dH, Constraint C)
{// find the subgraph with each node satisfying the constraint C.
	int idx = 0;
	for (int i = 0; i < dim_size[dL + 1]; i++) {
		if (visit[vert[dL + 1][i].nodeId] == 1)
			continue;
		int flag = 1;
		for (int j = 1; j < dL; j++) {
			if (vert[dL + 1][i].attributes[j] <= C.corner[j]) {
				flag = 0;
				break;
			}
		}
		if (flag) {
			for (int j = dL; j <= dH; j++) {
				if (vert[dL + 1][i].attributes[j] < C.corner[j]) {
					flag = 0;
					break;
				}
			}
		}
		if (flag == 0) { // doesn't satisfy the constraint
			delDFS(vert[dL + 1][i].nodeId);
		}
	}
	for (int i = 0; i < dim_size[dL + 1]; i++) {
		if (visit[vert[dL + 1][i].nodeId] == 0) {
			vert[dL][idx] = vert[dL + 1][i];
			vert[dL][idx++].d = dL;
		}
	}
	dim_size[dL] = idx;
}

double SkylineComm_Search::DimMax(int dL, int dH, Constraint C)
{ // compute the max value at the (dL)th dimension
	recover(dL + 1); // recover the status of the last dimension
	LD_degree_calculation(dL + 1); // compute degree of last dimension
	FindSubgraph(dL, dH, C); // find subgraph of this dimension
	sort_by_attribute(0, dim_size[dL], dL);
	//degree_calculation(dL, dim_size[dL]);
	int ptr = 0;
	int fg = 1;
	int unode;
	double maxval = 0;
	while (true) {
		ptr = find_next_node(ptr, dim_size[dL], dL);
		if (ptr == -1)
			break;
		unode = vert[dL][ptr].nodeId;
		key_node[dL] = unode;
		maxval = max(maxval, vert[dL][ptr].attributes[dL]);
		fg = DFS(unode, dL, dH, C);
		if (fg == 0) 
			break;
	}
	return maxval;
}

void SkylineComm_Search::UpdateCornerPoints(rskyline sl, int d)
{// update the corner points at dimension d
	int num = LCP[d].size();
	int idx = 0;
	Constraint *CP; // new generated corner points
	Constraint cstt;
	CP = new Constraint[num*d + 2];
	LC::iterator it;
	for (it = LCP[d].begin(); it != LCP[d].end();) {
		int flag = 1;
		for (int i = 1; i <= d; i++) {
			if ((*it).corner[i] >= sl.IF[i]) { // only strictly < is considered
				flag = 0;
				break;
			}
		}
		if (flag) {
			for (int j = 1; j <= d; j++) {
				cstt = *it;
				cstt.corner[0] = 0;
				cstt.corner[j] = sl.IF[j];
				CP[idx++] = cstt; // all the possible new corner points
			}
			it = LCP[d].erase(it);
			num--;
		}
		else
			it++;
	}
	for (int i = 0; i < idx; i++) { // some corner points may dominate other corner points
		for (int j = 0; j < idx; j++) {
			if ((i != j) && (CP[j].d == 0)) {
				int fg = 1;
				for (int kk = 1; kk <= d; kk++) {
					if (CP[i].corner[kk] < CP[j].corner[kk]) {
						fg = 0;
						break;
					}
				}
				if (fg) {
					CP[i].d = -1; // CP[i] is dominated by CP[j]
					break;
				}
			}
		}
	}
	for (int i = 0; i < idx; i++) {
		if (CP[i].d == 0) { // some corner points may dominate by previous corner points
			int len = 0;
			int ffg = 1;
			for (it = LCP[d].begin(); it != LCP[d].end(); it++) {
				ffg = 0;
				for (int ii = 1; ii <= d; ii++) {
					if ((*it).corner[ii] > CP[i].corner[ii]) {
						ffg = 1;
						break;
					}
				}
				len++;
				if ((len == num) || (ffg == 0))
					break;
			}
			if (ffg == 1)
				LCP[d].push_back(CP[i]);
		}
	}
	delete[] CP;
}

double SkylineComm_Search::Skyline2D(int dH, Constraint C)
{
	double rt = 0;
	C.corner[2] = 0;
	double c = DimMax(2, dH, C);
	Constraint cstt;
	cstt = C;
	cstt.corner[2] = c;
	cstt.keyNodes[2] = key_node[2];
	rskyline sl;
	while (c > 0.0) {
		rt = max(rt, c);
		sl.IF[1] = DimMax(1, dH, cstt);
		sl.IF[2] = c;
//==============================================
		/*int len = RES[2].size();
		int idctr = 0;
		for (int i = 0; i < len; i++) {
			idctr = 1;
			for (int j = 1; j <= 2; j++) {
				if (RES[2][i].IF[j] < sl.IF[j]) { // >=
					idctr = 0;
					break;
				}
			}
			if (idctr == 1)
				break;
		}
		if (idctr == 0)*/
			RES[2].push_back(sl); // A new skyline has been found.
//==============================================
		cstt.corner[1] = sl.IF[1];
		cstt.corner[2] = 0;
		c = DimMax(2, dH, cstt);
		//------------------------------------This may be an optimization
		for (int i = 0; i < dim_size[2]; i++) {
			vert[3][i] = vert[2][i];
		}
		dim_size[3] = dim_size[2];
		//------------------------------------
		cstt.corner[2] = c;
		cstt.keyNodes[2] = key_node[2];
//================================================
		//cout << "2: " << sl.IF[1] << " " << sl.IF[2] <<endl;
//================================================
	}
	return rt;
}

void SkylineComm_Search::Skyline3D(int dL, int dH, Constraint C)
{

    clock_t edd;

	edd = clock();
    double tst = (double)(edd - st) / CLOCKS_PER_SEC;
    if (tst >= 50000) return;

	C.corner[dL] = 0;
	double c = DimMax(dL, dH, C);
	priority_queue< Constraint > Q;
	while (!Q.empty()) Q.pop();
	Constraint cstt;
	cstt = C;
	cstt.d = dL;
	cstt.corner[dL] = c;
	cstt.keyNodes[dL] = key_node[dL];
	Q.push(cstt);
	Constraint cstt1;
	rskyline sl;

	LCP[dL - 1].clear();
	LCP[dL - 1].push_back(cstt1);
	
	while (!Q.empty()) {
		//cout << dL << " : " << endl;
		c = Q.top().corner[dL];
		RES[dL - 1].clear();
		double c2 = 0.0;
		while ((!Q.empty()) && (Q.top().corner[dL] == c)) {
			cstt1 = Q.top();
			Q.pop();
			c2 = max(c2, cstt1.corner[2]);
			cstt1.corner[2] = c2;
//=============================================
			recover(dL + 1);
			LD_degree_calculation(dL + 1);
			FindSubgraph(dL, dH, cstt1);
			//for (int i = 0; i < dim_size[dL]; i++)
			//	cout << vert[dL][i].nodeId << " ";
			//cout << endl;
//=============================================
			double c22 = Skyline2D(dH, cstt1);
			c2 = max(c2, c22);
		}
		for (int i = 0; i < RES[dL - 1].size(); i++) {
			sl = RES[dL - 1][i];
			sl.IF[dL] = c;
//=============================================
			int len = RES[dL].size();
			int idctr = 0;
			for (int i = 0; i < len; i++) {
				idctr = 1;
				for (int j = 1; j <= dL; j++) {
					if (RES[dL][i].IF[j] < sl.IF[j]) { // >=
						idctr = 0;
						break;
					}
				}
				if (idctr == 1)
					break;
			}
			if (idctr == 0) {
				RES[dL].push_back(sl); // A new skyline has been found.
				UpdateCornerPoints(sl, dL - 1);
			}
//=============================================
			//for (int j = 1; j < dL; j++)
			//	cout << sl.IF[j] << " ";
			//cout << c << endl;
		}
		LC::iterator it;
		for (it = LCP[dL - 1].begin(); it != LCP[dL - 1].end(); it++) {
			//cout << "begin" << endl;
			//for (int ii = 1; ii <= dL; ii++) {
			//	cout << (*it).corner[ii] << " ";
			//}
			//cout << "end" << endl;
			if ((*it).corner[0] == 1)
				continue; // the corner point has been handled, thus there's no need to handle it again later. 
			cstt = (*it);
			cstt.d = dL;
			cstt.corner[dL] = 0;
			for (int j = dL + 1; j <= dH; j++) {
				cstt.corner[j] = C.corner[j];
				cstt.keyNodes[j] = key_node[j];
			}	
			cstt.corner[dL] = DimMax(dL, dH, cstt);
			if (cstt.corner[dL] > 0.0) {
				cstt.keyNodes[dL] = key_node[dL];
				Q.push(cstt);
			}
			(*it).corner[0] = 1;
		}
	}
}

void SkylineComm_Search::SkylineHighD(int dL, int dH, Constraint C)
{ // dL: the current dimension; dH: the starting dimension

    clock_t edd;

	edd = clock();
    double tst = (double)(edd - st) / CLOCKS_PER_SEC;
    if (tst >= 50000) return;

	if (dL == 3) {
		Skyline3D(dL, dH, C);
		return;
	}
	C.corner[dL] = 0;
	double c = DimMax(dL, dH, C);
	if (c == 0.0)
		return;
	priority_queue< Constraint > Q;
	while (!Q.empty()) Q.pop();
	Constraint cstt;
	cstt = C;
	cstt.d = dL;
	cstt.corner[dL] = c;
	cstt.keyNodes[dL] = key_node[dL];
	Q.push(cstt);
	Constraint cstt1;
	rskyline sl;

	LCP[dL - 1].clear();
	LCP[dL - 1].push_back(cstt1);

	while (!Q.empty()) {
		//cout << dL << " : " << endl;
		c = Q.top().corner[dL];
		RES[dL - 1].clear();
		while ((!Q.empty()) && (Q.top().corner[dL] == c)) {
			cstt1 = Q.top();
			Q.pop();
//================================
			recover(dL + 1);
			LD_degree_calculation(dL + 1);
			FindSubgraph(dL, dH, cstt1);
			//for (int i = 0; i < dim_size[dL]; i++)
			//	cout << vert[dL][i].nodeId << " ";
			//cout << endl;
//================================
			SkylineHighD(dL - 1, dH, cstt1);
		}
		for (int i = 0; i < RES[dL - 1].size(); i++) {
			sl = RES[dL - 1][i];
			sl.IF[dL] = c;
//=================================
			int len = RES[dL].size();
			int idctr = 0;
			for (int i = 0; i < len; i++) { // There may be some identical skyline communities.
				idctr = 1;
				for (int j = 1; j <= dL; j++) {
					if (RES[dL][i].IF[j] < sl.IF[j]) { // >=
						idctr = 0;
						break;
					}
				}
				if (idctr == 1)
					break;
			}
			if (idctr == 0) {
				RES[dL].push_back(sl); // A new skyline has been found.
				UpdateCornerPoints(sl, dL - 1); // update corner points
			}
//===================================
			//for (int j = 1; j < dL; j++)
			//	cout << sl.IF[j] << " ";
			//cout << c << endl;
		}
		LC::iterator it;
		for (it = LCP[dL - 1].begin(); it != LCP[dL - 1].end(); it++) {
			if ((*it).corner[0] == 1)
				continue; // the corner point has been handled previously, thus there's no need to handle it again later. 
			cstt = (*it);
			cstt.d = dL;
			cstt.corner[dL] = 0;
			for (int j = dL + 1; j <= dH; j++) {
				cstt.corner[j] = C.corner[j];
				cstt.keyNodes[j] = key_node[j];
			}
			cstt.corner[dL] = DimMax(dL, dH, cstt);
			if (cstt.corner[dL] > 0.0) { //cstt.corner[dL] == 0.0 indicates that there is no skylinecomm in the subspace.
				cstt.keyNodes[dL] = key_node[dL];
				Q.push(cstt);
			}
			(*it).corner[0] = 1;
		}
	}
}

//=========================Entry of the search process=======================
int SkylineComm_Search::skycomm_search(string path, string M, string OO, int kcore, int dimL, int dimH, int gsize)
{// dim >= 3, if not, there is no need to use this improved algorithm.
 // There are dimH attributes of each node, but we only consider the first dimL attributes. 
//	if (dimL <= 2) {
//		cout << "The improved algorithm doesn't handle the case of dimension <= 2!" << endl;
//		return;
//	}
	//Adim = dimH;
	initialize(kcore, gsize);
	load_data(path, M, OO, kcore, dimL, dimH);
//---------------------------------------------------
	/*cout << "Nodes of the primitive entire k core: " << endl;
	for (int i = 0; i < c_size; i++)
		cout << vert[dimL + 1][i].nodeId << " ";
	cout << endl;*/
//---------------------------------------------------
	clock_t ed;
	st = clock();

	Constraint cstt;
	if (dimL > 2)
		SkylineHighD(dimL, dimL, cstt);
	else
		Skyline2D(2, cstt);

	ed = clock();
    double tst = (double)(ed - st) / CLOCKS_PER_SEC;
	cout << "Time for improved algorithm: " << tst << endl;
//---------------------------------------------------
	cout << "size: " << RES[dimL].size() << endl;
	/*for (int i = 0; i < RES[dimL].size(); i++) {
		for (int j = 1; j <= dimL; j++)
			cout << RES[dimL][i].IF[j] << " ";
		cout << endl;
	}*/
//---------------------------------------------------

string ppdr = path + "result.txt";
ofstream ofile;
ofile.open(ppdr, ios::out | ios::app);
if (ofile.good()) cout << "success" << endl;
else cout << "fail" << endl;

ofile << path << endl;
ofile << M << " " << OO << endl;
ofile << "kcore: "<< kcore << " dim: " << dimL << endl;
ofile << "time: " << tst << " size: " << RES[dimL].size() << endl;
ofile << endl;
ofile.close();

	clr();
 //int mmmm = 1;
 //if (tst >= 150000) mmmm = 0;
 //return mmmm;
 return coresize;
}

void SkylineComm_Search::clr()
{
	delete[] key_node;
	delete[] visit;
	delete[] core;
	delete[] deg;
	delete[] neighbour;
	for (int i = 0; i < Adim + 1; i++) {
		delete[] vert[i];
	}
	for (int i = 0; i < Adim; i++) {
		LCP[i].clear();
		RES[i].clear();
	}
	key_node = NULL;
	visit = NULL;
	core = NULL;
	deg = NULL;
	neighbour = NULL;
	vert = NULL;
}

Constraint::Constraint()
{
	d = 0;
	for (int i = 0; i < Adim; i++) {
		corner[i] = 0;
		keyNodes[i] = 0;
	}
}

rskyline::rskyline()
{
	for (int i = 0; i < Adim; i++)
		IF[i] = 0; // initialization
}

rSnode::rSnode()
{
	next = NULL;
}

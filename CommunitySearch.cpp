//#pragma comment(linker,"/STACK:102400000,1024000")
//#include "stdafx.h"
#include "CommunitySearch.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <ctime>

bool xcmp(const node & unode, const node & vnode) // sort by first attribute asc
{
	return unode.attributes[0] < vnode.attributes[0];
}

bool ycmp(const node & unode, const node & vnode) // sort by second attribute asc
{
	return unode.attributes[1] < vnode.attributes[1];
}

bool zcmp(const node & unode, const node & vnode) // sort by third attribute asc
{
	return unode.attributes[2] < vnode.attributes[2];
}

bool ucmp(const node & unode, const node & vnode) // sort by fourth attribute asc
{
	return unode.attributes[3] < vnode.attributes[3];
}

bool vcmp(const node & unode, const node & vnode) // sort by fifth attribute asc
{
	return unode.attributes[4] < vnode.attributes[4];
}

Snode::Snode()
{
	next = NULL;
}

community_search::community_search()
{
	vertx = NULL;
	verty = NULL;
	vertz = NULL;
	vertu = NULL;
	vertv = NULL;
	del = NULL;
	visit = NULL;
	core = NULL;
	deg = NULL;
	ddeg = NULL;
	timestamp = NULL;
	keyNodes = NULL;
	neighbour = NULL;
}

void community_search::initialization(int gsize)
{
	graph_size = gsize;
	snum = 1;
	vertx = new node[graph_size + 2];
	verty = new node[graph_size + 2];
	vertz = new node[graph_size + 2];
	vertu = new node[graph_size + 2];
	vertv = new node[graph_size + 2];
	del = new int[graph_size + 2];
	visit = new int[graph_size + 2];
	core = new int[graph_size + 2];
	deg = new int[graph_size + 2];
	ddeg = new int[graph_size + 2];
	neighbour = new Snode[graph_size + 2];
	timestamp = new int*[graph_size + 2];
	keyNodes = new node*[graph_size + 2];
	res.clear();
	for (int i = 0; i < graph_size + 2; i++) {
		timestamp[i] = new int[6];
		keyNodes[i] = new node[6];
	}
	//cout << timestamp << " " << &timestamp[0] << " " << timestamp[0] << " " << &timestamp[0][0] << endl;
}

int community_search::initkcore(string path, string M, string OO, int k, int d, int dH)
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

	node_num = 0;
	coresize = 0;
	ifstream core_fin;
	core_fin.open(core_path.c_str()); // load core number
	if (core_fin) {
		while (!core_fin.eof()) {
			core_fin >> unode >> ucore;
			if (unode == -1)
				break;
			core[unode] = ucore;
			for (int idx = 0; idx < 6; idx++) {
				timestamp[unode][idx] = 0; // initialization
			}
			del[unode] = 0;
			deg[unode] = 0;
			ddeg[unode] = 0;
			visit[unode] = 0;
			if (ucore >= k)
				node_num++;
			else {
				del[unode] = 1;
				visit[unode] = 1;
			}
			unode = -1;
		}
	}
	else {
		cout << "Failed to open file: " + core_path << endl;
		exit(1);
	}
	core_fin.close();
	coresize = node_num;

	ifstream list_fin;
	list_fin.open(list_path.c_str()); // load adjacency list
	if (list_fin) {
		while (!list_fin.eof()) {
			list_fin >> unode >> vnode;
			if (unode == -1)
				break;
			else if ((core[unode] >= k) && (core[vnode] >= k)) {
				Snode *sn = new Snode();
				sn->nodeId = vnode;
				if (neighbour[unode].next == NULL) {
					neighbour[unode].next = sn;
				}
				else {
					Snode *msn;
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
				if (d == 2) {
					verty[idx].nodeId = unode;
					for (int j = 0; j < d; j++) {
						verty[idx].attributes[j] = data[j];
					}
				}
				else if (d == 3) {
					vertz[idx].nodeId = unode;
					for (int j = 0; j < d; j++) {
						vertz[idx].attributes[j] = data[j];
					}
				}
				else if (d == 4) {
					vertu[idx].nodeId = unode;
					for (int j = 0; j < d; j++) {
						vertu[idx].attributes[j] = data[j];
					}
				}
				else if (d == 5) {
					vertv[idx].nodeId = unode;
					for (int j = 0; j < d; j++) {
						vertv[idx].attributes[j] = data[j];
					}
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
	return idx;
}

void community_search::sort_by_attribute(int st, int ed, int d)
{
	if (d == 1)
		sort(vertx + st, vertx + ed, xcmp);
	else if (d == 2)
		sort(verty + st, verty + ed, ycmp);
	else if (d == 3)
		sort(vertz + st, vertz + ed, zcmp);
	else if (d == 4)
		sort(vertu + st, vertu + ed, ucmp);
	else if (d == 5)
		sort(vertv + st, vertv + ed, vcmp);
}

int community_search::find_next_node(int st, int ed, node *arr)
{// find the next unvisited node in node array--arr
	while (st < ed) {
		if (visit[ arr[st].nodeId ] == 0)
			return st;
		else
			st++;
	}
	return -1;
}

void community_search::degree_calculation(int type, int size, node *arr)
{
	int u;
	Snode *sn;
	for (int i = 0; i < size; i++) {
		u = arr[i].nodeId;
		int dg = 0;
		sn = &neighbour[u];
		while (sn->next != NULL) {
			sn = sn->next;
			int v = sn->nodeId;
			if (visit[v] == 0)
				dg++;
		}
		deg[u] = dg;
		if (type == 2)
			ddeg[u] = dg;
	}
	sn = NULL;
}

int community_search::DFS(int u, int k, int d, int tp, node *key_node) // tp means timestamp
{ // when dealing with key_node, end dfs process
	int mrk = 1;
	for (int i = d + 1; i < 6; i++) {
		if (u == key_node[i].nodeId) {
			mrk = 0;
			break; // key nodes cannot be deleted.
		}
	}
	if (mrk == 0)
		return 0;

	int v;
	int fg = 1;
	visit[u] = 1;
	timestamp[u][d] = tp;
	Snode *sn;
	sn = &neighbour[u];
	while (sn->next != NULL) {
		sn = sn->next;
		v = sn->nodeId;
		if (visit[v] == 0) {
			deg[v]--;
			if (deg[v] < k) {
				fg = DFS(v, k, d, tp, key_node);
				if (fg == 0)
					break;
			}
		}
	}
	sn = NULL;
	return fg;
}

void community_search::delDFS(int u, int k)
{
	int v;
	del[u] = 1;
	Snode *sn;
	sn = &neighbour[u];
	while (sn->next != NULL) {
		sn = sn->next;
		v = sn->nodeId;
		if (del[v] == 0) {
			ddeg[v]--;
			if (ddeg[v] < k)
				delDFS(v, k);
		}
	}
	sn = NULL;
}

void community_search::find_initial_graph_for_second_round(int tp)
{
	int u;
	int idx = 0;
	for (int i = 0; i < node_num; i++) {
		u = verty[i].nodeId;
		if (timestamp[u][2] == tp) {
			visit[u] = 0;
			timestamp[u][1] = 0;
			vertx[idx++] = verty[i];
		}
		else
			visit[u] = 1;
	}
	sub_num = idx;
}

void community_search::delete_nodes_by_dfs(double threshold, int k) // to find the next k-core
{
	int u;
	for (int i = 0; i < node_num; i++) {
		u = verty[i].nodeId;
		if ((del[u] == 0) && (verty[i].attributes[0] <= threshold))
			delDFS(u, k); // delete all the nodes that violate the k-core constraint or the threshold requirement
	}
}

void community_search::shrink_kcore() // to find the next k-core
{
	int u;
	int idx = 0;
	for (int i = 0; i < node_num; i++) { // 
		u = verty[i].nodeId;
		if (del[u] == 0) {
			vertx[idx++] = verty[i];
			visit[u] = 0; // reinitialize visit
		}
		else
			visit[u] = 1;
		timestamp[u][2] = timestamp[u][1] = 0;
	}
	node_num = idx; // up to now, we obtain the new k-core with new node_num nodes
	for (int i = 0; i < node_num; i++) {
		verty[i] = vertx[i];
	}
}

double community_search::save_results(string path, int k, int idx, int tp)
{
	clock_t st, ed;
	st = clock();

	string result = "";
	for (int i = 0; i < sub_num; i++) { // write to a string stream
		stringstream ss;
		int u = vertx[i].nodeId;
		if (timestamp[u][1] == tp) {
			ss << u;
			result += ss.str();
			result += "\n";
		}
	}

	stringstream sss;
	stringstream sss1;
	sss << idx;
	sss1 << k;
	path = path + "community/community-" + sss1.str() + "-" + sss.str() +".txt";
	ofstream fout(path.c_str());
	fout << result;
	fout.close();

	ed = clock();
	return (double)((ed - st) / CLOCKS_PER_SEC);
}

void community_search::clear()
{
	for (int i = 0; i < graph_size + 2; i++) {
		delete[] timestamp[i];
		delete[] keyNodes[i];
	}
	delete[] timestamp;
	delete[] keyNodes;
	delete[] vertx;
	delete[] verty;
	delete[] vertz;
	delete[] vertu;
	delete[] vertv;
	delete[] del;
	delete[] visit;
	delete[] core;
	delete[] deg;
	delete[] ddeg;
	delete[] neighbour;
	vertx = NULL;
	verty = NULL;
	vertz = NULL;
	vertu = NULL;
	vertv = NULL;
	del = NULL;
	ddeg = NULL;
	visit = NULL;
	core = NULL;
	deg = NULL;
	timestamp = NULL;
	neighbour = NULL;
	res.clear();
}

int community_search::cptTreeNodes(int k, int d, int size, node *arr, node *key_node)
{// core: k, attribute: d, node number: size, node array: arr, the starting node of a tree vertex in d+1 dimension: key_node
	int unode;
	int tp = 0;
	int ptr = 0;
	int fg = 1;
//------------------------------
	/*cout << "Now we are considering dimension " << d << ": ";
	for (int i = 0; i < size; i++)
		cout << arr[i].nodeId << " ";
	cout << endl;*/
//-------------------------------
	degree_calculation(d, size, arr);
	while (true) {
		ptr = find_next_node(ptr, size, arr);
		if (ptr == -1)
			break;
		tp++;
		unode = arr[ptr].nodeId;
		keyNodes[tp][d] = arr[ptr];
		fg = DFS(unode, k, d, tp, key_node);
		if (fg == 0) {
			for (int i = ptr; i < size; i++) {
				int vnode = arr[i].nodeId;
				visit[vnode] = 1;
				if (timestamp[vnode][d] == 0)
					timestamp[vnode][d] = tp;
			}
			break;
		}
	}
	return tp;
}

//================Entry of two attributes===================
void community_search::two_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl)
{
	double interval = 0.0;
	node_num = gsize;
	sort_by_attribute(0, gsize, 2);

	while (node_num > 0) { // if node_num == 0, just end the process
		//cout << node_num << endl;
		int tp = cptTreeNodes(k, 2, node_num, verty, key_node);
		ythreshold = keyNodes[tp][2].attributes[1];
		find_initial_graph_for_second_round(tp);
		sort_by_attribute(0, sub_num, 1);
		tp = cptTreeNodes(k, 1, sub_num, vertx, key_node);
		xthreshold = keyNodes[tp][1].attributes[0];

		sl.IF[1] = ythreshold;
		sl.IF[0] = xthreshold;
		int len = res.size();
		int flag = 0;
		for (int i = 0; i < len; i++) {
			flag = 1;
			for (int j = 0; j < dim; j++) {
				if (res[i].IF[j] < sl.IF[j]) { // >=
					flag = 0;
					break;
				}
			}
		/*	if (flag == 1) { 
				for (int j = 0; j < dim; j++) { 
					if (res[i].IF[j] > sl.IF[j]) { // > strictly
						flag = 2; // if flag = 2, sl is dominated by res[i]
						break;
					}
				}
			}*/
			if (flag == 1)
				break;
		}
		if (flag != 1) {
			res.push_back(sl); // A new skyline has been found.
//----------------------------------------------
			answer_size++;
			//cout << answer_size << endl;
		/*	cout << "A new skyline community has been found: ";
			for (int cc = 0; cc < dim; cc++) {
				cout << sl.IF[cc] << " ";
			}
			cout << endl;*/
//----------------------------------------------
			//cout << "k-community: " << snum << endl;
			//interval += save_results(path, k, snum++, tp);
		}
//----------------------------------------------
	/*	else {
			cout << "This skyline community has been dominated: ";
			//for (int cc = 0; cc < dim; cc++) {
			//	cout << sl.IF[cc] << " ";
			//}
			//cout << endl;
		}*/
//----------------------------------------------
		delete_nodes_by_dfs(xthreshold, k); // Note: based on the original node degree
		shrink_kcore(); // to obtain the next k-core for the next cycle
	}
}

//================Entry of three attributes=================
void community_search::three_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl)
{ // maxtp is the maximum number of tree vertices
	sort_by_attribute(0, gsize, 3); // sort by third attribute (in a reversed way)
	int maxtp = cptTreeNodes(k, 3, gsize, vertz, key_node); // computing tree vertices according to the 3rd attribute
	for (int o = maxtp; o > 0; o--) // There are maxtp tree vertices totally. Each vertex is stored in verty.
	{
		clock_t interv;
		interv = clock();
		if (((double)(interv - ststime) / CLOCKS_PER_SEC) >= ttsh)
			return;
//---------------------------------
		//cout << "2-turn: " << maxtp - o + 1 << endl;
//---------------------------------
		int idx = 0;
		for (int q = 0; q < gsize; q++)
		{
			int vnode = vertz[q].nodeId;
			if (timestamp[vnode][3] >= o) {
				verty[idx++] = vertz[q];
				visit[vnode] = 0;
				del[vnode] = 0;
			}
			else {
				visit[vnode] = 1;
				del[vnode] = 1;
			}
			timestamp[vnode][2] = 0;
			timestamp[vnode][1] = 0;
		}
		sl.IF[2] = keyNodes[o][3].attributes[2];
		key_node[3] = keyNodes[o][3];

		//===============================this operation is very important!!!(每求出一个keynode即可得到skyline的一个上界)
		double thd[5];
		for (int ee = 0; ee < 5; ee++)
			thd[ee] = upth;
		for (int ths = 0; ths < dim; ths++) {
			for (int nm = 3; nm <= dim; nm++) {
				thd[ths] = min(thd[ths], key_node[nm].attributes[ths]);
			}
		}
		int len = res.size();
		int flag = 0;
		for (int i = 0; i < len; i++) {
			flag = 1;
			for (int j = 0; j < dim; j++) {
				if (res[i].IF[j] < thd[j]) { // >=
					flag = 0;
					break;
				}
			}
			if (flag == 1) {
				for (int j = 0; j < dim; j++) {
					if (res[i].IF[j] > thd[j]) { // > strictly
						flag = 2; // if flag = 2, sl is dominated by res[i]
						break;
					}
				}
			}
			if (flag == 2)
				break;
		}
		//===============================
		if (flag != 2)
			two_attributes_community_search(path, k, dim, idx, key_node, sl);
	}
}

//================Entry of four attributes=================
void community_search::four_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl)
{
	sort_by_attribute(0, gsize, 4); // sort by 4th attribute (in a reversed way)
	int maxtp = cptTreeNodes(k, 4, gsize, vertu, key_node); // computing tree vertices according to the 4th attribute
	for (int o = maxtp; o > 0; o--) // There are maxtp tree vertices totally. Each vertex is stored in vertz.
	{
		clock_t interv;
		interv = clock();
		if (((double)(interv - ststime) / CLOCKS_PER_SEC) >= ttsh)
			return;
//---------------------------------
		//cout << "3-turn: " << maxtp - o + 1 << endl;
//---------------------------------
		int idx = 0;
		for (int q = 0; q < gsize; q++)
		{
			int vnode = vertu[q].nodeId;
			if (timestamp[vnode][4] >= o) {
				vertz[idx++] = vertu[q];
				visit[vnode] = 0;
				del[vnode] = 0;
			}
			else {
				visit[vnode] = 1;
				del[vnode] = 1;
			}
			timestamp[vnode][3] = 0;
			timestamp[vnode][2] = 0;
			timestamp[vnode][1] = 0;
		}
		sl.IF[3] = keyNodes[o][4].attributes[3];
		key_node[4] = keyNodes[o][4];

		//===============================this operation is very important!!!
		double thd[5];
		for (int ee = 0; ee < 5; ee++)
			thd[ee] = upth;
		for (int ths = 0; ths < dim; ths++) {
			for (int nm = 4; nm <= dim; nm++) {
				thd[ths] = min(thd[ths], key_node[nm].attributes[ths]);
			}
		}
		int len = res.size();
		int flag = 0;
		for (int i = 0; i < len; i++) {
			flag = 1;
			for (int j = 0; j < dim; j++) {
				if (res[i].IF[j] < thd[j]) { // >=
					flag = 0;
					break;
				}
			}
			if (flag == 1) {
				for (int j = 0; j < dim; j++) {
					if (res[i].IF[j] > thd[j]) { // > strictly
						flag = 2; // if flag = 2, sl is dominated by res[i]
						break;
					}
				}
			}
			if (flag == 2)
				break;
		}
		//===============================
		if (flag != 2)
			three_attributes_community_search(path, k, dim, idx, key_node, sl);
	}
}

//================Entry of five attributes=================
void community_search::five_attributes_community_search(string path, int k, int dim, int gsize, node *key_node, skyline sl)
{
	sort_by_attribute(0, gsize, 5); // sort by 5th attribute (in a reversed way)
	int maxtp = cptTreeNodes(k, 5, gsize, vertv, key_node); // computing tree vertices according to the 5th attribute
	for (int o = maxtp; o > 0; o--) // There are maxtp tree vertices totally. Each vertex is stored in vertu.
	{
		clock_t interv;
		interv = clock();
		if (((double)(interv - ststime) / CLOCKS_PER_SEC) >= ttsh)
			return;
//---------------------------------
		//cout << "4-turn: " << maxtp - o + 1 << endl;
//---------------------------------
		int idx = 0;
		for (int q = 0; q < gsize; q++)
		{
			int vnode = vertv[q].nodeId;
			if (timestamp[vnode][5] >= o) {
				vertu[idx++] = vertv[q];
				visit[vnode] = 0;
				del[vnode] = 0;
			}
			else {
				visit[vnode] = 1;
				del[vnode] = 1;
			}
			timestamp[vnode][4] = 0;
			timestamp[vnode][3] = 0;
			timestamp[vnode][2] = 0;
			timestamp[vnode][1] = 0;
		}
		sl.IF[4] = keyNodes[o][5].attributes[4];
		key_node[5] = keyNodes[o][5]; 

		//===============================this operation is very important!!!
		double thd[5];
		for (int ee = 0; ee < 5; ee++)
			thd[ee] = upth;
		for (int ths = 0; ths < dim; ths++) {
			for (int nm = 5; nm <= dim; nm++) {
				thd[ths] = min(thd[ths], key_node[nm].attributes[ths]);
			}
		}
		int len = res.size();
		int flag = 0;
		for (int i = 0; i < len; i++) {
			flag = 1;
			for (int j = 0; j < dim; j++) {
				if (res[i].IF[j] < thd[j]) { // >=
					flag = 0;
					break;
				}
			}
			if (flag == 1) {
				for (int j = 0; j < dim; j++) {
					if (res[i].IF[j] > thd[j]) { // > strictly
						flag = 2; // if flag = 2, sl is dominated by res[i]
						break;
					}
				}
			}
			if (flag == 2)
				break;
		}
		//===============================
		if (flag != 2)
			four_attributes_community_search(path, k, dim, idx, key_node, sl);
	}
}

//================Entry of skyine search process==================
int community_search::skyline_search(string path, string M, string OO, int k, int dimL, int dimH, int gsize)
{
	answer_size = 0;
	initialization(gsize);
	int ggsize = initkcore(path, M, OO, k, dimL, dimH);
	cout << k << "-core size: " << ggsize << endl;

	clock_t st, ed;
	st = clock();
	ststime = (double)st;

	skyline sl;
	node *keynode; // store those nodes that cannot be removed when finding a skyline community
	keynode = new node[6];
	for (int i = 0; i < 6; i++) {
		keynode[i].nodeId = -1;
		for (int j = 0; j < 5; j++)
			keynode[i].attributes[j] = upth;
	}

	if (dimL == 2) {
		two_attributes_community_search(path, k, dimL, ggsize, keynode, sl);
	}
	else if (dimL == 3) {
		three_attributes_community_search(path, k, dimL, ggsize, keynode, sl);
	}
	else if (dimL == 4) {
		four_attributes_community_search(path, k, dimL, ggsize, keynode, sl);
	}
	else if (dimL == 5) {
		five_attributes_community_search(path, k, dimL, ggsize, keynode, sl);
	}

	ed = clock();
	double tst = (double)(ed - st) / CLOCKS_PER_SEC;
	cout << "Time for basic algorithm: " << tst << endl;
	cout << "size: " << answer_size << endl;

	string ppdr = path + "result-basic.txt";
	ofstream ofile;
	ofile.open(ppdr, ios::out | ios::app);
	if (ofile.good()) cout << "success" << endl;
	else cout << "fail" << endl;

	ofile << path << endl;
	ofile << M << "         " << OO << endl;
	ofile << "kcore: " << k << " dim: " << dimL << endl;
	ofile << "time: " << tst << " size: " << res.size() << endl;
	ofile << endl;
	ofile.close();

	delete[] keynode;
	clear();
  //if (tst >= 100000) return 0;
  //else return 1;
	return coresize;
}

skyline::skyline()
{
	for (int i = 0; i < 5; i++)
		IF[i] = -1; // initialization
}

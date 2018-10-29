// MAIC.cpp: Defines the entry point for the console application. Nodes are labled by 0~(N-1).
// @date: 2016/7/18 

//#include "stdafx.h"
#include "CommunitySearch.h"
#include "SkylineSearch.h"
#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

int basicAlgorithm(string path, string M, string OO, int k, int dimL, int dimH, int n) {
	community_search *app = new community_search();
	int fl = app->skyline_search(path, M, OO, k, dimL, dimH, n);
	delete app;
	app = NULL;
  return fl;
}

int impAlgorithm(string path, string M, string OO, int k, int dimL, int dimH, int n) {
	SkylineComm_Search *app = new SkylineComm_Search();
	int fl = app->skycomm_search(path, M, OO, k, dimL, dimH, n);
	delete app;
	app = NULL;
  return fl;
}

int main()
{
  string M1[3] = {"new1I", "new2C", "new3A"};
  string M2 = "o"; // without preprocessing
  string M3 = "p";

  int dH = 6;

  string path[10]; // edge list, attributes, and k-core
  int n[10]; // number of nodes

  // specify the data set infomation here

  for (int j = 0; j <= 0; j++)
  for (int i = 0; i <= 2; i++) // attributes (independent, correlated, anti-correlated)
  for (int d = 2; d <= 5; d++) { // dimension, when d increases, time cost increases significantly
    for (int k = 5; k <= 25; k += 5) { // k-core
      int d0 = impAlgorithm(path[j], M1[i], M2, k, d, dH, n[j]);
    }
  }

  return 0;
}

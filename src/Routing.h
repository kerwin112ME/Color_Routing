// **************************************************************************
//  File       [Routing.h]
//  Author     [Team six]
//  Synopsis   [The header file of routing]
//  Modify     [2018/05/28 Dollar]
// **************************************************************************
#ifndef _ROUTING_H
#define _ROUTING_H

#include <algorithm>
#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

class node { 
friend class routing;
private:
	int x;
	int y;
	int detour; // used in Hadlock algorithm
	int layer;
	int via;
	vector<int> pred; // predecessor, store layer and x y coordinate
	vector<int> MSTpred;
	int color; //white:0  black:1  blue:2

public:
	int order;
	node() {
		x = 0;
		y = 0;
		layer = 0;
		detour = 0;
		via = 0;
		color = 0;
		order = 0;
		pred.resize(3);
	}
	node(int l,int a,int b);
	void set(int l, int a, int b);
	void setDet(int d);
	void setVia(int v);
	void setColor(int c);
	void setPred(vector<int> p);
	void setMSTPred(vector<int> p);
	int getx();
	int gety();
	int getLayer();
	int getDetour();
	int getVia();
	int getColor();
	vector<int> getIndex();
	vector<int> getPred();
	vector<int> getMSTPred();
};

struct cmp{
    bool operator() (node& lhs,node& rhs){
        return lhs.getDetour() > rhs.getDetour();
    }
};

struct cmpHL{
    bool operator() (node& lhs, node& rhs){
    	if(lhs.getDetour() != rhs.getDetour())
    		return lhs.getDetour() > rhs.getDetour();
    	else {
    		if(lhs.getVia() != rhs.getVia())
    			return lhs.getVia() > rhs.getVia();
    		else
    			return lhs.order > rhs.order;
    	}
    }
};
// 宣告方式 : priority_queue <node, vector<node>, struct cmpHL> pQ;
// 為 min heap
// minimum: pQ.top()  (note that top() do not pop out the element)
// extract min: pQ.pop()
// insert: pQ.push(node n)
// is empty?: pQ.empty()

class line {
private:
	node node1,node2;
	int lineColor;
	int layer;

public:
	void setN1(node n1);
	void setN2(node n2);
	node getN1();
	node getN2();
	void setLC(int lc); // set line color
	int getLENGTH();
	int getLC(); // get line color
	void setLY(int ly); // set line layer
	int getLY(); // get line layer
	line *p; // parent of disjoint set
	int rank;
};

class routing {
public:
	vector< vector<node> > map[4]; // resize map after reading data files
	vector<line> Hadlock(node n1, node n2, bool MST); // input two nodes to connect and return a sequence of lines
	vector<line> MST(vector< vector<int> > netCoor); // input a net of nodes then return an arranged net after MST
	void classifyline(vector<line> &outline);//make outline be classified to m1 m2 m3 m4
	void ColorPat(); // input a net of lines then do the color-patterning
	vector<line> *m[4];//store lines in layers
	int Mdis(node a, node b);
	void prt_Map(int map_layer, int origin_x, int origin_y); // print the maps to verify the result
	void createblock(vector<node> b);//create blockage in maps
	void netOrdering(vector< vector<int> > &nets, vector<node> pin); // sort the nets by their total rectangular area
	int NetArea(vector<int> net, vector<node> pin); 
	int MSTCost(vector< vector<int> > netCoor);
	vector< vector<int> > HananGrid(vector< vector<int> > netCoor);
	void addSteiner(vector< vector<int> > &netCoor, vector< vector<int> > hananGrid);
	int viaTotal;
	void Checker(vector< vector<line> > allLines, vector< vector< vector<int> > > netPins, vector<node> blockage);
	void MakeSet(line *l);
	line* FindSet(line *l);
	void Union(line *l1, line *l2);

private:
	int  checkline(int layer,int posi);//check line color
};

#endif

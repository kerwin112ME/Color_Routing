// **************************************************************************
//  File       [Routing.cpp]
//  Author     [Team six]
//  Synopsis   [The implementation of routing]
//  Modify     [2018/06/01 Sheng-Chia Yu]
// **************************************************************************

#include "Routing.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <iomanip>

#define INF 2147483647
//====================================
// node
node::node(int l,int a,int b){
	layer = l;
	x = a;
	y = b;
}
void node::set(int l, int a, int b){
	layer = l;
	x = a;
	y = b;
}
void node::setDet(int d){
	detour = d;
}
void node::setVia(int v){
	via = v;
}
void node::setColor(int c){
	color = c;
}
void node::setPred(vector<int> p){
	pred[0] = p[0];
	pred[1] = p[1];
	pred[2] = p[2];
}

void node::setMSTPred(vector<int> p){ // p[0]:l  p[1]:x  p[2]:y
	MSTpred.clear();
	MSTpred.assign(p.begin(),p.end());
}

int node::getx(){
	return x;
}
int node::gety(){
	return y;
}
int node::getLayer(){
	return layer;
}
int node::getDetour() {
	return detour;
}
int node::getVia(){
	return via;
}
vector<int> node::getPred(){
    return pred; 
}

vector<int> node::getMSTPred(){
    return MSTpred; 
}

vector<int> node::getIndex() {
	int ind[] = {layer,x,y};
	vector<int> index;
	index.assign(ind,ind+3);

	return index;
}

int node::getColor(){
	return color;
}
//=========================================
//line
void line::setN1(node n1){
	node1 = n1; 
}
void line::setN2(node n2){
	node2 = n2; 
}
node line::getN1(){
	return node1;
}
node line::getN2(){
	return node2;
}
int  line::getLENGTH(){
	return abs(node1.getx()-node2.getx())+abs(node1.gety()-node2.gety());
}
void line::setLC(int lc){
	lineColor = lc;
}
int line::getLC(){
	return lineColor;
}
void line::setLY(int ly){
	layer = ly;
}
int line::getLY(){
	return layer;
}
//============================================
//Rounting
int routing::NetArea(vector<int> net, vector<node> pin) {
	vector< vector<int> > netCoor;
	netCoor.clear();
	for(int i=1; i<net.size(); i++) {
		netCoor.push_back(pin[net[i]].getIndex());
	}
	int xmin = INF, xmax = -INF, ymin = INF, ymax = -INF;
	for(int i=0; i<netCoor.size(); i++) {
		if(netCoor[i][1] < xmin)
			xmin = netCoor[i][1];
		if(netCoor[i][1] > xmax)
			xmax = netCoor[i][1];
		if(netCoor[i][2] < ymin)
			ymin = netCoor[i][2];
		if(netCoor[i][2] > ymax)
			ymax = netCoor[i][2];
	}

	int area = (xmax-xmin)*(ymax-ymin);
	return area;
}

void routing::netOrdering(vector< vector<int> > &nets, vector<node> pin) {
	for(int i=1; i<nets.size(); i++) {
		int key = NetArea(nets[i],pin);
		vector<int> keyVector = nets[i];
		int j = i-1;
		while(j>=0 && NetArea(nets[j],pin)>key) {
			nets[j+1] = nets[j];
			j = j-1;
		}
		nets[j+1] = keyVector;
	}
}

int routing::Mdis(node a, node b) {
        int d = abs(b.getx()-a.getx())+abs(b.gety()-a.gety())+abs(b.getLayer()-a.getLayer());
        return d;
};

int routing::MSTCost(vector< vector<int> > netCoor) {
	priority_queue <node, vector<node>, struct cmp> pQ;
	int vcnt = 0;
	int cost = 0;
	node n;
	vector<int> chosen(netCoor.size(),0);
	
	node source;
	source.set(netCoor[0][0],netCoor[0][1],netCoor[0][2]);

	for(int i=1; i<netCoor.size(); i++) {
		node temp;
		temp.set(netCoor[i][0],netCoor[i][1],netCoor[i][2]);
		temp.order = i;
		temp.setMSTPred(source.getIndex());
		temp.setDet(Mdis(source, temp));
		pQ.push(temp);
	}

	int size = pQ.size();
	while(!pQ.empty() && vcnt<size) {
		n = pQ.top();
		pQ.pop();
		if(chosen[n.order-1] == 1)
			continue;
		cost += n.getDetour();
		chosen[n.order-1] = 1;

		for(int i=1; i<netCoor.size(); i++) {
			if(chosen[i-1] == 1)
				continue;
			node temp;
			temp.set(netCoor[i][0],netCoor[i][1],netCoor[i][2]);
			temp.order = i;
			temp.setMSTPred(n.getIndex());
			temp.setDet(Mdis(n, temp));
			pQ.push(temp);
		}

		vcnt++;
	}

	return cost;
}

vector< vector<int> > routing::HananGrid(vector< vector<int> > netCoor) {
	int originCost = MSTCost(netCoor);
	int xmin = INF, xmax = -INF, ymin = INF, ymax = -INF;
	for(int i=0; i<netCoor.size(); i++) {
		if(netCoor[i][1] < xmin)
			xmin = netCoor[i][1];
		if(netCoor[i][1] > xmax)
			xmax = netCoor[i][1];
		if(netCoor[i][2] < ymin)
			ymin = netCoor[i][2];
		if(netCoor[i][2] > ymax)
			ymax = netCoor[i][2];
	}

	vector< vector<int> > xSet,ySet;
	xSet.clear();
	ySet.clear();

	for(int i=0; i<netCoor.size(); i++) {
		int x = netCoor[i][1];
		if(x != xmin && x != xmax) {
			vector<int> tempx(2,0);
			tempx[0] = x;
			tempx[1] = netCoor[i][0];
			xSet.push_back(tempx);
		}

		int y = netCoor[i][2];
		if(y != ymin && y != ymax) {
			vector<int> tempy(2,0);
			tempy[0] = y;
			tempy[1] = netCoor[i][0];
			ySet.push_back(tempy);
		}
	}

	vector< vector<int> > hananGrid;
	hananGrid.clear();
	bool xLayer, yLayer;

	for(int i=0; i<xSet.size(); i++) {
		if(xSet[i][0]%2 != 0)
			xLayer = false;
		else
			xLayer = true;

		for(int j=0; j<ySet.size(); j++) {
			if(ySet[j][0]%2 == 0)
				yLayer = false;
			else
				yLayer = true;

			if(xLayer == false && yLayer == false)
				continue;

			vector<int> tempVec(3,0); // [0]:l [1]:x [2]:y ([3]:delta(MST))
			if(xLayer)
				tempVec[0] = xSet[i][1];
			else
				tempVec[0] = ySet[i][1];
			tempVec[1] = xSet[i][0];
			tempVec[2] = ySet[j][0];
			if(map[tempVec[0]][tempVec[1]][tempVec[2]].getColor() != 0)
				continue;
			vector< vector<int> > tempCoor;
			tempCoor = netCoor;
			tempCoor.push_back(tempVec);
			tempVec.push_back(originCost - MSTCost(tempCoor));

			hananGrid.push_back(tempVec);
		}
	}	

	for(int i=1; i<hananGrid.size(); i++) {
		int key = hananGrid[i][3];
		vector<int> keyVector = hananGrid[i];
		int j = i-1;
		while(j>=0 && hananGrid[j][3]<key) {
			hananGrid[j+1] = hananGrid[j];
			j = j-1;
		}
		hananGrid[j+1] = keyVector;
	}

	return hananGrid; // in order of non-increasing delta(MST)
	
}

void routing::addSteiner(vector< vector<int> > &netCoor, vector< vector<int> > hananGrid) {
	vector< vector<int> > Add;
	int oldCost = 0;
	int newCost = MSTCost(netCoor);
	for(int i=0; i<hananGrid.size(); i++) {
		oldCost = newCost;
		Add.clear();
		Add = netCoor;
		Add.push_back(hananGrid[i]);
		newCost = MSTCost(Add);
		if(newCost-oldCost > 0)
			netCoor.push_back(hananGrid[i]);

	}
}

vector<line> routing::Hadlock(node n1, node n2, bool MST){
	// map 0,2:horizontal 1,3:vertical
	vector<node> Tree;    //record all the blocks which have been explored
	priority_queue <node, vector<node>, struct cmpHL> pQ;// processing list after comparing detour and via
	vector<node> tmp_ans;
	vector<line> ans;     // return answer consisting of corners
	vector<int> direction; // record line direction to find the corners
	node tmp_node;
	node next_node;
	line tmp_line;
	vector<int> pred;
	bool pinpin = false; // for checking pin-pin adjacency
	bool found = false;
	int new_layer = 0;
	int new_x = 0;
	int new_y = 0;
	int lay[4] = {1, -1, 0, 0};// same plane 0 1
	int via[4] = {0, 0, -1, 1};// via 2 3
	int color = 1;// white:0  black:1  blue:2
	unsigned int order = 1;
	direction.resize(2);
	pred.resize(3);

	if(MST == true)
		color = 2;
	// check pin-pin adjacency and remove blockage set for color patterning n1
	for(int i = 0; i < 2; i++){
		if(n1.getLayer() == 0 || n1.getLayer() == 2){
			new_x = n1.getx() + lay[i];
			new_y = n1.gety();
			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && (map[n1.getLayer()][new_x][new_y].getColor() == 3)){
				for(int j = 0; j < 2; j++){
					new_x = n1.getx();
					new_y = n1.gety() + lay[j];
					if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	           (new_y < map[0][0].size()) && (map[n1.getLayer()][new_x][new_y].getColor() == 5)){
						map[n1.getLayer()][new_x][new_y].setColor(0);
					}
				}
				pinpin = true;
				break;
			}
		}
		else{
			new_x = n1.getx();
			new_y = n1.gety() + lay[i];
			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && (map[n1.getLayer()][new_x][new_y].getColor() == 3)){
				for(int j = 0; j < 2; j++){
					new_x = n1.getx() + lay[j];
					new_y = n1.gety();
					if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	           (new_y < map[0][0].size()) && (map[n1.getLayer()][new_x][new_y].getColor() == 5)){
						map[n1.getLayer()][new_x][new_y].setColor(0);
					}
				}
				pinpin = true;
				break;
			}
		}
	}
	if(!pinpin){
		for(int i = 0; i < 4; i++){
			new_x = n1.getx() + lay[i];
			new_y = n1.gety() + via[i];
			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && (map[n1.getLayer()][new_x][new_y].getColor() == 5)){
				map[n1.getLayer()][new_x][new_y].setColor(0);
			}
		}
	}
	pinpin = false;
	// check pin-pin adjacency and remove blockage set for color patterning n2
	for(int i = 0; i < 2; i++){
		if(n2.getLayer() == 0 || n2.getLayer() == 2){
			new_x = n2.getx() + lay[i];
			new_y = n2.gety();
			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && (map[n2.getLayer()][new_x][new_y].getColor() == 3)){
				for(int j = 0; j < 2; j++){
					new_x = n2.getx();
					new_y = n2.gety() + lay[j];
					if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	           (new_y < map[0][0].size()) && (map[n2.getLayer()][new_x][new_y].getColor() == 5)){
						map[n2.getLayer()][new_x][new_y].setColor(0);
					}
				}
				pinpin = true;
				break;
			}
		}
		else{
			new_x = n2.getx();
			new_y = n2.gety() + lay[i];
			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && (map[n2.getLayer()][new_x][new_y].getColor() == 3)){
				for(int j = 0; j < 2; j++){
					new_x = n2.getx() + lay[j];
					new_y = n2.gety();
					if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	           (new_y < map[0][0].size()) && (map[n2.getLayer()][new_x][new_y].getColor() == 5)){
						map[n2.getLayer()][new_x][new_y].setColor(0);
					}
				}
				pinpin = true;
				break;
			}
		}
	}
	if(!pinpin){
		for(int i = 0; i < 4; i++){
			new_x = n2.getx() + lay[i];
			new_y = n2.gety() + via[i];
			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && (map[n2.getLayer()][new_x][new_y].getColor() == 5)){
				map[n2.getLayer()][new_x][new_y].setColor(0);
			}
		}
	}
	pinpin = false;
	// start Hadlock's algorithm to search
	new_x = 0;
	new_y = 0;
	pQ.push(n1);

	while(!pQ.empty() && !found){
		tmp_node = pQ.top(); // extract min
		pQ.pop();
		for(int i = 0; i < 4; i++){
			//set coordinate of new node
			if(tmp_node.getLayer() == 0 || tmp_node.getLayer() == 2){ //layer 0 2
				new_layer = tmp_node.getLayer() + via[i];
				new_x = tmp_node.getx() + lay[i];
				new_y = tmp_node.gety();
			}
			else{                                                                   //layer 1 3
				new_layer = tmp_node.getLayer() + via[i];
				new_x = tmp_node.getx();
				new_y = tmp_node.gety() + lay[i];
			}
			// check if the node reach the target
			if(new_layer == n2.getLayer() && new_x == n2.getx() && new_y == n2.gety()){
				pred[0] = tmp_node.getLayer();
				pred[1] = tmp_node.getx();
				pred[2] = tmp_node.gety();
				map[new_layer][new_x][new_y].setPred(pred);
				map[new_layer][new_x][new_y].setVia(tmp_node.getVia() + abs(via[i]));
				Tree.push_back(map[new_layer][new_x][new_y]);
				viaTotal += map[new_layer][new_x][new_y].getVia();
				found = true;
				break;
			}
			//check if the node reach the line in same net
			if((new_layer >= 0) && (new_x >= 0) && (new_y >= 0) && (new_layer <= 3) && 
				(new_x < map[0].size()) && (new_y < map[0][0].size()) && (map[new_layer][new_x][new_y].getColor() == 2) ){
				pred[0] = tmp_node.getLayer();
				pred[1] = tmp_node.getx();
				pred[2] = tmp_node.gety();
				map[new_layer][new_x][new_y].setPred(pred);
				map[new_layer][new_x][new_y].setVia(tmp_node.getVia() + abs(via[i]));
				Tree.push_back(map[new_layer][new_x][new_y]);
				viaTotal += map[new_layer][new_x][new_y].getVia();
				found = true;
				break;
			}
			// determine if the block is available to go or not
			if( (new_layer >= 0) && (new_x >= 0) && (new_y >= 0) && (new_layer <= 3) && 
				(new_x < map[0].size()) && (new_y < map[0][0].size()) && (map[new_layer][new_x][new_y].getColor() == 0) ){
				// set the new node
				map[new_layer][new_x][new_y].set(new_layer, new_x, new_y);//layer x y
				map[new_layer][new_x][new_y].setColor(1);// color
				pred[0] = tmp_node.getLayer();// pred
				pred[1] = tmp_node.getx();
				pred[2] = tmp_node.gety();
				map[new_layer][new_x][new_y].setPred(pred);
				// determine detour
				if(tmp_node.getLayer() == 0 || tmp_node.getLayer() == 2){ // determine detour layer 0 2
					if( abs(new_x - n2.getx()) > abs(tmp_node.getx() - n2.getx()) && abs(new_layer - n2.getLayer()) > abs(tmp_node.getLayer() - n2.getLayer()) ){
						map[new_layer][new_x][new_y].setDet(tmp_node.getDetour() + 2);
					}
					else if(abs(new_x - n2.getx()) > abs(tmp_node.getx() - n2.getx()) || abs(new_layer - n2.getLayer()) > abs(tmp_node.getLayer() - n2.getLayer()) ){
						map[new_layer][new_x][new_y].setDet(tmp_node.getDetour() + 1);
					}
					else{
						map[new_layer][new_x][new_y].setDet(tmp_node.getDetour());
					}
				}
				else{                                                                   // determine detour layer 1 3
					if( abs(new_y - n2.gety()) > abs(tmp_node.gety() - n2.gety()) && abs(new_layer - n2.getLayer()) > abs(tmp_node.getLayer() - n2.getLayer()) ){
						map[new_layer][new_x][new_y].setDet(tmp_node.getDetour() + 2);
					}
					else if(abs(new_y - n2.gety()) > abs(tmp_node.gety() - n2.gety()) || abs(new_layer - n2.getLayer()) > abs(tmp_node.getLayer() - n2.getLayer()) ){
						map[new_layer][new_x][new_y].setDet(tmp_node.getDetour() + 1);
					}
					else{
						map[new_layer][new_x][new_y].setDet(tmp_node.getDetour());
					}
				}
				// determine via
				map[new_layer][new_x][new_y].setVia(tmp_node.getVia() + abs(via[i]));
				// determine order
				map[new_layer][new_x][new_y].order = order;
				order += 1;
				//push into Tree to record the node which have been explored
				Tree.push_back(map[new_layer][new_x][new_y]);	
				//push into priority queue to determine the process order 
				pQ.push(map[new_layer][new_x][new_y]);
			}
		}
	}
	//clean the blocks which have been used 
	for(int i = 0; i < Tree.size()-1; i++){
		map[Tree[i].getLayer()][Tree[i].getx()][Tree[i].gety()].setColor(0);
	}
	//output the corners and record the path
	if(found){
		tmp_node = map[Tree.back().getLayer()][Tree.back().getx()][Tree.back().gety()];
		next_node = map[tmp_node.getPred()[0]][tmp_node.getPred()[1]][tmp_node.getPred()[2]];
		// find current direction
		if(next_node.getx() != tmp_node.getx())      direction[1] = 1; // horizontal line
		else if(next_node.gety() != tmp_node.gety()) direction[1] = 2; // vertical line
		else                                         direction[1] = 3; // via line
		tmp_node = next_node;

		while(tmp_node.getLayer() != n1.getLayer() || tmp_node.getx() != n1.getx() || tmp_node.gety() != n1.gety()){
			map[tmp_node.getLayer()][tmp_node.getx()][tmp_node.gety()].setColor(color);
			next_node = map[tmp_node.getPred()[0]][tmp_node.getPred()[1]][tmp_node.getPred()[2]];
			// find current direction
			if(next_node.getx() != tmp_node.getx())      direction[0] = 1; // horizontal line
			else if(next_node.gety() != tmp_node.gety()) direction[0] = 2; // vertical line
			else                                         direction[0] = 3; // via line
			// compare current direction[0] and last direction[1]
			if(direction[0] != direction[1]) tmp_ans.insert(tmp_ans.begin(), tmp_node);

			tmp_node = next_node;
			direction[1] = direction[0]; // current direction[0] becomes last direction[1]
		}

		tmp_ans.insert(tmp_ans.begin(), n1);
		tmp_ans.push_back(map[Tree.back().getLayer()][Tree.back().getx()][Tree.back().gety()]);

		for(int i = 0; i < tmp_ans.size()-1; i++){
			tmp_line.setN1(tmp_ans[i]);
			tmp_line.setN2(tmp_ans[i+1]);
			tmp_line.setLY(tmp_ans[i].getLayer());
			ans.push_back(tmp_line);
		}
	}
	else{
		ans.clear();
	}
	// add blockage for color patterning
	for(int i = 0; i < 4; i++){
		new_x = n1.getx() + lay[i];
		new_y = n1.gety() + via[i];
		if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   (new_y < map[0][0].size()) && (map[n1.getLayer()][new_x][new_y].getColor() == 0)){
			map[n1.getLayer()][new_x][new_y].setColor(5);
		}
		new_x = n2.getx() + lay[i];
		new_y = n2.gety() + via[i];
		if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   (new_y < map[0][0].size()) && (map[n2.getLayer()][new_x][new_y].getColor() == 0)){
			map[n2.getLayer()][new_x][new_y].setColor(5);
		}
	}
	// reset Tree and pQ 
	Tree.clear();
	tmp_ans.clear();
	
	return ans;
}

vector<line> routing::MST(vector< vector<int> > netCoor){ // vector<int> : [layer,x,y]
	priority_queue <node, vector<node>, struct cmp> pQ;
	int vcnt = 0;
	node tempp;
	vector<line> Route;
	Route.clear();

	node tempn; // extract min
	node temps; // temp source
	node tempt; // temp target

	temps = map[netCoor[0][0]][netCoor[0][1]][netCoor[0][2]]; // source, the first pin;
	map[netCoor[0][0]][netCoor[0][1]][netCoor[0][2]].setColor(3); // set the color of source to pin

	for(int i=1; i<netCoor.size(); i++) { // initialize
		tempp.set(netCoor[i][0],netCoor[i][1],netCoor[i][2]);
		tempp.setMSTPred(temps.getIndex());
		tempp.setDet(Mdis(temps, tempp));
		map[netCoor[i][0]][netCoor[i][1]][netCoor[i][2]].setColor(6); 
		pQ.push(tempp); // push every node in
	}

	int size = pQ.size();

	while(!pQ.empty() && vcnt<size) { 
		cout << "size : " << size << " , vcnt : " << vcnt << endl;

		tempn =	pQ.top(); // extract min
		pQ.pop();
		tempt = map[tempn.getMSTPred()[0]][tempn.getMSTPred()[1]][tempn.getMSTPred()[2]]; //tempn's predecessor

		if(map[tempn.getIndex()[0]][tempn.getIndex()[1]][tempn.getIndex()[2]].getColor() != 6) { // if not green, continue
			continue;
		}
		
		map[tempn.getIndex()[0]][tempn.getIndex()[1]][tempn.getIndex()[2]].setColor(3);
		vector<line> tempRoute = Hadlock(map[tempn.getLayer()][tempn.getx()][tempn.gety()], map[tempt.getLayer()][tempt.getx()][tempt.gety()], true);
		cout << "node : " << tempn.getLayer() << "," << tempn.getx() << "," << tempn.gety() << endl;
		cout << "pred : " << tempt.getLayer() << "," << tempt.getx() << "," << tempt.gety() << endl;
		cout << endl;
		if(!tempRoute.empty()) {
			map[tempn.getIndex()[0]][tempn.getIndex()[1]][tempn.getIndex()[2]].setColor(3); // set to pin color
			map[tempn.getIndex()[0]][tempn.getIndex()[1]][tempn.getIndex()[2]].setMSTPred(tempt.getIndex());

			for(int i=1; i<netCoor.size(); i++) { // update new det 
				vector<int> currentInd = netCoor[i];
				if(map[currentInd[0]][currentInd[1]][currentInd[2]].getColor() == 6) { // if green, update
					node tempp;
					tempp.set(currentInd[0],currentInd[1],currentInd[2]); 
					tempp.setDet(Mdis(tempn,tempp));
					tempp.setMSTPred(tempn.getIndex());
					pQ.push(tempp);
				}
			}
			
			for(int j=0; j<tempRoute.size(); j++) { // push lines of single hadlock into route of MST
				Route.push_back(tempRoute[j]);
			}
			vcnt ++; // vertex count ++

		}
		if(pQ.empty()) break;

	}
	// After MST

	if(pQ.empty() && vcnt < size) {
		vector<line> fail;
		fail.clear();
		return fail; // if fail, return an empty vector<line>
	}

	// set route to black
	for(int i=0; i<Route.size(); i++) {
		node n1 = Route[i].getN1();
		node n2 = Route[i].getN2();
		if(n1.getLayer() == n2.getLayer()) {
			if(n1.getLayer()%2 == 0) { // horizontal
				int smaller = min(n1.getIndex()[1] , n2.getIndex()[1]);
				int bigger = max(n1.getIndex()[1] , n2.getIndex()[1]);
				for(int j=smaller; j<=bigger; j++)
					map[n1.getIndex()[0]][j][n1.getIndex()[2]].setColor(1);
			}
			else {
				int smaller = min(n1.getIndex()[2] , n2.getIndex()[2]);
				int bigger = max(n1.getIndex()[2] , n2.getIndex()[2]);
				for(int j=smaller; j<=bigger; j++)
					map[n1.getIndex()[0]][n1.getIndex()[1]][j].setColor(1);
			}
		}
		else {
			int smaller = min(n1.getIndex()[0] , n2.getIndex()[0]);
			int bigger = max(n1.getIndex()[0] , n2.getIndex()[0]);
			for(int j=smaller; j<=bigger; j++)
				map[j][n1.getIndex()[1]][n1.getIndex()[2]].setColor(1);
		}
	}

	for(int i=0; i<netCoor.size(); i++) { 
		vector<int> currentInd = netCoor[i];
		map[currentInd[0]][currentInd[1]][currentInd[2]].setColor(3);
	}
	
	// add blockage for color patterning
	int new_x = 0;
	int new_y = 0;
	int lay[4] = {1, -1, 0, 0};// same plane 0 1
	for(int i = 0; i < Route.size(); i++){
		for(int j = 0; j < 2; j++){
			if(Route[i].getN1().getLayer() == 0 || Route[i].getN1().getLayer() == 2){
				new_x = Route[i].getN1().getx()+lay[j];
				new_y = Route[i].getN1().gety();
			}
			else{
				new_x = Route[i].getN1().getx();
				new_y = Route[i].getN1().gety() + lay[j];
			}

			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && map[Route[i].getN1().getLayer()][new_x][new_y].getColor() == 0
		   		&& map[Route[i].getN1().getLayer()][Route[i].getN1().getx()][Route[i].getN1().gety()].getColor() != 3)
				map[Route[i].getN1().getLayer()][new_x][new_y].setColor(7);
		}
		for(int j = 0; j < 2; j++){
			if(Route[i].getN2().getLayer() == 0 || Route[i].getN2().getLayer() == 2){
				new_x = Route[i].getN2().getx()+lay[j];
				new_y = Route[i].getN2().gety();
			}
			else{
				new_x = Route[i].getN2().getx();
				new_y = Route[i].getN2().gety() + lay[j];
			}

			if((new_x >= 0) && (new_y >= 0) && (new_x < map[0].size()) && 
		   	   (new_y < map[0][0].size()) && map[Route[i].getN2().getLayer()][new_x][new_y].getColor() == 0
		   		&& map[Route[i].getN2().getLayer()][Route[i].getN2().getx()][Route[i].getN2().gety()].getColor() != 3)
				map[Route[i].getN2().getLayer()][new_x][new_y].setColor(7);
		}
	}
	cout << "MST complete" << endl;
	return Route;
}

int  routing::checkline(int lay,int posi){
	int outcolor=0;
	if(lay==0||lay==2){
		for(int i=0;i<m[lay][posi].size();i++){
			node x1=m[lay][posi][i].getN1();
			node x2=m[lay][posi][i].getN2();
			if(x1.getx()<x2.getx()){
			for(int j=x1.getx();j<x2.getx()+1;j++){
				outcolor=map[lay][j][posi-1].getColor();
				if(outcolor==-1||outcolor==-2)
				 return outcolor;
			}
			}
			else{
			for(int j=x2.getx();j<x1.getx()+1;j++){
			outcolor=map[lay][j][posi-1].getColor();
			if(outcolor==-1||outcolor==-2)
			return outcolor;
			}
			}
		}	
	}
	else if(lay==1||lay==3){
			for(int i=0;i<m[lay][posi].size();i++){
			node x1=m[lay][posi][i].getN1();
			node x2=m[lay][posi][i].getN2();
			if(x1.gety()<x2.gety()){
			for(int j=x1.gety();j<x2.gety()+1;j++){
				outcolor=map[lay][posi-1][j].getColor();
				if(outcolor==-1||outcolor==-2)
				 return outcolor;
			}
			}
			else{
			for(int j=x2.gety();j<x1.gety()+1;j++){
				outcolor=map[lay][posi-1][j].getColor();
				if(outcolor==-1||outcolor==-2)
				 return outcolor;	
			}
			}
	}
	}
	return 0;	
} 
void routing::classifyline(vector<line> &outline){
	for(int i=0;i<outline.size();i++){
		line now=outline[i];
		if(now.getLY()==0){
			node a=now.getN1();
			m[0][a.gety()].push_back(now);
		}
		else if(now.getLY()==1){
			node a=now.getN1();
			m[1][a.getx()].push_back(now);
		}
		else if(now.getLY()==2){
			node a=now.getN1();
			m[2][a.gety()].push_back(now);
		}
		else if(now.getLY()==3){
			node a=now.getN1();
			m[3][a.getx()].push_back(now);
		}
	}
	
}
void routing::ColorPat(){
	double color1=0,color2=0;//total length ofcolor1 and color2 
	bool isline=false;//if there is line ahead of current line (avoid conflict)
	int colorless=-1,colormore=-2;//default draw color1 first(-1)
	int xl=map[0][0].size();//the length of m[0] m[2] 
	int yl=map[0].size();////the length of m[1] m[3]
	double cm[4]={0,0,0,0},cm2[4]={0,0,0,0};
	for(int l=0;l<3;l+=2){//for layer1 and 3
		color1=0;
		color2=0;
		for(int i=0;i<xl;i++){
			if(m[l][i].empty())
				isline=false;
			else if(!m[l][i].empty()){
				if(color1>color2){
					colorless=-2;
					colormore=-1;	
				}
				else{
					colorless=-1;
					colormore=-2;	
				}
				if(!isline){//«e­±¨S¦³½u
					for(int v=0;v<m[l][i].size();v++){
						node x1=m[l][i][v].getN1();
						node x2=m[l][i][v].getN2();
						if(colorless==-1){
							color1+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
							cm[l]+=m[l][i][v].getLENGTH();
						} 
						else{
							color2+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
							cm2[l]+=m[l][i][v].getLENGTH();
						}
						if(x1.getx()<x2.getx()){
							for(int j=x1.getx();j<x2.getx()+1;j++)
								map[l][j][x1.gety()].setColor(colorless);
						}
						else if(x1.getx()>x2.getx()){
							for(int j=x2.getx();j<x1.getx()+1;j++)
								map[l][j][x1.gety()].setColor(colorless);		
						}
					}//end for
				}//end if	!isline
				else if(isline){
					int lastcolor=checkline(l,i);
					for(int v=0;v<m[l][i].size();v++){
						node x1=m[l][i][v].getN1();
						node x2=m[l][i][v].getN2();
						if(lastcolor!=colorless){
							if(colorless==-1){
								color1+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm[l]+=m[l][i][v].getLENGTH();
							} 
							else{
								color2+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm2[l]+=m[l][i][v].getLENGTH();
							}
							if(x1.getx()<x2.getx()){
								for(int j=x1.getx();j<x2.getx()+1;j++)
									map[l][j][x1.gety()].setColor(colorless);
							}
							else if(x1.getx()>x2.getx()){
								for(int j=x2.getx();j<x1.getx()+1;j++)
									map[l][j][x1.gety()].setColor(colorless);		
							}		
						}
						else{	
							if(colormore==-1){
								color1+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm[l]+=m[l][i][v].getLENGTH();
							} 
							else{
								color2+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm2[l]+=m[l][i][v].getLENGTH();
							}
							if(x1.getx()<x2.getx()){
								for(int j=x1.getx();j<x2.getx()+1;j++)
									map[l][j][x1.gety()].setColor(colormore);
							}
							else if(x1.getx()>x2.getx()){
								for(int j=x2.getx();j<x1.getx()+1;j++)
									map[l][j][x1.gety()].setColor(colormore);		
							}
						}
					}
				}
				isline=true;	
			}//end else if  !empty
		} 
	}
	for(int l=1;l<4;l+=2){
		color1=0;
		color2=0;
		for(int i=0;i<yl;i++){
			if(m[l][i].empty())
				isline=false;
			else if(!m[l][i].empty()){
				if(color1>color2){
					colorless=-2;
					colormore=-1;	
				}
				else{
					colorless=-1;
					colormore=-2;	
				}
				if(!isline){//«e­±¨S¦³½u
					for(int v=0;v<m[l][i].size();v++){
						node x1=m[l][i][v].getN1();
						node x2=m[l][i][v].getN2();
						if(colorless==-1){
							color1+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
							cm[l]+=m[l][i][v].getLENGTH();
						} 
						else{
							color2+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
							cm2[l]+=m[l][i][v].getLENGTH();
						}	
						if(x1.gety()<x2.gety()){
							for(int j=x1.gety();j<x2.gety()+1;j++)
								map[l][x1.getx()][j].setColor(colorless);
						}
						else if(x1.gety()>x2.gety()){
							for(int j=x2.gety();j<x1.gety()+1;j++)
								map[l][x1.getx()][j].setColor(colorless);		
						}
					}//end for
				}//end if	!isline
				else if(isline){
					int lastcolor=checkline(l,i);
					for(int v=0;v<m[l][i].size();v++){
						node x1=m[l][i][v].getN1();
						node x2=m[l][i][v].getN2();
						if(lastcolor!=colorless){
							if(colorless==-1){
								color1+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm[l]+=m[l][i][v].getLENGTH();
							} 
							else{
								color2+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm2[l]+=m[l][i][v].getLENGTH();
							}	
							if(x1.gety()<x2.gety()){
								for(int j=x1.gety();j<x2.gety()+1;j++)
									map[l][x1.getx()][j].setColor(colorless);
							}
							else if(x1.gety()>x2.gety()){
								for(int j=x2.gety();j<x1.gety()+1;j++)
									map[l][x1.getx()][j].setColor(colorless);		
							}		
						}
						else{	
							if(colormore==-1){
								color1+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm[l]+=m[l][i][v].getLENGTH();
							} 
							else{
								color2+=m[l][i][v].getLENGTH();//¥[¸ô½uªø
								cm2[l]+=m[l][i][v].getLENGTH();
							}	
							if(x1.gety()<x2.gety()){
								for(int j=x1.gety();j<x2.gety()+1;j++)
									map[l][x1.getx()][j].setColor(colormore);
							}
							else if(x1.gety()>x2.gety()){
								for(int j=x2.gety();j<x1.gety()+1;j++)
									map[l][x1.getx()][j].setColor(colormore);		
							}
						}
					}
				}
				isline=true;// there is line 	
			}//end else if !empty
		} 
	}
	color1=0;
	color2=0;
	//cout<<"Total length : "<<(color1+color2)/2<<endl;
	for(int i=0;i<4;i++){
		cout<<"layer "<<i+1<<" color 1 : "<< cm[i]/2.0<<", color 2 : "<< cm2[i]/2.0<<endl;
		color1+=cm[i];
		color2+=cm2[i];
	}
	cout<<"color 1 : "<<color1/2.0<<endl;
	cout<<"color 2 : "<<color2/2.0<<endl;
	cout<<"Total length : "<<(color1+color2)/2.0<<endl;
}

void routing::createblock(vector<node> b){
	for(int i=0;i<b.size();i+=2){
		int x=b[i].x;
		int xd=b[i+1].x;
		int y=b[i].y;
		int yd=b[i+1].y;	
		for(int i=x;i<xd+1;i++){
			for(int m=0;m<4;m++){
				map[m][i][y].setColor(4);
				map[m][i][yd].setColor(4);
			}
		}		
		for(int j=y;j<yd+1;j++){
			for(int m=0;m<4;m++){
				map[m][x][j].setColor(4);
				map[m][xd][j].setColor(4);
			}
		}		
	}
}
void routing::prt_Map(int map_layer, int origin_x, int origin_y){
	int diff = 25;
	for(int i = origin_y+diff; i >= origin_y; i--){ //print from y=i to y=i+19
        
        if(i <= 9) cout<<i<<"    ";
        else if(i > 9 && i <= 99) cout<<i<<"   ";
        else if(i > 99 && i <= 999) cout<<i<<"  ";
        else cout<<i<<" ";		
        
        for(int j = origin_x; j <= origin_x+diff; j++){//print from x=j to x=j+19
            cout<<map[map_layer][j][i].getColor()<<" ";
        }
        cout<<endl;
    }
    cout<<"     "<<origin_x<<endl;
}

void routing::MakeSet(line *l) {
	l->p = l;
	l->rank = 0;
}

line* routing::FindSet(line *l) {
	if(l != l->p) {
		l->p = FindSet(l->p);
	}
	return l->p;
}

void routing::Union(line *l1, line *l2) {
	line *x = FindSet(l1);
	line *y = FindSet(l2);

	if(x->rank > y->rank) {
		y->p = x;
	}
	else {
		x->p = y;
		if(x->rank == y->rank) 
			y->rank ++;
	}
}

void routing::Checker(vector< vector<line> > allLines, vector< vector< vector<int> > > netPins, vector<node> blockage) {
	// color 
	// 0:white, 1:completed net, 2:running net, 3:pins, 4:blockage
	int colorMap[4][map[0].size()][map[0][0].size()];
	vector< vector<int> > walkThrough;
	int error = 0;
	int setNumber;

//	colorMap.resize(4);
//	for(int i=0; i<4; i++) {
//		colorMap[i].resize(map[0].size());
//		for(int j=0; j<map[0].size(); j++) 
//			colorMap[i][j].resize(map[0][0].size());
//	}

	// color the blockages
	for(int i=0; i<blockage.size(); i+=2) {
		int leftDown[2] = {blockage[i].getx() , blockage[i].gety()};
		int rightTop[2] = {blockage[i+1].getx() , blockage[i+1].gety()};
		for(int j=leftDown[0]; j<=rightTop[0]; j++) {
			for(int k=leftDown[1]; k<=rightTop[1]; k++) {
				for(int l=0; l<4; l++) 
					colorMap[l][j][k] = 4;
			}
		}
	}

	// color the pins
	for(int i=0; i<netPins.size(); i++) {
		for(int j=0; j<netPins[i].size(); j++) {
			colorMap[netPins[i][j][0]][netPins[i][j][1]][netPins[i][j][2]] = 3;
		}
	}

	for(int net=1; net<allLines.size(); net++) {
		for(int i=0; i<allLines[net].size(); i++)
			MakeSet(&allLines[net][i]);
		walkThrough.clear();
		setNumber = allLines[net].size();
		for(int i=0; i<netPins[net].size(); i++) {
			colorMap[netPins[net][i][0]][netPins[net][i][1]][netPins[net][i][2]] = 0;
		}
		for(int i=0; i<allLines[net].size(); i++) {
			vector<int> start = (allLines[net][i].getN1()).getIndex();
			vector<int> end = (allLines[net][i].getN2()).getIndex();

			// check the line color
			if(start[1] != end[1]) { // horizontal
				for(int x=start[1]; x<=end[1]; x++) {
					if(colorMap[start[0]][x][start[2]] != 0 && colorMap[start[0]][x][start[2]] != 2) {
						cout << "color error" << endl;
						error ++;
					}
					colorMap[start[0]][x][start[2]] = 2;
					walkThrough.push_back(map[start[0]][x][start[2]].getIndex());
				}
			}
			else {
				for(int y=start[2]; y<=end[2]; y++) {
					if(colorMap[start[0]][start[1]][y] != 0 && colorMap[start[0]][start[1]][y] != 2) {
						cout << "color error" << endl;
						error ++;
					}
					colorMap[start[0]][start[1]][y] = 2;
					walkThrough.push_back(map[start[0]][start[1]][y].getIndex());
				}
			}

		}

		// check the pins connecting
		for(int i=0; i<netPins[net].size(); i++) {
			bool fail = true;
			for(int l=0; l<4; l++) {
				if(colorMap[l][netPins[net][i][1]][netPins[net][i][2]] = 2)
					fail = false;
			}
			if(fail == true) {
				cout << "net" << net << " pin" << i+1 << " failed !" << endl;
				error ++;
			}
		}

		// color the net to 1
		for(int i=0; i<walkThrough.size(); i++) {
			colorMap[walkThrough[i][0]][walkThrough[i][1]][walkThrough[i][2]] = 1;
		}

	}

	if(error == 0)
		cout << "excellent bro !" << endl;


}

bool routing::checkcolor(){
	int xl=map[0][0].size();
	int yl=map[0].size();
	bool conflict=false;
	int curr;
	for(int l=0;l<3;l+=2)
		for(int i=0;i<xl-1;i++){
			if(m[l][i].empty())
				conflict=false;
			else if(!m[l][i].empty()){
				for(int v=0;v<m[l][i].size();v++){
					node x1=m[l][i][v].getN1();
					node x2=m[l][i][v].getN2();
					if(x1.getx()<x2.getx()){
						if(map[l][x2.getx()+1][x1.gety()].getColor()==-1||map[l][x2.getx()+1][x1.gety()].getColor()==-2){
							//cout<<"layer "<<l<<" "<<x2.getx()<<" . "<<x2.gety()<<endl;
							//return false;
							}
						if(map[l][x1.getx()-1][x1.gety()].getColor()==-1||map[l][x1.getx()-1][x1.gety()].getColor()==-2){
							//cout<<"mm1"<<endl;
							//return false;
							}	
						curr=map[l][x1.getx()][x1.gety()].getColor();	
						for(int j=x1.getx();j<x2.getx()+1;j++)
							if(curr==map[l][j][x1.gety()+1].getColor())
								return false;
					}
					else if(x1.getx()>x2.getx()){
						if(map[l][x2.getx()-1][x1.gety()].getColor()==-1||map[l][x2.getx()-1][x1.gety()].getColor()==-2){
							//cout<<"mm"<<endl;
							//return false;
							}
						if(map[l][x1.getx()+1][x1.gety()].getColor()==-1||map[l][x1.getx()+1][x1.gety()].getColor()==-2){
							//cout<<"mm1"<<endl;
							//return false;
							}	
						curr=map[l][x1.getx()][x1.gety()].getColor();	
						for(int j=x2.getx();j<x1.getx()+1;j++)
							if(curr==map[l][j][x1.gety()+1].getColor())
								return false;		
					}
				}
			}
		}
	for(int l=1;l<4;l+=2)
		for(int i=0;i<yl-1;i++){
			if(m[l][i].empty())
				conflict=false;
			else if(!m[l][i].empty()){
				for(int v=0;v<m[l][i].size();v++){
					node x1=m[l][i][v].getN1();
					node x2=m[l][i][v].getN2();
					if(x1.gety()<x2.gety()){
						if(map[l][x2.getx()][x2.gety()+1].getColor()==-1||map[l][x2.getx()][x2.gety()+1].getColor()==-2){
							//cout<<"yy"<<endl;
							//return false;
							}
						if(map[l][x1.getx()][x1.gety()-1].getColor()==-1||map[l][x1.getx()][x1.gety()-1].getColor()==-2){
							//cout<<"yy1"<<endl;
							//return false;
							}	
						curr=map[l][x1.getx()][x1.gety()].getColor();	
						for(int j=x1.gety();j<x2.gety()+1;j++)
							if(curr==map[l][x1.getx()+1][j].getColor())
								return false;
					}
					else if(x1.gety()>x2.gety()){
						if(map[l][x2.getx()][x1.gety()+1].getColor()==-1||map[l][x2.getx()][x1.gety()+1].getColor()==-2){
							//cout<<"yy"<<endl;
							//return false;
							}
						if(map[l][x1.getx()][x2.gety()-1].getColor()==-1||map[l][x1.getx()][x2.gety()-1].getColor()==-2){
							//cout<<"yy1"<<endl;
							//return false;
							}		
						curr=map[l][x1.getx()][x1.gety()].getColor();	
						for(int j=x2.gety();j<x1.gety()+1;j++)
							if(curr==map[l][x1.getx()+1][j].getColor())
								return false;		
					}
				}
			}
		}			

	return true;				

}

//=============================================
//main

//int main(){
//	routing route;
//	// resize maps
//	int size = 26;
//	for(int i = 0; i < 4; i++){
//		route.map[i].resize(size);
//	}
//	for(int i = 0; i < size; i++){
//		for(int j = 0; j < 4; j++){
//			route.map[j][i].resize(size);
//		}
//	}
//	// input data of pins
//	int pin = 3;
//	route.map[1][10][10].set(1,10,10);
//	route.map[0][25][25].set(0,25,25);
//	route.map[0][10][25].set(0,10,25);
//	route.map[1][20][25].set(1,20,25);
//	route.map[3][12][25].set(3,12,25);
//	route.map[1][10][10].setColor(pin);
//	route.map[0][25][25].setColor(pin);
//	route.map[0][10][25].setColor(pin);
//	route.map[1][20][25].setColor(pin);
//	route.map[3][12][25].setColor(pin);
//	// input data of blockage
//	int block = 4;
//	for(int i = 0; i < 6; i++){
//		for(int j = 15; j < 21; j++){
//			for(int k = 0; k < 4; k++){
//				route.map[k][j][15].setColor(block);
//				route.map[k][j][20].setColor(block);
//			}	
//		}
//		for(int j = 15; j < 21; j++){
//			for(int k = 0; k < 4; k++){
//				route.map[k][15][j].setColor(block);
//				route.map[k][20][j].setColor(block);
//			}
//		}
//	}
//	
//	// set extra blockage for color patterning        
//	// connect the pins of the nets
//	vector<line> l;
//	vector< vector<int> > net;
//	int p1[] = {1,10,10}, p2[] = {0,25,25}, p3[] = {0,10,25}, p4[] = {1,20,25}, p5[] = {3,12,25};
//	net.clear();
//	net.resize(4);
//	net[0].assign(p1,p1+3);
//	net[1].assign(p2,p2+3);
//	net[2].assign(p3,p3+3);
//	net[3].assign(p4,p4+3);
//	//net[4].assign(p5,p5+3);
//
//	l = route.MST(net);
////	l = route.Hadlock(route.map[3][12][25],route.map[0][10][25],0);
//
//	// print maps
//	for(int i = 0; i < 4; i++){
//	}
//	
//		route.prt_Map(i, 0, 0);
//	for(int i=0; i<l.size(); i++) {
//		node n1 = l[i].getN1();
//		node n2 = l[i].getN2();
//		cout << n1.getLayer() << " " << n1.getx() << " " << n1.gety() 
//		     << "  to  " << n2.getLayer() << " " << n2.getx() << " " << n2.gety() << endl;
//	}
//
//	return 0;
//}

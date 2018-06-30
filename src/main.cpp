
//  File       [main.cpp]
//  Author     [Jheng-Ying Yu]
//  Synopsis   [The main program of 2018 Spring Algorithm PA3]
//  Modify     [2018/04/08 Jheng-Ying Yu]
// **************************************************************************

#include <iostream>
#include <fstream>
#include "../lib/tm_usage.h"
#include "Routing.h"
#include <vector>
#include <cstring>
#include <string>
#include <cstdlib>
#include <iomanip>

using namespace std;

void help_message() {
	cout << "usage: Routing <input_file_pin> <input_file_net> <input_file_block> <output_file>" << endl;
}

int main(int argc, char* argv[])
{	
	if(argc != 5) {
		help_message();
		return 0;
	}
	CommonNs::TmUsage tmusg;
	CommonNs::TmStat stat;

	//////////// read the input file /////////////
	fstream finp(argv[1]);
	fstream finn(argv[2]);
	fstream finb(argv[3]);
	fstream fout;
	fout.open(argv[4],ios::out);
	int pinum,blnu,netnu;
	double maxx=0,maxy=0;
	double minx=0,miny=0;   
	vector<node> pinall,pinnew;//pinnew is new coordinate
	vector<node> block;
	routing a; 
	double x,y,z,l,w;
	int mapx,mapy;
	pinall.push_back(node(0,0,0));//let number start from 1
	pinnew.push_back(node(0,0,0));

	a.viaTotal = 0;

	while(finp){
		finp>>z>>l>>x>>y;//f
		if(x>maxx)
			maxx = x;
		if(y>maxy)
			maxy = y;
		if(x<minx)
			minx = x;
		if(y<miny)
			miny = y;
		node  now(l-1,2*x,2*y);
		pinall.push_back(now);
	}
	cout<<"finish pin"<<endl;
	while(finb){
		finb>>x>>y>>z>>w;//f
		node  left(0,2*x-2*minx,2*y-2*miny);
		node  right(0,2*z-2*minx,2*w-2*miny);
		block.push_back(left);
		block.push_back(right);
	}
	cout<<"finish blockage"<<endl;
	for(int i=1;i<pinall.size();i++){
		x=pinall[i].getx()-2*minx;//probability of negative coordinate
		y=pinall[i].gety()-2*miny;
		l=pinall[i].getLayer();
		node cur(l,x,y);
		pinnew.push_back(cur);	
	}
	cout<<"finish pin transform"<<endl;
	mapx=2*maxx-2*minx+20;//+20 for enlarge map
	mapy=2*maxy-2*miny+20;
	cout<<mapx<<","<<mapy<<endl;
	//map initialize  
	for(int i = 0; i < 4; i++){
		a.map[i].resize(mapx);
	}
	cout<<"complish x resize"<<endl;
	for(int i = 0; i < mapx; i++){
		for(int j = 0; j < 4; j++){
			a.map[j][i].resize(mapy);
		}
	}
	a.m[0]=new vector<line>[mapy];
	a.m[1]=new vector<line>[mapx];
	a.m[2]=new vector<line>[mapy];
	a.m[3]=new vector<line>[mapx];
	cout<<"map initialize"<<endl;
	//pin and pin block on map
	for(int i=1;i<pinnew.size();i++){
		int xp=pinnew[i].getx();
		int yp=pinnew[i].gety();
		int lp=pinnew[i].getLayer();
		a.map[lp][xp][yp].set(lp,xp,yp);
		a.map[lp][xp][yp].setColor(3);//pin color
		if(a.map[lp][xp][yp+1].getColor()==0&&(yp+1)<=mapy)
			a.map[lp][xp][yp+1].setColor(5);
		if(a.map[lp][xp][yp-1].getColor()==0&&(yp-1)>=0)
			a.map[lp][xp][yp-1].setColor(5);	
		if(a.map[lp][xp-1][yp].getColor()==0&&(xp-1)>=0)
			a.map[lp][xp-1][yp].setColor(5);	
		if(a.map[lp][xp+1][yp].getColor()==0&&(xp+1)<=mapx)
			a.map[lp][xp+1][yp].setColor(5);
	}
	a.createblock(block);
	a.prt_Map(0,0,0);
	//string s;
	//vector<vector<int> > totalnet;
	vector<vector<int> > crtinet,normnet;//critnet is Y normnet is N
	vector<int> net;
	//int n=10;
	while(finn){
		string s;
		getline(finn,s);
		int l=s.length();
		char c;
		//c=char(s[l-1]);
		//cout<<"this is "<<l<<endl;
		if(s.find("Y")!=string::npos)
			c='Y';
		else
			c='N';
		s=s.substr(0,l-2);
		l=s.length();
		if(s[s.size()-1]==' ')//remove space
			s=s.substr(0,l-1);
		int r,t;
		r=s.find(" "); 
		t=s.find(" ",r+1);
		int k=1;
		net.push_back(atoi(s.substr(0,r).c_str()));
		r=s.find(" "); 
		while(r!=-1){
			net.push_back(atoi(s.substr(r+1,t-r-1).c_str()));
			r=t;
			t=s.find(" ",t+1);
			k++;
		}
		if(k!=1){
			if(c=='Y')
				crtinet.push_back(net);
			else
				normnet.push_back(net);
			net.clear();
		}
	}//end while
	for(int i=0;i<crtinet.size();i++){
		for(int i1=0;i1<crtinet[i].size();i1++)
			cout<<crtinet[i][i1];
		cout<<endl;
	}

	//for(int j=0;j<normnet.size();j++)
	//cout<<normnet[j];
	//cout<<endl;

	/////////////////perform  routing on the nets////

	tmusg.periodStart();
 
	vector< vector<int> > netCoor;
	vector< vector<line> > wires;
	vector< vector< vector<int> > > netPins;
	vector< vector<line> > allLines;
	netPins.clear();
	netPins.resize(crtinet.size() + normnet.size() + 1);
	allLines.clear();
	allLines.resize(crtinet.size() + normnet.size() + 1);
	wires.clear();

	a.netOrdering(crtinet, pinnew);
	a.netOrdering(normnet, pinnew);

	for(int i=0; i<crtinet.size(); i++) {
		netCoor.clear();
		for(int j=1; j<crtinet[i].size(); j++) 
			netCoor.push_back(pinnew[crtinet[i][j]].getIndex());
		for(int k=0; k<netCoor.size(); k++) 
			cout << netCoor[k][0] << " " << netCoor[k][1] << " " << netCoor[k][2] << endl;
		netPins[crtinet[i][0]] = netCoor;

		vector<line> l;
		// if(netCoor.size() == 2) {
		// 	l = a.Hadlock(a.map[netCoor[0][0]][netCoor[0][1]][netCoor[0][2]], a.map[netCoor[1][0]][netCoor[1][1]][netCoor[1][2]], false);
		// }
		// else {
		// 	vector< vector<int> > hananGrid = a.HananGrid(netCoor);
		// 	cout << "originCost = " << a.MSTCost(netCoor) << endl;
		// 	a.addSteiner(netCoor, hananGrid);
		// 	cout << "SteinerCost = " << a.MSTCost(netCoor) << endl;
		// 	l = a.MST(netCoor);
		// }
		l = a.MST(netCoor);
		allLines[crtinet[i][0]] = l;
		a.classifyline(l);
		wires.push_back(l);
	}

	cout << "////////////////////// normnet ///////////////////////" << endl;
	for(int i=0; i<normnet.size(); i++) {
		for(int j=0; j<normnet[i].size(); j++)
			cout << normnet[i][j];
		cout << endl;
	}
	cout << "//////////////////////////////////////////////////////" << endl;

	for(int i=0; i<normnet.size(); i++) {
		netCoor.clear();
		for(int j=1; j<normnet[i].size(); j++) 
			netCoor.push_back(pinnew[normnet[i][j]].getIndex());
		for(int k=0; k<netCoor.size(); k++) 
			cout << netCoor[k][0] << " " << netCoor[k][1] << " " << netCoor[k][2] << endl;
		netPins[normnet[i][0]] = netCoor;

		vector<line> l;
		// if(netCoor.size() == 2) {
		// 	l = a.Hadlock(a.map[netCoor[0][0]][netCoor[0][1]][netCoor[0][2]], a.map[netCoor[1][0]][netCoor[1][1]][netCoor[1][2]], false);
		// }
		// else {
		// 	vector< vector<int> > hananGrid = a.HananGrid(netCoor);
		// 	cout << "originCost = " << a.MSTCost(netCoor) << endl;
		// 	a.addSteiner(netCoor, hananGrid);
		// 	cout << "SteinerCost = " << a.MSTCost(netCoor) << endl;
		// 	l = a.MST(netCoor);
		// }
		l = a.MST(netCoor);
		allLines[normnet[i][0]] = l;
		a.classifyline(l);
		wires.push_back(l);
	}
	a.ColorPat();	
	for(int i=0;i<crtinet.size();i++){
		fout<<"Net "<<crtinet[i][0]<<endl;
		for(int j=0;j<wires[i].size();j++){
			node x1=wires[i][j].getN1();
			node x2=wires[i][j].getN2();
			double xx1=x1.getx();
			double yy1=x1.gety();
			double xx2=x2.getx();
			double yy2=x2.gety();
			if(xx1!=xx2||yy1!=yy2){
				fout<<fixed<<setprecision(1)<<"+ "<<(xx1+2*minx)/2<<" "<<(yy1+2*miny)/2<<" "<<(xx2+2*minx)/2<<" "<<(yy2+2*miny)/2;
				fout<<" "<<wires[i][j].getLY()+1<<" "<<-a.map[wires[i][j].getLY()][x1.getx()][x1.gety()].getColor()<<endl;
			}	
		}
	}	
	for(int i=0;i<normnet.size();i++){
		fout<<"Net "<<normnet[i][0]<<endl;
		for(int j=0;j<wires[i+crtinet.size()].size();j++){
			node x1=wires[i+crtinet.size()][j].getN1();
			node x2=wires[i+crtinet.size()][j].getN2();
			double xx1=x1.getx();
			double yy1=x1.gety();
			double xx2=x2.getx();
			double yy2=x2.gety();
			if(xx1!=xx2||yy1!=yy2){
				fout<<fixed<<setprecision(1)<<"+ "<<(xx1+2*minx)/2<<" "<<(yy1+2*miny)/2<<" "<<(xx2+2*minx)/2<<" "<<(yy2+2*miny)/2;
				fout<<" "<<wires[i+crtinet.size()][j].getLY()+1<<" "<<-a.map[wires[i+crtinet.size()][j].getLY()][x1.getx()][x1.gety()].getColor()<<endl;		       }
		}
	}			

	cout << "Total via = " << a.viaTotal << endl;
	cout << "//////////////////////////////// Checker ////////////////////////////////" << endl;
	cout << endl;
	a.Checker(allLines, netPins, block);
	cout << endl;
	cout << "/////////////////////////////////////////////////////////////////////////" << endl;

	tmusg.getPeriodUsage(stat);

	cout <<"# run time = " << (stat.uTime + stat.sTime) / 1000000.0 << "sec" << endl;
	cout <<"# memory =" << stat.vmPeak / 1000.0 << "MB" << endl;

	return 0;
}

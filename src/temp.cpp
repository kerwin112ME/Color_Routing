

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
			vector< vector<int> > tempCoor;
			tempCoor = netCoor;
			tempCoor.push_back(tempVec);
			tempVec.push_back(originCost - MSTCost(tempCoor));
			if(tempVec[3] > 0)
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

// void B1S(vector< vector<int> > &netCoor, vector< vector<int> > hananGrid) {
// 	vector< vector<int> > oldCoor;
// 	vector< vector<int> > Add;
// 	int oldCost = 0;
// 	while(!hananGrid.empty()) {
// 		Add.clear();
// 		oldCoor.clear();
// 		oldCoor = netCoor;
// 		oldCost = MSTCost(oldCoor);

// 		for(int i=0; i<hananGrid.size(); i++) {
// 			vector< vector<int> > tempCoor = netCoor;
// 			tempCoor.push_back(hananGrid)
// 			if((MSTCost(netCoor)-))
// 		}
// 	}
// }

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
	// for(int i=0; i<4; i++) {
	// 	colorMap[i].resize(map[0].size());
	// 	for(int j=0; j<map[0].size(); j++) 
	// 		colorMap[i][j].resize(map[0][0].size());
	// }

	// color the pins
	for(int i=0; i<netPins.size(); i++) {
		//colorMap[netPins[i][0]][netPins[i][1]][netPins[i][2]] = 3;
		colorMap[0][0][0] = 3;
	}

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
					colorMap[start[0]][x][start[2]] = 1;
				}
			}
			else {
				for(int y=start[2]; y<=end[2]; y++) {
					if(colorMap[start[0]][start[1]][y] != 0 && colorMap[start[0]][start[1]][y] != 2)
						cout << "color error" << endl;
					colorMap[start[0]][x][start[2]] = 1;
					error ++;
				}
			}

		}

		// check the pins connecting
		for(int i=0; i<netPins[net].size(); i++) {
			if(colorMap[netPins[net][i][0]][netPins[net][i][1]][netPins[net][i][2]] != 1) {
				cout << "net" << net << " pin" << i+1 << " failed !" << endl;
				error ++;
			}
		}

	}

	if(error == 0)
		cout << "excellent bro !" << endl;


}


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



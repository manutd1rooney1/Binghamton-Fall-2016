#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Node {
	public:
		Node();
		Node(int nodeID_, string hostname_, int controlPort_, int dataPort_);
		int nodeID;
		string hostName;
		int controlPort;
		int dataPort;
		int packetsSent;
		vector<int> neighbors;
		
		vector<Node*> dataNeighbor;
		vector<vector<int>> linkTable;
		
		void nebularAdd(int neighborID);
		void nebularRemove(int neighborID);
		void outputNode();
};

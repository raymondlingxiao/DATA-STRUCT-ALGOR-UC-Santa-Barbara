#include <cstdlib>
/*
This part is the Graph Structure, I used the 1000 to represent the Broken Edge
The array begins to store information from [1][1]
*/
enum { Broken = 10000 };

class Graph {
private:
	int numberNode;
	int** adjMatrix;
	int getRandUniform() {
		return (rand() % 100) + 1;
	}
	int getRandCost() {
		return (rand() % 221) - 120;
	}
public:
	Graph(int numNodes, int seed);
	Graph() {
		this->numberNode = 0;
	}
	Graph(Graph&g);
	int getNumNodes() { return this->numberNode; }
	int** getAdjMatrix() { return this->adjMatrix; }
	void changeNode(int i, int j, int newValue) {
		this->adjMatrix[i][j] = this->adjMatrix[j][i] = newValue;
	}
};

Graph::Graph(Graph&g) {
	this->numberNode = g.numberNode;
	this->adjMatrix = g.adjMatrix;
}
Graph::Graph(int numNodes, int seed) {
	this->numberNode = numNodes;
	srand(seed);
	this->adjMatrix = new int*[numNodes];
	for (int i = 1; i <= numNodes; i++) {
		this->adjMatrix[i] = new int[numNodes];
	}

	for (int i = 1; i <= numNodes; i++) {
		for (int j = 1; j <= i; j++) {
			this->adjMatrix[j][i] = adjMatrix[i][j] = this->getRandCost();
		}
	}

	for (int i = 1; i <= numNodes; i++) {
		int max = -1337;
		int maxIndex = -1337;
		for (int j = 1; j <= numNodes; j++) {
			if (this->adjMatrix[i][j] > max) {
				max = this->adjMatrix[i][j];
				maxIndex = j;
			}
			if (this->adjMatrix[i][j] <= 0) {
				this->adjMatrix[i][j] = -1337;
			}
		}
		if (max <= 0) {
			this->adjMatrix[i][maxIndex] = this->getRandUniform();
		}
	}

	for (int i = 1; i <= numNodes; i++) {
		for (int j = 1; j <= numNodes; j++) {
			if (this->adjMatrix[i][j] == -1337 || i == j) {
				this->adjMatrix[i][j] = Broken; //To make it be ignored when building MST
			}
		}
	}
}

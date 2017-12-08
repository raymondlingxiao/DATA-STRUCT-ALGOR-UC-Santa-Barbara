#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Graph.h"
using namespace std;
/*
This part is to realise these functions:
(1) Find, used a Parent Array to store the infor of each node's parent
(2) Union, union 2 nodes if they don't have the same parent
(3) Kruscal algorithm based on Union-find
I've used a struct called Edge to store the information about a pair of 2 nodes, which
I thoght could be very convenient to handle in union-find instead of directly making changes
on the Graph Structure.
First I tranfer the Graph structure into pairs of Edges, and compress the 2-d Array to speed up
the process of Union-find and building MST.
The details will be mentioned below
*/
vector<int> DeleteList;
enum { parentSize = 10000000 };
int parent[parentSize];
//Show each node is compromised or not
enum Flag { Compromised, Uncompromised };

struct Computer {
	Flag flag = Uncompromised;
};
//To store the pair of 2 nodes
class Edge {
public:
	int node1, node2, weight;
	//Transfer Graph into Edge
	int transfer(Edge *edge, Graph graph);
	//show missing nodes(Attacked)
	void showMissing();
	//Show the Sum of weights of a Spanning tree
	int LastSpanTree(Edge *edge, Computer*computer, int m);
	//Clear the Parent array
	void UFset();
	//After fix, restore a node
	void Restore(int node, Computer*computer);
	//After attack 
	void Delete(int node, Computer *computer);
	int find(int i);
	void merge(int a, int b);
	//Show two nodes are connected or not
	bool connected(int a, int b, Computer *computer);
	//Show the attacked node is an isolated node or not
	bool singleCon(int a, int m, Computer *computer);
	//Except this attacked node, if other nodes are connected,
	//if it is, we can build a spanning tree
	bool restCon(int a, int m, Computer *computer);
	//Clear the parent array and build new Spanning tree
	void rebuildSpan(Computer *computer, Edge *edge, int edgeNum);
	//Build MST
	void buildMST(int edgeNum, Edge *edge, Computer *computer);
	//Show graph
	void printGraph(Edge* edge, int edgeNum, Computer *computer);
	//Improved version of SingleCon
	bool single(int a, int m, Computer *computer);

};
//To calculate the Edge number based on the number of nodes in the Graph
int calculateEdge(int nodeNum) {
	int total = nodeNum;
	int count = nodeNum;
	while (count != 0) {
		total += (--nodeNum);
		count -= 1;
	}
	return total;
}

int Edge::transfer(Edge *edge, Graph graph) {
	int **adjMatrix = graph.getAdjMatrix();
	int k = 0;
	for (int i = 1; i <= graph.getNumNodes(); i++) {
		for (int j = i; j <= graph.getNumNodes(); j++) { //compress
			edge[k].node1 = i;
			edge[k].node2 = j;
			edge[k].weight = adjMatrix[i][j];
			k++;
		}
	}
	return k;
}

void Edge::showMissing() {

	vector<int>::iterator it;
	cout << "Missing Nodes :\n";
	for (it = DeleteList.begin(); it != DeleteList.end(); it++) {
		cout << " " << *it << " ";
	}
	cout << endl;
}

int Edge::LastSpanTree(Edge *edge, Computer*computer, int m) {//edgeNum
	int SumWeight = 0;
	//edge start from 0
	for (int i = 0; i <m; i++) {
		int w = edge[i].node1;
		int n = edge[i].node2;
		if (computer[w].flag != Compromised&&computer[n].flag != Compromised&&edge[i].weight != Broken) {
			SumWeight += edge[i].weight;
		}
	}
	showMissing();
	printf("Weight of Current Spanning Tree is %d\n", SumWeight);
	return SumWeight;
}

void Edge::UFset() {
	for (int i = 1; i <= parentSize - 1; i++)
		parent[i] = -1;
}

void Edge::Restore(int node, Computer*computer) {
	computer[node].flag = Uncompromised;

	vector<int>::iterator it;
	for (it = DeleteList.begin(); it != DeleteList.end(); ) {
		if (*it == node) {
			it = DeleteList.erase(it++);
		}
		else ++it;
	}
}

void Edge::Delete(int node, Computer *computer) {
	if (computer[node].flag == Compromised)
		return;
	DeleteList.push_back(node);
	computer[node].flag = Compromised;


	//check whether isolated

	//check others whether connected

	//if so, clear parent, then rebuild the graph


}

int Edge::find(int i) {
	int temp;
	//find the parents of nodes
	for (temp = i; parent[temp] >= 0; temp = parent[temp]);
	//compress the path 
	while (temp != i) {
		int t = parent[i];
		parent[i] = temp;
		i = t;
	}
	return temp;
}

void Edge::merge(int a, int b) {
	int r1 = find(a);
	int r2 = find(b);
	int tmp = parent[r1] + parent[r2]; //Sum of node from 2 sets
	if (parent[r1] > parent[r2]) {
		parent[r1] = r2;
		parent[r2] = tmp;
	}
	else {
		parent[r2] = r1;
		parent[r1] = tmp;
	}
}
bool Edge::connected(int a, int b, Computer *computer) {

	if ((computer[a].flag != Compromised&&a != b) && (computer[b].flag != Compromised&&a != b)) {
		return (find(a) == find(b));
	}
	else
		return 0;
}

bool Edge::singleCon(int a, int m, Computer *computer) { //return 1 indicates is a singlepoint
	bool connect = true;
	if (computer[a].flag == Compromised) {
		//printf("This is a Compromised node\n");
		return false;
	}
	for (int i = 1; i <= m; i++) {
		if (computer[m].flag == Compromised)
			connect = true;
		if (i == a)
			continue;
		else
			if (find(i) == find(a)) {
				connect = false;
				return false;
			}
			else
				connect = true;
	}
	return connect;
}

bool Edge::single(int a, int m, Computer *computer) { //return 1 indicates is a singlepoint
	bool connect = true;
	for (int i = 1; i <= m; i++) {
		if (i == a)
			continue;
	    if (find(i) == find(a)) {
				connect = false;
				return false;
			}
			else
				connect = true;
	}
	return connect;
}

bool Edge::restCon(int a, int m, Computer *computer) { //except node a, others
	bool connect = true;
	for (int i = 1; i <= m; i++) {
		if (i == a)
			continue;
		if (singleCon(i, m, computer)) {
			connect = false;
			return 0;
		}
	}
	return connect;
}
void Edge::rebuildSpan(Computer *computer, Edge *edge, int edgeNum) { //no double
	UFset();
	for (int i = 0; i < edgeNum; i++) {
		//cout << i << endl;
		if ((computer[edge[i].node1].flag != Compromised &&
			computer[edge[i].node2].flag != Compromised&&
			edge[i].weight != Broken)) {

			if (find(edge[i].node1) != find(edge[i].node2)) {
				merge(edge[i].node1, edge[i].node2);
			}
		}
	}
	//LastSpanTree(edge, computer, edgeNum);
}

int cmp(const void * a, const void * b) {
	Edge * e1 = (Edge *)a;
	Edge * e2 = (Edge *)b;
	//cout << e1->node1 << " " << e1->node2 << " ";
	return e1->weight - e2->weight;
}
void Edge::buildMST(int edgeNum, Edge *edge, Computer *computer) { //m indicates edgeNum
	int sumWeight = 0;
	int node1, node2;
	UFset();
	qsort(edge, edgeNum, sizeof(Edge), cmp);
	for (int i = 0; i < edgeNum; i++)
	{
		node1 = edge[i].node1;
		node2 = edge[i].node2;
		//After Compromised, donnot union-find it!
		if ((computer[node1].flag != Compromised&&computer[node2].flag != Compromised&&
			edge[i].weight != Broken)) {
			if (find(node1) != find(node2)) { //if two node are not from one set
				sumWeight += edge[i].weight;
				merge(node1, node2); //merge them together
			}
		}
	}
	printf("Weight of MST is %d\n", sumWeight);
}

void Edge::printGraph(Edge* edge, int edgeNum, Computer *computer) {
	for (int i = 0; i < edgeNum; i++) {
		if (computer[edge[i].node1].flag == Compromised || computer[edge[i].node2].flag == Compromised)
			cout << edge[i].node1 << " " << edge[i].node2 << " " << "Broken" << endl;
		if (computer[edge[i].node1].flag != Compromised&&computer[edge[i].node2].flag != Compromised)
			cout << edge[i].node1 << " " << edge[i].node2 << " " << edge[i].weight << endl;
	}
}

//void main() {
//	Graph graph(1000, 1234);
//	int nodeNum = graph.getNumNodes();
//	int edgeNum = calculateEdge(nodeNum);
//	Edge *edge = new Edge[edgeNum];
//	Computer *computer = new Computer[nodeNum * 2];
//
//	edge->transfer(edge, graph);
//
//	edge->UFset();
//
//	edge->rebuildSpan(computer, edge, edgeNum);

	//printGraph(edge, edgeNum,computer);
	//Delete(1, computer);

	//rebuildSpan(computer, edge, edgeNum);
	//printGraph(edge, edgeNum, computer);

	//cout << "**"<<restCon(2, nodeNum, computer) << endl;

	//edge->Delete(2, computer);

	//edge->rebuildSpan(computer, edge, edgeNum);

	////cout << "**" <<restCon(2, edgeNum, computer)<<endl;


	//edge->buildMST(edgeNum, edge, computer);

	//system("pause");




	/*Computer *computer = new Computer[200];
	Edge *edge = new Edge[16];
	for (int i = 0; i<16; i++) {
	scanf_s("%d %d %d", &edge[i].node1, &edge[i].node2, &edge[i].weight);
	}
	//Initial parent
	UFset(100);
	rebuildSpan(4, 16, edge, computer);

	//delete node
	//cout << find(3) << find(1)<<find(2)<<find(4)<<endl;
	if (singleCon(3, 4, computer)) {//if it is a isolated node
	Delete(3, computer);
	rebuildSpan(4, 16, edge, computer);
	if (restCon(3, 4, computer)) {
	buildMST(16, edge, computer);
	}
	else
	printf("No MST\n");
	}
	printf("It is not a partitioned node!\n");
	Delete(3, computer);
	rebuildSpan(4, 16, edge, computer);
	//**********************above are a whole process
	//clear parent
	//UFset(100);
	//rebuild Graph

	//rebuildSpan(4,16 ,edge, computer); //nodeNum(no 2),edgeNum
	//cout << find(3) << find(1) << find(2) << find(4) << endl;
	//rebuildGraph(1, 2, computer, edge, 10);
	//rebuildGraph(2, 1, computer, edge, 10);
	//rebuildGraph(1, 3, computer, edge, 10);
	//rebuildGraph(3, 1, computer, edge, 10);
	//rebuildGraph(3, 4, computer, edge, 10);
	//rebuildGraph(4, 3, computer, edge, 10);
	//rebuildGraph(2, 4, computer, edge, 10);
	//rebuildGraph(4, 2, computer, edge, 10);
	//rebuildGraph(1, 4, computer, edge, 10);
	//rebuildGraph(4, 1, computer, edge, 10);

	//check isolated
	//cout << connected(2,4, computer); //poinnt to point
	cout<<singleCon(2, 4, computer)<<endl; // target, numNode(no double)

	//check others are connected
	cout << restCon(2, 4, computer)<<endl;

	//build MST
	system("pause");

	/*
	1 2 10000
	1 1 10000
	2 2 10000
	3 3 10000
	4 4 10000
	2 1 10000
	1 3 3
	3 1 3
	2 4 10000
	4 2 10000
	3 4 4
	4 3 4
	2 3 10000
	3 2 10000
	1 4 1
	4 1 1

	*/

//}
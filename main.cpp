#include<iostream>  
#include<functional>  
#include<queue>  
#include <random>
#include <iostream>  
#include <stack> 
#include"SpanningTree.h"
/*
Name:Lingxiao Li
Extension ID: X291657
Email:raymondlx@foxmail.com
*/
/*
This part is the logic part, basically it uses the priority queue in STL to suppoprt
the structure of Event queue. Also I use the stack in the STL to support the structure of
the SystemAdmin Fix queue, plus a array to check the membership. Due to the Stack and the
use of look-up array, all the functions are O(1) which fits the requirement perfectly.

But somehow the Edge Structure makes the program run a little slower than direct changing the 2d array
I tested on 1000 nodes, it cost me 5min to complete it. 
But the results are correct if you can use the small number, I would be appreciated it!

*/
using namespace std;
enum { Exist = 1, NotExist =0 };
bool rebuild = NotExist;
enum Action { Attack, Fix,Rebuild };
enum EndCondition { Queue_Empty, Max_Attack };
enum Tag { Occupied, Unoccupied };
int GlobalTime = 0;

struct Sysadmin {
	Tag tag = Unoccupied;
};

struct Event {
	Action action;
	int source;
	int target;
	int ScheduleTime;
	bool operator < (const Event &e) const {
		return ScheduleTime>e.ScheduleTime;//MinTime first
	}
	bool operator > (const Event &e) const {
		return ScheduleTime<e.ScheduleTime;
	}

};

class Simulator {
public:
	int numComputer;
	int numAttacker;
	int numSysAdmin;
	int attackCount;
	int edgeNum;
	Sysadmin *sys;
	Computer *computer;
	Graph graph;
	Edge *edge;
	priority_queue<Event> queue;
	stack<int> sys_stack;
	//number_of_attackers number_of_sysadmins number_of_nodes random_seed
	Simulator(int NumAttacker, int numSysadmin,int numNode,int seed ) {
		srand(seed);
		this->numAttacker = NumAttacker;
		this->numComputer = numNode;
		this->numSysAdmin = numSysadmin;
		this->edgeNum = calculateEdge(numNode);
		this->sys = new Sysadmin[this->numSysAdmin];
		this->graph = Graph(numNode, seed);
		this->edge = new Edge[this->edgeNum];
		this->computer = new Computer[numNode* 2];
		edge->transfer(edge, graph);
		edge->rebuildSpan(computer, edge, edgeNum);
		
	}
	bool checkOccupation();
	int returnSys();
	void run();
	Event fetch();
	void process(Event& e);

	void scheduleRebuild(int target);
	void scheduleAttack(int source);
	void scheduleFix(int source, int target);

	void processRebuild(Event e);
	void processFix(Event e);
	void processAttack(Event e);

	int getFixRandUniform();
	int getTimeRandUniform();
	int getComputerRandUniform(int numComputer);

};
int Simulator::returnSys() {
	for (int i = 0; i < this->numSysAdmin; i++) {
		if (sys[i].tag == Unoccupied)
			return i;
	}
}
bool Simulator::checkOccupation() {
	for (int i = 0; i < this->numSysAdmin; i++) {
		if (sys[i].tag == Unoccupied)
			return true;
	}
	return false;
}
void Simulator::scheduleAttack(int source) {
	Event e;
	e.action = Attack;
	e.source = source;
	e.target = this->getComputerRandUniform(this->numComputer);
	e.ScheduleTime = GlobalTime + this->getTimeRandUniform();
	queue.push(e);
}

void Simulator::processAttack(Event e) {
	GlobalTime = e.ScheduleTime;
	//This is not a real buildSpan,this is used for clear the parent[], build a new graph
	//then check if there exists a spanTree, then build MST
	if (computer[e.target].flag != Compromised) {
		edge->Delete(e.target, computer);
		//edge->rebuildSpan(computer, edge, this->edgeNum);
		this->scheduleAttack(e.source);
		cout << "Attack(" << GlobalTime << ", " << e.source << ", " << e.target << ")" << std::endl;
		attackCount++;
		queue.pop();
		if (checkOccupation()) {
			edge->rebuildSpan(computer, edge, this->edgeNum);
			int Sys = returnSys();
			sys[Sys].tag = Occupied;
			this->scheduleFix(Sys, e.target);
		}
		else
			sys_stack.push(e.target);//else push in wait for vacancy
		if (rebuild == NotExist) {
			if (edge->single(e.target, this->numComputer, this->computer)) {
				//schedule
				edge->rebuildSpan(computer, edge, this->edgeNum);
				this->scheduleRebuild(e.target);
			}		
		}
		//Cannot print out! Too many!
		//else
			//printf("A Rebuild already Exists!\n");
	}
	else {
		edge->rebuildSpan(computer, edge, this->edgeNum);
		this->scheduleAttack(e.source);
		cout << "Attack(" << GlobalTime << ", " << e.source << ", " << e.target << ")" << std::endl;
		attackCount++;
		queue.pop();
	}

}
void Simulator::scheduleRebuild(int target) {
	Event e;
	e.action = Rebuild;
	e.target = target;  //node
	rebuild = Exist; //indicate rebuild exist
	e.ScheduleTime = GlobalTime + 20;
	cout << "ScheduleRebuild(" << e.ScheduleTime << ")" << std::endl;
	queue.push(e);

}

void Simulator::processRebuild(Event e) {
	GlobalTime = e.ScheduleTime;
	//This is not a real buildSpan,this is used for clear the parent[], build a new graph
	//then check if there exists a spanTree, then build MST
	//edge->rebuildSpan(computer, edge, this->edgeNum);
	if (edge->restCon(e.target, this->numComputer, this->computer)) {
		cout<<"...........Building MST..........\n";
		edge->LastSpanTree(edge, computer, this->edgeNum);
		edge->buildMST(this->edgeNum, edge, computer);
		cout<<"...............END...............\n";
	}
	else
		cout<<".......................................................\nThere is no Spanning Tree, Cannot rebuild!\n.......................................................\n";
	rebuild = NotExist;
	queue.pop();
}

void Simulator::scheduleFix(int source, int target) {
	Event e;
	e.action = Fix;
	e.source = source;
	e.target = target;
	e.ScheduleTime = GlobalTime + this->getFixRandUniform();
	queue.push(e);
}

void Simulator::processFix(Event e) {
	GlobalTime = e.ScheduleTime;
	edge->Restore(e.target, computer);
	//Not a real build, just clear the parent array for building a new graph
	edge->rebuildSpan(computer, edge, this->edgeNum);
	sys[e.source].tag = Unoccupied;
	queue.pop();
	cout << "**Fix(" << GlobalTime << ", " << e.source << ", " << e.target << ")" << std::endl;
	//when a admin finishes,if there is computer in the stack, schedule a 
	//fix by itself 
	if (!sys_stack.empty()) {
		int target = sys_stack.top();
		sys[e.source].tag = Occupied;
		this->scheduleFix(e.source, target);
		sys_stack.pop();
	}
	if (rebuild == NotExist) {
		this->scheduleRebuild(e.target);
	}
	//else
		//printf("A Rebuild already Exist!\n");
}

int Simulator::getFixRandUniform() {
	return (rand() % 1001) + 1000;
}

int Simulator::getTimeRandUniform() {
	return (rand() % 901) + 100;
}

int Simulator::getComputerRandUniform(int numComputer) {

	int random = (rand() % numComputer) + 1;

	return random;
}

Event Simulator::fetch() {
	if (attackCount == 2000) throw Max_Attack;
	if (queue.empty()) throw Queue_Empty;
	//********************************
	auto next = queue.top();

	return next;
}
void Simulator::process(Event& e) {
	switch (e.action) {
	case Attack:
		this->processAttack(e);
		break;
	case Fix:
		this->processFix(e);
		break;
	case Rebuild:
		this->processRebuild(e);
		break;
	}
}
void Simulator::run() {
	std::cout << "STARTING SIMULATION" << std::endl;
	for (int i = 1; i <= this->numAttacker; i++) {
		this->scheduleAttack(i);
	}
	//this->scheduleAttack(1);
	try {
		Event fetched;
		while (true) {
			fetched = this->fetch();
			this->process(fetched);
		}
	}
	catch (EndCondition e) {
		switch (e) {
		case Max_Attack:
			std::cout << "2000 Attacks have occured" << std::endl;
			break;
		case Queue_Empty:
			std::cerr << "The queue is empty. This is not intended. Simulation terminating." << std::endl;
			exit(1);
		}
	}
}
int main(int  argc,   char** argv)
{
    //Default value
    int computerNum=20;
    int attackerNum=20;
    int adminNum=20;
    int seed=1234;

    cout<<"#number_of_attackers number_of_sysadmins number_of_nodes random_seed"<<argc<<endl;

    if(argc==5){
        attackerNum=atoi(argv[1]);
        adminNum=atoi(argv[2]);
        computerNum=atoi(argv[3]);
        seed=atoi(argv[4]);
    }
	//number_of_attackers number_of_sysadmins number_of_nodes random_seed
	Simulator simulator(attackerNum,adminNum,computerNum,seed);
	simulator.run();

	system("pause");
}

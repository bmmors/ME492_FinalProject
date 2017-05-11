//Britny Mors
//ME 493 - Autonomy
//Final Project
//Q-learner that learns how to exit a house no matter which room you place it in

#include <iostream>
#include <vector>
#include <assert.h>
#include <time.h>
#include <random>
#include <cmath>
#include <fstream>

using namespace std;
#define BMMRAND (double)rand()/RAND_MAX

//----------Agent Setup----------//
class agent {
public:
	int state;
	int startstate;

	void init();
	void update(int newstate);
	void reset();
};

void agent::update(int newstate) {
	state = newstate;
}

void agent::reset() {
	state = startstate;
}

void agent::init() {
	startstate = rand() % 6;
	while (startstate == 5) {
		startstate = rand() % 5; //make sure agent does not initialize outside of the house
	}
	state = startstate;
}
//----------End Agent Setup----------//

//----------Q-learner Setup----------//
class qlearner {
public:
	vector<vector <int>> rtable;
	vector<vector <double>> qtable;
	double e = 0.1;
	double qval;
	double qmax;
	double reward;
	double alpha = 0.1;
	double gamma = 0.9;
	int oldstate;

	void init(int nroom);
	int decide(int state,int nroom);
	void react(int newstate,int nroom);
};

void qlearner::init(int nroom) {
	for (int i = 0; i < nroom; i++) {
		vector<int> temp;
		for (int j = 0; j < nroom; j++) {
			temp.push_back(0);
		}
		assert(temp.size() == nroom);
		rtable.push_back(temp);
	}

	rtable[0][4] = -1;
	rtable[1][3] = -1;
	rtable[2][3] = -1;
	rtable[3][1] = -1;
	rtable[3][2] = -1;
	rtable[3][4] = -1;
	rtable[4][0] = -1;
	rtable[4][3] = -1;
	rtable[5][1] = -1;
	rtable[5][4] = -1;

	rtable[1][5] = 100;
	rtable[4][5] = 100;
	rtable[5][5] = 100;

	for (int i = 0; i < nroom; i++) {
		vector<double> t;
		for (int j = 0; j < nroom; j++) {
			if (rtable[i][j] == 0) {
				t.push_back(-100000);
			}
			if (rtable[i][j] == -1 || rtable[i][j] == 100) {
				t.push_back(BMMRAND);
			}
		}
		assert(t.size() == nroom);
		qtable.push_back(t);
	}
}

int qlearner::decide(int state,int nroom) {
	//take agents current state
	//choose exploratory vs greedy
	int newstate = 0;
	oldstate = state;
	double rand1 = BMMRAND;
	if (rand1 <= e) {
		//exploratory
		newstate = rand() % 6;
		while (rtable[state][newstate] == 0) {
			newstate = rand() % 6;
		}
		qval = qtable[state][newstate];
	}
	else {
		//greedy
		double temp = qtable[state][0];
		for (int i = 0; i < nroom; i++) {
			if (qtable[state][i] > temp) {
				temp = qtable[state][i];
				newstate = i;
			}
		}
		qval = qtable[state][newstate];
	}
	return  newstate;
}

void qlearner::react(int newstate, int nroom) {
	double t = qtable[newstate][0];
	for (int i = 0; i < nroom; i++) {
		if (qtable[newstate][i] > t) {
			t = qtable[newstate][i];
			qmax = t;
		}
	}

	reward = rtable[oldstate][newstate];
	qtable[oldstate][newstate] = qval + alpha*(reward + (gamma*qmax) - qval);

}
//----------End Q-Learner Setup----------//

int main() {
	int nroom = 6;
	qlearner q;
	q.init(nroom);
	agent a;
	a.init();

	int stat_run = 300;
	int sim = 1000;
	bool goalfound = false;
	int time[1000];
	for (int i = 0; i < sim; i++) {
		time[i] = 0;
	}

	for (int i = 0; i < stat_run; i++) {
		for (int j = 0; j < sim; j++) {
			a.reset();
			goalfound = false;
			while (!goalfound) {
				int select = q.decide(a.state, nroom);
				a.update(select);
				q.react(a.state, nroom);

				if (q.reward == 100) {
					goalfound = true;
				}
				time[j]++;
			}
		}
		q.qtable.clear();
		q.init(nroom);
	}

	ofstream myfile;
	myfile.open("learningcurve.csv");
	myfile.clear();
	for (int i = 0; i < sim; i++) {
		myfile << time[i] << endl;
	}
	myfile.close();
}


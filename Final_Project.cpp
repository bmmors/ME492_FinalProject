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
#include <string>
#include <iterator>
#include <sstream>
#include <algorithm>

using namespace std;
#define BMMRAND (double)rand()/RAND_MAX

//----------Agent Setup----------//
class agent {
public:
	int state;
	int startstate;

	void init(int nroom);
	void update(int newstate);
	void reset();
};

void agent::update(int newstate) {
	state = newstate;
}

void agent::reset() {
	state = startstate;
}

void agent::init(int nroom) {
	startstate = rand() % nroom;
	while (startstate == nroom) {
		startstate = rand() % nroom; //make sure agent does not initialize outside of the house
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

	void init_qtable(int nroom);
	void init_rtable(int nroom);
	int decide(int state,int nroom);
	void react(int newstate,int nroom);
};

void qlearner::init_rtable(int nroom) {
	for (int i = 0; i < nroom; i++) {
		vector<int> tp;
		for (int j = 0; j < nroom; j++) {
			tp.push_back(0);
		}
		assert(tp.size() == nroom);
		rtable.push_back(tp);
	}
	string::size_type sz;
	ifstream infile("rtable.csv");
	string temp;
	for(int i = 0; i < nroom; i++) {
		for (int iterator = 0; iterator < nroom; iterator++) {
			if (iterator < nroom-1) {
				getline(infile, temp, ',');
				//cout << temp << endl;
			}
			else {
				getline(infile, temp, '\n');
				//cout << temp << endl;
			}
			rtable[iterator][i] = stoi(temp, &sz);
		}
	}
	assert(rtable.size() == nroom);


	for (int i = 0; i < nroom; i++) {
		for (int j = 0; j < nroom; j++) {
			cout << rtable.at(i).at(j) << "\t";
		}
		cout << endl;
	}
	


}

void qlearner::init_qtable(int nroom) {
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

	/*for (int i = 0; i < nroom; i++) {
		for (int j = 0; j < nroom; j++) {
			cout << qtable.at(i).at(j) << "\t";
		}
		cout << endl;
	}
	*/
}

int qlearner::decide(int state,int nroom) {
	//take agents current state
	//choose exploratory vs greedy
	int newstate = 0;
	oldstate = state;
	double rand1 = BMMRAND;
	if (rand1 <= e) {
		//exploratory
		newstate = rand() % nroom;
		while (rtable[state][newstate] == 0) {
			newstate = rand() % nroom;
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
		//cout << "state:" << state << "\tnewstate:" << newstate << endl;
		qval = qtable[state][newstate];
		//cout << "qval:" << qval << endl;
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
	int nroom = 13;
	qlearner q;
	q.init_rtable(nroom);
	q.init_qtable(nroom);
	agent a;
	a.init(nroom);

	
	int stat_run = 30;
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
		q.init_qtable(nroom);
	}

	ofstream myfile;
	myfile.open("learningcurve.csv");
	myfile.clear();
	for (int i = 0; i < sim; i++) {
		myfile << time[i] << endl;
	}
	myfile.close();
	
	
}


/*
JONAS MANNO
CS 461
STALLICA HANK
7 MARCH 2021 

i decided to use a graph because i wanted to allow for multiple branches of a node rather than two
this is to input all moves.
i did a graph search algorithm in which i had two data structures hold the frontier
and the explored nodes. if explored, do not expand node and pop off frontier.
if not explored, try all legal moves available + not in explored
also update depth if move from current state is legal (m > c for both sides after move)
*/

#include <iostream>
#include <stdio.h>
#include <vector>
using namespace std;

class node {
public:
	node(int ml, int cl, int b, int mr, int cr, int d) {
		missionariesLeft = ml;
		cannibalsLeft = cl;

		missionariesRight = mr;
		cannibalsRight = cr;

		boatPos = b;

		depth = d;
	}

	bool equals(node* n2) {
		if (missionariesLeft == n2->missionariesLeft &&
			cannibalsLeft == n2->cannibalsLeft &&
			boatPos == n2->boatPos &&
			missionariesRight == n2->missionariesRight &&
			cannibalsRight == n2->cannibalsRight) return true;

		return false;
	}

	vector<node> addChildren() {
		int arrays[5][2] = { {2,0}, {1,0}, {1,1}, {0,1}, {0,2} }; // all possible moves: 2m, 1m, 1m1c, 1c, 2c

		for (int i = 0; i < 5; i++) {
			if (legalMove(arrays[i][0], arrays[i][1])) { 
				children.push_back(newNode(arrays[i][0], arrays[i][1], boatPos, depth));
			}
		}

		return children;
	}

	/*
	ACIONS 
	legal move first checks the boat position of the parent node. 0 = left, 1 = right.
	then checks whether that sides missionaries - the missionaries moving > cannibals - cannibals moving 
	and also if after the move the missionaries on other side + moved missionaries > cannibals + cannibals moved
	action[0] denotes moved missionaries to get to current state - action[1] - cannibals moved = edge 
	*/

	bool legalMove(int mm, int mc) {
		if (action[0] == mm && action[1] == mc) return false;

			if (boatPos == 0) {
				if (missionariesLeft >= mm && cannibalsLeft >= mc) {

					if (missionariesLeft - mm >= cannibalsLeft - mc ||
						missionariesLeft - mm == 0) {

						if (missionariesRight + mm >= cannibalsRight + mc ||
							missionariesRight + mm == 0) {

							return true;
						}
					}
				}
			}

			else if (boatPos == 1) {
				if (missionariesRight >= mm && cannibalsRight >= mc) {

					if (missionariesRight - mm >= cannibalsRight - mc ||
						missionariesRight - mm == 0) {

						if (missionariesLeft + mm >= cannibalsLeft + mc ||
							missionariesLeft + mm == 0) {

							return true;
						}
					}
				}
			}
			
		return false;
	}

	/* SUCCESSOR - ONLY CREATED IF LEGALMOVE == TRUE */
	node newNode(int moveMission, int moveCannibal, int bPos, char dpth) {
		node* child = new node(0, 0, bPos, 0, 0, 0);
		child->action[0] = moveMission;
		child->action[1] = moveCannibal;

		if (bPos == 0) {
			child->missionariesLeft = missionariesLeft - moveMission;
			child->cannibalsLeft = cannibalsLeft - moveCannibal;

			child->missionariesRight = missionariesRight + moveMission;
			child->cannibalsRight = cannibalsRight + moveCannibal;

			child->boatPos = 1;
		}
		else if (bPos == 1) {
			child->missionariesLeft = missionariesLeft + moveMission;
			child->cannibalsLeft = cannibalsLeft + moveCannibal;

			child->missionariesRight = missionariesRight - moveMission;
			child->cannibalsRight = cannibalsRight - moveCannibal;

			child->boatPos = 0;
		}

		child->depth = dpth + 1;
		if (child->missionariesLeft == 3 && child->cannibalsLeft == 3) child->goalState = true;

		return *child;
	}


	int missionariesLeft;
	int missionariesRight;
	int cannibalsLeft;
	int cannibalsRight;
	int boatPos;

	bool goalState = false;
	int depth;
	int action[2] = { -1,-1 };

	vector<node>children;
};

bool iterate(node* r, vector<node>srched) {
	for (vector<node>::iterator it = srched.begin(); it != srched.end(); ++it) {
		node test = *it;
		if (r->equals(&test)) return true;
	}

	return false;
}

void display(vector<node>f) {
	int counter = 0;

	while (counter < f.size()) { // displays graph 
		cout << " m:" << f[counter].missionariesLeft << " c:" << f[counter].cannibalsLeft;
		if (f[counter].boatPos == 0) cout << " BR"; else cout << " RB";
		cout << " m:" << f[counter].missionariesRight << " c:" << f[counter].cannibalsRight <<
			". Depth:" << f[counter].depth << " & Goal State: " << f[counter].goalState << endl;
		counter++;
	}
	cout << endl;

	node temp = *f.begin(); 
	while (temp.goalState == false) { // displays path to goalState
		temp.addChildren();
		temp = temp.children.back();

		if (temp.boatPos == 0) cout << "Moved " << temp.action[0] << " missionaries & " << temp.action[1] << " cannibals left. " << endl;
		else cout << "Moved " << temp.action[0] << " missionaries & " << temp.action[1] << " cannibals right. " << endl;

		cout << "Current state: ";
		cout << " m:" << temp.missionariesLeft << " c:" << temp.cannibalsLeft;
		if (temp.boatPos == 0) cout << " BR"; else cout << " RB";
		cout << " m:" << temp.missionariesRight << " c:" << temp.cannibalsRight <<
			". Goal State: " << temp.goalState << endl;
	}
}

/*
GOAL TEST - IF NOT EXPLORED & NOT GOAL STATE - EXPAND
Uses node as argument, then sets up two vectors: frontier and explored
if the node is not in explored, add to explored then iterate through children
add children onto frontier then repeat until goalstate == true
*/

void graphSearch(node *r) {
	node* newer = r;
	vector<node> frontier, explored;
	frontier.push_back(*r);

	int counter = 0;
	while (counter < frontier.size()) { // if stack not empty
		*r = frontier[counter];
		counter++;

		if (!(iterate(r, explored) && r->goalState == false)) { // & node is not in explored and not goal state
			explored.push_back(*r); // add to explored stack
			r->addChildren();

			for (int i = 0; i < r->children.size(); i++) frontier.push_back(r->children[i]);
		}
	}

	display(frontier);
}

int main() {
	node* root = new node(0, 0, 1, 3, 3, 0); // missionaries left, cannibals left, boat(0=left,1=right) , missionaries right, cannibals right, depth of node 
	graphSearch(root);
}
#include <iostream>
#include <string>
#include "API.h"
#include <queue>

const int ROWS = 8;
const int COLUMNS = 8;

// int ptr = 0;
// int backPtr = 0;
// int fwdPtr;

struct coordinate{
	int y;
	int x;
};

static coordinate XY;
static coordinate XY_prev;

static coordinate dumXY;
static coordinate dumXY_prev;
static int dumOrient;

static int ptr;
static int backPtr;
static int fwdPtr;
static char back_path[ROWS*COLUMNS];
static char fwd_path[ROWS*COLUMNS];
static char direction;

struct surroundCoor {
	struct coordinate N;
	struct coordinate S;
	struct coordinate W;
	struct coordinate E;
};

static bool L = false;
static bool R = false;
static bool F = false;


// array 16x16


void log(const std::string& text) {
    std::cerr << text << std::endl;
}

int flood[ROWS][COLUMNS]={
    {6, 5, 4, 3, 3, 4, 5, 6},
    {5, 4, 3, 2, 2, 3, 4, 5},
    {4, 3, 2, 1, 1, 2, 3, 4},
    {3, 2, 1, 0, 0, 1, 2, 3},
    {3, 2, 1, 0, 0, 1, 2, 3},
    {4, 3, 2, 1, 1, 2, 3, 4},
    {5, 4, 3, 2, 2, 3, 4, 5},
    {6, 5, 4, 3, 3, 4, 5, 6}
}
;

int cells[ROWS][COLUMNS]={
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1}
}
;

int backFlood[ROWS][COLUMNS]={
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1},
    {-1,-1,-1,-1,-1,-1,-1,-1}
}
;


void updateWalls(struct coordinate point, int orient, bool L, bool R, bool F) {
	if ((L && R) && F) {
		if (orient == 0) {
			cells[point.y][point.x] = 13;
		}            //|-|
		else if (orient == 1) {
			cells[point.y][point.x] = 12;
		}            //_-|
		else if (orient == 2) {
			cells[point.y][point.x] = 11;
		}            //|_|
		else if (orient == 3) {
			cells[point.y][point.x] = 14;
		}            //|-_
	} else if ((L && R) && (!F)) {
		if (orient == 0) {
			cells[point.y][point.x] = 9;
		}            //| |
		else if (orient == 1) {
			cells[point.y][point.x] = 10;
		}            //_-
		else if (orient == 2) {
			cells[point.y][point.x] = 9;
		}            //| |
		else if (orient == 3) {
			cells[point.y][point.x] = 10;
		}            //_-
	} else if ((L && F) && (!R)) {
		if (orient == 0) {
			cells[point.y][point.x] = 8;
		}            //|-
		else if (orient == 1) {
			cells[point.y][point.x] = 7;
		}            //-|
		else if (orient == 2) {
			cells[point.y][point.x] = 6;
		}            //_|
		else if (orient == 3) {
			cells[point.y][point.x] = 5;
		}            //|_
	} else if ((R && F) && (!L)) {
		if (orient == 0) {
			cells[point.y][point.x] = 7;
		}            //-|
		else if (orient == 1) {
			cells[point.y][point.x] = 6;
		}            //_|
		else if (orient == 2) {
			cells[point.y][point.x] = 5;
		}            //|_
		else if (orient == 3) {
			cells[point.y][point.x] = 8;
		}            //|-
	} else if (F) {
		if (orient == 0) {
			cells[point.y][point.x] = 2;
		}            //-
		else if (orient == 1) {
			cells[point.y][point.x] = 3;
		}            // |
		else if (orient == 2) {
			cells[point.y][point.x] = 4;
		}            //_
		else if (orient == 3) {
			cells[point.y][point.x] = 1;
		}            //|
	} else if (L) {
		if (orient == 0) {
			cells[point.y][point.x] = 1;
		}            //|
		else if (orient == 1) {
			cells[point.y][point.x] = 2;
		}            //-
		else if (orient == 2) {
			cells[point.y][point.x] = 3;
		}            // |
		else if (orient == 3) {
			cells[point.y][point.x] = 4;
		}            //_
	} else if (R) {
		if (orient == 0) {
			cells[point.y][point.x] = 3;
		}            // |
		else if (orient == 1) {
			cells[point.y][point.x] = 4;
		}            //_
		else if (orient == 2) {
			cells[point.y][point.x] = 1;
		}            //|
		else if (orient == 3) {
			cells[point.y][point.x] = 2;
		}            //-
	} else {
		cells[point.y][point.x] = 0;            //
	}
}

bool isAccessible(struct coordinate p, struct coordinate p1) {
	if (p1.x < 0 || p1.y < 0 || p1.x >=ROWS  || p1.y >= COLUMNS) {
		return false;
	}

	if (p.x == p1.x) {
		if (p.y > p1.y) {
			if (cells[p.y][p.x] == 4 || cells[p.y][p.x] == 5
					|| cells[p.y][p.x] == 6 || cells[p.y][p.x] == 10
					|| cells[p.y][p.x] == 11 || cells[p.y][p.x] == 12
					|| cells[p.y][p.x] == 14) {
				return false;
			}

			else {
				return true;
			}
		} else {
			if (cells[p.y][p.x] == 2 || cells[p.y][p.x] == 7
					|| cells[p.y][p.x] == 8 || cells[p.y][p.x] == 10
					|| cells[p.y][p.x] == 12 || cells[p.y][p.x] == 13
					|| cells[p.y][p.x] == 14) {
				return false;
			}

			else {
				return true;
			}

		}
	} else if (p.y == p1.y) {
		if (p.x > p1.x) {
			if (cells[p.y][p.x] == 1 || cells[p.y][p.x] == 5
					|| cells[p.y][p.x] == 8 || cells[p.y][p.x] == 9
					|| cells[p.y][p.x] == 11 || cells[p.y][p.x] == 13
					|| cells[p.y][p.x] == 14) {
				return false;
			} else {
				return true;
			}
		} else {
			if (cells[p.y][p.x] == 3 || cells[p.y][p.x] == 6
					|| cells[p.y][p.x] == 7 || cells[p.y][p.x] == 9
					|| cells[p.y][p.x] == 11 || cells[p.y][p.x] == 12
					|| cells[p.y][p.x] == 13) {
				return false;
			} else {
				return true;
			}
		}
	} else {
		return false;
	}
}

struct surroundCoor getSurrounds(struct coordinate p) {
	struct surroundCoor surCoor;
	surCoor.N.x = p.x;
	surCoor.N.y = p.y + 1;

	surCoor.S.x = p.x;
	surCoor.S.y = p.y - 1;

	surCoor.W.x = p.x - 1;
	surCoor.W.y = p.y;

	surCoor.E.x = p.x + 1;
	surCoor.E.y = p.y;

	if (surCoor.N.x >= ROWS) {
		surCoor.N.x = -1;
	}
	if (surCoor.W.y >= COLUMNS) {
		surCoor.W.y = -1;
	}

	return surCoor;
}

bool isConsistant(struct coordinate p) {
	struct surroundCoor surr = getSurrounds(p);
	int minVals[4]={-1,-1,-1,-1};
	if (surr.N.x >= 0 && surr.N.y >= 0) {
		if (isAccessible(p, surr.N)) {
			minVals[0] = flood[surr.N.y][surr.N.x];
		}
	}
	if (surr.E.x >= 0 && surr.E.y >= 0) {
		if (isAccessible(p, surr.E)) {
			minVals[1] = flood[surr.E.y][surr.E.x];
		}
	}
	if (surr.S.x >= 0 && surr.S.y >= 0) {
		if (isAccessible(p, surr.S)) {
			minVals[2] = flood[surr.S.y][surr.S.x];
		}
	}
	if (surr.W.x >= 0 && surr.W.y >= 0) {
		if (isAccessible(p, surr.W)) {
			minVals[3] = flood[surr.W.y][surr.W.x];
		}
	}
	int val = flood[p.y][p.x];
	int minCount = 0;
	for (int i = 0; i < 4; i++) {
		if (minVals[i] == val - 1 && minVals[i] != -1) {
			minCount++;
		}
	}

	if (minCount > 0 || val == 0) {
		return true;
	} else {
		return false;
	}

}

void makeConsistant(struct coordinate p) {
	struct surroundCoor surr = getSurrounds(p);
	int minVals[4]={-1,-1,-1,-1};
	if (surr.N.y >= 0 && surr.N.x >= 0) {
		if (isAccessible(p, surr.N)) {
			minVals[0] = flood[surr.N.y][surr.N.x];
		}
	}
	if (surr.E.y >= 0 && surr.E.x >= 0) {
		if (isAccessible(p, surr.E)) {
			minVals[1] = flood[surr.E.y][surr.E.x];
		}
	}
	if (surr.S.y >= 0 && surr.S.x >= 0) {
		if (isAccessible(p, surr.S)) {
			minVals[2] = flood[surr.S.y][surr.S.x];
		}
	}
	if (surr.W.y >= 0 && surr.W.x >= 0) {
		if (isAccessible(p, surr.W)) {
			minVals[3] = flood[surr.W.y][surr.W.x];
		}
	}
	int minimum = 1000;
	for (int i = 0; i < 4; i++) {
		if (minVals[i] == -1) {
			minVals[i] = 1000;
		}
		if (minVals[i] < minimum) {
			minimum = minVals[i];
		}
	}
	flood[p.y][p.x] = minimum + 1;
}

void floodFill(struct coordinate p, struct coordinate prev) {
	if (!isConsistant(p)) {
		flood[p.y][p.x] = flood[prev.y][prev.x] + 1;
	}

	std::queue<coordinate> q;
	q.push(p);

    struct surroundCoor surr = getSurrounds(p);
    if (surr.N.x >= 0 && surr.N.y >= 0) {
        if (isAccessible(p, surr.N)) {
            q.push(surr.N);
        }
    }
    if (surr.E.x >= 0 && surr.E.y >= 0) {
        if (isAccessible(p, surr.E)) {
            q.push(surr.E);
        }
    }
    if (surr.S.x >= 0 && surr.S.y >= 0) {
        if (isAccessible(p, surr.S)) {
            q.push(surr.S);
        }
    }
    if (surr.W.x >= 0 && surr.W.y >= 0) {
        if (isAccessible(p, surr.W)) {
            q.push(surr.W);
        }
    }

	while (!q.empty()) {
		struct coordinate crun = q.front();
		q.pop();
		if (isConsistant(crun)) {

		} else {
			makeConsistant(crun);
			q.push(crun);
			struct surroundCoor surr = getSurrounds(crun);
			if (surr.N.y >= 0 && surr.N.x >= 0) {
				if (isAccessible(crun, surr.N)) {
					q.push(surr.N);
				}
			}
			if (surr.E.y >= 0 && surr.E.x >= 0) {
				if (isAccessible(crun, surr.E)) {
					q.push(surr.E);
				}
			}
			if (surr.S.y >= 0 && surr.S.x >= 0) {
				if (isAccessible(crun, surr.S)) {
					q.push(surr.S);
				}
			}
			if (surr.W.y >= 0 && surr.W.x >= 0) {
				if (isAccessible(crun, surr.W)) {
					q.push(surr.W);
				}
			}
		}
	}

}

char toMove(struct coordinate p, struct coordinate prevPos, int orient) {
	struct surroundCoor surr = getSurrounds(p);

//	int val = flood[p.x][p.y];
	int minVals[4] = { 1000, 1000, 1000, 1000 };
	int prevDir = 0;
	if (surr.N.y >= 0 && surr.N.x >= 0) {
		if (isAccessible(p, surr.N)) {
			minVals[0] = flood[surr.N.y][surr.N.x];
			if (surr.N.x == prevPos.x && surr.N.y == prevPos.y) {
				prevDir = 0;
			}
		}
	}
	if (surr.E.y >= 0 && surr.E.x >= 0) {
		if (isAccessible(p, surr.E)) {
			minVals[1] = flood[surr.E.y][surr.E.x];
			if (surr.E.x == prevPos.x && surr.E.y == prevPos.y) {
				prevDir = 1;
			}
		}
	}
	if (surr.S.y >= 0 && surr.S.x >= 0) {
		if (isAccessible(p, surr.S)) {
			minVals[2] = flood[surr.S.y][surr.S.x];
			if (surr.S.x == prevPos.x && surr.S.y == prevPos.y) {
				prevDir = 2;
			}
		}
	}
	if (surr.W.y >= 0 && surr.W.x >= 0) {
		if (isAccessible(p, surr.W)) {
			minVals[3] = flood[surr.W.y][surr.W.x];
			if (surr.W.x == prevPos.x && surr.W.y == prevPos.y) {
				prevDir = 3;
			}
		}
	}

	int minimum = 1000;
	int noMovements = 0;
	for (int i = 0; i < 4; i++) {
		if (minVals[i] != 1000) {
			noMovements++;
		}
	}

	int minCell = 0;
	for (int i = 0; i < 4; i++) {
		if (minVals[i] < minimum) {
			if (noMovements == 1) {
				minimum = minVals[i];
				minCell = i;
			} else {
				if (i == prevDir) {

				} else {
					minimum = minVals[i];
					minCell = i;
				}
			}

		}
	}


    using namespace std;
    string line ="";
    for(int b = 0; b < 4; b++)
    {
        line = line +  to_string(minVals[b])+"\t";
    }
    string minCellS = to_string(minCell);


	if (minCell == orient) {
		return 'F';
	} else if (minCell == orient - 1 || minCell == orient + 3) {
		return 'L';
	} else if (minCell == orient + 1 || minCell == orient - 3) {
		return 'R';
	} else {
		return 'B';
	}

}

char toMoveBack(struct coordinate p, struct coordinate prevPos, int orient) {
	struct surroundCoor surr = getSurrounds(p);
//	int val = backFlood[p.x][p.y];
	int minVals[4] = { 1000, 1000, 1000, 1000 };

	if (surr.N.y >= 0 && surr.N.x >= 0) {
		if (isAccessible(p, surr.N)) {
			minVals[0] = backFlood[surr.N.y][surr.N.x];
		}
	}
	if (surr.E.y >= 0 && surr.E.x >= 0) {
		if (isAccessible(p, surr.E)) {
			minVals[1] = backFlood[surr.E.y][surr.E.x];
		}
	}
	if (surr.S.y >= 0 && surr.S.x >= 0) {
		if (isAccessible(p, surr.S)) {
			minVals[2] = backFlood[surr.S.y][surr.S.x];
		}
	}
	if (surr.W.y >= 0 && surr.W.x >= 0) {
		if (isAccessible(p, surr.W)) {
			minVals[3] = backFlood[surr.W.y][surr.W.x];
		}
	}

	int minimum = 1000;
	int minCell = 0;
	for (int i = 0; i < 4; i++) {
		if ((minVals[i] != 1000 && minVals[i] != -1 ) && minVals[i] < minimum) {
			minimum = minVals[i];
			minCell = i;
		}
	}

	if (minCell == orient) {
		return 'F';
	} else if (minCell == orient - 1 || minCell == orient + 3) {
		return 'L';
	} else if (minCell == orient + 1 || minCell == orient - 3) {
		return 'R';
	} else {
		return 'B';
	}
}


char toMoveFastForward(struct coordinate p, struct coordinate prevPos, int orient) {
	struct surroundCoor surr = getSurrounds(p);
//	int val = backFlood[p.x][p.y];
	int minVals[4] = { 1000, 1000, 1000, 1000 };

	if (surr.N.y >= 0 && surr.N.x >= 0) {
		if (isAccessible(p, surr.N)) {
			minVals[0] = backFlood[surr.N.y][surr.N.x];
		}
	}
	if (surr.E.y >= 0 && surr.E.x >= 0) {
		if (isAccessible(p, surr.E)) {
			minVals[1] = backFlood[surr.E.y][surr.E.x];
		}
	}
	if (surr.S.y >= 0 && surr.S.x >= 0) {
		if (isAccessible(p, surr.S)) {
			minVals[2] = backFlood[surr.S.y][surr.S.x];
		}
	}
	if (surr.W.y >= 0 && surr.W.x >= 0) {
		if (isAccessible(p, surr.W)) {
			minVals[3] = backFlood[surr.W.y][surr.W.x];
		}
	}

	int minimum = 1000;
	int minCell = 0;
	for (int i = 0; i < 4; i++) {
		if ((minVals[i] != 1000 && minVals[i] != -1 ) && minVals[i] < minimum) {
			minimum = minVals[i];
			minCell = i;
		}
	}

	if (minCell == orient) {
		return 'F';
	} else if (minCell == orient - 1 || minCell == orient + 3) {
		return 'L';
	} else if (minCell == orient + 1 || minCell == orient - 3) {
		return 'R';
	} else {
		return 'B';
	}
}

void backtrack() {
	struct coordinate p;
	p.y = 0;
	p.x = 0;
	int distance = 1;

	backFlood[0][0] = 0;
	std::queue<coordinate> q;

	q.push(p);

	while (!q.empty()) {
		std::queue<coordinate> temq;

		int br = 0;

		while (!q.empty()) {
			p = q.front();
			q.pop();

			if (flood[p.y][p.x] == 0) {
				br = 1;
				break;
			}
			struct surroundCoor surr = getSurrounds(p);

			if (surr.N.y >= 0 && surr.N.x >= 0 && cells[surr.N.y][surr.N.x] >= 0
					&& backFlood[surr.N.y][surr.N.x] == -1) {
				if (isAccessible(p, surr.N)) {
					temq.push(surr.N);
					backFlood[surr.N.y][surr.N.x] = distance;
				}
			}
			if (surr.E.y >= 0 && surr.E.x >= 0 && cells[surr.E.y][surr.E.x] >= 0
					&& backFlood[surr.E.y][surr.E.x] == -1) {
				if (isAccessible(p, surr.E)) {
					temq.push(surr.E);
					backFlood[surr.E.y][surr.E.x] = distance;
				}
			}
			if (surr.S.y >= 0 && surr.S.x >= 0 && cells[surr.S.y][surr.S.x] >= 0
					&& backFlood[surr.S.y][surr.S.x] == -1) {
				if (isAccessible(p, surr.S)) {
					temq.push(surr.S);
					backFlood[surr.S.y][surr.S.x] = distance;
				}
			}
			if (surr.W.y >= 0 && surr.W.x >= 0 && cells[surr.W.y][surr.W.x] >= 0
					&& backFlood[surr.W.y][surr.W.x] == -1) {
				if (isAccessible(p, surr.W)) {
					temq.push(surr.W);
					backFlood[surr.W.y][surr.W.x] = distance;
				}
			}
		}

		if (br) {
			break;
		}
		q = temq;
		distance += 1;
	}

}

int orientation(int orient, char turning) {
	if (turning == 'L') {
		orient -= 1;
		if (orient == -1) {
			orient = 3;
		}
	}

	else if (turning == 'R') {
		orient += 1;
		if (orient == 4) {
			orient = 0;
		}
	}

	else if (turning == 'B') {
		if (orient == 0) {
			orient = 2;
		} else if (orient == 1) {
			orient = 3;
		} else if (orient == 2) {
			orient = 0;
		} else if (orient == 3) {
			orient = 1;
		}
	}
	return orient;
}

struct coordinate updateCoordinates(struct coordinate coordi, int orient) {
	if (orient == 0) {
		coordi.y += 1;
	}
	if (orient == 1) {
		coordi.x += 1;
	}
	if (orient == 2) {
		coordi.y -= 1;
	}
	if (orient == 3) {
		coordi.x -= 1;
	}
	return coordi;
}

void forwardtrack(struct coordinate dumXY,struct coordinate dumXY_prev, int dumOrient){
	while(1){
		if (backFlood[dumXY.y][dumXY.x]==0){
			ptr -= 1;
			fwdPtr = ptr;
			// log("breaking");
			break;
		}
		else{
			direction= toMoveBack(dumXY, dumXY_prev,dumOrient);
			back_path[ptr] = direction;
			if (direction == 'L'){
				fwd_path[ptr] = 'R';
			}
			else if (direction == 'R'){
				fwd_path[ptr] = 'L';
			}
			else{
				fwd_path[ptr] = direction;
			}

			dumOrient = orientation(dumOrient, direction);
			dumXY_prev = dumXY;
			dumXY = updateCoordinates(dumXY, dumOrient);

			ptr+=1;
		}
	}

}

void showFlood(struct coordinate p) {
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            API::setText(x, y, std::to_string(flood[y][x]));
        }
    }
}

void colorWalls(){
	if (cells[XY.y][XY.x] == 14) {
			API::setWall(XY.x, XY.y, 'n');
			API::setWall(XY.x, XY.y, 's');
			API::setWall(XY.x, XY.y, 'w');
		}
		else if (cells[XY.y][XY.x] == 13)
		{
			API::setWall(XY.x, XY.y, 'n');
			API::setWall(XY.x, XY.y, 'e');
			API::setWall(XY.x, XY.y, 'w');
		}
		else if (cells[XY.y][XY.x] == 12)
		{
			API::setWall(XY.x, XY.y, 'n');
			API::setWall(XY.x, XY.y, 's');
			API::setWall(XY.x, XY.y, 'e');
		}
		else if (cells[XY.y][XY.x] == 11)
		{
			API::setWall(XY.x, XY.y, 'w');
			API::setWall(XY.x, XY.y, 's');
			API::setWall(XY.x, XY.y, 'e');
		}
		else if (cells[XY.y][XY.x] == 10)
		{
			API::setWall(XY.x, XY.y, 'n');
			API::setWall(XY.x, XY.y, 's');
		}
		else if (cells[XY.y][XY.x] == 9)
		{
			API::setWall(XY.x, XY.y, 'e');
			API::setWall(XY.x, XY.y, 'w');
		}
		else if (cells[XY.y][XY.x] == 8)
		{
			API::setWall(XY.x, XY.y, 'n');
			API::setWall(XY.x, XY.y, 'w');
		}
		else if (cells[XY.y][XY.x] == 7)
		{
			API::setWall(XY.x, XY.y, 'n');
			API::setWall(XY.x, XY.y, 'e');
		}
		else if (cells[XY.y][XY.x] == 6)
		{
			API::setWall(XY.x, XY.y, 'e');
			API::setWall(XY.x, XY.y, 's');
		}
		else if (cells[XY.y][XY.x] == 5)
		{
			API::setWall(XY.x, XY.y, 'w');
			API::setWall(XY.x, XY.y, 's');
		}
		else if (cells[XY.y][XY.x] == 4)
		{
			API::setWall(XY.x, XY.y, 's');
		}
		else if (cells[XY.y][XY.x] == 3)
		{
			API::setWall(XY.x, XY.y, 'e');
		}
		else if (cells[XY.y][XY.x] == 2)
		{
			API::setWall(XY.x, XY.y, 'n');
		}
		else if (cells[XY.y][XY.x] == 1)
		{
			API::setWall(XY.x, XY.y, 'w');
		}
}

void getSensorReadings(){
    L = API::wallLeft();
    R = API::wallRight();
    F = API::wallFront();
}


int main() {

	XY.x = 0;
	XY.y = 0;
	XY_prev.x = 0;
	XY_prev.y = 0;
	int orient = 0;
    int runState = 1;
    int mouseState = 2;


    // int x = 0;
    // int y = 0;
    // int xprev = 0;
    // int yprev = 0;
    // int orient = 0;

    while (true) {

        switch (mouseState)
        {
        case 2:

            if (runState == 0) { 
                // Starting from the (0,0) position and go to the first cell

            } else if (runState == 1) { 
                // Decision taking whether to run forward floodfill or backward
                getSensorReadings();
                updateWalls(XY, orient, L, R, F);
                colorWalls();
            
                if (flood[XY.y][XY.x] != 0) { //not at the center
                    floodFill(XY, XY_prev);
                } else { //at the center
				
                    backtrack();

					dumXY.x = XY.x;
					dumXY.y = XY.y;
					dumXY_prev.x = XY_prev.x;
					dumXY_prev.y = XY_prev.y;
					dumOrient = orient;

					forwardtrack(dumXY, dumXY_prev, dumOrient);

					mouseState = 3;
					runState = 1;
					backPtr = 0;
                }                       

                runState = 3;

            } else if (runState == 2) { 
                // Moving to the center of the cell in front
                // Logging and moving forward
                log("moveForward");
                showFlood(XY);
                API::moveForward();

                runState = 4;

            } else if (runState == 3) { 

                // Call the function to determine the next move
                direction = toMove(XY, XY_prev, orient);

                // Turning
                if (direction == 'L') {
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                } else if (direction == 'R') {
                    API::turnRight();
                    orient = orientation(orient, 'R');
                } else if (direction == 'B') {
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                }          

                runState = 2;


            } else if (runState == 4) { 
                // Move to edge and updating the coordinates

                // Update previous coordinates and the current ones
                XY_prev = XY;
                std::pair<int, int> newCoordinates = API::updateCoordinates(XY.x, XY.y, orient);
                XY.x = newCoordinates.first;
                XY.y = newCoordinates.second;

                runState = 1;


            } else if (runState == 5) { 
                // Front alignment

            }

            break;

		case 3:

			if (runState == 0){ //Stop the robot
				mouseState = 2;
				runState = 1;
				
			}

			else if (runState == 1){
				getSensorReadings();

				if (backFlood[XY.y][XY.x] == 0) //if the robot is at the starting point
				{
					backPtr = 0;
					runState = 0;
				}
				else // robot is not at the starting point
				{
					direction = back_path[backPtr];
					backPtr += 1;
					runState = 3;
				}

			}

			else if (runState == 2){
				// Moving to the center of the cell in front
                // Logging and moving forward
                log("moveForward");
                showFlood(XY);
                API::moveForward();

				runState = 4;
			}

			else if(runState == 3){

				// Call the function to determine the next move
                direction = toMoveBack(XY, XY_prev, orient);

				// Turning
                if (direction == 'L') {
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                } else if (direction == 'R') {
                    API::turnRight();
                    orient = orientation(orient, 'R');
                } else if (direction == 'B') {
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                }
				runState = 2;          
			}else if (runState == 4) { 
                // Move to edge and updating the coordinates

                // Update previous coordinates and the current ones
                XY_prev = XY;
                std::pair<int, int> newCoordinates = API::updateCoordinates(XY.x, XY.y, orient);
                XY.x = newCoordinates.first;
                XY.y = newCoordinates.second;

                runState = 1;


            } else if (runState == 5) { 
                // Front alignment

            }

			break;

		case 4: //Fast Idle -- Here the state should go to the fastforward state when a button is pressed

			break;

		case 5: //Fast Forward
			
			if (runState == 0){ //Stop the robot
				
				runState = 1;
				
			}

			else if (runState == 1){
				getSensorReadings();

				if (flood[XY.y][XY.x] >= 1)
				{
					direction = fwd_path[fwdPtr];
					fwdPtr -= 1;
					runState = 2;
				}
				else
				{			
					fwdPtr = ptr;
					mouseState = 6;
					runState = 1;
					backPtr = 0;
				}
				break;

			}

			else if (runState == 2){
				// Moving to the center of the cell in front
                // Logging and moving forward
                log("moveForward");
                showFlood(XY);
                API::moveForward();

				runState = 4;
			}

			else if(runState == 3){

				// Call the function to determine the next move
                direction = toMoveBack(XY, XY_prev, orient);

				// Turning
                if (direction == 'L') {
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                } else if (direction == 'R') {
                    API::turnRight();
                    orient = orientation(orient, 'R');
                } else if (direction == 'B') {
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                    API::turnLeft();
                    orient = orientation(orient, 'L');
                }
				runState = 2;          
			}else if (runState == 4) { 
                // Move to edge and updating the coordinates

                // Update previous coordinates and the current ones
                XY_prev = XY;
                std::pair<int, int> newCoordinates = API::updateCoordinates(XY.x, XY.y, orient);
                XY.x = newCoordinates.first;
                XY.y = newCoordinates.second;

                runState = 1;


            } else if (runState == 5) { 
                // Front alignment

            }


			} 

		


}

    return 0;
}

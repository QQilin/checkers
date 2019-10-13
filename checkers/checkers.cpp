#include <iostream>
#include <queue>

using namespace std;

typedef uint32_t num;
typedef uint32_t board;
typedef uint64_t ebit;

board square[32];
board tsquare[32];
const board possibleLeftUp = 1048343118, possibleLeftDown = 2087865918, possibleRightDown = 4193372472, possibleRightUp = 955904457;
board all, enemy, emptys, singles, queens;
board tryLeftUp, tryRightUp, tryLeftDown, tryRightDown;
ebit bitTry;
board maxLeftUp, maxLeftDown, maxRightUp, maxRightDown, maxLeftUpEndTurn, maxLeftDownEndTurn, maxRightUpEndTurn, maxRightDownEndTurn;
board turnLeftUp, turnLeftDown, turnRightUp, turnRightDown;
board x[7];
board y[8];
board x24, x15, x06, y34, y25, y16, y07;
board buf;
ebit buf64;
board buf32, newbuf32;
board y04, y57, yall;
ebit bit8y04, bit8y57, bity;
ebit queens64, enemy64, emptys64, nomy64;
num first = 0, last = 1;
ebit mask, buf64No2;
bool isQueenTurn;

struct pos {
	board possingle;
	board posenemy;
	board posemptys;
	pos(board s, board e, board a) {
		possingle = s;
		posenemy = e;
		posemptys = a;
	}
};

ebit to64(board thisBoard) {
	buf64 = thisBoard;
	return (buf64 << 32) & bit8y57 | buf64 & bit8y04;
}

board to32(ebit thisEbit) {
	return (board)(((thisEbit & bit8y57) >> 32) | thisEbit & bit8y04);
}

struct pos64 {
	ebit possingle;
	ebit posenemy;
	ebit posemptys;
	pos64(board s, board e, board a) {
		possingle = s;
		possingle = (possingle << 32) & bit8y57 | possingle & bit8y04;
		posenemy = e;
		posenemy = (posenemy << 32) & bit8y57 | posenemy & bit8y04;
		posemptys = a;
		posemptys = (posemptys << 32) & bit8y57 | posemptys & bit8y04;
	}
	pos64(ebit s, ebit e, ebit a) {
		possingle = s;
		posenemy = e;
		posemptys = a;
	}
};

board cash[67108864][4];
num parent[67108864];
queue<pos> turns;
queue<pos64> qturns;

void queenWhite() {

}

void nextWhite() {
	pos current = turns.front();
	turns.pop();
	tryLeftUp = (((current.possingle & possibleLeftUp) << 1 & current.posenemy) << 1) & current.posemptys;
	tryRightDown = (((current.possingle & possibleRightDown) >> 1 & current.posenemy) >> 1) & current.posemptys;
	buf32 = ((current.possingle & possibleRightUp) << 7 | (current.possingle & possibleRightUp) >> 25) & current.posenemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25)) & current.posemptys;
	buf32 = ((current.possingle & possibleLeftDown) >> 7 | (current.possingle & possibleLeftDown) << 25) & current.posenemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & current.posemptys;
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		if (tryLeftUp) {
			if (current.possingle | maxLeftUp) {
				qturns.push(pos64(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
			}
		}
		if (tryLeftDown) {
			turns.push(pos((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
		}
		if (tryRightDown) {
			turns.push(pos(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
		}
		if (tryRightUp) {
			if (current.possingle | maxRightUp) {
				qturns.push(pos64((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
			}
		}
	}
	else {
		cash[last][3] = current.possingle | cash[first][3] & ~current.posemptys;
		cash[last][1] = current.possingle | cash[first][1] & ~current.posemptys;
		cash[last][0] = cash[first][0] & ~current.posemptys;
		cash[last][2] = cash[first][2] & ~current.posemptys;
		parent[last] = first;
		last++;
	}
	if (!turns.empty()) {
		nextWhite();
	}
}
void nextBlack() {
	pos current = turns.front();
	turns.pop();
	tryLeftUp = (((current.possingle & possibleLeftUp) << 1 & current.posenemy) << 1) & current.posemptys;
	tryRightDown = (((current.possingle & possibleRightDown) >> 1 & current.posenemy) >> 1) & current.posemptys;
	buf32 = ((current.possingle & possibleRightUp) << 7 | (current.possingle & possibleRightUp) >> 25) & current.posenemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25)) & current.posemptys;
	buf32 = ((current.possingle & possibleLeftDown) >> 7 | (current.possingle & possibleLeftDown) << 25) & current.posenemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & current.posemptys;
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		if (tryLeftUp) {
			turns.push(pos(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
		}
		if (tryLeftDown) {
			if (current.possingle | maxLeftDown) {
				qturns.push(pos64((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
			}
		}
		if (tryRightDown) {
			if (current.possingle | maxRightDown) {
				qturns.push(pos64(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
			}
		}
		if (tryRightUp) {
			turns.push(pos((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
		}
	}
	else {
		cash[last][2] = current.possingle | cash[first][2] & ~current.posemptys;
		cash[last][0] = current.possingle | cash[first][0] & ~current.posemptys;
		cash[last][1] = cash[first][1] & ~current.posemptys;
		cash[last][3] = cash[first][3] & ~current.posemptys;
		parent[last] = first;
		last++;
	}
	if (!turns.empty()) {
		nextBlack();
	}
}

void getTurnWhite() {
	all = cash[first][0] | cash[first][1];
	enemy = cash[first][1];
	emptys = ~all;
	queens = cash[first][0] ^ cash[first][2];
	singles = cash[first][2];
	tryLeftUp = (((singles & possibleLeftUp) << 1 & enemy) << 1) & emptys;
	tryRightDown = (((singles & possibleRightDown) >> 1 & enemy) >> 1) & emptys;
	buf32 = ((singles & possibleRightUp) << 7 | (singles & possibleRightUp) >> 25) & enemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25)) & emptys;
	buf32 = ((singles & possibleLeftDown) >> 7 | (singles & possibleLeftDown) << 25) & enemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & emptys;
	isQueenTurn = false;
	if (queens) {
		queens64 = queens;
		queens64 = (queens64 << 32) & bit8y57 | queens64 & bit8y04;
		emptys64 = emptys;
		emptys64 = (emptys64 << 32) & bit8y57 | emptys64 & bit8y04;
		enemy64 = enemy;
		enemy64 = (enemy64 << 32) & bit8y57 | enemy64 & bit8y04;
		nomy64 = enemy64 | emptys64; //Пустые или с врагами клетки
		buf64 = queens64 << 1 & nomy64; //Ходим вверх-влево
		while (buf64) {
			bitTry = ((buf64 & enemy64)) << 1 & emptys64; //смогли срубить (есть ли враг на текущей клетке, и пуста ли следущая клетка)
			buf64 = (buf64 & emptys64) << 1; //не смогли срубить (текущая клетка пуста)
			isQueenTurn |= bitTry;
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					qturns.push(pos64(mask >> 2, enemy64 & ~(mask >> 1), emptys | mask >> 2));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				bitTry = (bitTry << 1) & emptys64;
			}
			
		}
		
	}
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		if (tryLeftUp) {
			buf = tryLeftUp & maxLeftUpEndTurn;
			tryLeftUp ^= maxLeftUpEndTurn;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				qturns.push(pos64(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
				buf &= buf - 1;
			}
			while (tryLeftUp) {
				buf32 = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
				turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
				tryLeftUp &= tryLeftUp - 1;
			}
		}
		while (tryRightDown) {
			buf32 = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
			turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
			tryRightDown &= tryRightDown - 1;
		}
		while (tryLeftDown) {
			buf32 = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
			turns.push(pos(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
			tryLeftDown &= tryLeftDown - 1;
		}
		if (tryRightUp) {
			buf = tryRightUp & maxRightUpEndTurn;
			tryRightUp ^= maxRightUpEndTurn;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				qturns.push(pos64(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
				buf &= buf - 1;
			}
			while (tryRightUp) {
				buf32 = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
				turns.push(pos(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
				tryRightUp &= tryRightUp - 1;
			}
		}
		nextWhite();
	}
	else {

	}
}
void getTurnBlack() {
	all = cash[first][0] | cash[first][1];
	enemy = cash[first][0];
	emptys = ~all;
	queens = cash[first][1] ^ cash[first][3];
	singles = cash[first][3];
	tryLeftUp = (((singles & possibleLeftUp) << 1 & enemy) << 1) & emptys;
	tryRightDown = (((singles & possibleRightDown) >> 1 & enemy) >> 1) & emptys;
	buf32 = ((singles & possibleRightUp) << 7 | (singles & possibleRightUp) >> 25) & enemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25)) & emptys;
	buf32 = ((singles & possibleLeftDown) >> 7 | (singles & possibleLeftDown) << 25) & enemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & emptys;
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		while (tryLeftUp) {
			buf32 = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
			turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
			tryLeftUp &= tryLeftUp - 1;
		}
		if (tryRightDown) {
			buf = tryRightDown & maxRightDownEndTurn;
			tryRightDown ^= maxRightDownEndTurn;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				qturns.push(pos64(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
				buf &= buf - 1;
			}
			while (tryRightDown) {
				buf32 = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
				turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
				tryRightDown &= tryRightDown - 1;
			}
		}
		if (tryLeftDown) {
			buf = tryLeftDown & maxLeftDownEndTurn;
			tryLeftDown ^= maxLeftDownEndTurn;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				qturns.push(pos64(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
				buf &= buf - 1;
			}
			while (tryLeftDown) {
				buf32 = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
				turns.push(pos(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
				tryLeftDown &= tryLeftDown - 1;
			}
		}
		while (tryRightUp) {
			buf32 = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
			turns.push(pos(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
			tryRightUp &= tryRightUp - 1;
		}
		nextBlack();
	}
	else {

	}
}

void main() {
	square[0] = 1;
	tsquare[0] = ~square[0];
	for (int i = 1; i < 32; i++) {
		square[i] = square[i - 1] << 1;
		tsquare[i] = ~square[i];
	}
	maxLeftUp = square[29] | square[3] | square[9];
	maxLeftUpEndTurn = maxLeftUp << 2;
	maxRightDown = square[8] | square[14] | square[20];
	maxRightDownEndTurn = maxRightDown >> 2;
	maxLeftDown = square[26] | square[14] | square[20];
	maxLeftDownEndTurn = maxLeftDown >> 14;
	maxRightUp = square[23] | square[29] | square[3];
	maxRightUpEndTurn = square[5] | square[11] | square[17];
	turnLeftUp = possibleLeftUp | square[7] | square[15] | square[23] | square[30] | square[4] | square[10];
	turnRightDown = possibleRightDown | square[7] | square[13] | square[19] | square[26] | square[2] | square[10];
	turnLeftDown = possibleLeftDown | square[30] | square[22] | square[14] | square[13] | square[19] | square[25];
	turnRightUp = possibleRightUp | square[2] | square[26] | square[30] | square[4] | square[10];
	x[0] = square[24] | square[31];
	x[1] = square[5] | square[30] | square[23] | square[16];
	x[2] = square[11] | square[4] | square[29] | square[22] | square[15] | square[8];
	x[3] = square[17] | square[10] | square[3] | square[28] | square[21] | square[14] | square[7] | square[0];
	x[4] = square[9] | square[2] | square[27] | square[20] | square[13] | square[6];
	x[5] = square[1] | square[26] | square[19] | square[12];
	x[6] = square[25] | square[18];
	y[0] = square[0];
	y[1] = square[8] | square[7] | square[6];
	y[2] = square[16] | square[15] | square[14] | square[13] | square[12];
	y[3] = square[24] | square[23] | square[22] | square[21] | square[20] | square[19] | square[18];
	y[4] = square[31] | square[30] | square[29] | square[28] | square[27] | square[26] | square[25];
	y[5] = square[5] | square[4] | square[3] | square[2] | square[1];
	y[6] = square[11] | square[10] | square[9];
	y[7] = square[17];
	x24 = x[2] | x[4];
	x15 = x[1] | x[5];
	x06 = x[0] | x[6];
	y34 = y[3] | y[4];
	y25 = y[2] | y[5];
	y16 = y[1] | y[6];
	y07 = y[0] | y[7];
	y04 = y[0] | y[1] | y[2] | y[3] | y[4];
	y57 = y[5] | y[6] | y[7];
	bit8y04 = y04;
	bit8y57 = y57;
	bit8y57 <<= 32;
	bity = bit8y57 | bit8y04;
	yall = y04 | y57;
}
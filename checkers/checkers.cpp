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
ebit bittry;
board maxLeftUp, maxLeftDown, maxRightUp, maxRightDown;
board turnLeftUp, turnLeftDown, turnRightUp, turnRightDown;
board x[7];
board y[8];
board x24, x15, x06, y34, y25, y16, y07;
board buf;
ebit buf64, buf64empty;
board buf32;
board y04, y57, yall;
ebit bit8y04, bit8y57, bity;
ebit queens64, enemy64, emptys64, nomy64;
num first = 0, last = 1;
board thisMask;

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

//Внедрить подобие бинарного поиска

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
	if (queens) {
		queens64 = queens;
		queens64 = (queens64 << 32) & bit8y57 | queens64 & bit8y04;
		emptys64 = emptys;
		emptys64 = (emptys64 << 32) & bit8y57 | emptys64 & bit8y04;
		enemy64 = enemy;
		enemy64 = (enemy64 << 32) & bit8y57 | enemy64 & bit8y04;
		nomy64 = enemy64 | emptys64; //Пустые или с врагами клетки
		buf64 = queens64 << 7 & nomy64;
		if (buf64) {
			bittry = ((buf64 & enemy) << 7) & emptys64; //Смогли срубить
			buf64 = buf64 ^ bittry;
			while (bittry) {
				thisMask = bittry ^ (bittry & (bittry - 1));
				bittry &= bittry - 1;
			}
		}
	}
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		if (tryLeftUp) {
			if (tryLeftUp & square[24])
				turns.push(pos(square[24], enemy & tsquare[23], emptys | square[22]));
			if (tryLeftUp & square[30])
				turns.push(pos(square[30], enemy & tsquare[29], emptys | square[28]));
			if (tryLeftUp & square[4])
				turns.push(pos(square[4], enemy & tsquare[3], emptys | square[2]));
			if (tryLeftUp & square[23])
				turns.push(pos(square[23], enemy & tsquare[22], emptys | square[21]));
			if (tryLeftUp & square[29])
				turns.push(pos(square[29], enemy & tsquare[28], emptys | square[27]));
			if (tryLeftUp & square[3])
				turns.push(pos(square[3], enemy & tsquare[2], emptys | square[1]));
			if (tryLeftUp & square[16])
				turns.push(pos(square[16], enemy & tsquare[15], emptys | square[14]));
			if (tryLeftUp & square[22])
				turns.push(pos(square[22], enemy & tsquare[21], emptys | square[20]));
			if (tryLeftUp & square[28])
				turns.push(pos(square[28], enemy & tsquare[27], emptys | square[26]));
			if (tryLeftUp & square[15])
				turns.push(pos(square[15], enemy & tsquare[14], emptys | square[13]));
			if (tryLeftUp & square[21])
				turns.push(pos(square[21], enemy & tsquare[20], emptys | square[19]));
			if (tryLeftUp & square[27])
				turns.push(pos(square[27], enemy & tsquare[26], emptys | square[25]));
			if (tryLeftUp & square[8])
				turns.push(pos(square[8], enemy & tsquare[7], emptys | square[6]));
			if (tryLeftUp & square[14])
				turns.push(pos(square[14], enemy & tsquare[13], emptys | square[12]));
			if (tryLeftUp & square[20])
				turns.push(pos(square[20], enemy & tsquare[19], emptys | square[18]));
			if (tryLeftUp & square[31])
				qturns.push(pos64(square[31], enemy & tsquare[30], emptys | square[29]));
			if (tryLeftUp & square[5])
				qturns.push(pos64(square[5], enemy & tsquare[4], emptys | square[3]));
			if (tryLeftUp & square[11])
				qturns.push(pos64(square[11], enemy & tsquare[10], emptys | square[9]));
		}
		if (tryRightDown) {
			if (tryRightDown & square[13])
				turns.push(pos(square[13], enemy & tsquare[14], emptys | square[15]));
			if (tryRightDown & square[19])
				turns.push(pos(square[19], enemy & tsquare[20], emptys | square[21]));
			if (tryRightDown & square[25])
				turns.push(pos(square[25], enemy & tsquare[26], emptys | square[27]));
			if (tryRightDown & square[9])
				turns.push(pos(square[9], enemy & tsquare[10], emptys | square[11]));
			if (tryRightDown & square[29])
				turns.push(pos(square[29], enemy & tsquare[30], emptys | square[31]));
			if (tryRightDown & square[3])
				turns.push(pos(square[3], enemy & tsquare[4], emptys | square[5]));
			if (tryRightDown & square[2])
				turns.push(pos(square[2], enemy & tsquare[3], emptys | square[4]));
			if (tryRightDown & square[22])
				turns.push(pos(square[22], enemy & tsquare[23], emptys | square[24]));
			if (tryRightDown & square[28])
				turns.push(pos(square[28], enemy & tsquare[29], emptys | square[30]));
			if (tryRightDown & square[1])
				turns.push(pos(square[1], enemy & tsquare[2], emptys | square[3]));
			if (tryRightDown & square[21])
				turns.push(pos(square[21], enemy & tsquare[22], emptys | square[23]));
			if (tryRightDown & square[27])
				turns.push(pos(square[27], enemy & tsquare[28], emptys | square[29]));
			if (tryRightDown & square[26])
				turns.push(pos(square[26], enemy & tsquare[27], emptys | square[28]));
			if (tryRightDown & square[14])
				turns.push(pos(square[14], enemy & tsquare[15], emptys | square[16]));
			if (tryRightDown & square[20])
				turns.push(pos(square[20], enemy & tsquare[21], emptys | square[22]));
			if (tryRightDown & square[6])
				turns.push(pos(square[6], enemy & tsquare[7], emptys | square[8]));
			if (tryRightDown & square[12])
				turns.push(pos(square[12], enemy & tsquare[13], emptys | square[14]));
			if (tryRightDown & square[18])
				turns.push(pos(square[18], enemy & tsquare[19], emptys | square[20]));
		}
		if (tryLeftDown) {
			if (tryLeftUp & square[13])
				turns.push(pos(square[13], enemy & tsquare[20], emptys | square[27]));
			if (tryLeftUp & square[19])
				turns.push(pos(square[19], enemy & tsquare[26], emptys | square[1]));
			if (tryLeftUp & square[7])
				turns.push(pos(square[7], enemy & tsquare[14], emptys | square[21]));
			if (tryLeftUp & square[23])
				turns.push(pos(square[23], enemy & tsquare[30], emptys | square[5]));
			if (tryLeftUp & square[29])
				turns.push(pos(square[29], enemy & tsquare[4], emptys | square[11]));
			if (tryLeftUp & square[3])
				turns.push(pos(square[3], enemy & tsquare[10], emptys | square[17]));
			if (tryLeftUp & square[16])
				turns.push(pos(square[16], enemy & tsquare[23], emptys | square[30]));
			if (tryLeftUp & square[22])
				turns.push(pos(square[22], enemy & tsquare[29], emptys | square[4]));
			if (tryLeftUp & square[28])
				turns.push(pos(square[28], enemy & tsquare[3], emptys | square[10]));
			if (tryLeftUp & square[15])
				turns.push(pos(square[15], enemy & tsquare[22], emptys | square[29]));
			if (tryLeftUp & square[21])
				turns.push(pos(square[21], enemy & tsquare[28], emptys | square[3]));
			if (tryLeftUp & square[27])
				turns.push(pos(square[27], enemy & tsquare[2], emptys | square[9]));
			if (tryLeftUp & square[8])
				turns.push(pos(square[8], enemy & tsquare[15], emptys | square[22]));
			if (tryLeftUp & square[14])
				turns.push(pos(square[14], enemy & tsquare[21], emptys | square[28]));
			if (tryLeftUp & square[20])
				turns.push(pos(square[20], enemy & tsquare[27], emptys | square[2]));
			if (tryLeftUp & square[6])
				turns.push(pos(square[6], enemy & tsquare[13], emptys | square[20]));
			if (tryLeftUp & square[12])
				turns.push(pos(square[12], enemy & tsquare[19], emptys | square[26]));
			if (tryLeftUp & square[0])
				turns.push(pos(square[0], enemy & tsquare[7], emptys | square[14]));
		}
		if (tryRightUp) {
			if (tryRightUp & square[10])
				turns.push(pos(square[10], enemy & tsquare[3], emptys | square[28]));
			if (tryRightUp & square[30])
				turns.push(pos(square[30], enemy & tsquare[23], emptys | square[16]));
			if (tryRightUp & square[4])
				turns.push(pos(square[4], enemy & tsquare[29], emptys | square[22]));
			if (tryRightUp & square[9])
				turns.push(pos(square[9], enemy & tsquare[2], emptys | square[27]));
			if (tryRightUp & square[29])
				turns.push(pos(square[29], enemy & tsquare[22], emptys | square[15]));
			if (tryRightUp & square[3])
				turns.push(pos(square[3], enemy & tsquare[28], emptys | square[21]));
			if (tryRightUp & square[2])
				turns.push(pos(square[2], enemy & tsquare[27], emptys | square[20]));
			if (tryRightUp & square[22])
				turns.push(pos(square[22], enemy & tsquare[15], emptys | square[8]));
			if (tryRightUp & square[28])
				turns.push(pos(square[28], enemy & tsquare[21], emptys | square[14]));
			if (tryRightUp & square[1])
				turns.push(pos(square[1], enemy & tsquare[26], emptys | square[19]));
			if (tryRightUp & square[21])
				turns.push(pos(square[21], enemy & tsquare[14], emptys | square[7]));
			if (tryRightUp & square[27])
				turns.push(pos(square[27], enemy & tsquare[20], emptys | square[13]));
			if (tryRightUp & square[26])
				turns.push(pos(square[26], enemy & tsquare[19], emptys | square[12]));
			if (tryRightUp & square[14])
				turns.push(pos(square[14], enemy & tsquare[7], emptys | square[0]));
			if (tryRightUp & square[20])
				turns.push(pos(square[20], enemy & tsquare[13], emptys | square[6]));
			if (tryRightUp & square[17])
				qturns.push(pos64(square[17], enemy & tsquare[10], emptys | square[3]));
			if (tryRightUp & square[5])
				qturns.push(pos64(square[5], enemy & tsquare[30], emptys | square[23]));
			if (tryRightUp & square[11])
				qturns.push(pos64(square[11], enemy & tsquare[4], emptys | square[29]));
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
		if (tryLeftUp) {
			if (tryLeftUp & square[24])
				turns.push(pos(square[24], enemy & tsquare[23], emptys | square[22]));
			if (tryLeftUp & square[30])
				turns.push(pos(square[30], enemy & tsquare[29], emptys | square[28]));
			if (tryLeftUp & square[4])
				turns.push(pos(square[4], enemy & tsquare[3], emptys | square[2]));
			if (tryLeftUp & square[23])
				turns.push(pos(square[23], enemy & tsquare[22], emptys | square[21]));
			if (tryLeftUp & square[29])
				turns.push(pos(square[29], enemy & tsquare[28], emptys | square[27]));
			if (tryLeftUp & square[3])
				turns.push(pos(square[3], enemy & tsquare[2], emptys | square[1]));
			if (tryLeftUp & square[16])
				turns.push(pos(square[16], enemy & tsquare[15], emptys | square[14]));
			if (tryLeftUp & square[22])
				turns.push(pos(square[22], enemy & tsquare[21], emptys | square[20]));
			if (tryLeftUp & square[28])
				turns.push(pos(square[28], enemy & tsquare[27], emptys | square[26]));
			if (tryLeftUp & square[15])
				turns.push(pos(square[15], enemy & tsquare[14], emptys | square[13]));
			if (tryLeftUp & square[21])
				turns.push(pos(square[21], enemy & tsquare[20], emptys | square[19]));
			if (tryLeftUp & square[27])
				turns.push(pos(square[27], enemy & tsquare[26], emptys | square[25]));
			if (tryLeftUp & square[8])
				turns.push(pos(square[8], enemy & tsquare[7], emptys | square[6]));
			if (tryLeftUp & square[14])
				turns.push(pos(square[14], enemy & tsquare[13], emptys | square[12]));
			if (tryLeftUp & square[20])
				turns.push(pos(square[20], enemy & tsquare[19], emptys | square[18]));
			if (tryLeftUp & square[31])
				turns.push(pos(square[31], enemy & tsquare[30], emptys | square[29]));
			if (tryLeftUp & square[5])
				turns.push(pos(square[5], enemy & tsquare[4], emptys | square[3]));
			if (tryLeftUp & square[11])
				turns.push(pos(square[11], enemy & tsquare[10], emptys | square[9]));
		}
		if (tryRightDown) {
			if (tryRightDown & square[13])
				turns.push(pos(square[13], enemy & tsquare[14], emptys | square[15]));
			if (tryRightDown & square[19])
				turns.push(pos(square[19], enemy & tsquare[20], emptys | square[21]));
			if (tryRightDown & square[25])
				turns.push(pos(square[25], enemy & tsquare[26], emptys | square[27]));
			if (tryRightDown & square[9])
				turns.push(pos(square[9], enemy & tsquare[10], emptys | square[11]));
			if (tryRightDown & square[29])
				turns.push(pos(square[29], enemy & tsquare[30], emptys | square[31]));
			if (tryRightDown & square[3])
				turns.push(pos(square[3], enemy & tsquare[4], emptys | square[5]));
			if (tryRightDown & square[2])
				turns.push(pos(square[2], enemy & tsquare[3], emptys | square[4]));
			if (tryRightDown & square[22])
				turns.push(pos(square[22], enemy & tsquare[23], emptys | square[24]));
			if (tryRightDown & square[28])
				turns.push(pos(square[28], enemy & tsquare[29], emptys | square[30]));
			if (tryRightDown & square[1])
				turns.push(pos(square[1], enemy & tsquare[2], emptys | square[3]));
			if (tryRightDown & square[21])
				turns.push(pos(square[21], enemy & tsquare[22], emptys | square[23]));
			if (tryRightDown & square[27])
				turns.push(pos(square[27], enemy & tsquare[28], emptys | square[29]));
			if (tryRightDown & square[26])
				turns.push(pos(square[26], enemy & tsquare[27], emptys | square[28]));
			if (tryRightDown & square[14])
				turns.push(pos(square[14], enemy & tsquare[15], emptys | square[16]));
			if (tryRightDown & square[20])
				turns.push(pos(square[20], enemy & tsquare[21], emptys | square[22]));
			if (tryRightDown & square[6])
				qturns.push(pos64(square[6], enemy & tsquare[7], emptys | square[8]));
			if (tryRightDown & square[12])
				qturns.push(pos64(square[12], enemy & tsquare[13], emptys | square[14]));
			if (tryRightDown & square[18])
				qturns.push(pos64(square[18], enemy & tsquare[19], emptys | square[20]));
		}
		if (tryLeftDown) {
			if (tryLeftUp & square[13])
				turns.push(pos(square[13], enemy & tsquare[20], emptys | square[27]));
			if (tryLeftUp & square[19])
				turns.push(pos(square[19], enemy & tsquare[26], emptys | square[1]));
			if (tryLeftUp & square[7])
				turns.push(pos(square[7], enemy & tsquare[14], emptys | square[21]));
			if (tryLeftUp & square[23])
				turns.push(pos(square[23], enemy & tsquare[30], emptys | square[5]));
			if (tryLeftUp & square[29])
				turns.push(pos(square[29], enemy & tsquare[4], emptys | square[11]));
			if (tryLeftUp & square[3])
				turns.push(pos(square[3], enemy & tsquare[10], emptys | square[17]));
			if (tryLeftUp & square[16])
				turns.push(pos(square[16], enemy & tsquare[23], emptys | square[30]));
			if (tryLeftUp & square[22])
				turns.push(pos(square[22], enemy & tsquare[29], emptys | square[4]));
			if (tryLeftUp & square[28])
				turns.push(pos(square[28], enemy & tsquare[3], emptys | square[10]));
			if (tryLeftUp & square[15])
				turns.push(pos(square[15], enemy & tsquare[22], emptys | square[29]));
			if (tryLeftUp & square[21])
				turns.push(pos(square[21], enemy & tsquare[28], emptys | square[3]));
			if (tryLeftUp & square[27])
				turns.push(pos(square[27], enemy & tsquare[2], emptys | square[9]));
			if (tryLeftUp & square[8])
				turns.push(pos(square[8], enemy & tsquare[15], emptys | square[22]));
			if (tryLeftUp & square[14])
				turns.push(pos(square[14], enemy & tsquare[21], emptys | square[28]));
			if (tryLeftUp & square[20])
				turns.push(pos(square[20], enemy & tsquare[27], emptys | square[2]));
			if (tryLeftUp & square[6])
				qturns.push(pos64(square[6], enemy & tsquare[13], emptys | square[20]));
			if (tryLeftUp & square[12])
				qturns.push(pos64(square[12], enemy & tsquare[19], emptys | square[26]));
			if (tryLeftUp & square[0])
				qturns.push(pos64(square[0], enemy & tsquare[7], emptys | square[14]));
		}
		if (tryRightUp) {
			if (tryRightUp & square[10])
				turns.push(pos(square[10], enemy & tsquare[3], emptys | square[28]));
			if (tryRightUp & square[30])
				turns.push(pos(square[30], enemy & tsquare[23], emptys | square[16]));
			if (tryRightUp & square[4])
				turns.push(pos(square[4], enemy & tsquare[29], emptys | square[22]));
			if (tryRightUp & square[9])
				turns.push(pos(square[9], enemy & tsquare[2], emptys | square[27]));
			if (tryRightUp & square[29])
				turns.push(pos(square[29], enemy & tsquare[22], emptys | square[15]));
			if (tryRightUp & square[3])
				turns.push(pos(square[3], enemy & tsquare[28], emptys | square[21]));
			if (tryRightUp & square[2])
				turns.push(pos(square[2], enemy & tsquare[27], emptys | square[20]));
			if (tryRightUp & square[22])
				turns.push(pos(square[22], enemy & tsquare[15], emptys | square[8]));
			if (tryRightUp & square[28])
				turns.push(pos(square[28], enemy & tsquare[21], emptys | square[14]));
			if (tryRightUp & square[1])
				turns.push(pos(square[1], enemy & tsquare[26], emptys | square[19]));
			if (tryRightUp & square[21])
				turns.push(pos(square[21], enemy & tsquare[14], emptys | square[7]));
			if (tryRightUp & square[27])
				turns.push(pos(square[27], enemy & tsquare[20], emptys | square[13]));
			if (tryRightUp & square[26])
				turns.push(pos(square[26], enemy & tsquare[19], emptys | square[12]));
			if (tryRightUp & square[14])
				turns.push(pos(square[14], enemy & tsquare[7], emptys | square[0]));
			if (tryRightUp & square[20])
				turns.push(pos(square[20], enemy & tsquare[13], emptys | square[6]));
			if (tryRightUp & square[17])
				turns.push(pos(square[17], enemy & tsquare[10], emptys | square[3]));
			if (tryRightUp & square[5])
				turns.push(pos(square[5], enemy & tsquare[30], emptys | square[23]));
			if (tryRightUp & square[11])
				turns.push(pos(square[11], enemy & tsquare[4], emptys | square[29]));
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
	maxRightDown = square[8] | square[14] | square[20];
	maxLeftDown = square[26] | square[14] | square[20];
	maxRightUp = square[23] | square[29] | square[3];
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
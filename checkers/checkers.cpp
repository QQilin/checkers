#include <iostream>
#include <queue>

using namespace std;

typedef uint32_t num;
typedef uint32_t board;
typedef uint64_t ebit;

#define to32(thisEbit) ((board)(((thisEbit & bit8y57) >> 32) | thisEbit & bit8y04))

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
ebit queens64, enemy64, emptys64;
num first = 0, last = 1;
ebit mask, buf64No2;
bool isQueenTurn;
ebit turnLeftUp64, turnLeftDown64, turnRightUp64, turnRightDown64, possibleLeftUp64, possibleLeftDown64, possibleRightUp64, possibleRightDown64;
unsigned char counter;

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

const  board buffersize = 67108864;
board cash[buffersize][4];
//0 - все шашки белых
//1 - все шашки чёхных
//2 - все простые шашки белых
//3 - все простые шашки чёрных
num parent[buffersize];
unsigned char active[buffersize];
unsigned char act; //Содержит предыдущую активную сторону, 
//1 сейчас ход белых
//0 сейчас ход чёрных
queue<pos> turns;
queue<pos64> qturns;
pos current;
pos64 current64;

void getQueenTurn() {
	current64 = qturns.front();
	qturns.pop();
	isQueenTurn = false;
	buf64 = (current64.possingle & possibleLeftUp) << 1; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & current64.posenemy) << 1 & current64.posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		isQueenTurn = isQueenTurn || bitTry;
		counter = 1;
		while (bitTry) {
			qturns.push(pos64(bitTry, current64.posenemy & ~(bitTry >> counter), current64.posemptys | bitTry >> (counter + 1)));
			counter++;
			bitTry = ((bitTry & turnLeftUp64) << 1) & current64.posemptys;
		}
		buf64 = (buf64 & possibleLeftUp64 & current64.posemptys) << 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (current64.possingle & possibleRightDown) >> 1; //Ходим вниз-вправо, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & current64.posenemy) >> 1 & current64.posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 1;
		isQueenTurn = isQueenTurn || bitTry;
		while (bitTry) {
			qturns.push(pos64(bitTry, current64.posenemy & ~(bitTry << counter), current64.posemptys | bitTry << (counter + 1)));
			counter++;
			bitTry = ((bitTry & turnRightDown64) >> 1) & current64.posemptys;
		}
		buf64 = (buf64 & possibleRightDown & current64.posemptys) >> 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (current64.possingle & possibleRightUp) >> 7; //Ходим вверх-вправо, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & current64.posenemy) >> 7 & current64.posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 7;
		isQueenTurn = isQueenTurn || bitTry;
		while (bitTry) {
			qturns.push(pos64(bitTry, current64.posenemy & ~(bitTry << counter), current64.posemptys | bitTry << (counter + 7)));
			counter += 7;
			bitTry = ((bitTry & turnRightUp64) >> 7) & current64.posemptys;
		}
		buf64 = (buf64 & possibleRightUp & current64.posemptys) >> 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (current64.possingle & possibleLeftDown) << 7; //Ходим вниз-влево, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & current64.posenemy) << 7 & current64.posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 7;
		isQueenTurn = isQueenTurn || bitTry;
		while (bitTry) {
			qturns.push(pos64(bitTry, current64.posenemy & ~(bitTry >> counter), current64.posemptys | bitTry >> (counter + 7)));
			counter += 7;
			bitTry = ((bitTry & turnLeftDown64) << 7) & current64.posemptys;
		}
		buf64 = (buf64 & possibleLeftDown & current64.posemptys) << 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	if (isQueenTurn) {
		getQueenTurn();
	}
	else {
		cash[last][1 ^ act] = cash[first][1 ^ act] & ~to32(current64.posemptys) | to32(current64.possingle);
		cash[last][act] = cash[first][act] & ~to32(current64.posemptys);
		cash[last][3 ^ act] = cash[first][3 ^ act] & ~to32(current64.posemptys) ^ to32(current64.possingle);
		cash[last][2 | act] = cash[first][2 | act] & ~to32(current64.posemptys);
		parent[last] = first;
		active[last] = 1 ^ act;
		last++;
		if (!qturns.empty())
			getQueenTurn();
	}
}

void next() {
	current = turns.front();
	turns.pop();
	tryLeftUp = (((current.possingle & possibleLeftUp) << 1 & current.posenemy) << 1) & current.posemptys;
	tryRightDown = (((current.possingle & possibleRightDown) >> 1 & current.posenemy) >> 1) & current.posemptys;
	buf32 = ((current.possingle & possibleRightUp) << 7 | (current.possingle & possibleRightUp) >> 25) & current.posenemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25)) & current.posemptys;
	buf32 = ((current.possingle & possibleLeftDown) >> 7 | (current.possingle & possibleLeftDown) << 25) & current.posenemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & current.posemptys;
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		if (tryLeftUp) {
			if (act && (current.possingle & maxLeftUp)) {
				qturns.push(pos64(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
			}
		}
		if (tryLeftDown) {
			if (!act && (current.possingle & maxLeftDown)) {
				qturns.push(pos64((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
			}
		}
		if (tryRightDown) {
			if (!act && (current.possingle & maxRightDown)) {
				qturns.push(pos64(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
			}
		}
		if (tryRightUp) {
			if (act && (current.possingle & maxRightUp)) {
				qturns.push(pos64((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
			}
			else {
				turns.push(pos((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
			}
		}
		next();
	}
	else {
		cash[last][act & 3] = current.possingle | cash[first][act & 3] & ~current.posemptys;
		cash[last][act] = current.possingle | cash[first][act] & ~current.posemptys;
		cash[last][act ^ 1] = cash[first][act ^ 1] & ~current.posemptys;
		cash[last][act ^ 3] = cash[first][act ^ 3] & ~current.posemptys;
		parent[last] = first;
		active[last] = act ^ 1;
		last++;
		if (!turns.empty()) {
			next();
		}
	}
}

void getTurn() {
	act = active[first];
	enemy = cash[first][act];
	if (enemy == 0) {
		first++;
		return;
	}
	singles = cash[first][act ^ 3];
	all = cash[first][0] | cash[first][1];
	emptys = ~all;
	queens = cash[first][act ^ 1] ^ singles;
	isQueenTurn = false;
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
		buf64 = (queens64 & possibleLeftUp64) << 1; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) << 1 & emptys64; //можно ли срубить? (хранит конечные положения)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					qturns.push(pos64(mask, enemy64 & ~(mask >> counter), emptys64 | mask >> (counter + 1)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				counter++;
				bitTry = ((bitTry & turnLeftUp64) << 1) & emptys64; //Для вычисления всех конечных положений сдвигаем по направлению движения срубившие шашки
			}
			buf64 = (buf64 & possibleLeftUp64 & emptys64) << 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
		buf64 = (queens64 & possibleRightDown64) >> 1; //Ходим вниз-вправо, оставляем только те клетки, с которых можно рубить
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) >> 1 & emptys64; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					qturns.push(pos64(mask, enemy64 & ~(mask << counter), emptys64 | mask << (counter + 1)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				counter++;
				bitTry = ((bitTry & turnRightDown64) >> 1) & emptys64;
			}
			buf64 = (buf64 & possibleRightDown64  & emptys64) >> 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
		buf64 = (queens64 & possibleRightUp64) >> 7; //Ходим вверх-вправо, оставляем только те клетки, с которых можно рубить
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) >> 7 & emptys64; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					qturns.push(pos64(mask, enemy64 & ~(mask << counter), emptys64 | mask << (counter + 7)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				bitTry = ((bitTry & turnRightUp64) >> 7) & emptys64;
				counter += 7;
			}
			buf64 = (buf64 & possibleRightUp64 & emptys64) >> 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
		buf64 = (queens64 & possibleLeftDown64) << 7; //Ходим вниз-влево, оставляем только те клетки, с которых можно рубить
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) << 7 & emptys64; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					qturns.push(pos64(mask, enemy64 & ~(mask >> counter), emptys64 | mask >> (counter + 7)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				bitTry = ((bitTry & turnLeftDown64) << 7) & emptys64;
				counter += 7;
			}
			buf64 = (buf64 & possibleLeftDown64  & emptys64) << 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
	}
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown || isQueenTurn) {
		if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
			if (tryLeftUp) {
				if (act) {
					buf = tryLeftUp & maxLeftUpEndTurn;
					tryLeftUp ^= maxLeftUpEndTurn;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						qturns.push(pos64(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
						buf &= buf - 1;
					}
				}
				while (tryLeftUp) {
					buf32 = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
					turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
					tryLeftUp &= tryLeftUp - 1;
				}
			}
			if (tryRightDown) {
				if (!act) {
					buf = tryRightDown & maxRightDownEndTurn;
					tryRightDown ^= maxRightDownEndTurn;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						qturns.push(pos64(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
						buf &= buf - 1;
					}
				}
				while (tryRightDown) {
					buf32 = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
					turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
					tryRightDown &= tryRightDown - 1;
				}
			}
			if (tryLeftDown) {
				if (!act) {
					buf = tryLeftDown & maxLeftDownEndTurn;
					tryLeftDown ^= maxLeftDownEndTurn;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						qturns.push(pos64(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
						buf &= buf - 1;
					}
				}
				while (tryLeftDown) {
					buf32 = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
					turns.push(pos(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
					tryLeftDown &= tryLeftDown - 1;
				}
			}
			if (tryRightUp) {
				if (act) {
					buf = tryRightUp & maxRightUpEndTurn;
					tryRightUp ^= maxRightUpEndTurn;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						qturns.push(pos64(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
						buf &= buf - 1;
					}
				}
				while (tryRightUp) {
					buf32 = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
					turns.push(pos(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
					tryRightUp &= tryRightUp - 1;
				}
			}
			next();
		}
		if (isQueenTurn || !qturns.empty())
			getQueenTurn();
	}
	else {
		if (queens) {
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnLeftUp64) << 1 & emptys64; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) { //можно ли срубить чисто теоретически с клеток?
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					cash[last][2] = cash[first][2];
					cash[last][3] = cash[first][3];
					cash[last][act] = cash[first][act];
					cash[last][1 ^ act] = (cash[first][1 ^ act] | to32(mask)) & ~to32(mask >> counter);
					parent[last] = first;
					active[last] = 1 ^ act;
					last++;
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter++;
				buf64 = (buf64 & turnLeftUp64) << 1 & emptys64; //Текущая клетка пуста
			}
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnRightDown64) >> 1 & emptys64;; //Ходим вниз-вправо, оставляем только те клетки, с которых можно ходить
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) {
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					cash[last][2] = cash[first][2];
					cash[last][3] = cash[first][3];
					cash[last][act] = cash[first][act];
					cash[last][1 ^ act] = (cash[first][1 ^ act] | to32(mask)) & ~to32(mask << counter);
					parent[last] = first;
					active[last] = 1 ^ act;
					last++;
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter++;
				buf64 = (buf64 & turnRightDown64) >> 1 & emptys64; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
			}
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnRightUp64) >> 7 & emptys64;; //Ходим вверх-вправо, оставляем только те клетки, с которых можно ходить
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) {
				bitTry = buf64; 
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					cash[last][2] = cash[first][2];
					cash[last][3] = cash[first][3];
					cash[last][act] = cash[first][act];
					cash[last][1 ^ act] = (cash[first][1 ^ act] | to32(mask)) & ~to32(mask << counter);
					parent[last] = first;
					active[last] = 1 ^ act;
					last++;
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter+=7;
				buf64 = (buf64 & turnRightUp64) >> 7 & emptys64; //Текущая клетка пуста, со следущей клетки чисто теоретически можно ходить
			}
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnLeftDown64) << 7 & emptys64;; //Ходим вниз-влево
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) {
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					cash[last][2] = cash[first][2];
					cash[last][3] = cash[first][3];
					cash[last][act] = cash[first][act];
					cash[last][1 ^ act] = (cash[first][1 ^ act] | to32(mask)) & ~to32(mask >> counter);
					parent[last] = first;
					active[last] = 1 ^ act;
					last++;
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter += 7;
				buf64 = (buf64 & turnLeftDown64) << 7 & emptys64; //Текущая клетка пуста, со следущей клетки чисто теоретически можно ходить
			}
		}
		if (act) {
			tryLeftUp = (singles & turnLeftUp) << 1 & emptys;
			isQueenTurn = isQueenTurn || tryLeftUp;
			while (tryLeftUp)
			{
				buf = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
				cash[last][0] = (cash[first][0] | buf) & ~(buf >> 1);
				cash[last][2] = (cash[first][2] | buf) & ~(buf >> 1);
				cash[last][1] = cash[first][1];
				cash[last][3] = cash[first][3];
				parent[last] = first;
				active[last] = 0;
				last++;
				tryLeftUp &= tryLeftUp - 1;
			}
			tryRightUp = ((singles & turnRightUp) << 7 | (singles & turnRightUp) >> 25) & emptys;
			isQueenTurn = isQueenTurn || tryRightUp;
			while (tryRightUp)
			{
				buf = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
				cash[last][0] = (cash[first][0] | buf) & ~(buf >> 7 | buf << 25);
				cash[last][2] = (cash[first][2] | buf) & ~(buf >> 7 | buf << 25);
				cash[last][1] = cash[first][1];
				cash[last][3] = cash[first][3];
				parent[last] = first;
				active[last] = 0;
				last++;
				tryRightUp &= tryRightUp - 1;
			}
		}
		else {
			tryRightDown = (singles & turnRightDown) >> 1 & emptys;
			isQueenTurn = isQueenTurn || tryRightUp;
			while (tryRightDown)
			{
				buf = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
				cash[last][1] = (cash[first][1] | buf) & ~(buf << 1);
				cash[last][3] = (cash[first][3] | buf) & ~(buf << 1);
				cash[last][0] = cash[first][0];
				cash[last][2] = cash[first][2];
				parent[last] = first;
				active[last] = 1;
				last++;
				tryRightDown &= tryRightDown - 1;
			}
			tryLeftDown = ((singles & turnLeftDown) >> 7 | (singles & turnLeftDown) << 25) & emptys;
			isQueenTurn = isQueenTurn || tryLeftDown;
			while (tryLeftDown)
			{
				buf = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
				cash[last][1] = (cash[first][1] | buf) & ~(buf << 7 | buf >> 25);
				cash[last][3] = (cash[first][3] | buf) & ~(buf << 7 | buf >> 25);
				cash[last][0] = cash[first][0];
				cash[last][2] = cash[first][2];
				parent[last] = first;
				active[last] = 1;
				last++;
				tryLeftDown &= tryLeftDown - 1;
			}
		}
		if (!isQueenTurn) { //нельзя походить
			cash[last][act] = 0;
			parent[last] = first;
			active[last] = 1 ^ act;
			last++;
		}
	}
	first++;
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
	buf64 = 1;
	turnLeftUp64 = bity ^ buf64 ^ (buf64 << 8) ^ (buf64 << 16) ^ (buf64 << 24) ^ (buf64 << 31) ^ (buf64 << 37) ^ (buf64 << 43) ^ (buf64 << 49);
	turnRightUp64 = bity ^ (buf64 << 31) ^ (buf64 << 37) ^ (buf64 << 43) ^ (buf64 << 49) ^ (buf64 << 41) ^ (buf64 << 33) ^ (buf64 << 25);
	turnRightDown64 = bity ^ buf64 ^ (buf64 << 6) ^ (buf64 << 12) ^ (buf64 << 18) ^ (buf64 << 49) ^ (buf64 << 41) ^ (buf64 << 33) ^ (buf64 << 25);
	turnLeftDown64 = bity ^ buf64 ^ (buf64 << 6) ^ (buf64 << 12) ^ (buf64 << 18) ^ (buf64 << 8) ^ (buf64 << 16) ^ (buf64 << 24);
	possibleLeftDown64 = turnLeftDown64 ^ (buf64 << 31) ^ (buf64 << 23) ^ (buf64 << 15) ^ (buf64 << 7) ^ (buf64 << 13) ^ (buf64 << 19) ^ (buf64 << 25);
	possibleLeftUp64 = turnLeftUp64 ^ (buf64 << 7) ^ (buf64 << 15) ^ (buf64 << 23) ^ (buf64 << 30) ^ (buf64 << 36) ^ (buf64 << 42);
	possibleRightDown64 = turnRightDown64 ^ (buf64 << 7) ^ (buf64 << 13) ^ (buf64 << 19) ^ (buf64 << 26) ^ (buf64 << 34) ^ (buf64 << 42);
	possibleRightUp64 = turnRightUp64 ^ (buf << 24) ^ (buf64 << 30) ^ (buf64 << 36) ^ (buf64 << 42) ^ (buf64 << 34) ^ (buf64 << 26) ^ (buf64 << 18);
	cash[0][0] = cash[0][2] = y[0] | y[1] | y[2];
	cash[0][1] = cash[0][2] = y[7] | y[6] | y[5];
	active[0] = 1;
	parent[0] = -1;
	while (first != 1000000) {
		getTurn();
	}
	cout << last;
}
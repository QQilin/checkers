#include <iostream>
#include <queue>
#include <vector>
#include <intrin.h>
#include <ctime>

using namespace std;

typedef uint32_t num;
typedef uint32_t board;
typedef uint64_t ebit;

board square[32];
board tsquare[32];
const board possibleLeftUp = 1048343118, possibleLeftDown = 2087865918, possibleRightDown = 4193372472, possibleRightUp = 955904457;
board all, enemy, emptys, singles, queens, enemySimple, emptySimpleReverse;
board tryLeftUp, tryRightUp, tryLeftDown, tryRightDown;
board nexttryLeftUp, nexttryRightUp, nexttryLeftDown, nexttryRightDown;
ebit bitTry;
board maxLeftUp, maxLeftDown, maxRightUp, maxRightDown, maxLeftUpEndTurn, maxLeftDownEndTurn, maxRightUpEndTurn, maxRightDownEndTurn;
board turnLeftUp, turnLeftDown, turnRightUp, turnRightDown;
board x[7];
board y[8];
board x24, x15, x06, y34, y25, y16, y07;
board buf;
board buf32, buf32_1, buf32_2, buf32_3;
board y04, y57, yall;
ebit bit8y04, bit8y57, bity;
ebit queens64, enemy64, emptys64;
num first = 0, last = 1;
ebit mask, buf64No2;
ebit turnLeftUp64, turnLeftDown64, turnRightUp64, turnRightDown64, possibleLeftUp64, possibleLeftDown64, possibleRightUp64, possibleRightDown64;

board tableZobrist[32][5];

const num buffersize = 64 * 1024 * 1024;
const num HASHNUM = buffersize - 1;
//0 - все шашки белых
//1 - все шашки чёхных
//2 - все простые шашки белых
//3 - все простые шашки чёрных
unsigned char act; //Содержит предыдущую активную сторону, 
//1 сейчас ход белых
//0 сейчас ход чёрных
num hashNum; //Данный хэш-номер

unsigned char n, n1;

num randNum;

void initNum() {
	randNum = rand() % 256;
	for (num l = 0; l < 3; l++) {
		randNum <<= 8;
		randNum += rand() % 256;
	}
	randNum &= HASHNUM;
}

void initZobrist() {
	for (num i = 0; i < 32; i++) {
		for (num j = 0; j < 5; j++) {	
			initNum();
			tableZobrist[i][j] = randNum;
		}
	}
}


struct dataSet {
	board thisBoard[4];
	unsigned char thisActive;
	dataSet* next;
	num zobrist;
	vector <dataSet*> parents;
	vector <dataSet*> childrens;
	dataSet() {
		next = NULL;
		thisActive = 1 ^ act;
	}
};


dataSet* hashTable[buffersize+1];

dataSet* cache[buffersize+1]; //Буфер значений, из которого берём следущий элемент для просчёта

dataSet* active;

dataSet* children;

num phash;


num is() {
	num retrez = 0;
	for (int i = 0; i <= first; i++) {
		for (int j = 0; j < i; j++) {
			if (cache[i]->thisActive == cache[j]->thisActive && cache[i]->thisBoard[0] == cache[j]->thisBoard[0] && cache[i]->thisBoard[1] == cache[j]->thisBoard[1]
				&& cache[i]->thisBoard[2] == cache[j]->thisBoard[2] && cache[i]->thisBoard[3] == cache[j]->thisBoard[3]) {
				retrez++;
			}
		}
	}
	return retrez;
}

/*
void is() {
	for (int i = 0; i < last; i++) {
		if (cache[i]->thisActive == cache[last]->thisActive && cache[i]->thisBoard[0] == cache[last]->thisBoard[0] && cache[i]->thisBoard[1] == cache[last]->thisBoard[1]
			&& cache[i]->thisBoard[2] == cache[last]->thisBoard[2] && cache[i]->thisBoard[3] == cache[last]->thisBoard[3]) {
			cout << endl << endl << last << endl;
			return;
		}
	}
}


num kkk[5];
num kkk1[5];
void ish() {
	num i, j, kk;
	num hashNum1, hashNum2;
	for (i = 0; i < last; i++) {
		hashNum1 = ((cache[i]->thisBoard[0] | cache[i]->thisBoard[1]) ^ ReverseBits((cache[i]->thisBoard[2] | cache[i]->thisBoard[3]))) & HASHNUM;
		for (j = 0; j < i; j++) {
			hashNum2 = ((cache[j]->thisBoard[0] | cache[j]->thisBoard[1]) ^ ReverseBits((cache[j]->thisBoard[2] | cache[j]->thisBoard[3]))) & HASHNUM;
			if (hashNum1 == hashNum2) {
				if (cache[i]->thisActive == 1) {
					for (kk = 0; kk < 4; kk++) {
						if (cache[i]->thisBoard[kk] == cache[j]->thisBoard[kk]) {
							kkk[kk]++;
						}
					}
					if (cache[i]->thisActive == cache[j]->thisActive)
						kkk[4]++;
				}
				else {
					for (kk = 0; kk < 4; kk++) {
						if (cache[i]->thisBoard[kk] == cache[j]->thisBoard[kk]) {
							kkk1[kk]++;
							if (cache[i]->thisActive == cache[j]->thisActive)
								kkk1[4]++;
						}
					}
				}
			}
		}
	}
}

*/

unsigned char deep[buffersize+1];

board to32(ebit bit64) {
	return (board)(((bit64 & bit8y57) >> 32) | bit64 & bit8y04);
}

ebit to64(board bit32) {
	ebit rez = bit32;
	return (rez << 32) & bit8y57 | rez & bit8y04;
}

void getQueenTurn(ebit possingle, ebit posenemy, ebit posemptys) {
	unsigned char counter;
	num newhash;
	bool isQueenTurn = true;
	ebit bitTry, buf64 = (possingle & possibleLeftUp) << 1; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) << 1 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 1;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(bitTry, posenemy & ~(bitTry >> counter), posemptys);
				counter++;
				bitTry = ((bitTry & turnLeftUp64) << 1) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleLeftUp64 & posemptys) << 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (possingle & possibleRightDown) >> 1; //Ходим вниз-вправо, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) >> 1 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 1;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(bitTry, posenemy & ~(bitTry << counter), posemptys);
				counter++;
				bitTry = ((bitTry & turnRightDown64) >> 1) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleRightDown & posemptys) >> 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (possingle & possibleRightUp) >> 7; //Ходим вверх-вправо, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) >> 7 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 7;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(bitTry, posenemy & ~(bitTry << counter), posemptys);
				counter += 7;
				bitTry = ((bitTry & turnRightUp64) >> 7) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleRightUp & posemptys) >> 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (possingle & possibleLeftDown) << 7; //Ходим вниз-влево, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) << 7 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 7;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(bitTry, posenemy & ~(bitTry >> counter), posemptys);
				counter += 7;
				bitTry = ((bitTry & turnLeftDown64) << 7) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleLeftDown & posemptys) << 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	if (isQueenTurn) {
		board sing = to32(possingle);
		board empt = ~to32(posemptys);
		board ene = to32(posenemy);
		buf32_1 = active->thisBoard[1 ^ act] & empt | sing; //0 1
		buf32 = active->thisBoard[3 ^ act] & empt; //2 3
		buf32_2 = active->thisBoard[act] & ene; //1 0
		buf32_3 = active->thisBoard[act ^ 2] & ene; //3 2
		board k = buf32_1 ^ active->thisBoard[act ^ 1] & ~sing;
		hashNum = phash ^ tableZobrist[__lzcnt(sing)][act ^ 1] ^ tableZobrist[__lzcnt(k)][k & active->thisBoard[act ^ 3] ? act ^ 3 : act ^ 1];
		board bufqueens = buf32_2 ^ active->thisBoard[act];
		board bufsimple = bufqueens & active->thisBoard[act ^ 2];
		bufqueens ^= bufsimple;
		while (bufsimple) {
			hashNum ^= tableZobrist[__lzcnt(bufsimple ^ (bufsimple & (bufsimple - 1)))][act ^ 2];
			bufsimple &= bufsimple - 1;
		}
		while (bufqueens) {
			hashNum ^= tableZobrist[__lzcnt(bufqueens ^ (bufqueens & (bufqueens - 1)))][act];
			bufqueens &= bufqueens - 1;
		}
		if (hashTable[hashNum] != NULL) {
			children = hashTable[hashNum];
			while (children->next != NULL) {
				if (children->thisActive != act &&
					children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
					children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
					active->childrens.push_back(children);
					children->parents.push_back(active);
					break;
				}
				children = children->next;
			}
			if (children->thisActive != act &&
				children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
				children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
				active->childrens.push_back(children);
				children->parents.push_back(active);
			}
			else {
				children->next = cache[last] = new dataSet();
				deep[last] = deep[first] + 1;
				active->childrens.push_back(cache[last]);
				cache[last]->parents.push_back(active);
				cache[last]->thisBoard[act] = buf32_2;
				cache[last]->thisBoard[act ^ 1] = buf32_1;
				cache[last]->thisBoard[act ^ 2] = buf32_3;
				cache[last]->thisBoard[act ^ 3] = buf32;
				cache[last]->zobrist = hashNum;
				last++;
			}
		}
		else {
			cache[last] = hashTable[hashNum] = new dataSet();
			deep[last] = deep[first] + 1;
			active->childrens.push_back(cache[last]);
			cache[last]->parents.push_back(active);
			cache[last]->thisBoard[act] = buf32_2;
			cache[last]->thisBoard[act ^ 1] = buf32_1;
			cache[last]->thisBoard[act ^ 2] = buf32_3;
			cache[last]->thisBoard[act ^ 3] = buf32;
			cache[last]->zobrist = hashNum;
			last++;
		}
	}
}

void next(board possingle, board posenemy, board posemptys) {
	board nexttryLeftUp = (((possingle & possibleLeftUp) << 1 & posenemy) << 1) & posemptys;
	board nexttryRightDown = (((possingle & possibleRightDown) >> 1 & posenemy) >> 1) & posemptys;
	buf32 = ((possingle & possibleRightUp) << 7 | (possingle & possibleRightUp) >> 25) & posenemy;
	board nexttryRightUp = ((buf32 << 7) | (buf32 >> 25)) & posemptys;
	buf32 = ((possingle & possibleLeftDown) >> 7 | (possingle & possibleLeftDown) << 25) & posenemy;
	board nexttryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & posemptys;
	if (nexttryLeftUp || nexttryLeftDown || nexttryRightUp || nexttryRightDown) {
		if (nexttryLeftUp) {
			if (act && (possingle & maxLeftUp)) {
				getQueenTurn(to64(possingle << 2), to64(posenemy & ~(possingle << 1)), to64(posemptys));
			}
			else {
				next(possingle << 2, posenemy & ~(possingle << 1), posemptys);
			}
		}
		if (nexttryLeftDown) {
			if (!act && (possingle & maxLeftDown)) {
				getQueenTurn(to64((possingle >> 14) | (possingle << 18)), to64(posenemy & ~((possingle >> 7) | (possingle << 25))), to64(posemptys));
			}
			else {
				next(possingle >> 14 | possingle << 18, posenemy & ~((possingle >> 7) | (possingle << 25)), posemptys);
			}
		}
		if (nexttryRightDown) {
			if (!act && (possingle & maxRightDown)) {
				getQueenTurn(to64(possingle >> 2), to64(posenemy & ~(possingle >> 1)), to64(posemptys));
			}
			else {
				next(possingle >> 2, posenemy & ~(possingle >> 1), posemptys);
			}
		}
		if (nexttryRightUp) {
			if (act && (possingle & maxRightUp)) {
				getQueenTurn(to64((possingle << 14) | (possingle >> 18)), to64(posenemy & ~((possingle << 7) | (possingle >> 25))), to64(posemptys));
			}
			else {
				next(possingle << 14 | possingle >> 18, posenemy & ~((possingle << 7) | (possingle >> 25)), posemptys);
			}
		}
	}
	else {
		buf32_1 = active->thisBoard[act ^ 1] & ~posemptys | possingle; //0 1
		buf32 = active->thisBoard[act ^ 3] & ~posemptys | possingle; //2 3
		buf32_2 = active->thisBoard[act] & posenemy; //1 0
		buf32_3 = active->thisBoard[act ^ 2] & posenemy; //3 2
		hashNum = phash ^ tableZobrist[__lzcnt(possingle)][act ^ 3] ^ tableZobrist[__lzcnt((buf32_1 ^ active->thisBoard[act ^ 1]) & ~possingle)][act ^ 3];
		board bufqueens = buf32_2 ^ active->thisBoard[act];
		board bufsimple = bufqueens & active->thisBoard[act ^ 2];
		bufqueens ^= bufsimple;
		while (bufsimple) {
			hashNum ^= tableZobrist[__lzcnt(bufsimple ^ (bufsimple & (bufsimple - 1)))][act ^ 2];
			bufsimple &= bufsimple - 1;
		}
		while (bufqueens) {
			hashNum ^= tableZobrist[__lzcnt(bufqueens ^ (bufqueens & (bufqueens - 1)))][act];
			bufqueens &= bufqueens - 1;
		}
		if (hashTable[hashNum] != NULL) {
			children = hashTable[hashNum];
			while (children->next != NULL) {
				if (children->thisActive != act &&
					children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
					children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
					active->childrens.push_back(children);
					children->parents.push_back(active);
					break;
				}
				children = children->next;
			}
			if (children->thisActive != act &&
				children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
				children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
				active->childrens.push_back(children);
				children->parents.push_back(active);
			}
			else {
				children->next = cache[last] = new dataSet();
				deep[last] = deep[first] + 1;
				active->childrens.push_back(cache[last]);
				cache[last]->parents.push_back(active);
				cache[last]->thisBoard[act] = buf32_2;
				cache[last]->thisBoard[act ^ 1] = buf32_1;
				cache[last]->thisBoard[act ^ 2] = buf32_3;
				cache[last]->thisBoard[act ^ 3] = buf32;
				cache[last]->zobrist = hashNum;
				last++;
			}
		}
		else {
			cache[last] = hashTable[hashNum] = new dataSet();
			deep[last] = deep[first] + 1;
			active->childrens.push_back(cache[last]);
			cache[last]->parents.push_back(active);
			cache[last]->thisBoard[act] = buf32_2;
			cache[last]->thisBoard[act ^ 1] = buf32_1;
			cache[last]->thisBoard[act ^ 2] = buf32_3;
			cache[last]->thisBoard[act ^ 3] = buf32;
			cache[last]->zobrist = hashNum;
			last++;
		}
	}
}

void getTurn() {
	active = cache[first];
	act = active->thisActive;
	enemy = active->thisBoard[act];
	singles = active->thisBoard[act ^ 3];
	queens = active->thisBoard[act ^ 1] ^ singles;
	if (singles == 0 && queens == 0) {
		active->thisBoard[act ^ 1] = active->thisBoard[act ^ 3] = 0;
		active->thisBoard[act] = active->thisBoard[act ^ 2] = 0xFFFFFFFF;
		first++;
		return;
	}
	if (enemy == 0) {
		active->thisBoard[act ^ 1] = active->thisBoard[act ^ 3] = 0xFFFFFFFF;
		active->thisBoard[act] = active->thisBoard[act ^ 2] = 0;
		first++;
		return;
	}
	phash = active->zobrist;
	all = active->thisBoard[0] | active->thisBoard[1];
	emptys = ~all;
	bool isQueenTurn = false;
	tryLeftUp = (((singles & possibleLeftUp) << 1 & enemy) << 1) & emptys;
	tryRightDown = (((singles & possibleRightDown) >> 1 & enemy) >> 1) & emptys;
	buf32 = ((singles & possibleRightUp) << 7 | (singles & possibleRightUp) >> 25) & enemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25)) & emptys;
	buf32 = ((singles & possibleLeftDown) >> 7 | (singles & possibleLeftDown) << 25) & enemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & emptys;
	if (queens) {
		ebit buf64;
		unsigned char counter;
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
					getQueenTurn(mask, enemy64 & ~(mask >> counter), emptys64 | (mask >> (counter + 1)));
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
					getQueenTurn(mask, enemy64 & ~(mask << counter), emptys64 | (mask << (counter + 1)));
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
					getQueenTurn(mask, enemy64 & ~(mask << counter), emptys64 | (mask << (counter + 7)));
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
					getQueenTurn(mask, enemy64 & ~(mask >> counter), emptys64 | (mask >> (counter + 7)));
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
					tryLeftUp ^= buf;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						getQueenTurn(to64(buf32), to64(enemy & ~(buf32 >> 1)), to64(emptys | buf32 >> 2));
						buf &= buf - 1;
					}
				}
				while (tryLeftUp) {
					buf32 = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
					next(buf32, enemy & ~(buf32 >> 1), emptys | (buf32 >> 2));
					tryLeftUp &= tryLeftUp - 1;
				}
			}
			if (tryRightDown) {
				if (!act) {
					buf = tryRightDown & maxRightDownEndTurn;
					tryRightDown ^= buf;
					isQueenTurn = isQueenTurn || buf;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						getQueenTurn(to64(buf32), to64(enemy & ~(buf32 << 1)), to64(emptys | (buf32 << 2)));
						buf &= buf - 1;
					}
				}
				while (tryRightDown) {
					buf32 = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
					next(buf32, enemy & ~(buf32 << 1), emptys | (buf32 << 2));
					tryRightDown &= tryRightDown - 1;
				}
			}
			if (tryLeftDown) {
				if (!act) {
					buf = tryLeftDown & maxLeftDownEndTurn;
					tryLeftDown ^= buf;
					isQueenTurn = isQueenTurn || buf;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						getQueenTurn(to64(buf32), to64(enemy & ~(buf32 << 7 | buf32 >> 25)), to64(emptys | (buf32 << 14) | (buf32 >> 18)));
						buf &= buf - 1;
					}
				}
				while (tryLeftDown) {
					buf32 = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
					next(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | (buf32 << 14) | (buf32 >> 18));
					tryLeftDown &= tryLeftDown - 1;
				}
			}
			if (tryRightUp) {
				if (act) {
					buf = tryRightUp & maxRightUpEndTurn;
					tryRightUp ^= buf;
					isQueenTurn = isQueenTurn || buf;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						getQueenTurn(to64(buf32), to64(enemy & ~(buf32 >> 7 | buf32 << 25)), to64(emptys | (buf32 >> 14) | (buf32 << 18)));
						buf &= buf - 1;
					}
				}
				while (tryRightUp) {
					buf32 = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
					next(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | (buf32 >> 14) | (buf32 << 18));
					tryRightUp &= tryRightUp - 1;
				}
			}
		}
	}
	else {
		if (queens) {
			unsigned char counter = 1; //считает какой по счёту ход дамкой от конечного положения
			ebit buf64 = (queens64 & turnLeftUp64) << 1 & emptys64; //Ходим вверх-влево
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) { //можно ли срубить чисто теоретически с клеток?
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					buf32 = to32(mask);
					buf32_1 = to32(mask >> counter);
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						children = hashTable[hashNum];
						buf32 = ((active->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						while (children -> next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] && 
								children->thisBoard[act ^ 1] == buf32) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
						}
						else {
							children -> next = cache[last] = hashTable[hashNum] = new dataSet();
							deep[last] = deep[first] + 1;
							active->childrens.push_back(cache[last]);
							cache[last]->parents.push_back(active);
							cache[last]->thisBoard[2] = active->thisBoard[2];
							cache[last]->thisBoard[3] = active->thisBoard[3];
							cache[last]->thisBoard[act] = active->thisBoard[act];
							cache[last]->thisBoard[act ^ 1] = buf32;
							cache[last]->zobrist = hashNum;
							last++;
						}
					}
					else {
						cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[2] = active->thisBoard[2];
						cache[last]->thisBoard[3] = active->thisBoard[3];
						cache[last]->thisBoard[act] = active->thisBoard[act];
						cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | buf32) & ~buf32_1;
						cache[last]->zobrist = hashNum;
						last++;
					}
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
					buf32 = to32(mask);
					buf32_1 = to32(mask << counter);
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						buf32 = ((active->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
						}
						else {
							children->next = cache[last] = new dataSet();
							deep[last] = deep[first] + 1;
							active->childrens.push_back(cache[last]);
							cache[last]->parents.push_back(active);
							cache[last]->thisBoard[2] = active->thisBoard[2];
							cache[last]->thisBoard[3] = active->thisBoard[3];
							cache[last]->thisBoard[act] = active->thisBoard[act];
							cache[last]->thisBoard[act ^ 1] = buf32;
							cache[last]->zobrist = hashNum;
							last++;
						}
					}
					else {
						cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[2] = active->thisBoard[2];
						cache[last]->thisBoard[3] = active->thisBoard[3];
						cache[last]->thisBoard[act] = active->thisBoard[act];
						cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | buf32) & buf32_1;
						cache[last]->zobrist = hashNum;
						last++;
					}
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
					buf32 = to32(mask);
					buf32_1 = to32(mask << counter);
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						buf32 = ((active->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
						}
						else {
							children->next = cache[last] = new dataSet();
							deep[last] = deep[first] + 1;
							active->childrens.push_back(cache[last]);
							cache[last]->parents.push_back(active);
							cache[last]->thisBoard[2] = active->thisBoard[2];
							cache[last]->thisBoard[3] = active->thisBoard[3];
							cache[last]->thisBoard[act] = active->thisBoard[act];
							cache[last]->thisBoard[act ^ 1] = buf32;
							cache[last]->zobrist = hashNum;
							last++;
						}
					}
					else {
						cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[2] = active->thisBoard[2];
						cache[last]->thisBoard[3] = active->thisBoard[3];
						cache[last]->thisBoard[act] = active->thisBoard[act];
						cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | buf32) & ~buf32_1;
						cache[last]->zobrist = hashNum;
						last++;
					}
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
					buf32 = to32(mask);
					buf32_1 = to32(mask >> counter);
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						buf32 = ((active->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
						}
						else {
							children->next = cache[last] = new dataSet();
							deep[last] = deep[first] + 1;
							active->childrens.push_back(cache[last]);
							cache[last]->parents.push_back(active);
							cache[last]->thisBoard[2] = active->thisBoard[2];
							cache[last]->thisBoard[3] = active->thisBoard[3];
							cache[last]->thisBoard[act] = active->thisBoard[act];
							cache[last]->thisBoard[act ^ 1] = buf32;
							cache[last]->zobrist = hashNum;
							last++;
						}
					}
					else {
						cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[2] = active->thisBoard[2];
						cache[last]->thisBoard[3] = active->thisBoard[3];
						cache[last]->thisBoard[act] = active->thisBoard[act];
						cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | buf32) & ~buf32_1;
						cache[last]->zobrist = hashNum;
						last++;
					}
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
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n][2] ^ tableZobrist[n + 1][2];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (active->thisBoard[0] | buf) & ~(buf >> 1);
					buf32 = (singles | buf) & ~(buf >> 1);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == active->thisBoard[1] &&                          //act === 1, act потомка act === 0
							children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1 && children->thisActive != 1) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisBoard[1] == active->thisBoard[1] &&                          //act === 1, act потомка act === 0
						children->thisBoard[3] == active->thisBoard[3] &&
						children->thisBoard[2] == buf32 &&
						children->thisBoard[0] == buf32_1 && children->thisActive != 1) {
						active->childrens.push_back(children);
						children->parents.push_back(active);
					}
					else {
						children->next = cache[last] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[1] = active->thisBoard[1];
						cache[last]->thisBoard[3] = active->thisBoard[3];
						cache[last]->thisBoard[0] = buf32_1;
						cache[last]->thisBoard[2] = buf32;
						cache[last]->zobrist = hashNum;
						last++;
					}
				}
				else {
					cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(cache[last]);
					cache[last]->parents.push_back(active);
					cache[last]->thisBoard[1] = active->thisBoard[1];
					cache[last]->thisBoard[3] = active->thisBoard[3];
					cache[last]->thisBoard[2] = (singles | buf) & ~(buf >> 1);
					cache[last]->thisBoard[0] = (active->thisBoard[0] | buf) & ~(buf >> 1);
					cache[last]->zobrist = hashNum;
					last++;
				}
				tryLeftUp &= tryLeftUp - 1;
			}
			tryRightUp = ((singles & turnRightUp) << 7 | (singles & turnRightUp) >> 25) & emptys;
			isQueenTurn = isQueenTurn || tryRightUp;
			while (tryRightUp)
			{
				buf = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n < 25 ? n + 7 : n - 25][2] ^ tableZobrist[n][2];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (active->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
					buf32 = (singles | buf) & ~(buf >> 7 | buf << 25);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == active->thisBoard[1] &&                          //act === 1, act потомка act === 0
							children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1 && children->thisActive != 1) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisBoard[1] == active->thisBoard[1] &&                          //act === 1, act потомка act === 0
						children->thisBoard[3] == active->thisBoard[3] &&
						children->thisBoard[2] == buf32 &&
						children->thisBoard[0] == buf32_1 && children->thisActive != 1) {
						active->childrens.push_back(children);
						children->parents.push_back(active);
					}
					else {
						children->next = cache[last] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[1] = active->thisBoard[1];
						cache[last]->thisBoard[3] = active->thisBoard[3];
						cache[last]->thisBoard[0] = buf32_1;
						cache[last]->thisBoard[2] = buf32;
						cache[last]->zobrist = hashNum;
						last++;
					}
				}
				else {
					cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(cache[last]);
					cache[last]->parents.push_back(active);
					cache[last]->thisBoard[1] = active->thisBoard[1];
					cache[last]->thisBoard[3] = active->thisBoard[3];
					cache[last]->thisBoard[2] = (singles | buf) & ~(buf >> 7 | buf << 25);
					cache[last]->thisBoard[0] = (active->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
					cache[last]->zobrist = hashNum;
					last++;
				}
				tryRightUp &= tryRightUp - 1;
			}
		}
		else {
			tryRightDown = (singles & turnRightDown) >> 1 & emptys;
			isQueenTurn = isQueenTurn || tryRightUp;
			while (tryRightDown)
			{
				buf = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n][3] ^ tableZobrist[n - 1][3];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (active->thisBoard[1] | buf) & ~(buf << 1);
					buf32 = (singles | buf) & ~(buf << 1);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32 &&
							children->thisBoard[0] == active->thisBoard[0] &&
							children->thisBoard[2] == active->thisBoard[2] &&
							children->thisActive != 0) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisBoard[1] == buf32_1 &&
						children->thisBoard[3] == buf32 &&
						children->thisBoard[0] == active->thisBoard[0] &&
						children->thisBoard[2] == active->thisBoard[2] &&
						children->thisActive != 0) {
						active->childrens.push_back(children);
						children->parents.push_back(active);
					}
					else {
						children->next = cache[last] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[0] = active->thisBoard[0];
						cache[last]->thisBoard[2] = active->thisBoard[2];
						cache[last]->thisBoard[1] = buf32_1;
						cache[last]->thisBoard[3] = buf32;
						cache[last]->zobrist = hashNum;
						last++;
					}
				}
				else {
					cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(cache[last]);
					cache[last]->parents.push_back(active);
					cache[last]->thisBoard[0] = active->thisBoard[0];
					cache[last]->thisBoard[2] = active->thisBoard[2];
					cache[last]->thisBoard[1] = (active->thisBoard[1] | buf) & ~(buf << 1);
					cache[last]->thisBoard[3] = (singles | buf) & ~(buf << 1);
					cache[last]->zobrist = hashNum;
					last++;
				}
				tryRightDown &= tryRightDown - 1;
			}
			tryLeftDown = ((singles & turnLeftDown) >> 7 | (singles & turnLeftDown) << 25) & emptys;
			isQueenTurn = isQueenTurn || tryLeftDown;
			while (tryLeftDown)
			{
				buf = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n][3] ^ tableZobrist[n > 6 ? n - 7 : n + 25][3];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (active->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
					buf32 = (singles | buf) & ~(buf << 7 | buf >> 25);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32 &&
							children->thisBoard[0] == active->thisBoard[0] &&
							children->thisBoard[2] == active->thisBoard[2] &&
							children->thisActive != 0) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisBoard[1] == buf32_1 &&
						children->thisBoard[3] == buf32 &&
						children->thisBoard[0] == active->thisBoard[0] &&
						children->thisBoard[2] == active->thisBoard[2] &&
						children->thisActive != 0) {
						active->childrens.push_back(children);
						children->parents.push_back(active);
					}
					else {
						children->next = cache[last] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(cache[last]);
						cache[last]->parents.push_back(active);
						cache[last]->thisBoard[0] = active->thisBoard[0];
						cache[last]->thisBoard[2] = active->thisBoard[2];
						cache[last]->thisBoard[1] = buf32_1;
						cache[last]->thisBoard[3] = buf32;
						cache[last]->zobrist = hashNum;
						last++;
					}
				}
				else {
					cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(cache[last]);
					cache[last]->parents.push_back(active);
					cache[last]->thisBoard[0] = active->thisBoard[0];
					cache[last]->thisBoard[2] = active->thisBoard[2];
					cache[last]->thisBoard[1] = (active->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
					cache[last]->thisBoard[3] = (singles | buf) & ~(buf << 7 | buf >> 25);
					cache[last]->zobrist = hashNum;
					last++;
				}
				tryLeftDown &= tryLeftDown - 1;
			}
		}
		if (!isQueenTurn) { //нельзя походить
			active->thisBoard[act ^ 1] = active->thisBoard[act ^ 3] = 0;
			active->thisBoard[act] = active->thisBoard[act ^ 2] = 0xFFFFFFFF;
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
	turnLeftDown = possibleLeftDown | square[31] | square[23] | square[15] | square[13] | square[19] | square[25] | square[7];
	turnRightUp = possibleRightUp | square[2] | square[24] | square[30] | square[4] | square[10] | square[18] | square[26];
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
	ebit buf64 = 1;
	turnLeftUp64 = bity ^ buf64 ^ (buf64 << 8) ^ (buf64 << 16) ^ (buf64 << 24) ^ (buf64 << 31) ^ (buf64 << 37) ^ (buf64 << 43) ^ (buf64 << 49);
	turnRightUp64 = bity ^ (buf64 << 31) ^ (buf64 << 37) ^ (buf64 << 43) ^ (buf64 << 49) ^ (buf64 << 41) ^ (buf64 << 33) ^ (buf64 << 25);
	turnRightDown64 = bity ^ buf64 ^ (buf64 << 6) ^ (buf64 << 12) ^ (buf64 << 18) ^ (buf64 << 49) ^ (buf64 << 41) ^ (buf64 << 33) ^ (buf64 << 25);
	turnLeftDown64 = bity ^ buf64 ^ (buf64 << 6) ^ (buf64 << 12) ^ (buf64 << 18) ^ (buf64 << 8) ^ (buf64 << 16) ^ (buf64 << 24);
	possibleLeftDown64 = turnLeftDown64 ^ (buf64 << 31) ^ (buf64 << 23) ^ (buf64 << 15) ^ (buf64 << 7) ^ (buf64 << 13) ^ (buf64 << 19) ^ (buf64 << 25);
	possibleLeftUp64 = turnLeftUp64 ^ (buf64 << 7) ^ (buf64 << 15) ^ (buf64 << 23) ^ (buf64 << 30) ^ (buf64 << 36) ^ (buf64 << 42);
	possibleRightDown64 = turnRightDown64 ^ (buf64 << 7) ^ (buf64 << 13) ^ (buf64 << 19) ^ (buf64 << 26) ^ (buf64 << 34) ^ (buf64 << 42);
	possibleRightUp64 = turnRightUp64 ^ (buf64 << 24) ^ (buf64 << 30) ^ (buf64 << 36) ^ (buf64 << 42) ^ (buf64 << 34) ^ (buf64 << 26) ^ (buf64 << 18);
	initZobrist();
	hashNum = tableZobrist[31-31][3] ^ tableZobrist[31-5][3] ^ tableZobrist[31-11][3] ^ tableZobrist[31-17][3] ^
		tableZobrist[31-24][3] ^ tableZobrist[31-30][3] ^ tableZobrist[31-4][3] ^ tableZobrist[31-10][3] ^
		tableZobrist[31-23][3] ^ tableZobrist[31-29][3] ^ tableZobrist[31-3][3] ^ tableZobrist[31-9][3] ^
		tableZobrist[31-16][4] ^ tableZobrist[31-22][4] ^ tableZobrist[31-28][4] ^ tableZobrist[31-2][4] ^
		tableZobrist[31-15][4] ^ tableZobrist[31-21][4] ^ tableZobrist[31-27][4] ^ tableZobrist[31-1][4] ^
		tableZobrist[31-8][2] ^ tableZobrist[31-14][2] ^ tableZobrist[31-20][2] ^ tableZobrist[31-26][2] ^
		tableZobrist[31-7][2] ^ tableZobrist[31-13][2] ^ tableZobrist[31-19][2] ^ tableZobrist[31-25][2] ^
		tableZobrist[31-0][2] ^ tableZobrist[31-6][2] ^ tableZobrist[31-12][2] ^ tableZobrist[31-18][2];
	for (int iji = 0; iji < 4; iji++) {
		for (int ki = 0; ki < 32; ki++) {
			tableZobrist[ki][iji] ^= tableZobrist[ki][4];
		}
	}
	act = 0;
	memset(cache, NULL, buffersize);
	memset(hashTable, NULL, buffersize);
	cache[0] = hashTable[hashNum] = new dataSet();
	buf32 = square[0] | square[7] | square[8];
	buf32_1 = buf32 | (buf32 << 6) | (buf32 << 12) | (buf32 << 18);
	buf32 = square[23] | square[24] | square[31];
	buf32 = buf32 | (buf32 << 6) | (buf32 << 12) | (buf32 << 18) | (buf32 >> 26) | (buf32 >> 20) | (buf32 >> 14);
	cache[0]->zobrist = hashNum;
	cache[0]->thisBoard[0] = cache[0]->thisBoard[2] = buf32_1;
	cache[0]->thisBoard[1] = cache[0]->thisBoard[3] = buf32;
	deep[0] = 0;
	//num starttime = time(NULL);
	while (deep[first] <= 9) {
		getTurn();
	}
	//num endtime = time(NULL);
	cout << first - 1 << endl;
	cout << first - 1 - is() << endl;
	//memset(kkk, 0, 5);
	//memset(kkk1, 0, 5);
	//ish();
	//cout << endl << kkk[0] << endl << kkk[1] << endl << kkk[2] << endl << kkk[3] << endl << kkk[4] << endl << endl;
	//cout << endl << kkk1[0] << endl << kkk1[1] << endl << kkk1[2] << endl << kkk1[3] << endl << k
	system("pause");
}
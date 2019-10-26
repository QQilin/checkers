#include <iostream>
#include <intrin.h>
#include <vector>

using namespace std;

typedef uint32_t num;
typedef uint32_t board;
typedef uint64_t ebit;
typedef unsigned char uchar;

const board possibleLeftUp = 1048343118, possibleLeftDown = 2087865918, possibleRightDown = 4193372472, possibleRightUp = 955904457;
const board maxLeftUp = 536871432, maxLeftDown = 68173824, maxRightUp = 545259528, maxRightDown = 1065216,
maxLeftUpEndTurn = 2147485728, maxLeftDownEndTurn = 4161, maxRightUpEndTurn = 133152, maxRightDownEndTurn = 266304,
maxUpEndTurn = 2147616800, maxDownEndTurn = 266305;
const board turnLeftUp = 2130507486, turnLeftDown = 4277857982, turnRightUp = 2113795549, turnRightDown = 4261014972;
const board x24 = 676375380, x15 = 1149833250, x06 = 2198077440, y34 = 4294705152, y25 = 127038, y16 = 4032, y07 = 131073, y04 = 4294832577, y57 = 134718, yall = 4294967295;
const ebit turnLeftUp64 = 6728049291456, turnLeftDown64 = 578613681905792, turnRightUp64 = 4520419389889, turnRightDown64 = 13456098582912,
possibleLeftUp64 = 2260201140288, possibleLeftDown64 = 578611491913728, possibleRightUp64 = 35315642817, possibleRightDown64 = 9040804561152;
const ebit bit8y04 = 4294832577, bit8y57 = 578609404182528, bity = 578613699015105;
board all, enemy, emptys, singles, queens;
board tryLeftUp, tryRightUp, tryLeftDown, tryRightDown, tryLeftUpQueen, tryRightUpQueen, tryLeftDownQueen, tryRightDownQueen;
board nexttryLeftUp, nexttryRightUp, nexttryLeftDown, nexttryRightDown;
board buf;
board buf32_1, buf32_2, buf32_3;
ebit queens64, enemy64, emptys64;
ebit mask, buf64No2;
num stokehash;

const board x1 = 266305, x2 = 34087040, x3 = 68174080, x4 = 136347650, x5 = 272695300, x6 = 545260040, x7 = 1090520080, x8 = 2147616800;

board tableZobrist[32][5];

const num buffersize = 64 * 1024 * 1024;
const num HASHNUM = buffersize - 1;
//0 - все шашки белых
//1 - все шашки чёхных
//2 - все простые шашки белых
//3 - все простые шашки чёрных
uchar act; //Содержит предыдущую активную сторону, 
//1 сейчас ход белых
//0 сейчас ход чёрных
num hashNum; //Данный хэш-номер

uchar n, n1;

num initNum() {
	num randNum = rand() % 256;
	for (num l = 0; l < 3; l++) {
		randNum <<= 8;
		randNum += rand() % 256;
	}
	randNum &= HASHNUM;
	return randNum;
}

void initZobrist() {
	for (num i = 0; i < 32; i++) {
		for (num j = 0; j < 5; j++) {
			tableZobrist[i][j] = initNum();
		}
	}
}

struct Node {
	board thisBoard[4];
	Node* next;
	uchar thisAct;
	bool isTerminal;
	vector<Node*> childrens;
	float getvalue() {
		float myvalue = 30 * __popcnt(thisBoard[0]) - 14 * __popcnt(thisBoard[2] & x7) - 15 * __popcnt(thisBoard[2] & x6) - 16 * __popcnt(thisBoard[2] & x5) -
			+17 * __popcnt(thisBoard[2] & x4) - 18 * __popcnt(thisBoard[2] & x3) - 19 * __popcnt(thisBoard[2] & x2) - 20 * __popcnt(thisBoard[2] & x1);
		float enemyvalue = 30 * __popcnt(thisBoard[1]) - 14 * __popcnt(thisBoard[3] & x2) - 15 * __popcnt(thisBoard[3] & x3) - 16 * __popcnt(thisBoard[3] & x4) -
			+17 * __popcnt(thisBoard[3] & x5) - 18 * __popcnt(thisBoard[3] & x6) - 19 * __popcnt(thisBoard[3] & x7) - 20 * __popcnt(thisBoard[3] & x8);
		return 1 - 2 * enemyvalue / (myvalue + enemyvalue);
	}
	Node() {
		next = NULL;
		thisAct = 1 ^ act;
		isTerminal = false;
	}
	Node(board b0, board b1, board b2, board b3) {
		thisBoard[0] = b0;
		thisBoard[1] = b1;
		thisBoard[2] = b2;
		thisBoard[3] = b3;
		next = NULL;
		thisAct = 1 ^ act;
		isTerminal = false;
	}
};

void printNode(Node* element) {
	cout << element->thisBoard[0] << " " << element->thisBoard[1] << " " << element->thisBoard[2] << " " << element->thisBoard[3] << endl;
}

Node* hashTable[buffersize];
Node* children;
num phash;

board to32(ebit bit64) {
	return (board)(((bit64 & bit8y57) >> 32) | bit64 & bit8y04);
}

ebit to64(board bit32) {
	ebit rez = bit32;
	return (rez << 32) & bit8y57 | rez & bit8y04;
}

void getQueenTurn(Node* element, ebit possingle, ebit posenemy, ebit posemptys) {
	uchar counter;
	bool isQueenTurn = true;
	ebit bitTry, buf64 = (possingle & possibleLeftUp64) << 1; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) << 1 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 1;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(element, bitTry, posenemy & ~(bitTry >> counter), posemptys);
				counter++;
				bitTry = ((bitTry & turnLeftUp64) << 1) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleLeftUp64 & posemptys) << 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (possingle & possibleRightDown64) >> 1; //Ходим вниз-вправо, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) >> 1 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 1;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(element, bitTry, posenemy & ~(bitTry << counter), posemptys);
				counter++;
				bitTry = ((bitTry & turnRightDown64) >> 1) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleRightDown64 & posemptys) >> 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (possingle & possibleRightUp64) << 7; //Ходим вверх-вправо, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) << 7 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 7;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(element, bitTry, posenemy & ~(bitTry >> counter), posemptys);
				counter += 7;
				bitTry = ((bitTry & turnRightUp64) << 7) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleRightUp64 & posemptys) << 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	buf64 = (possingle & possibleLeftDown64) >> 7; //Ходим вниз-влево, оставляем только те клетки, с которых можно рубить
	while (buf64) { //можно ли срубить чисто теоретически с клеток?
		bitTry = (buf64 & posenemy) >> 7 & posemptys; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
		counter = 7;
		if (bitTry) {
			isQueenTurn = false;
			do {
				getQueenTurn(element, bitTry, posenemy & ~(bitTry << counter), posemptys);
				counter += 7;
				bitTry = ((bitTry & turnLeftDown64) >> 7) & posemptys;
			} while (bitTry);
		}
		buf64 = (buf64 & possibleLeftDown64 & posemptys) >> 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
	}
	if (isQueenTurn) {
		board sing = to32(possingle);
		board empt = ~to32(posemptys);
		board ene = to32(posenemy);
		buf32_1 = element->thisBoard[1 ^ act] & empt | sing; //0 1  //Все шашки актиивной стороны сейчас
		board buf32 = element->thisBoard[3 ^ act] & empt; //2 3
		buf32_2 = element->thisBoard[act] & ene; //1 0
		buf32_3 = element->thisBoard[act ^ 2] & ene; //3 2
		board k = buf32_1 ^ element->thisBoard[act ^ 1] ^ sing;
		hashNum = phash ^ tableZobrist[__lzcnt(sing)][act ^ 1] ^ tableZobrist[__lzcnt(k)][k & element->thisBoard[act ^ 3] ? act ^ 3 : act ^ 1];
		board bufqueens = (buf32_2 ^ buf32_3) ^ (element->thisBoard[act] ^ element->thisBoard[act ^ 2]);
		board bufsimple = buf32_3 ^ element->thisBoard[act ^ 2];
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
				if (children->thisAct != act &&
					children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
					children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
					element->childrens.push_back(children);
					break;
				}
				children = children->next;
			}
			if (children->next == NULL && children->thisAct != act &&
				children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
				children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
				element->childrens.push_back(children);
			}
			else {
				children = children->next = new Node();
				element->childrens.push_back(children);
				children->thisBoard[act] = buf32_2;
				children->thisBoard[act ^ 1] = buf32_1;
				children->thisBoard[act ^ 2] = buf32_3;
				children->thisBoard[act ^ 3] = buf32;
			}
		}
		else {
			hashTable[hashNum] = new Node();
			element->childrens.push_back(hashTable[hashNum]);
			hashTable[hashNum]->thisBoard[act] = buf32_2;
			hashTable[hashNum]->thisBoard[act ^ 1] = buf32_1;
			hashTable[hashNum]->thisBoard[act ^ 2] = buf32_3;
			hashTable[hashNum]->thisBoard[act ^ 3] = buf32;
		}
	}
}

void next(Node* element, board possingle, board posenemy, board posemptys) {
	board nexttryLeftUp = (((possingle & possibleLeftUp) << 1 & posenemy) << 1) & posemptys;
	board nexttryRightDown = (((possingle & possibleRightDown) >> 1 & posenemy) >> 1) & posemptys;
	board buf32 = ((possingle & possibleRightUp) << 7 | (possingle & possibleRightUp) >> 25) & posenemy;
	board nexttryRightUp = ((buf32 << 7) | (buf32 >> 25)) & posemptys;
	buf32 = ((possingle & possibleLeftDown) >> 7 | (possingle & possibleLeftDown) << 25) & posenemy;
	board nexttryLeftDown = ((buf32 >> 7) | (buf32 << 25)) & posemptys;
	if (nexttryLeftUp || nexttryLeftDown || nexttryRightUp || nexttryRightDown) {
		if (nexttryLeftUp) {
			if (act && (possingle & maxLeftUp)) {
				getQueenTurn(element, to64(possingle << 2), to64(posenemy & ~(possingle << 1)), to64(posemptys));
			}
			else {
				next(element, possingle << 2, posenemy & ~(possingle << 1), posemptys);
			} 
		}
		if (nexttryLeftDown) {
			if (!act && (possingle & maxLeftDown)) {
				getQueenTurn(element, to64((possingle >> 14) | (possingle << 18)), to64(posenemy & ~((possingle >> 7) | (possingle << 25))), to64(posemptys));
			}
			else {
				next(element, possingle >> 14 | possingle << 18, posenemy & ~((possingle >> 7) | (possingle << 25)), posemptys);
			}
		}
		if (nexttryRightDown) {
			if (!act && (possingle & maxRightDown)) {
				getQueenTurn(element, to64(possingle >> 2), to64(posenemy & ~(possingle >> 1)), to64(posemptys));
			}
			else {
				next(element, possingle >> 2, posenemy & ~(possingle >> 1), posemptys);
			}
		}
		if (nexttryRightUp) {
			if (act && (possingle & maxRightUp)) {
				getQueenTurn(element, to64((possingle << 14) | (possingle >> 18)), to64(posenemy & ~((possingle << 7) | (possingle >> 25))), to64(posemptys));
			}
			else {
				next(element, possingle << 14 | possingle >> 18, posenemy & ~((possingle << 7) | (possingle >> 25)), posemptys);
			}
		}
	}
	else {
		buf32_1 = element->thisBoard[act ^ 1] & ~posemptys | possingle; //0 1
		buf32 = element->thisBoard[act ^ 3] & ~posemptys | possingle; //2 3
		buf32_2 = element->thisBoard[act] & posenemy; //1 0
		buf32_3 = element->thisBoard[act ^ 2] & posenemy; //3 2
		if (buf32_1 != element->thisBoard[act ^ 1]) {
			hashNum = phash ^ tableZobrist[__lzcnt(possingle)][act ^ 3] ^ tableZobrist[__lzcnt(emptys ^ posemptys)][act ^ 3];
		}
		else {
			hashNum = phash;
		}
		board bufqueens = (buf32_2 ^ buf32_3) ^ (element->thisBoard[act] ^ element->thisBoard[act ^ 2]);
		board bufsimple = buf32_3 ^ element->thisBoard[act ^ 2];
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
				if (children->thisAct != act &&
					children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
					children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
					element->childrens.push_back(children);
					break;
				}
				children = children->next;
			}
			if (children->next == NULL && children->thisAct != act &&
				children->thisBoard[act] == buf32_2 && children->thisBoard[act ^ 2] == buf32_3 &&
				children->thisBoard[act ^ 3] == buf32 && children->thisBoard[act ^ 1] == buf32_1) {
				element->childrens.push_back(children); //Если 
			}
			else {
				children = children->next = new Node();
				element->childrens.push_back(children);
				children->thisBoard[act] = buf32_2;
				children->thisBoard[act ^ 1] = buf32_1;
				children->thisBoard[act ^ 2] = buf32_3;
				children->thisBoard[act ^ 3] = buf32;
			}
		}
		else {
			hashTable[hashNum] = new Node();
			element->childrens.push_back(hashTable[hashNum]);
			hashTable[hashNum]->thisBoard[act] = buf32_2;
			hashTable[hashNum]->thisBoard[act ^ 1] = buf32_1;
			hashTable[hashNum]->thisBoard[act ^ 2] = buf32_3;
			hashTable[hashNum]->thisBoard[act ^ 3] = buf32;
		}
	}
}

bool getChop(Node* element) {
	act = element->thisAct;
	enemy = element->thisBoard[act];
	singles = element->thisBoard[act ^ 3];
	queens = element->thisBoard[act ^ 1] ^ singles;
	all = element->thisBoard[0] | element->thisBoard[1];
	emptys = ~all;
	phash = stokehash;
	board buf32 = element->thisBoard[2];
	while (buf32) {
		buf = buf32 ^ (buf32 & (buf32 - 1));
		phash ^= tableZobrist[__lzcnt(buf)][2];
		buf32 &= buf32 - 1;
	}
	buf32 = element->thisBoard[2] ^ element->thisBoard[0];
	while (buf32) {
		buf = buf32 ^ (buf32 & (buf32 - 1));
		phash ^= tableZobrist[__lzcnt(buf)][0];
		buf32 &= buf32 - 1;
	}
	buf32 = element->thisBoard[3];
	while (buf32) {
		buf = buf32 ^ (buf32 & (buf32 - 1));
		phash ^= tableZobrist[__lzcnt(buf)][3];
		buf32 &= buf32 - 1;
	}
	buf32 = element->thisBoard[3] ^ element->thisBoard[1];
	while (buf32) {
		buf = buf32 ^ (buf32 & (buf32 - 1));
		phash ^= tableZobrist[__lzcnt(buf)][1];
		buf32 &= buf32 - 1;
	}
	tryLeftUp = (((singles & possibleLeftUp) << 1 & enemy) << 1) & emptys;
	tryRightDown = (((singles & possibleRightDown) >> 1 & enemy) >> 1)& emptys;
	buf32 = ((singles & possibleRightUp) << 7 | (singles & possibleRightUp) >> 25)& enemy;
	tryRightUp = ((buf32 << 7) | (buf32 >> 25))& emptys;
	buf32 = ((singles & possibleLeftDown) >> 7 | (singles & possibleLeftDown) << 25) & enemy;
	tryLeftDown = ((buf32 >> 7) | (buf32 << 25))& emptys;
	bool isQueenTurn = tryLeftUp || tryLeftDown || tryRightDown || tryRightUp;
	if (queens) {
		uchar counter, counterfirst = 1;
		queens64 = queens;
		queens64 = (queens64 << 32) & bit8y57 | queens64 & bit8y04;
		emptys64 = emptys;
		emptys64 = (emptys64 << 32) & bit8y57 | emptys64 & bit8y04;
		enemy64 = enemy;
		enemy64 = (enemy64 << 32) & bit8y57 | enemy64 & bit8y04;
		ebit bitTry, buf64 = (queens64 & possibleLeftUp64) << 1; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) << 1 & emptys64; //можно ли срубить? (хранит конечные положения)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) { //Пока есть шашки, которые срубили
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					getQueenTurn(element, mask, enemy64 & ~(mask >> counter), emptys64 | (mask >> (counter + counterfirst)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				counter++;
				bitTry = ((bitTry & turnLeftUp64) << 1) & emptys64; //Для вычисления всех конечных положений сдвигаем по направлению движения срубившие шашки
			}
			counterfirst++;
			buf64 = (buf64 & possibleLeftUp64 & emptys64) << 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
		buf64 = (queens64 & possibleRightDown64) >> 1; //Ходим вниз-вправо, оставляем только те клетки, с которых можно рубить
		counterfirst = 1;
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) >> 1 & emptys64; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					getQueenTurn(element, mask, enemy64 & ~(mask << counter), emptys64 | (mask << (counter + counterfirst)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				counter++;
				bitTry = ((bitTry & turnRightDown64) >> 1)& emptys64;
			}
			counterfirst++;
			buf64 = (buf64 & possibleRightDown64 & emptys64) >> 1; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
		buf64 = (queens64 & possibleRightUp64) << 7; //Ходим вверх-вправо, оставляем только те клетки, с которых можно рубить
		counterfirst = 7;
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) << 7 & emptys64; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					getQueenTurn(element, mask, enemy64 & ~(mask >> counter), emptys64 | (mask >> (counter + counterfirst)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				bitTry = ((bitTry & turnRightUp64) << 7) & emptys64;
				counter += 7;
			}
			counterfirst += 7;
			buf64 = (buf64 & possibleRightUp64 & emptys64) << 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
		buf64 = (queens64 & possibleLeftDown64) >> 7; //Ходим вниз-влево, оставляем только те клетки, с которых можно рубить
		counterfirst = 7;
		while (buf64) { //можно ли срубить чисто теоретически с клеток?
			bitTry = (buf64 & enemy64) >> 7 & emptys64; //можно ли срубить? (конечное положение)(есть ли враг на текущей клетке, и пуста ли следущая клетка)
			isQueenTurn = isQueenTurn || bitTry; //Срубили ли хоть раз королевой?
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			while (bitTry) {
				buf64No2 = bitTry;
				do {
					mask = buf64No2 ^ (buf64No2 & (buf64No2 - 1));
					getQueenTurn(element, mask, enemy64 & ~(mask << counter), emptys64 | (mask << (counter + counterfirst)));
					buf64No2 &= buf64No2 - 1;
				} while (buf64No2);
				bitTry = (bitTry & turnLeftDown64) >> 7 & emptys64;
				counter += 7;
			}
			counterfirst += 7;
			buf64 = (buf64 & possibleLeftDown64 & emptys64) >> 7; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
		}
	}
	if (tryLeftUp) {
		if (act) {
			buf = tryLeftUp & maxLeftUpEndTurn;
			tryLeftUp ^= buf;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				getQueenTurn(element, to64(buf32), to64(enemy & ~(buf32 >> 1)), to64(emptys | buf32 >> 2));
				buf &= buf - 1;
			}
		}
		while (tryLeftUp) {
			buf32 = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
			next(element, buf32, enemy & ~(buf32 >> 1), emptys | (buf32 >> 2));
			tryLeftUp &= tryLeftUp - 1;
		}
	}
	if (tryRightDown) {
		if (!act) {
			buf = tryRightDown & maxRightDownEndTurn;
			tryRightDown ^= buf;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				getQueenTurn(element, to64(buf32), to64(enemy & ~(buf32 << 1)), to64(emptys | (buf32 << 2)));
				buf &= buf - 1;
			}
		}
		while (tryRightDown) {
			buf32 = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
			next(element, buf32, enemy & ~(buf32 << 1), emptys | (buf32 << 2));
			tryRightDown &= tryRightDown - 1;
		}
	}
	if (tryLeftDown) {
		if (!act) {
			buf = tryLeftDown & maxLeftDownEndTurn;
			tryLeftDown ^= buf;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				getQueenTurn(element, to64(buf32), to64(enemy & ~(buf32 << 7 | buf32 >> 25)), to64(emptys | (buf32 << 14) | (buf32 >> 18)));
				buf &= buf - 1;
			}
		}
		while (tryLeftDown) {
			buf32 = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
			next(element, buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | (buf32 << 14) | (buf32 >> 18));
			tryLeftDown &= tryLeftDown - 1;
		}
	}
	if (tryRightUp) {
		if (act) {
			buf = tryRightUp & maxRightUpEndTurn;
			tryRightUp ^= buf;
			while (buf) {
				buf32 = buf ^ (buf & (buf - 1));
				getQueenTurn(element, to64(buf32), to64(enemy & ~(buf32 >> 7 | buf32 << 25)), to64(emptys | (buf32 >> 14) | (buf32 << 18)));
				buf &= buf - 1;
			}
		}
		while (tryRightUp) {
			buf32 = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
			next(element, buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | (buf32 >> 14) | (buf32 << 18));
			tryRightUp &= tryRightUp - 1;
		}
	}
	return isQueenTurn;
}

void getTurn(Node* element) {
	board buf32;
	bool isQueenTurn = getChop(element);
	if(!isQueenTurn) {
		if (queens) {
			uchar counter = 1; //считает какой по счёту ход дамкой от конечного положения
			ebit bitTry, buf64 = (queens64 & turnLeftUp64) << 1 & emptys64; //Ходим вверх-влево
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) { //можно ли срубить чисто теоретически с клеток?
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					buf32 = to32(mask); //Содержит походившую дамку
					buf32_1 = to32(mask >> counter); //Содержит первоначальное положение
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						children = hashTable[hashNum];
						buf32 = ((element->thisBoard[act ^ 1] | buf32) & ~buf32_1);  //Теперь содержит все шашки активной стороны
						while (children->next != NULL) {
							if (children->thisAct != act &&
								children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
								children->thisBoard[act] == element->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								element->childrens.push_back(children);
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisAct != act &&
							children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[act] == element->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							element->childrens.push_back(children);
							
						}
						else {
							children = children->next = hashTable[hashNum] = new Node();
							element->childrens.push_back(children);
							children->thisBoard[2] = element->thisBoard[2];
							children->thisBoard[3] = element->thisBoard[3];
							children->thisBoard[act] = element->thisBoard[act];
							children->thisBoard[act ^ 1] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
						hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
						hashTable[hashNum]->thisBoard[act] = element->thisBoard[act];
						hashTable[hashNum]->thisBoard[act ^ 1] = (element->thisBoard[act ^ 1] | buf32) & ~buf32_1;
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
						buf32 = ((element->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisAct != act &&
								children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
								children->thisBoard[act] == element->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								element->childrens.push_back(children);
								
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisAct != act &&
							children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[act] == element->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							element->childrens.push_back(children);
							
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[2] = element->thisBoard[2];
							children->thisBoard[3] = element->thisBoard[3];
							children->thisBoard[act] = element->thisBoard[act];
							children->thisBoard[act ^ 1] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
						hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
						hashTable[hashNum]->thisBoard[act] = element->thisBoard[act];
						hashTable[hashNum]->thisBoard[act ^ 1] = (element->thisBoard[act ^ 1] | buf32) & ~buf32_1;
					}
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter++;
				buf64 = (buf64 & turnRightDown64) >> 1 & emptys64; //Текущая клетка пуста, со следущей клетки чисто теоретически можно срубить
			}
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnRightUp64) << 7 & emptys64;; //Ходим вверх-вправо, оставляем только те клетки, с которых можно ходить
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) {
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					buf32 = to32(mask);
					buf32_1 = to32(mask >> counter);
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						buf32 = ((element->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisAct != act &&
								children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
								children->thisBoard[act] == element->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								element->childrens.push_back(children);
								
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisAct != act &&
							children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[act] == element->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							element->childrens.push_back(children);
							
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[2] = element->thisBoard[2];
							children->thisBoard[3] = element->thisBoard[3];
							children->thisBoard[act] = element->thisBoard[act];
							children->thisBoard[act ^ 1] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
						hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
						hashTable[hashNum]->thisBoard[act] = element->thisBoard[act];
						hashTable[hashNum]->thisBoard[act ^ 1] = (element->thisBoard[act ^ 1] | buf32) & ~buf32_1;
					}
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter += 7;
				buf64 = (buf64 & turnRightUp64) << 7 & emptys64; //Текущая клетка пуста, со следущей клетки чисто теоретически можно ходить
			}
			counter = 7; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnLeftDown64) >> 7 & emptys64;; //Ходим вниз-влево
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) {
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					buf32 = to32(mask);
					buf32_1 = to32(mask << counter);
					hashNum = phash ^ tableZobrist[__lzcnt(buf32)][act ^ 1] ^ tableZobrist[__lzcnt(buf32_1)][act ^ 1];
					if (hashTable[hashNum] != NULL) {
						buf32 = ((element->thisBoard[act ^ 1] | buf32) & ~buf32_1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisAct != act &&
								children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
								children->thisBoard[act] == element->thisBoard[act] &&
								children->thisBoard[act ^ 1] == buf32) {
								element->childrens.push_back(children);
								
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisAct != act &&
							children->thisBoard[2] == element->thisBoard[2] && children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[act] == element->thisBoard[act] &&
							children->thisBoard[act ^ 1] == buf32) {
							element->childrens.push_back(children);
							
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[2] = element->thisBoard[2];
							children->thisBoard[3] = element->thisBoard[3];
							children->thisBoard[act] = element->thisBoard[act];
							children->thisBoard[act ^ 1] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
						hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
						hashTable[hashNum]->thisBoard[act] = element->thisBoard[act];
						hashTable[hashNum]->thisBoard[act ^ 1] = (element->thisBoard[act ^ 1] | buf32) & ~buf32_1;
					}
					bitTry &= bitTry - 1;
				} while (bitTry);
				counter += 7;
				buf64 = (buf64 & turnLeftDown64) >> 7 & emptys64; //Текущая клетка пуста, со следущей клетки чисто теоретически можно ходить
			}
		}
		if (act) {
			tryLeftUp = (singles & turnLeftUp) << 1 & emptys;
			tryLeftUpQueen = tryLeftUp & maxUpEndTurn;
			isQueenTurn = isQueenTurn || tryLeftUp;
			if (tryLeftUpQueen) {
				tryLeftUp ^= tryLeftUpQueen;
				do {
					buf = tryLeftUpQueen ^ (tryLeftUpQueen & (tryLeftUpQueen - 1));
					n = __lzcnt(buf);
					hashNum = phash ^ tableZobrist[n][0] ^ tableZobrist[n + 1][2];
					if (hashTable[hashNum] != NULL) {
						buf32_1 = (element->thisBoard[0] | buf) & ~(buf >> 1);
						buf32 = (singles) & ~(buf >> 1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
								children->thisBoard[3] == element->thisBoard[3] &&
								children->thisBoard[2] == buf32 &&
								children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
								element->childrens.push_back(children);
								
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
							children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
							element->childrens.push_back(children);
							
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[1] = element->thisBoard[1];
							children->thisBoard[3] = element->thisBoard[3];
							children->thisBoard[0] = buf32_1;
							children->thisBoard[2] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[1] = element->thisBoard[1];
						hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
						hashTable[hashNum]->thisBoard[2] = (singles) & ~(buf >> 1);
						hashTable[hashNum]->thisBoard[0] = (element->thisBoard[0] | buf) & ~(buf >> 1);
					}
					tryLeftUpQueen &= tryLeftUpQueen - 1;
				} while (tryLeftUpQueen);
			}
			while (tryLeftUp)
			{
				buf = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n][2] ^ tableZobrist[n + 1][2];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (element->thisBoard[0] | buf) & ~(buf >> 1);
					buf32 = (singles | buf) & ~(buf >> 1);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
							children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
							element->childrens.push_back(children);
							
							break;
						}
						children = children->next;
					}
					if (children->next == NULL && children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
						children->thisBoard[3] == element->thisBoard[3] &&
						children->thisBoard[2] == buf32 &&
						children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
						element->childrens.push_back(children);
						
					}
					else {
						children = children->next = new Node();
						element->childrens.push_back(children);
						children->thisBoard[1] = element->thisBoard[1];
						children->thisBoard[3] = element->thisBoard[3];
						children->thisBoard[0] = buf32_1;
						children->thisBoard[2] = buf32;
					}
				}
				else {
					hashTable[hashNum] = new Node();
					element->childrens.push_back(hashTable[hashNum]);
					hashTable[hashNum]->thisBoard[1] = element->thisBoard[1];
					hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
					hashTable[hashNum]->thisBoard[2] = (singles | buf) & ~(buf >> 1);
					hashTable[hashNum]->thisBoard[0] = (element->thisBoard[0] | buf) & ~(buf >> 1);
				}
				tryLeftUp &= tryLeftUp - 1;
			}
			tryRightUp = ((singles & turnRightUp) << 7 | (singles & turnRightUp) >> 25) & emptys;
			tryRightUpQueen = tryRightUp & maxUpEndTurn;
			isQueenTurn = isQueenTurn || tryRightUp;
			if (tryRightUpQueen) {
				tryRightUp ^= tryRightUpQueen;
				do {
					buf = tryRightUpQueen ^ (tryRightUpQueen & (tryRightUpQueen - 1));
					n = __lzcnt(buf);
					hashNum = phash ^ tableZobrist[n < 25 ? n + 7 : n - 25][2] ^ tableZobrist[n][0];
					if (hashTable[hashNum] != NULL) {
						buf32_1 = (element->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
						buf32 = (singles) & ~(buf >> 7 | buf << 25);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
								children->thisBoard[3] == element->thisBoard[3] &&
								children->thisBoard[2] == buf32 &&
								children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
								element->childrens.push_back(children);
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
							children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
							element->childrens.push_back(children);
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[1] = element->thisBoard[1];
							children->thisBoard[3] = element->thisBoard[3];
							children->thisBoard[0] = buf32_1;
							children->thisBoard[2] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[1] = element->thisBoard[1];
						hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
						hashTable[hashNum]->thisBoard[2] = (singles) & ~(buf >> 7 | buf << 25);
						hashTable[hashNum]->thisBoard[0] = (element->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
					}
					tryRightUpQueen &= tryRightUpQueen - 1;
				} while (tryRightUpQueen);
			}
			while (tryRightUp)
			{
				buf = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n < 25 ? n + 7 : n - 25][2] ^ tableZobrist[n][2];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (element->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
					buf32 = (singles | buf) & ~(buf >> 7 | buf << 25);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
							children->thisBoard[3] == element->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
							element->childrens.push_back(children);
							
							break;
						}
						children = children->next;
					}
					if (children->next == NULL && children->thisBoard[1] == element->thisBoard[1] &&                          //act === 1, act потомка act === 0
						children->thisBoard[3] == element->thisBoard[3] &&
						children->thisBoard[2] == buf32 &&
						children->thisBoard[0] == buf32_1 && children->thisAct != 1) {
						element->childrens.push_back(children);
						
					}
					else {
						children = children->next = new Node();
						element->childrens.push_back(children);
						children->thisBoard[1] = element->thisBoard[1];
						children->thisBoard[3] = element->thisBoard[3];
						children->thisBoard[0] = buf32_1;
						children->thisBoard[2] = buf32;
					}
				}
				else {
					hashTable[hashNum] = new Node();
					element->childrens.push_back(hashTable[hashNum]);
					hashTable[hashNum]->thisBoard[1] = element->thisBoard[1];
					hashTable[hashNum]->thisBoard[3] = element->thisBoard[3];
					hashTable[hashNum]->thisBoard[2] = (singles | buf) & ~(buf >> 7 | buf << 25);
					hashTable[hashNum]->thisBoard[0] = (element->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
				}
				tryRightUp &= tryRightUp - 1;
			}
		}
		else {
			tryRightDown = (singles & turnRightDown) >> 1 & emptys;
			isQueenTurn = isQueenTurn || tryRightUp;
			tryRightDownQueen = tryRightDown & maxDownEndTurn;
			if (tryRightDownQueen) {
				tryRightDown ^= tryRightDownQueen;
				do {
					buf = tryRightDownQueen ^ (tryRightDownQueen & (tryRightDownQueen - 1));
					n = __lzcnt(buf);
					hashNum = phash ^ tableZobrist[n][1] ^ tableZobrist[n - 1][3];
					if (hashTable[hashNum] != NULL) {
						buf32_1 = (element->thisBoard[1] | buf) & ~(buf << 1);
						buf32 = (singles) & ~(buf << 1);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisBoard[1] == buf32_1 &&
								children->thisBoard[3] == buf32 &&
								children->thisBoard[0] == element->thisBoard[0] &&
								children->thisBoard[2] == element->thisBoard[2] &&
								children->thisAct != 0) {
								element->childrens.push_back(children);
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32 &&
							children->thisBoard[0] == element->thisBoard[0] &&
							children->thisBoard[2] == element->thisBoard[2] &&
							children->thisAct != 0) {
							element->childrens.push_back(children);
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[0] = element->thisBoard[0];
							children->thisBoard[2] = element->thisBoard[2];
							children->thisBoard[1] = buf32_1;
							children->thisBoard[3] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[0] = element->thisBoard[0];
						hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
						hashTable[hashNum]->thisBoard[1] = (element->thisBoard[1] | buf) & ~(buf << 1);
						hashTable[hashNum]->thisBoard[3] = (singles) & ~(buf << 1);
					}
					tryRightDownQueen &= tryRightDownQueen - 1;
				} while (tryRightDownQueen);
			}
			while (tryRightDown)
			{
				buf = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n][3] ^ tableZobrist[n - 1][3];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (element->thisBoard[1] | buf) & ~(buf << 1);
					buf32 = (singles | buf) & ~(buf << 1);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32 &&
							children->thisBoard[0] == element->thisBoard[0] &&
							children->thisBoard[2] == element->thisBoard[2] &&
							children->thisAct != 0) {
							element->childrens.push_back(children);
							break;
						}
						children = children->next;
					}
					if (children->next == NULL && children->thisBoard[1] == buf32_1 &&
						children->thisBoard[3] == buf32 &&
						children->thisBoard[0] == element->thisBoard[0] &&
						children->thisBoard[2] == element->thisBoard[2] &&
						children->thisAct != 0) {
						element->childrens.push_back(children);
					}
					else {
						children = children->next = new Node();
						element->childrens.push_back(children);
						children->thisBoard[0] = element->thisBoard[0];
						children->thisBoard[2] = element->thisBoard[2];
						children->thisBoard[1] = buf32_1;
						children->thisBoard[3] = buf32;
					}
				}
				else {
					hashTable[hashNum] = new Node();
					element->childrens.push_back(hashTable[hashNum]);
					hashTable[hashNum]->thisBoard[0] = element->thisBoard[0];
					hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
					hashTable[hashNum]->thisBoard[1] = (element->thisBoard[1] | buf) & ~(buf << 1);
					hashTable[hashNum]->thisBoard[3] = (singles | buf) & ~(buf << 1);
				}
				tryRightDown &= tryRightDown - 1;
			}
			tryLeftDown = ((singles & turnLeftDown) >> 7 | (singles & turnLeftDown) << 25) & emptys;
			tryLeftDownQueen = tryLeftDown & maxDownEndTurn;
			isQueenTurn = isQueenTurn || tryLeftDown;
			if (tryLeftDownQueen) {
				tryLeftDown ^= tryLeftDownQueen;
				do {
					buf = tryLeftDownQueen ^ (tryLeftDownQueen & (tryLeftDownQueen - 1));
					n = __lzcnt(buf);
					hashNum = phash ^ tableZobrist[n][1] ^ tableZobrist[n > 6 ? n - 7 : n + 25][3];
					if (hashTable[hashNum] != NULL) {
						buf32_1 = (element->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
						buf32 = (singles) & ~(buf << 7 | buf >> 25);
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisBoard[1] == buf32_1 &&
								children->thisBoard[3] == buf32 &&
								children->thisBoard[0] == element->thisBoard[0] &&
								children->thisBoard[2] == element->thisBoard[2] &&
								children->thisAct != 0) {
								element->childrens.push_back(children);
								break;
							}
							children = children->next;
						}
						if (children->next == NULL && children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32 &&
							children->thisBoard[0] == element->thisBoard[0] &&
							children->thisBoard[2] == element->thisBoard[2] &&
							children->thisAct != 0) {
							element->childrens.push_back(children);
						}
						else {
							children = children->next = new Node();
							element->childrens.push_back(children);
							children->thisBoard[0] = element->thisBoard[0];
							children->thisBoard[2] = element->thisBoard[2];
							children->thisBoard[1] = buf32_1;
							children->thisBoard[3] = buf32;
						}
					}
					else {
						hashTable[hashNum] = new Node();
						element->childrens.push_back(hashTable[hashNum]);
						hashTable[hashNum]->thisBoard[0] = element->thisBoard[0];
						hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
						hashTable[hashNum]->thisBoard[1] = (element->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
						hashTable[hashNum]->thisBoard[3] = (singles) & ~(buf << 7 | buf >> 25);
					}
					tryLeftDownQueen &= tryLeftDownQueen - 1;
				} while (tryLeftDownQueen);
			}
			while (tryLeftDown)
			{
				buf = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
				n = __lzcnt(buf);
				hashNum = phash ^ tableZobrist[n][3] ^ tableZobrist[n > 6 ? n - 7 : n + 25][3];
				if (hashTable[hashNum] != NULL) {
					buf32_1 = (element->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
					buf32 = (singles | buf) & ~(buf << 7 | buf >> 25);
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32 &&
							children->thisBoard[0] == element->thisBoard[0] &&
							children->thisBoard[2] == element->thisBoard[2] &&
							children->thisAct != 0) {
							element->childrens.push_back(children);
							break;
						}
						children = children->next;
					}
					if (children->next == NULL && children->thisBoard[1] == buf32_1 &&
						children->thisBoard[3] == buf32 &&
						children->thisBoard[0] == element->thisBoard[0] &&
						children->thisBoard[2] == element->thisBoard[2] &&
						children->thisAct != 0) {
						element->childrens.push_back(children);
					}
					else {
						children = children->next = new Node();
						element->childrens.push_back(children);
						children->thisBoard[0] = element->thisBoard[0];
						children->thisBoard[2] = element->thisBoard[2];
						children->thisBoard[1] = buf32_1;
						children->thisBoard[3] = buf32;
					}
				}
				else {
					hashTable[hashNum] = new Node();
					element->childrens.push_back(hashTable[hashNum]);
					hashTable[hashNum]->thisBoard[0] = element->thisBoard[0];
					hashTable[hashNum]->thisBoard[2] = element->thisBoard[2];
					hashTable[hashNum]->thisBoard[1] = (element->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
					hashTable[hashNum]->thisBoard[3] = (singles | buf) & ~(buf << 7 | buf >> 25);
				}
				tryLeftDown &= tryLeftDown - 1;
			}
		}
		if (!isQueenTurn) { //нельзя походить
			element->isTerminal = true;
		}
	}
}

float canChop(Node* element, float alpha, float beta) {
	if (element->childrens.size() || !getChop(element)) {
		return element->getvalue();
	}
	else {
		float value;
		if (element->thisAct) {
			for (uchar i = 0; i < element->childrens.size(); i++) {
				value = canChop(element->childrens[i], alpha, beta);
				alpha = alpha > value ? alpha : value;
				if (beta <= alpha) {
					return alpha;
				}
			}
			return alpha;
		}
		else {
			for (uchar i = 0; i < element->childrens.size(); i++) {
				value = canChop(element->childrens[i], alpha, beta);
				beta = beta < value ? beta : value;
				if (beta <= alpha) {
					return beta;
				}
			}
			return beta;
		}
	}
}

float alphabeta(Node* element, uchar depth, float alpha, float beta) {
	if (depth == 0) {
		return canChop(element, alpha, beta);
	}
	if (element->childrens.size() == 0) {
		getTurn(element);
	}
	if (element->isTerminal) {
		return element->thisAct ? -1 : 1;
	}
	float value;
	if (element->thisAct) {
		for (uchar i = 0; i < element->childrens.size(); i++) {
			value = alphabeta(element->childrens[i], depth - 1, alpha, beta);
			alpha = alpha > value ? alpha : value;
			if (beta <= alpha) {
				return alpha;
			}
		}
		return alpha;
	}
	else {
		for (uchar i = 0; i < element->childrens.size(); i++) {
			value = alphabeta(element->childrens[i], depth - 1, alpha, beta);
			beta = beta < value ? beta : value;
			if (beta <= alpha) {
				return beta;
			}
		}
		return beta;
	}
}

void getvariant(uchar theact, board b0, board b1, board b2, board b3) {
	memset(hashTable, NULL, buffersize);
	act = theact;
	Node* element = new Node(b0, b1, b2, b3);
	getTurn(element);
	Node* maxNode = element->childrens[0];
	const char deep = 12;
	if (element->childrens.size() > 1) {
		float current, max = alphabeta(element->childrens[0], deep, -1, 1);
		for (uchar i = 1; i < element->childrens.size(); i++) {
			current = alphabeta(element->childrens[i], deep, max, 1);
			if (current > max) {
				max = current;
				maxNode = element->childrens[i];
			}
		}
	}
	printNode(maxNode);
}

int main() {
	initZobrist();
	stokehash = tableZobrist[0][4];
	for (int iji = 1; iji < 32; iji++) {
		stokehash ^= tableZobrist[iji][4];
	}
	for (int iji = 0; iji < 4; iji++) {
		for (int ki = 0; ki < 32; ki++) {
			tableZobrist[ki][iji] ^= tableZobrist[ki][4];
		}
	}
	getvariant(0, 102527425, 3783396920, 102527425, 3783396920);
	return 0;
}
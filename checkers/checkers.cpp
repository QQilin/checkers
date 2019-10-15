#include <iostream>
#include <queue>
#include <vector>

using namespace std;

typedef uint32_t num;
typedef uint32_t board;
typedef uint64_t ebit;

#define to32(thisEbit) ((board)(((thisEbit & bit8y57) >> 32) | thisEbit & bit8y04))

board square[32];
board tsquare[32];
const board possibleLeftUp = 1048343118, possibleLeftDown = 2087865918, possibleRightDown = 4193372472, possibleRightUp = 955904457;
board all, enemy, emptys, singles, queens, enemySimple, emptySimpleReverse;
board tryLeftUp, tryRightUp, tryLeftDown, tryRightDown;
ebit bitTry;
board maxLeftUp, maxLeftDown, maxRightUp, maxRightDown, maxLeftUpEndTurn, maxLeftDownEndTurn, maxRightUpEndTurn, maxRightDownEndTurn;
board turnLeftUp, turnLeftDown, turnRightUp, turnRightDown;
board x[7];
board y[8];
board x24, x15, x06, y34, y25, y16, y07;
board buf;
ebit buf64;
board buf32, buf32_1, buf32_2, buf32_3;
board y04, y57, yall;
ebit bit8y04, bit8y57, bity;
ebit queens64, enemy64, emptys64;
num first = 0, last = 1;
ebit mask, buf64No2;
bool isQueenTurn;
ebit turnLeftUp64, turnLeftDown64, turnRightUp64, turnRightDown64, possibleLeftUp64, possibleLeftDown64, possibleRightUp64, possibleRightDown64;
unsigned char counter; 
bool isTurn;

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

const num buffersize = 8 * 1024 * 1024;
const num HASHNUM = buffersize - 1;
//0 - все шашки белых
//1 - все шашки чёхных
//2 - все простые шашки белых
//3 - все простые шашки чёрных
unsigned char act; //Содержит предыдущую активную сторону, 
//1 сейчас ход белых
//0 сейчас ход чёрных
num hashNum; //Данный хэш-номер

board ReverseBits(board argument) {
	board result = 0;
	unsigned number = 0;
	number = (argument << 24) >> 24;
	number = (number & 0x55) << 1 | (number & 0xAA) >> 1;
	number = (number & 0x33) << 2 | (number & 0xCC) >> 2;
	number = (number & 0x0F) << 4 | (number & 0xF0) >> 4;
	result |= number;
	result <<= 8;
	number = (argument << 16) >> 24;
	number = (number & 0x55) << 1 | (number & 0xAA) >> 1;
	number = (number & 0x33) << 2 | (number & 0xCC) >> 2;
	number = (number & 0x0F) << 4 | (number & 0xF0) >> 4;
	result |= number;
	result <<= 8;
	number = (argument << 8) >> 24;
	number = (number & 0x55) << 1 | (number & 0xAA) >> 1;
	number = (number & 0x33) << 2 | (number & 0xCC) >> 2;
	number = (number & 0x0F) << 4 | (number & 0xF0) >> 4;
	result |= number;
	result <<= 8;
	number = argument >> 24;
	number = (number & 0x55) << 1 | (number & 0xAA) >> 1;
	number = (number & 0x33) << 2 | (number & 0xCC) >> 2;
	number = (number & 0x0F) << 4 | (number & 0xF0) >> 4;
	result |= number;
	return result;
}

struct dataSet {
	board thisBoard[4];
	unsigned char thisActive;
	dataSet* next;
	vector <dataSet*> parents;
	vector <dataSet*> childrens;
	dataSet() {
		next = NULL;
		thisActive = 1 ^ act;
	}
};


dataSet* hashTable[buffersize];

dataSet* cache[buffersize]; //Буфер значений, из которого берём следущий элемент для просчёта

dataSet* active;

dataSet* children;

void is(int n) {
	/*for (int i = 0; i < last; i++) {
		if (cache[i]->thisActive == cache[last]->thisActive && cache[i]->thisBoard[0] == cache[last]->thisBoard[0] && cache[i]->thisBoard[1] == cache[last]->thisBoard[1]
			&& cache[i]->thisBoard[2] == cache[last]->thisBoard[2] && cache[i]->thisBoard[3] == cache[last]->thisBoard[3]) {
			cout << endl << n << endl << last << endl;
			return;
		}
	}*/
}

unsigned char deep[buffersize];

//Будет хэш-таблица, схранящая ссылки на списки вида struct data {datash, data* next} //В хэш-таблицу можно добавлять ссылки на детей, родителей; также записывать значение их оценочных функций
//Будет обычный массив(2) со ссылками на данные, в котором также записаны родители этих самых данных в виде индексов в данном массиве(2). 
//Если вдруг hashTable[hashNum(result)] != NULL, то ищём по списку, действительно у нас есть уже такой элемент. Если этот элемент уже действительно просчитан, то ищем его в массиве(2)	
//Ещё одна идея - сопоставить элементы действительно как граф, а именно в виде матрицы смежности. Т.е. пусть есть char mas[n][n]. Если hashTable[hashNum(result)] != NULL, то mas[this][result] = mas[result][this] = k (k - номер в хэш-таблице, 0 - нет номера)

//Хэш-таблица будет существовать для разных уровней??? Т.е. мы сначала проверяем наличие элемента в этом уровне. Потом проверяем наличие элемента в предыдущий уровнях. 

queue<pos> turns;
queue<pos64> qturns;
pos current(possibleLeftUp, possibleLeftUp, possibleLeftUp);
pos64 current64(possibleLeftUp, possibleLeftUp, possibleLeftUp);

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
		buf32_1 = active->thisBoard[1 ^ act] & ~to32(current64.posemptys) | to32(current64.possingle); //0 1
		buf32 = active->thisBoard[3 ^ act] & ~(to32(current64.posemptys) | to32(current64.possingle)); //2 3
		buf32_2 = active->thisBoard[act] & to32(current64.posenemy); //1 0
		buf32_3 = active->thisBoard[act ^ 2] & to32(current64.posenemy); //3 2
		hashNum = ((buf32_2 | buf32_1) ^ ReverseBits((buf32 | buf32_3))) & HASHNUM;
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
				is(240); last++;
			}
		}
		else {
			children = cache[last] = hashTable[hashNum] = new dataSet();
			deep[last] = deep[first] + 1;
			active->childrens.push_back(children);
			children->parents.push_back(active);
			children->thisBoard[act] = buf32_2;
			children->thisBoard[act ^ 1] = buf32_1;
			children->thisBoard[act ^ 2] = buf32_3;
			children->thisBoard[act ^ 3] = buf32;
			is(252); last++;
		}
		if (!qturns.empty()) {
			getQueenTurn();
		}
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
	isTurn = false;
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
		if (tryLeftUp) {
			if (act && (current.possingle & maxLeftUp)) {
				isQueenTurn = true;
				qturns.push(pos64(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
			}
			else {
				isTurn = true;
				turns.push(pos(current.possingle << 2, current.posenemy & ~(current.possingle << 1), current.posemptys | current.possingle));
			}
		}
		if (tryLeftDown) {
			if (!act && (current.possingle & maxLeftDown)) {
				isQueenTurn = true;
				qturns.push(pos64((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
			}
			else {
				isTurn = true;
				turns.push(pos((current.possingle >> 14) | (current.possingle << 18), current.posenemy & ~((current.possingle >> 7) | (current.possingle << 25)), current.posemptys | current.possingle));
			}
		}
		if (tryRightDown) {
			if (!act && (current.possingle & maxRightDown)) {
				isQueenTurn = true;
				qturns.push(pos64(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
			}
			else {
				isTurn = true;
				turns.push(pos(current.possingle >> 2, current.posenemy & ~(current.possingle >> 1), current.posemptys | current.possingle));
			}
		}
		if (tryRightUp) {
			if (act && (current.possingle & maxRightUp)) {
				isQueenTurn = true;
				qturns.push(pos64((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
			}
			else {
				isTurn = true;
				turns.push(pos((current.possingle << 14) | (current.possingle >> 18), current.posenemy & ~((current.possingle << 7) | (current.possingle >> 25)), current.posemptys | current.possingle));
			}
		}
		if (isTurn)
			next();
	}
	else {
		buf32_1 = (active->thisBoard[act ^ 1] | current.possingle) & ~current.posemptys; //0 1
		buf32 = (singles | current.possingle) & ~current.posemptys; //2 3
		buf32_2 = active->thisBoard[act] & current.posenemy; //1 0
		buf32_3 = active->thisBoard[act ^ 2] & current.posenemy; //3 2
		hashNum = ((buf32_2 | buf32_1) ^ ReverseBits((buf32 | buf32_3))) & HASHNUM;
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
				is(347); last++;
			}
		}
		else {
			children = cache[last] = hashTable[hashNum] = new dataSet();
			deep[last] = deep[first] + 1;
			active->childrens.push_back(children);
			children->parents.push_back(active);
			children->thisBoard[act] = buf32_2;
			children->thisBoard[act ^ 1] = buf32_1;
			children->thisBoard[act ^ 2] = buf32_3;
			children->thisBoard[act ^ 3] = buf32;
			is(359); last++;
		}
		if (!turns.empty()) {
			next();
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
	all = active->thisBoard[0] | active->thisBoard[1];
	emptys = ~all;
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
	isTurn = false;
	if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown || isQueenTurn) {
		if (tryLeftUp || tryLeftDown || tryRightUp || tryRightDown) {
			if (tryLeftUp) {
				if (act) {
					buf = tryLeftUp & maxLeftUpEndTurn;
					tryLeftUp ^= buf;
					isQueenTurn = isQueenTurn || buf;
					while (buf) {
						buf32 = buf ^ (buf & (buf - 1));
						qturns.push(pos64(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
						buf &= buf - 1;
					}
				}
				isTurn = isTurn || tryLeftUp;
				while (tryLeftUp) {
					buf32 = tryLeftUp ^ (tryLeftUp & (tryLeftUp - 1));
					turns.push(pos(buf32, enemy & ~(buf32 >> 1), emptys | buf32 >> 2));
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
						qturns.push(pos64(buf32, enemy & ~(buf32 << 1), emptys | buf32 << 2));
						buf &= buf - 1;
					}
				}
				isTurn = isTurn || tryRightDown;
				while (tryRightDown) {
					buf32 = tryRightDown ^ (tryRightDown & (tryRightDown - 1));
					turns.push(pos(buf32, enemy & ~(buf32 << 1), emptys | buf32 << 2));
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
						qturns.push(pos64(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
						buf &= buf - 1;
					}
				}
				isTurn = isTurn || tryLeftDown;
				while (tryLeftDown) {
					buf32 = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
					turns.push(pos(buf32, enemy & ~(buf32 << 7 | buf32 >> 25), emptys | buf32 << 14 | buf32 >> 18));
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
						qturns.push(pos64(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
						buf &= buf - 1;
					}
				}
				isTurn = isTurn || tryRightUp;
				while (tryRightUp) {
					buf32 = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
					turns.push(pos(buf32, enemy & ~(buf32 >> 7 | buf32 << 25), emptys | buf32 >> 14 | buf32 << 18));
					tryRightUp &= tryRightUp - 1;
				}
			}
			if (isTurn)
				next();
		}
		if (isQueenTurn) {
			getQueenTurn();
		}
	}
	else {
		if (queens) {
			emptySimpleReverse = ReverseBits(~(singles | active->thisBoard[act ^ 2]));
			counter = 1; //считает какой по счёту ход дамкой от конечного положения
			buf64 = (queens64 & turnLeftUp64) << 1 & emptys64; //Ходим вверх-влево, оставляем только те клетки, с которых можно рубить
			isQueenTurn = isQueenTurn || buf64;
			while (buf64) { //можно ли срубить чисто теоретически с клеток?
				bitTry = buf64;
				do {
					mask = bitTry ^ (bitTry & (bitTry - 1));
					hashNum = (((emptys | to32(mask)) & ~to32(mask >> counter)) ^ emptySimpleReverse) & HASHNUM;
					if (hashTable[hashNum] != NULL) {
						children = hashTable[hashNum];
						while (children -> next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] && 
								children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter))) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter))) {
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
							cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter);
							is(592); last++;
						}
					}
					else {
						children = cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(children);
						children->parents.push_back(active);
						children->thisBoard[2] = active->thisBoard[2];
						children->thisBoard[3] = active->thisBoard[3];
						children->thisBoard[act] = active->thisBoard[act];
						children->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter);
						is(604); last++;
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
					hashNum = (((emptys | to32(mask)) & ~to32(mask << counter)) ^ emptySimpleReverse) & HASHNUM;
					if (hashTable[hashNum] != NULL) {
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] &&
								children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter))) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter))) {
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
							cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter);
							is(648); last++;
						}
					}
					else {
						children = cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(children);
						children->parents.push_back(active);
						children->thisBoard[2] = active->thisBoard[2];
						children->thisBoard[3] = active->thisBoard[3];
						children->thisBoard[act] = active->thisBoard[act];
						children->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter);
						is(660); last++;
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
					hashNum = (((emptys | to32(mask)) & ~to32(mask << counter)) ^ emptySimpleReverse) & HASHNUM;
					if (hashTable[hashNum] != NULL) {
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] &&
								children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter))) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter))) {
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
							cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter);
							is(704); last++;
						}
					}
					else {
						children = cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(children);
						children->parents.push_back(active);
						children->thisBoard[2] = active->thisBoard[2];
						children->thisBoard[3] = active->thisBoard[3];
						children->thisBoard[act] = active->thisBoard[act];
						children->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask << counter);
						is(716); last++;
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
					hashNum = (((emptys | to32(mask)) & ~to32(mask >> counter)) ^ emptySimpleReverse) & HASHNUM;
					if (hashTable[hashNum] != NULL) {
						children = hashTable[hashNum];
						while (children->next != NULL) {
							if (children->thisActive != act &&
								children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
								children->thisBoard[act] == active->thisBoard[act] &&
								children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter))) {
								active->childrens.push_back(children);
								children->parents.push_back(active);
								break;
							}
							children = children->next;
						}
						if (children->thisActive != act &&
							children->thisBoard[2] == active->thisBoard[2] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[act] == active->thisBoard[act] &&
							children->thisBoard[act ^ 1] == ((active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter))) {
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
							cache[last]->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter);
							is(760); last++;
						}
					}
					else {
						children = cache[last] = hashTable[hashNum] = new dataSet();
						deep[last] = deep[first] + 1;
						active->childrens.push_back(children);
						children->parents.push_back(active);
						children->thisBoard[2] = active->thisBoard[2];
						children->thisBoard[3] = active->thisBoard[3];
						children->thisBoard[act] = active->thisBoard[act];
						children->thisBoard[act ^ 1] = (active->thisBoard[act ^ 1] | to32(mask)) & ~to32(mask >> counter);
						is(772); last++;
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
				buf32_1 = (active->thisBoard[0] | buf) & ~(buf >> 1);
				buf32 = (singles | buf) & ~(buf >> 1);
				hashNum = ((active->thisBoard[1] | buf32_1) ^ ReverseBits(buf32 | active->thisBoard[3])) & HASHNUM;
				if (hashTable[hashNum] != NULL) {
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisActive != 1 &&  //act === 1, act потомка act === 0
							children->thisBoard[1] == active->thisBoard[1] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisActive != 1 &&
						children->thisBoard[1] == active->thisBoard[1] && children->thisBoard[3] == active->thisBoard[3] &&
						children->thisBoard[2] == buf32 &&
						children->thisBoard[0] == buf32_1) {
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
						is(818); last++;
					}
				}
				else {
					children = cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(children);
					children->parents.push_back(active);
					children->thisBoard[1] = active->thisBoard[1];
					children->thisBoard[3] = active->thisBoard[3];
					children->thisBoard[2] = buf32;
					children->thisBoard[0] = buf32_1;
					is(830); last++;
				}
				tryLeftUp &= tryLeftUp - 1;
			}
			tryRightUp = ((singles & turnRightUp) << 7 | (singles & turnRightUp) >> 25) & emptys;
			isQueenTurn = isQueenTurn || tryRightUp;
			while (tryRightUp)
			{
				buf = tryRightUp ^ (tryRightUp & (tryRightUp - 1));
				buf32_1 = (active->thisBoard[0] | buf) & ~(buf >> 7 | buf << 25);
				buf32 = (singles | buf) & ~(buf >> 7 | buf << 25);
				hashNum = hashNum = ((active->thisBoard[1] | buf32_1) ^ ReverseBits((buf32 | active->thisBoard[3]))) & HASHNUM;
				if (hashTable[hashNum] != NULL) {
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisActive != 1 &&
							children->thisBoard[1] == active->thisBoard[1] && children->thisBoard[3] == active->thisBoard[3] &&
							children->thisBoard[2] == buf32 &&
							children->thisBoard[0] == buf32_1) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisActive != 1 &&
						children->thisBoard[1] == active->thisBoard[1] && children->thisBoard[3] == active->thisBoard[3] &&
						children->thisBoard[2] == buf32 &&
						children->thisBoard[0] == buf32_1) {
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
						is(871); last++;
					}
				}
				else {
					children = cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(children);
					children->parents.push_back(active);
					children->thisBoard[1] = active->thisBoard[1];
					children->thisBoard[3] = active->thisBoard[3];
					children->thisBoard[2] = buf32;
					children->thisBoard[0] = buf32_1;
					is(883); last++;
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
				buf32_1 = (active->thisBoard[1] | buf) & ~(buf << 1);
				buf32 = (singles | buf) & ~(buf << 1);
				hashNum = ((active->thisBoard[0] | buf32_1) ^ ReverseBits((buf32 | active->thisBoard[2]))) & HASHNUM;
				if (hashTable[hashNum] != NULL) {
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisActive != 0 &&
							children->thisBoard[0] == active->thisBoard[0] && children->thisBoard[2] == active->thisBoard[2] &&
							children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisActive != 0 &&
						children->thisBoard[0] == active->thisBoard[0] && children->thisBoard[2] == active->thisBoard[2] &&
						children->thisBoard[1] == buf32_1 &&
						children->thisBoard[3] == buf32) {
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
						is(926); last++;
					}
				}
				else {
					children = cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(children);
					children->parents.push_back(active);
					cache[last]->thisBoard[0] = active->thisBoard[0];
					cache[last]->thisBoard[2] = active->thisBoard[2];
					cache[last]->thisBoard[1] = buf32_1;
					cache[last]->thisBoard[3] = buf32;
					is(938); last++;
				}
				tryRightDown &= tryRightDown - 1;
			}
			tryLeftDown = ((singles & turnLeftDown) >> 7 | (singles & turnLeftDown) << 25) & emptys;
			isQueenTurn = isQueenTurn || tryLeftDown;
			while (tryLeftDown)
			{
				buf = tryLeftDown ^ (tryLeftDown & (tryLeftDown - 1));
				buf32_1 = (active->thisBoard[1] | buf) & ~(buf << 7 | buf >> 25);
				buf32 = (singles | buf) & ~(buf << 7 | buf >> 25);
				hashNum = ((active->thisBoard[0] | buf32_1) ^ ReverseBits((buf32 | active->thisBoard[2]))) & HASHNUM;
				if (hashTable[hashNum] != NULL) {
					children = hashTable[hashNum];
					while (children->next != NULL) {
						if (children->thisActive != 0 &&
							children->thisBoard[0] == active->thisBoard[0] && children->thisBoard[2] == active->thisBoard[2] &&
							children->thisBoard[1] == buf32_1 &&
							children->thisBoard[3] == buf32) {
							active->childrens.push_back(children);
							children->parents.push_back(active);
							break;
						}
						children = children->next;
					}
					if (children->thisActive != 0 &&
						children->thisBoard[0] == active->thisBoard[0] && children->thisBoard[2] == active->thisBoard[2] &&
						children->thisBoard[1] == buf32_1 &&
						children->thisBoard[3] == buf32) {
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
						is(979); last++;
					}
				}
				else {
					children = cache[last] = hashTable[hashNum] = new dataSet();
					deep[last] = deep[first] + 1;
					active->childrens.push_back(children);
					children->parents.push_back(active);
					cache[last]->thisBoard[0] = active->thisBoard[0];
					cache[last]->thisBoard[2] = active->thisBoard[2];
					cache[last]->thisBoard[1] = buf32_1;
					cache[last]->thisBoard[3] = buf32;
					is(991); last++;
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
	buf64 = 1;
	turnLeftUp64 = bity ^ buf64 ^ (buf64 << 8) ^ (buf64 << 16) ^ (buf64 << 24) ^ (buf64 << 31) ^ (buf64 << 37) ^ (buf64 << 43) ^ (buf64 << 49);
	turnRightUp64 = bity ^ (buf64 << 31) ^ (buf64 << 37) ^ (buf64 << 43) ^ (buf64 << 49) ^ (buf64 << 41) ^ (buf64 << 33) ^ (buf64 << 25);
	turnRightDown64 = bity ^ buf64 ^ (buf64 << 6) ^ (buf64 << 12) ^ (buf64 << 18) ^ (buf64 << 49) ^ (buf64 << 41) ^ (buf64 << 33) ^ (buf64 << 25);
	turnLeftDown64 = bity ^ buf64 ^ (buf64 << 6) ^ (buf64 << 12) ^ (buf64 << 18) ^ (buf64 << 8) ^ (buf64 << 16) ^ (buf64 << 24);
	possibleLeftDown64 = turnLeftDown64 ^ (buf64 << 31) ^ (buf64 << 23) ^ (buf64 << 15) ^ (buf64 << 7) ^ (buf64 << 13) ^ (buf64 << 19) ^ (buf64 << 25);
	possibleLeftUp64 = turnLeftUp64 ^ (buf64 << 7) ^ (buf64 << 15) ^ (buf64 << 23) ^ (buf64 << 30) ^ (buf64 << 36) ^ (buf64 << 42);
	possibleRightDown64 = turnRightDown64 ^ (buf64 << 7) ^ (buf64 << 13) ^ (buf64 << 19) ^ (buf64 << 26) ^ (buf64 << 34) ^ (buf64 << 42);
	possibleRightUp64 = turnRightUp64 ^ (buf64 << 24) ^ (buf64 << 30) ^ (buf64 << 36) ^ (buf64 << 42) ^ (buf64 << 34) ^ (buf64 << 26) ^ (buf64 << 18);
	buf32 = square[0] | square[7] | square[8];
	buf32_1 = buf32 | (buf32 << 6) | (buf32 << 12) | (buf32 << 18);
	buf32 = square[23] | square[24] | square[31];
	buf32 = buf32 | (buf32 << 6) | (buf32 << 12) | (buf32 << 18) | (buf32 >> 26) | (buf32 >> 20) | (buf32 >> 14);
	hashNum = ((buf32 | buf32_1) ^ ReverseBits(buf32 | buf32_1)) & HASHNUM;
	act = 0;
	memset(cache, NULL, buffersize);
	memset(hashTable, NULL, buffersize);
	cache[0] = hashTable[hashNum] = new dataSet();
	cache[0]->thisBoard[0] = cache[0]->thisBoard[2] = buf32_1;
	cache[0]->thisBoard[1] = cache[0]->thisBoard[3] = buf32;
	deep[0] = 0;
	int dp = 10;
	while (deep[first] <= dp) {
		getTurn();
	} 
	cout << first - 1 << endl;
}
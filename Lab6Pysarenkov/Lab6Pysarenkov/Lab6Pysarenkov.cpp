#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>

struct AdjV {
	int v;
	AdjV* next;
	AdjV(int v) {
		this->v = v;
		this->next = nullptr;
	}
};

struct Vertex {//Суміжні вершини
	AdjV* adj;
	Vertex() {}
	Vertex(AdjV* adj) {
		this->adj = adj;
	}
};

struct Set {//Множина, подана списком
	int n;
	Set* next;
	Set(int n) {
		this->n = n;
		this->next = nullptr;
	}
};

struct Edge { //Ребра
	int v1;
	int v2;
	Edge() {}
	Edge(int v1, int v2) {
		this->v1 = v1;
		this->v2 = v2;
	}
};

Set* addVertex(Set* s, int n) //Додавання вершини до множини, або ж об'єднання точкової множини із вершини n і множини s
{
	bool contains = false;
	Set* un = nullptr;
	Set* unt = nullptr;
	Set* siter = s;
	while (siter) {
		Set* sn = new Set(siter->n);
		if (!un)
		{
			un = sn;
			unt = sn;
		}
		else {
			unt->next = sn;
			unt = sn;
		}
		if (siter->n == n)
		{
			contains = true;
		}
		siter = siter->next;
	}
	if (contains)
		return un;
	Set* sn = new Set(n);
	if (!un)
	{
		un = sn;
		unt = sn;
	}
	else {
		unt->next = sn;
		unt = sn;
	}
	return un;
}

Set* delVertex(Set* s, int n) //Видалення вершини із множини
{
	Set* siter = s;
	Set* d = nullptr;
	Set* dt = nullptr;
	while (siter) {
		if (siter->n != n) {
			Set* sn = new Set(siter->n);
			if (!d)
			{
				d = sn;
				dt = sn;
			}
			else {
				dt->next = sn;
				dt = sn;
			}
		}
		siter = siter->next;
	}
	return d;
}

Set* setIntersection(Set* a, AdjV* b) {
	//Перетин двох множин: candidates або notg із множиною суміжних вершин
	Set* aiter = a;
	AdjV* biter;
	Set* inter = nullptr;
	Set* intert = nullptr;
	while (aiter) {
		biter = b;
		while (biter) {
			if (aiter->n == biter->v) {
				Set* sn = new Set(aiter->n);
				if (!inter)
				{
					inter = sn;
					intert = sn;
				}
				else {
					intert->next = sn;
					intert = sn;
				}
			}
			biter = biter->next;
		}
		aiter = aiter->next;
	}
	return inter;
}

bool SetCont(Set* s, int n) {
	//Перевірка наявності вершини у множині
	Set* iter = s;
	while (iter) {
		if (iter->n == n)
			return true;
		iter = iter->next;
	}
	return false;
}

int sizeOfSet(Set* s) {
	//Потужність множини
	Set* iter = s;
	int n = 0;
	while (s) {
		n++;
		s = s->next;
	}
	return n;
}

void output(Set* compsub, Edge** e, int esize) {
	//Просто проходимось по списку ребер, і, якщо кінці ребра є в compsub, виводимо ребро
	std::cout << "\nРебра макс. повного підграфу розміру "<< sizeOfSet(compsub) << ": \n";
	for (int i = 0; i < esize; i++) {
		if (SetCont(compsub, e[i]->v1) && SetCont(compsub, e[i]->v2))
			std::cout << "(" << e[i]->v1+1 << ", " << e[i]->v2+1 << ")\n";
	}
}

void buildSubGraph(Set* compsub, Set* candidates, Set* notg, int max, Edge** e, int esize, Vertex** va) {
	//Алгоритм Брона-Кербоша. Він "крутиться довкола" трьох множин: множина вершин, занесених у повний підграф,
	//кандидатів, тобто тих, що ще можна додати до підграфу
	//(в загальному випадку початкова множина кандидатів має всі вершини графа, а тут для отримання саме МАКСИМАЛЬНО повного підграфу множина складається
	//лише з вершин степеня >= max - 1),
	//і множини "not", тобто вершин, що вже були додані до підграфу або їх не можна додати в принципі до підграфу (not не можу назвати - компілятор огризається, тому така назва)
	
	if (!candidates && !notg) {
		//Якщо множини кандидатів і "нот" пусті, то виводимо той граф, який було знайдено, вважаючи його повним
		if (sizeOfSet(compsub) == max)
		{
			output(compsub, e, esize);
		}
		return;
	}
	//А інакше...
	Set* canditer = candidates;
	while(canditer) {//Йдемо по кожній вершині з "кандидатів"
		Set* new_compsub = addVertex(compsub, canditer->n);
		AdjV* av = va[canditer->n]->adj;
		Set* new_candidates = setIntersection(candidates, av);
		Set* new_not = setIntersection(notg, av);
		buildSubGraph(new_compsub, new_candidates, new_not, max, e, esize, va);
		//Викликаємо для них рекурсію, додаючи до повного підграфа дану вершину, 
		//а у якості "кандидатів" передаючи перетин теперішніх "кандидатів" із вершинами, суміжних до вершини,
		//а "not" - перетин "not" з тією ж множиною
		candidates = delVertex(candidates, canditer->n);//Повернувшись із рекурсії знаходимо різницю "кандидатів" з даною вершиною
		notg = addVertex(notg, canditer->n);//а в "not" додаємо її.
		canditer = canditer->next;
	}
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::cout << "Пошук максимально повного підграфа у заданому графі\n\n";
	std::ifstream fgr("graph.txt");
	if (!fgr)
		std::cout << "Файл graph.txt не виявлено. Створіть його і спробуйте ще раз.\n";
	else
	{
		std::string row, s;
		int Ne = -1; //Кількість ребер графа
		int Nv; //Кількість вершин графа
		int i, j;
		while (!fgr.eof())
		{
			std::getline(fgr, row);
			Ne++;
			if (!Ne) //В першому рядку записана кількість вершин.
				Nv = std::stoi(row);
		}

		Edge **e = new Edge*[Ne];
		for (i = 0; i < Ne; i++)
			e[i] = nullptr;
		fgr.seekg(0, fgr.beg);
		std::getline(fgr, row);
		int v1, v2;//Два кінця ребра
		int c;
		//Формулюємо масив ребер
		for (i = 0; i < Ne; i++) {
			std::getline(fgr, row);
			s = "";
			c = 0;
			int rl = row.length();
			for (j = 0; j < rl; j++) {
				if (row[j] != ' ')
					s += row[j];
				if (row[j] == ' ' || j == rl - 1) {
					switch (c) {
					case 0:
						v1 = std::stoi(s) - 1;
						break;
					case 1:
						v2 = std::stoi(s) - 1;
						break;
					}
					c++;
					s = "";
				}
			}
			e[i] = new Edge(v1, v2);
		}
		std::cout << "Ребра графа: \n";
		for (i = 0; i < Ne; i++)
			std::cout << "(" << e[i]->v1 + 1 << ", " << e[i]->v2 + 1 << ")\n";

		int *deg = new int[Nv];//Масив, куди записуються степені кожної вершини. Індекс елемента дорівнює індексу вершини мінус 1.
		for (i = 0; i <= Nv; i++) {
			deg[i] = 0; //Записуємо початкове значення
		}
		//Тепер запишемо для кожної вершини її степінь, пройдячись по масиву ребер
		for (i = 0; i < Ne; i++)
		{
			deg[e[i]->v1]++;
			deg[e[i]->v2]++;
		}
		//Вивід степенів вершин
		/*for (i = 0; i < Nv; i++)
			std::cout << "Вершина: " << (i + 1) << ", степінь: " << deg[i] << std::endl;*/

		//У повному графі із n вершинами кожна вершина має степінь n-1. Із цією думкою:
		//1) встановлюємо для ітерації зовнішнього циклу число imax вершин підграфу
		//2) підраховуємо кількість вершин, степінь яких >= imax-1 у внутрішньому циклі
		//3) якщо ця кількість не менше imax, то встановлюємо, що макс. повний підграф має imax вершин, інакше завершуємо цикл
		//4) повторюємо доти, доки imax не буде становити Nv
		int max = -1;//Кількість вершин у максимально повному підграфі заданого графу
		int imax;
		int vc = 0;//Кількість вершин степеня >= imax-1
		for (imax = 1; imax < Nv; imax++) {
			vc = 0;
			for (i = 1; i <= Nv; i++)
			{
				if (deg[i] >= imax - 1)
					vc++;
			}
			if (vc >= imax) {
				max = imax;
			}
			else
				break;
		}
		//Визначаємо вершини степеня max-1 і більше, що суміжні із вершиною
		Vertex** va = new Vertex*[Nv];//Масив вершин
		for (i = 0; i <= Nv; i++) {
			AdjV* avh = nullptr;
			AdjV* avt = nullptr;
			for (j = 0; j < Ne; j++) {
				bool badj = false;
				AdjV* av = nullptr;
				if (e[j]->v1 == i && deg[e[j]->v2] >= max - 1)
				{
					av = new AdjV(e[j]->v2); badj = true;
				}
				else if (e[j]->v2 == i && deg[e[j]->v1] >= max - 1)
				{
					av = new AdjV(e[j]->v1); badj = true;
				}
				if (badj)
				{
					if (!avh)
					{
						avh = av;
						avt = av;
					}
					else
					{
						avt->next = av;
						avt = av;
					}
				}
			}
			va[i] = new Vertex(avh);
		}
		//Вивід суміжних
		/*for (i = 0; i < Nv; i++) { 
			std::cout << i;
			AdjV* ai = va[i]->adj;
			while (ai) {
				std::cout << ai->v << " ";
				ai = ai->next;
			}
			std::cout << "\n";
		}*/

		//Тепер за алгоритмом Брона-Кербоша визначаэмо усs макс. повні підграфи (в загальних випадках алгоритм встановлює усі повні підграфи,
		//а в даній програмі він модифікований так, щоб він встановлював лише підграфи з max вершин)
		Set* compsub = nullptr;
		Set* notg = nullptr;
		Set* cand = nullptr;
		Set* candtail = nullptr;
		for (i = 0; i < Nv; i++)
		{
			if (deg[i] >= max - 1)
			{
				Set* s = new Set(i); //Заповнюємо список індексами вершин степеня >= max-1, щоб передати в процедуру
				if (!cand)
				{
					cand = s;
					candtail = s;
				}
				else
				{
					candtail->next = s;
					candtail = s;
				}
			}
		}
		buildSubGraph(compsub, cand, notg, max, e, Ne, va);
		return 0;
	}
}


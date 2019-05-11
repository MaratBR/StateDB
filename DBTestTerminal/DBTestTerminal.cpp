// DBTestTerminal.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>

#include <StateDB/bpt.h>
#include <chrono>

using namespace statedb::bpt;
using namespace std;

void leaf_test()
{
	leaf<size_t, char, 5> l(0, 'a');

	size_t i = 0;
	while (true)
	{

		char val;
		cin >> val;
		l.insert(i++, val);
		_D(
			l.debug_print();
		)
	}
}

void internal_test1()
{
	leaf<size_t, char, 5> l(100, 'a');
	bpt_internal<size_t, char, 5> i(&l);
}

void tree_test0()
{
	tree<size_t, size_t, 4> t;

	auto tm = chrono::system_clock::now();
	for (size_t i = 0; i < 10; i++)
	{
		t.set(i, i);
	}
	t.print_debug();
	cout << (double)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now() - tm).count() / 1000000.0 << "ms";
	cout << "Testing..." << endl;

	for (size_t i = 0; i < 5; i++)
	{
		if (t.get(i) == nullptr || *t.get(i) != i)
		{
			std::cout << i << endl;
		}
	}
}

int main()
{
	
	tree_test0();
	
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

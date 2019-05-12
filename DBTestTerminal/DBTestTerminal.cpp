// DBTestTerminal.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>

#include <StateDB/bpt.h>
#include <chrono>
#include <thread>

using namespace statedb::bpt;
using namespace std;

struct huge_state
{
	byte_t data[1024 * 1024 * 20];
};

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
	tree<size_t, huge_state*, 4> t;
	size_t treeSize = statedb::ram_monitoring::get_ram_size(t);
	cout << "tree size: " << treeSize << endl;
	const int count = 0xff;

	auto tm = chrono::system_clock::now();
	for (size_t i = 0; i < count; i++)
	{
		huge_state* s = new huge_state();
		this_thread::sleep_for(20ms);
		t.set(i, s);
		cout << (i + 1) << '/' << count << ' ' << ((float)(i+1) / (float)count * 100.0f) << '%' << '\r';
	}
	cout << (double)chrono::duration_cast<chrono::nanoseconds>(chrono::system_clock::now() - tm).count() / 1000000.0 << "ms";
	cout << "Testing..." << endl;

	cin.get();

	for (size_t i = 0; i < count; i++)
	{
		if (t.get(i) == nullptr)
		{
			std::cout << i << endl;
		}
	}
}

int main()
{
	
	tree_test0();
	
}
#include "SmartAssert.h"

#include <vector>
#include <iostream>

void TestEnsure()
{
	char* p = NULL;

	ENSURE(p != NULL).warn("p can not be NULL!");

	std::vector<int> numbers;
	int index = 3;

	ENSURE(index >= 0 && index < numbers.size())(index)(numbers.size()).warn("index valid!");

}
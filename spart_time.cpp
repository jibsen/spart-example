//
// Simple stable partitioning using constant extra space
//
// Example code for Reordering Arguments blog post (www.hardtoc.com)
//
// Copyright (c) 2016 Joergen Ibsen
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#ifdef __MINGW32__
#  define __USE_MINGW_ANSI_STDIO 1
#endif

#include "spart.hpp"

#include <stdio.h>

#include <chrono>
#include <random>
#include <vector>

// Struct containing an id for tracking order and a value for partitioning
struct Item {
	size_t id;
	bool value;
};

// A predicate for Item that counts the number of times it is used
struct PredCounter {
	bool operator()(const Item &i) { ++count; return i.value; }
	static size_t count;
};

size_t PredCounter::count = 0;

void
time_partitioning(const std::vector<Item> &items)
{
	{
		std::vector<Item> v(items);

		PredCounter::count = 0;

		auto start = std::chrono::steady_clock::now();

		std::stable_partition(std::begin(v), std::end(v), PredCounter());

		auto end = std::chrono::steady_clock::now();

		printf("  std::stable_partition      %7.2f ms (%zu predicates)\n", std::chrono::duration<double, std::milli>(end - start).count(), PredCounter::count);
	}

	{
		std::vector<Item> v(items);

		PredCounter::count = 0;

		auto start = std::chrono::steady_clock::now();

		stable_partition_recursive(std::begin(v), std::end(v), PredCounter());

		auto end = std::chrono::steady_clock::now();

		printf("  stable_partition_recursive %7.2f ms (%zu predicates)\n", std::chrono::duration<double, std::milli>(end - start).count(), PredCounter::count);
	}

	{
		std::vector<Item> v(items);

		PredCounter::count = 0;

		auto start = std::chrono::steady_clock::now();

		stable_partition_bottomup(std::begin(v), std::end(v), PredCounter());

		auto end = std::chrono::steady_clock::now();

		printf("  stable_partition_bottomup  %7.2f ms (%zu predicates)\n", std::chrono::duration<double, std::milli>(end - start).count(), PredCounter::count);
	}

	{
		std::vector<Item> v(items);

		PredCounter::count = 0;

		auto start = std::chrono::steady_clock::now();

		stable_partition_bsearch(std::begin(v), std::end(v), PredCounter());

		auto end = std::chrono::steady_clock::now();

		printf("  stable_partition_bsearch   %7.2f ms (%zu predicates)\n", std::chrono::duration<double, std::milli>(end - start).count(), PredCounter::count);
	}

	{
		std::vector<Item> v(items);

		PredCounter::count = 0;

		auto start = std::chrono::steady_clock::now();

		stable_partition_natural(std::begin(v), std::end(v), PredCounter());

		auto end = std::chrono::steady_clock::now();

		printf("  stable_partition_natural   %7.2f ms (%zu predicates)\n", std::chrono::duration<double, std::milli>(end - start).count(), PredCounter::count);
	}
}

int
main(void)
{
	std::vector<Item> items(1000 * 1000);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution dis(0.5);

	printf("Timing all false:\n");

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].id = i;
		items[i].value = false;
	}

	time_partitioning(items);

	printf("\nTiming all true:\n");

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].value = true;
	}

	time_partitioning(items);

	printf("\nTiming first half false:\n");

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].value = (i > items.size() / 2);
	}

	time_partitioning(items);

	printf("\nTiming alternating false/true:\n");

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].value = (i % 2 != 0);
	}

	time_partitioning(items);

	printf("\nTiming random:\n");

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].value = dis(gen);
	}

	time_partitioning(items);

	return 0;
}

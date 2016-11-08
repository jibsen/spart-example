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

#include <limits.h>

#include <vector>

#include "greatest.h"

// Struct containing an id for tracking order and a value for partitioning
struct Item {
	size_t id;
	bool value;
};

// Increment the binary number formed from the values of a range of Items
template<class ForwardIt>
bool
increment(ForwardIt first, ForwardIt last)
{
	for (ForwardIt i = first; i != last; ++i) {
		i->value = !i->value;

		if (i->value) {
			return true;
		}
	}

	return false;
}

static greatest_test_res
test_stable_partition(size_t num)
{
	const auto pred = [](const Item &i){ return i.value; };
	const auto cmp = [](const Item &lhs, const Item &rhs) { return lhs.id < rhs.id; };

	std::vector<Item> items(num);

	for (size_t i = 0; i < items.size(); ++i) {
		items[i].id = i;
		items[i].value = false;
	}

	do {
		std::vector<Item> ref_part(items);
		auto ref_m = std::stable_partition(std::begin(ref_part), std::end(ref_part), pred);

		{
			std::vector<Item> part(items);

			auto m = stable_partition_recursive(std::begin(part), std::end(part), pred);

			ASSERT(std::distance(std::begin(part), m) == std::distance(std::begin(ref_part), ref_m));
			ASSERT(m == std::find_if_not(std::begin(part), std::end(part), pred));
			ASSERT(std::is_sorted(std::begin(part), m, cmp) && std::is_sorted(m, std::end(part), cmp));
		}

		{
			std::vector<Item> part(items);

			auto m = stable_partition_bottomup(std::begin(part), std::end(part), pred);

			ASSERT(std::distance(std::begin(part), m) == std::distance(std::begin(ref_part), ref_m));
			ASSERT(m == std::find_if_not(std::begin(part), std::end(part), pred));
			ASSERT(std::is_sorted(std::begin(part), m, cmp) && std::is_sorted(m, std::end(part), cmp));
		}

		{
			std::vector<Item> part(items);

			auto m = stable_partition_bsearch(std::begin(part), std::end(part), pred);

			ASSERT(std::distance(std::begin(part), m) == std::distance(std::begin(ref_part), ref_m));
			ASSERT(m == std::find_if_not(std::begin(part), std::end(part), pred));
			ASSERT(std::is_sorted(std::begin(part), m, cmp) && std::is_sorted(m, std::end(part), cmp));
		}

		{
			std::vector<Item> part(items);

			auto m = stable_partition_natural(std::begin(part), std::end(part), pred);

			ASSERT(std::distance(std::begin(part), m) == std::distance(std::begin(ref_part), ref_m));
			ASSERT(m == std::find_if_not(std::begin(part), std::end(part), pred));
			ASSERT(std::is_sorted(std::begin(part), m, cmp) && std::is_sorted(m, std::end(part), cmp));
		}
	} while (increment(std::begin(items), std::end(items)));

	PASS();
}

TEST
test_partitioning(void)
{
	// Test partitioning of all possible arrays of size 0 to 19
	for (size_t num = 0; num < 20; ++num) {
		CHECK_CALL(test_stable_partition(num));
	}

	PASS();
}

template<class Difference>
static greatest_test_res
test_loop_internal(Difference n)
{
	if (n == 0) {
		PASS();
	}
	else if (n == 1) {
		PASS();
	}

	Difference width = 1;

	for (;;) {
		for (Difference i = 0, limit = 0; i < n; i += limit) {
			// limit = min(n - i, 2 * width), avoiding overflow
			limit = width > n - i ? n - i :
			        width > n - i - width ? n - i : 2 * width;
		}

		// Break if 2 * width >= n
		if (width >= n - width) {
			break;
		}
		width += width;
	}

	PASS();
}

TEST
test_loop(void)
{
	// Test loop construct terminates for sizes around limits
	CHECK_CALL(test_loop_internal<short>(2));
	CHECK_CALL(test_loop_internal<short>(3));
	CHECK_CALL(test_loop_internal<short>(4));
	CHECK_CALL(test_loop_internal<short>(5));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX / 2 - 2));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX / 2 - 1));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX / 2));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX / 2 + 1));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX / 2 + 2));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX - 2));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX - 1));
	CHECK_CALL(test_loop_internal<short>(SHRT_MAX));

	CHECK_CALL(test_loop_internal<unsigned short>(2));
	CHECK_CALL(test_loop_internal<unsigned short>(3));
	CHECK_CALL(test_loop_internal<unsigned short>(4));
	CHECK_CALL(test_loop_internal<unsigned short>(5));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX / 2 - 2));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX / 2 - 1));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX / 2));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX / 2 + 1));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX / 2 + 2));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX - 2));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX - 1));
	CHECK_CALL(test_loop_internal<unsigned short>(USHRT_MAX));

	PASS();
}

SUITE(spart)
{
	RUN_TEST(test_partitioning);
	RUN_TEST(test_loop);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
	GREATEST_MAIN_BEGIN();
	RUN_SUITE(spart);
	GREATEST_MAIN_END();
}

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

#ifndef SPART_HPP_INCLUDED
#define SPART_HPP_INCLUDED

#include <algorithm>
#include <iterator>

// Rotate two adjacent blocks using reversal. The partitioning algorithms use
// std::rotate, this is here as an example.
template <class BidirIt>
inline void
rotate_by_reverse(BidirIt first, BidirIt middle, BidirIt last)
{
	if (first != middle && middle != last) {
		std::reverse(first, middle);
		std::reverse(middle, last);
		std::reverse(first, last);
	}
}

// Find partition point using binary search. The partitioning algorithms use
// std::partition_point, this is here as an example.
template<class ForwardIt, class Difference, class UnaryPredicate>
ForwardIt
find_partition_point_n(ForwardIt first, Difference n, UnaryPredicate pred)
{
	while (n > 0) {
		Difference half = n / 2;
		ForwardIt middle = std::next(first, half);

		if (pred(*middle)) {
			first = std::next(middle);
			n -= half + 1;
		}
		else {
			n = half;
		}
	}

	return first;
}

template<class ForwardIt, class UnaryPredicate>
inline ForwardIt
find_partition_point(ForwardIt first, ForwardIt last, UnaryPredicate pred)
{
	return find_partition_point_n(first, std::distance(first, last), pred);
}

template<class RandIt, class Difference, class UnaryPredicate>
RandIt
stable_partition_recursive_n(RandIt first, Difference n, UnaryPredicate pred)
{
	if (n == 0) {
		return first;
	}
	else if (n == 1) {
		return pred(*first) ? std::next(first) : first;
	}

	RandIt l = stable_partition_recursive_n(first, n / 2, pred);
	RandIt m = std::next(first, n / 2);
	RandIt r = stable_partition_recursive_n(m, n - n / 2, pred);

	if (l != m && m != r) {
		std::rotate(l, m, r);
	}

	return std::next(l, std::distance(m, r));
}

template<class RandIt, class UnaryPredicate>
inline RandIt
stable_partition_recursive(RandIt first, RandIt last, UnaryPredicate pred)
{
	return stable_partition_recursive_n(first, std::distance(first, last), pred);
}

template<class BidirIt, class Difference, class UnaryPredicate>
BidirIt
stable_partition_bottomup_n(BidirIt first, Difference n, UnaryPredicate pred)
{
	if (n == 0) {
		return first;
	}
	else if (n == 1) {
		return pred(*first) ? std::next(first) : first;
	}

	BidirIt l, m, r;
	Difference width = 1;

	for (;;) {
		BidirIt next = first;

		for (Difference i = 0, limit = 0; i < n; i += limit) {
			// limit = min(n - i, 2 * width), avoiding overflow
			limit = width > n - i ? n - i :
			        width > n - i - width ? n - i : 2 * width;

			if (limit > width) {
				m = std::next(next, width);
				l = std::find_if_not(next, m, pred);
				next = std::next(m, limit - width);
				r = std::find_if_not(m, next, pred);

				if (l != m && m != r) {
					std::rotate(l, m, r);
				}
			}
		}

		// Break if 2 * width >= n
		if (width >= n - width) {
			break;
		}
		width += width;
	}

	return std::next(l, std::distance(m, r));
}

template<class BidirIt, class UnaryPredicate>
inline BidirIt
stable_partition_bottomup(BidirIt first, BidirIt last, UnaryPredicate pred)
{
	return stable_partition_bottomup_n(first, std::distance(first, last), pred);
}

template<class RandIt, class Difference, class UnaryPredicate>
RandIt
stable_partition_bsearch_n(RandIt first, Difference n, UnaryPredicate pred)
{
	if (n == 0) {
		return first;
	}
	else if (n == 1) {
		return pred(*first) ? std::next(first) : first;
	}

	RandIt l, m, r;
	Difference width = 1;

	for (;;) {
		RandIt next = first;

		for (Difference i = 0, limit = 0; i < n; i += limit) {
			// limit = min(n - i, 2 * width), avoiding overflow
			limit = width > n - i ? n - i :
			        width > n - i - width ? n - i : 2 * width;

			if (limit > width) {
				m = std::next(next, width);
				l = std::partition_point(next, m, pred);
				next = std::next(m, limit - width);
				r = std::partition_point(m, next, pred);

				if (l != m && m != r) {
					std::rotate(l, m, r);
				}
			}
		}

		// Break if 2 * width >= n
		if (width >= n - width) {
			break;
		}
		width += width;
	}

	return std::next(l, std::distance(m, r));
}

template<class RandIt, class UnaryPredicate>
inline RandIt
stable_partition_bsearch(RandIt first, RandIt last, UnaryPredicate pred)
{
	return stable_partition_bsearch_n(first, std::distance(first, last), pred);
}

template<class BidirIt, class Difference, class UnaryPredicate>
BidirIt
stable_partition_natural_n(BidirIt first, Difference n, UnaryPredicate pred)
{
	if (n == 0) {
		return first;
	}
	else if (n == 1) {
		return pred(*first) ? std::next(first) : first;
	}

	BidirIt last = std::next(first, n);
	BidirIt l, m, r;
	bool change = false;

	do {
		BidirIt next = first;
		change = false;

		do {
			l = std::find_if_not(next == first ? next : std::next(next), last, pred);
			m = (l == last) ? last : find_if(std::next(l), last, pred);
			r = (m == last) ? last : find_if_not(std::next(m), last, pred);
			next = (r == last) ? last : find_if(std::next(r), last, pred);

			if (m != r) {
				std::rotate(l, m, r);
				change = true;
			}
		} while (next != last);
	} while (change);

	return std::next(l, std::distance(m, r));
}

template<class BidirIt, class UnaryPredicate>
inline BidirIt
stable_partition_natural(BidirIt first, BidirIt last, UnaryPredicate pred)
{
	return stable_partition_natural_n(first, std::distance(first, last), pred);
}

#endif // SPART_HPP_INCLUDED

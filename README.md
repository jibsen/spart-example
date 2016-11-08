
Stable Partitioning Examples
============================

`spart.hpp` contains example code in C++ for the stable partitioning algorithms
discussed in the [Reordering Arguments][post] blog post.

`spart_test` compares the output of the various partitioning functions to that
of `std::stable_partition`.

`spart_time` compares the execution time and number of predicate applications
for a vector of 1,000,000 elements. Here is the output from a MSVC compile,
optimization turned on:

    Timing all false:
      std::stable_partition        11.13 ms (1000000 predicates)
      stable_partition_recursive    7.49 ms (1000000 predicates)
      stable_partition_bottomup     7.20 ms (1999998 predicates)
      stable_partition_bsearch     13.33 ms (3999959 predicates)
      stable_partition_natural      1.29 ms (1000000 predicates)

    Timing all true:
      std::stable_partition         2.09 ms (1000000 predicates)
      stable_partition_recursive    7.93 ms (1000000 predicates)
      stable_partition_bottomup    26.06 ms (19980544 predicates)
      stable_partition_bsearch     13.54 ms (2999961 predicates)
      stable_partition_natural      1.32 ms (1000000 predicates)

    Timing first half false:
      std::stable_partition         7.00 ms (1000000 predicates)
      stable_partition_recursive   12.16 ms (1000000 predicates)
      stable_partition_bottomup    21.66 ms (10980537 predicates)
      stable_partition_bsearch     18.27 ms (3499955 predicates)
      stable_partition_natural      6.13 ms (2000000 predicates)

    Timing alternating false/true:
      std::stable_partition         6.94 ms (1000000 predicates)
      stable_partition_recursive   20.87 ms (1000000 predicates)
      stable_partition_bottomup    37.17 ms (11490270 predicates)
      stable_partition_bsearch     36.23 ms (3499961 predicates)
      stable_partition_natural     41.18 ms (20000000 predicates)

    Timing random:
      std::stable_partition         9.53 ms (1000000 predicates)
      stable_partition_recursive   30.10 ms (1000000 predicates)
      stable_partition_bottomup    48.42 ms (11350257 predicates)
      stable_partition_bsearch     45.86 ms (3457751 predicates)
      stable_partition_natural     45.53 ms (19000000 predicates)

Some observations from these results:

  - bsearch uses between 3n and 4n predicate applications.
  - bottomup and natural use worst-case around 20 million predicate
    applications, which is n log n.
  - natural expectedly performs well when few changes are needed.

[post]: http://hardtoc.com/2016/11/07/reordering-arguments.html


License
-------

This projected is licensed under the terms of the [MIT license](LICENSE).

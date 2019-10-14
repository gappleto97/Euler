# Python Section

All problems are solved in type-hinted python3

## Organization

All files share a common prefix with their problem number. Several of the problems also have code leveraged as if they were a library. That list is (approximately):

* p0002 for the fibonacci sequence
* p0003 for prime numbers and factors
* p0004 for palindrome detection
* p0007 for is_prime()
* p0008 for iterating over a series in groups of n
* p0015 for n_choose_r
* p0018 for trianglar pathfinding
    * only used by p0018 and p0067
* p0021 for proper divisors
* p0052 and p0074 for digits functions
    * p0052 for a sequence
    * p0074 for a generator
* p0055 for retrieving a number from its digits

As a style note, to preserve Doxygen namespacing I have imported whole modules rather than individual pieces of modules, as would normally be my preference. Not doing this caused problems with Doxygen's namespace and call graph features.

## Makefile

There are three main recipes in this Makefile

### dependencies

This recipe installs all the required and test dependencies. See the Dependencies section for more info

### test

This recipe runs tests in a single thread and performs benchmarks and style checks on them.

### test_*

This recipe runs tests in multiple threads, using however many are specified by the number after the _. For example, `test_3` would spawn three python processes. Because benchmark disables itself when running in children processes, benchmark info is not available with this recipe.

## Tests

### Prime Numbers

There is a single test for the prime number infrastructure. It has three components:

1. It checks that it correctly generates the first million primes in the correct order
2. It checks that is_prime() operates correctly for all numbers in that range, including all composites between them
3. It checks that the whole series takes <200ns/prime to compute

### Generic Problems

For each problem it will check the answer against a known dictionary. If the problem is not in the "known slow" category (meaning that I generate the correct answer with a poor solution), it will run it as many times as the benchmark plugin wants. Otherwise it is run exactly once.

A test fails if it gets the wrong answer or if it takes more than 1 minute.

## Dependencies

I try to keep the dependencies of this project as small as possible, except for test plugins. At the moment there are only two non-test dependencies:

### u-msgpack-python

This serialization library encodes things in the MessagePack format. The reason it is not only for testing is that it allows me to read from a cache of prime numbers stored on disk. This is not required, and if you remove it (and the import) things would still run correctly.

### sortedcontainers

This library provides a collection of sorted containers. At the time of writing this, the only one I use is SortedSet() for the prime number generator. If I were willing to remove the prime number cache this would not be needed. I could also implement my own if need be.

## Environment Variables

### NO_OPTIONAL_TESTS

If this variable is defined to something other than 0 or an empty string, the test suite will skip any tests which are not directly related to Project Euler problems. This value will default to the same value as [`ONLY_SLOW`](#only-slow).

### NO_SLOW

If this variable is defined to something other than 0 or an empty string, problems in the known_slow group will not be tested. This variable defaults to True on Termux systems. If both [`NO_SLOW`](#no-slow) and [`ONLY_SLOW`](#only-slow) are truthy, they will be ignored and a warning will be issued.

### ONLY_SLOW

If this variable is defined to something other than 0 or an empty string, *only* problems in the known_slow group will be tested. If both [`NO_SLOW`](#no-slow) and [`ONLY_SLOW`](#only-slow) are truthy, they will be ignored and a warning will be issued.

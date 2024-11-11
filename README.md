# C++ istream parser

A simple *single header file* C++ `std::istream` parser that provides a high level structural parsing abstraction.

See [examples](#).

## Table of Contest
* [Dependencies](#dependencies)
* [Build](#build)
* [Documentation](#documentation)
	* [istream_parser::ParseInterface](#istream_parserparseinterface)
		* [ParseInterface::ParseInterface(std::istream &stream)](#parseinterfaceparseinterfacestdistream-stream)
	* [PARSE_INTERFACE](#parse_interface)
	* [DECLARE_PARSE_INTERFACE(name)](#declare_parse_interfacename)
	* [PARSE_SETUP](#parse_setup)
	* [PARSE(type)](#parsetype)
		* [T& parse_it<T>::operator() ()](#t-parse_ittoperator-)
		* [T& parse_it<T>::operator[] (size_t index)](#t-parse_ittoperator-size_t-index)
		* [std::vector<T>& parse_it<T>::vector()](#stdvectort-parse_ittvector)
		* [size_t parse_it<T>::size()](#size_t-parse_ittsize)
		* [std::vector<T>::iterator begin()](#stdvectortiterator-begin)
		* [std::vector<T>::iterator end()](#stdvectortiterator-end)
	* [SKIP(type)](#skiptype)
	* [SINGLE([value])](#singlevalue)
	* [COUNT(expr)](#countexpr)
	* [CALC_COUNT(scoped_expr)](#calc_countscoped_expr)
	* [UNTIL(value)](#untilvalue)
	* [UNTIL_END()](#until_end)
	* [THROUGH(value)](#throughvalue)
	* [THROUGH_LINE](#through_line)
* [Examples](#examples)
	* [Day 20](#day-20)
	* [Day 8](#day-8)
	* [Day 4](#day-4)
* [Tests](#tests)
* [Additional Information](#additional-information)
	* [Commands](#commands)
		* [all (default)](#all-default)
		* [custom](#custom)
		* [lib](#lib)
		* [clear](#clear)
* [License](#license)

## Dependencies

No external dependencies.

## Build

Requires at least C++20 standard compiler.

The implementation uses C++20  features like `if constexpr`, `concept` and `std::format`.

`make` default command builds the tests and creates test.exe file in the root directory.

`make custom` builds the playground file location test/mtest.cpp, and creates mtest.exe file in the root directory of the library.

## Documentation

The main usage of the library is in defining the structure of the object that needs to be parsed, and constructing the object.

To do so, the library provides an interface **istream_parser::ParseInterface** and a bunch of *macros* that can be used to define parsing fields.

### istream_parser::ParseInterface

The class that provides constructors and hints for parser to parse the data. Every parsing structure **must** inherit this interface in order for the parsing mechanism to work correctly.

The classes inherited from ParseInterface are **not** default constructable.

The interface provides **copy** and **move** constructors.

___

#### ParseInterface::ParseInterface(std::istream &stream)

This constructor when initialized parses the data into it's entire fields from the **stream**.

>[!NOTE]
>There is a macro `PARSE_INTERFACE` that can be used instead of `istream::ParseInterface`.

***Example:***

```c++
#include "parser.h"

struct ParseMe : PARSE_INTERFACE {
	PARSE_SETUP;

	PARSE(int) a = SINGLE();
	PARSE(float) b = SINGLE();
	PARSE(string) str = COUNT(3);
};

int main() {
	ParseMe parsed(cin);

	cout << parsed.a() << endl;
}
```

### PARSE_INTERFACE

Just a macro alias for `istream_parser::ParseInterface`.

### DECLARE_PARSE_INTERFACE(name)

The macro that helps to declare a parsing interface.

It accepts one parameter - **name** of the structure.

Using this macro is equivalent to `struct <name> : public istream_parse::ParseInterface`.

***Example:***

```c++
DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	...
};
```

### PARSE_SETUP

The macro that prepares the structure that inherits from `istream_parser::ParseInterface` (`PARSE_INTERFACE`).

All it does - it inherits constructors from the `ParseInterface` class.

>[!IMPORTANT]
>This macro **must** be used in every structural parsing declaration.

### PARSE(type)

The macro that declares a field type to be parsed. The usage of this macro **must** be followed by the *field name*, and assigned one of the parsing rules macros (e.g. SINGLE(), COUNT(), UNTIL(), etc.).

>[!NOTE]
>PARSE accepts only those types that can be parsed using `std::basic_istream::operator >> ()` [mechanism](https://en.cppreference.com/w/cpp/io/basic_istream/operator_gtgt), or the type that inherits ParseInterface.
>It includes integral types (`int`, `long int`, `size_t`, `unsigned`, etc.), floating point types (`float`, `double`, etc.), `char`, `std::string`, etc.

The field that is defined by using `PARSE` macro is then publicly accessible and contains the instance of `istream_parser::parse_it<T>` class.

>[!NOTE]
>If at any point the parsing could not be made (wrong type was passed, etc.) - the instance of `std::logic_error` is thrown.

The class defines a couple of public access functions:

#### T& parse_it\<T\>::operator() ()

Returns the reference to the parsed value.

#### T& parse_it\<T\>::operator[] (size_t index)

Returns the reference to the parsed vector's **index** element.

#### std::vector\<T\>& parse_it\<T\>::vector()

Returns the reference to the parsed vector.

#### size_t parse_it\<T\>::size()

Returns the number of parsed elements.

#### std::vector\<T\>::iterator begin()

Returns an iterator to the **begin** element in the parsed vector.

#### std::vector\<T\>::iterator end()

Returns an iterator to the **end** element in the parsed vector.

***Example:***

```c++
DECLARE_PARSE_INTERFACE(PartOfMe) {
	PARSE_SETUP;
	PARSE(int) val = SINGLE();
	PARSE(string) str = SINGLE();
};

DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	PARSE(PartOfMe) arr = THROUGH_LINE();
};

int main() {
	ParseMe parsed(cin);
	for (auto& part : parsed.arr) {
		cout << part.val() << " " << part.str() << endl;
	}
}
```

### SKIP(type)

The macro that skips some part of the stream. The usage of this macro **must not** be followed by the *field name*, but tit **must** still be assigned one of the parsing rules macros (e.g. SINGLE(), COUNT(), UNTIL(), etc.).

***Example:***

```c++
DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	PARSE(int) v1 = SINGLE();
	SKIP(char) = SINGLE(',');
	PARSE(int) v2 = SINGLE();
};
```

___

Next macros are *parsing rules macro* that are used to define how the field is suppose to be parsed.

### SINGLE([value])

Defines that the field is supposed to parse a single instance of the type provided to `PARSE` macro.

If the **value** is passed, it additionally checks that the parsed value matches the one that is passed to this macro. If it doesn't match - it throws an instance of **std::logic_error**.

### COUNT(expr)

Defines that the field may parse multiple values of the type that was provided to `PARSE` macro.

The number of parses is provided by the result of passed **expr** value.

***Example:***

```c++
DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	PARSE(int) v1 = SINGLE();
	PARSE(int) v2 = COUNT(3);
	PARSE(int) v3 = COUNT(v1());
	PARSE(int) v4 = COUNT(v1() * 10);
};
```

### CALC_COUNT(scoped_expr)

Defines that the field may parse multiple values of the type that was provided to `PARSE` macro.

The number of parses is provided by the result of passed **scoped_expr** value.

***Example:***

```c++
DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	PARSE(int) v1 = SINGLE();
	PARSE(int) v2 = COUNT({ return 3; });
	PARSE(int) v3 = COUNT({ return v1(); });
	PARSE(int) v4 = COUNT({
		int a=10;
		return v1()*a;
	});
};
```

### UNTIL(value)

Defines that the field may parse multiple values of the type that was provided to the `PARSE` macro.

The field is going to parse from the stream until the **value** is reached, or until the *EOF* is reached.

>[!NOTE]
>The only type that is allowed to be passed to **value** for now is `char`.

***Example:***

```c++
stringstream s("1 2 3 4 5 | 6 7 8 9 10");

DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	PARSE(int) arr = UNTIL('|');
	SKIP('|');
	PARSE(int) arr2 = UNTIL('|');
};

int main() {
	ParseMe parsed(s);
	s.arr.size() // 5 (1,2,3,4,5)
	s.arr2.size() // 5 (6,7,8,9,10)
}
```

### UNTIL_END()

The same as above, but parses until the end of buffer (*EOF*).

### THROUGH(value)

Works the same as `UNTIL(value)`, but also skips the **value** in the buffer.

***Example:***

```c++
stringstream s("1 2 3 4 5 | 6 7 8 9 10");

DECLARE_PARSE_INTERFACE(ParseMe) {
	PARSE_SETUP;
	PARSE(int) arr = THROUGH('|');
	PARSE(int) arr2 = UNTIL_END();
};

int main() {
	ParseMe parsed(s);
	s.arr.size() // 5 (1,2,3,4,5)
	s.arr2.size() // 5 (6,7,8,9,10)
}
```

### THROUGH_LINE

This macro is the equivalent of `THROUGH('\n')`.

## Examples

Lets take a couple of examples from **Advent Of Code** 2023 days.

### Day 20

***The input:***

```
...
%dx -> gt, dr
%lz -> qz, df
%dz -> fr
broadcaster -> cn, xk, rj, gf
%ct -> ks
%hq -> bz
...
```

***Parsing declaration:***

```c++
DECLARE_PARSE_INTERFACE(Deps) {
	PARSE_SETUP;
	PARSE(string) value = SINGLE();
	SKIP(char) = SINGLE(',');
};
DECLARE_PARSE_INTERFACE(Line) {
	PARSE_SETUP;
	PARSE(string) node = SINGLE();
	SKIP(string) = SINGLE("->");
	PARSE(Deps) arr = THROUGH_LINE();
};
DECLARE_PARSE_INTERFACE(Input) {
	PARSE_SETUP;
	PARSE(Line) lines = UNTIL_END();
};
```

### Day 8

***The input:***

```
LRRRLRRLRLRRRLRLLLL...

XKM = (FRH, RLM)
MVM = (JVG, TRK)
BXN = (SJH, XFB)
KJS = (QRD, QRD)
LCQ = (DQL, TRQ)
TRP = (FBR, BLD)
HSQ = (QQN, KSN)
...
```

***Parsing declaration:***

```c++
DECLARE_PARSE_INTERFACE(Line) {
	PARSE_SETUP;
	PARSE(string) node = SINGLE();
	SKIP(char) = COUNT(3); // " = "
	SKIP(char) = SINGLE('(');
	PARSE(string) val1 = THROUGH(',');
	PARSE(string) val2 = THROUGH(')')
};
DECLARE_PARSE_INTERFACE(Input) {
	PARSE_SETUP;
	PARSE(Line) lines = UNTIL_END();
};
```

### Day 4

***The input:***

```
Card   1: 27 61 49 69 58 44  2 29 39 10 | 97 96 49 78 26 58 27 77 69  9 39 88 53 10  2 29 61 62 48 87 18 44 74 34 11
Card   2: 65 22 99 75 72 29 38 82 80 66 | 87 15 21 50 55 72  4 54 46 29 95  2 65 75 18 89 99 80 38 82 56 33 22 66 71
Card   3: 27 95 96 34 14 42 40 60 50 93 | 35 95 22 34 50 27 29 77 78  4  5 61 93  8 59 63 16 45 80 81 92 23 42 88 82
...
```

***Parsing declaration:***

```c++
DECLARE_PARSE_INTERFACE(Line) {
	PARSE_SETUP;
	SKIP(string) = SINGLE("Card");
	PARSE(int) index = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(int) arr1 = THROUGH('|');
	PARSE(int) val2 = THROUGH_LINE();
};
DECLARE_PARSE_INTERFACE(Input) {
	PARSE_SETUP;
	PARSE(Line) lines = UNTIL_END();
};
```

## Tests

Tests are defined under the  **test**  folder that leaves in the root folder of the library.

By running  `make all`  _(default command)_, the  **test/test.cpp**  file is being compiled and the  **test.exe**  file is being generated in the root directory of the library. Run the file, to run library tests.

## Additional Information

**makefile**  contains  `custom`  command, that compiles  **test/mtest.cpp**  file. This file can be used as a quick start sandbox file, to try things out. There are some neat example already provided in the file.

### Commands

#### all (default)

Builds unit test and creates a  **test.exe**  executable file in a library root diretory.

#### custom

Builds a sandbox  _test/mtest.cpp_  file, and creates a  **mtest.exe**  executable file in a library root directory.

#### lib

Builds library file and generate an archive  **liboutput.a**  file that can be linked to the final executable file.

#### clear

Cleanup objects, executable and archive files.

## License

MIT

Have fun :)

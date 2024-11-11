#include <sstream>

#include "parser.h"

struct B : public ParseInterface {
    PARSE_SETUP;

    PARSE(float) val1 = SINGLE();
    PARSE(float) val2 = SINGLE();
};

struct A : public ParseInterface {
    PARSE_SETUP;

    PARSE(int) a = SINGLE(3);
    PARSE(B) b = SINGLE();
    PARSE(int) arr = COUNT(a());
    PARSE(int) arr1 = UNTIL('|');
    SKIP(std::string) = SINGLE("|");
    PARSE(int) arr2 = THROUGH('\n');
    PARSE(int) arr3 = UNTIL_END();
};

std::istringstream s("3 2.5 3.14 4 5 6 1 2 3 | 4 5 6\n7 8 9");
A a(s);

int main() {
	A aa = a;

    std::cout << a.a() << std::endl;
    std::cout << a.b().val1() << " " << a.b().val2() << std::endl;
    std::cout << aa.b().val2() << std::endl;
    for (auto a : aa.arr) {
		std::cout << "ARR: " << a << std::endl;
    }
    for (auto a : aa.arr1) {
		std::cout << "ARR1: " << a << std::endl;
    }
    for (auto a : aa.arr2) {
		std::cout << "ARR2: " << a << std::endl;
    }
    for (auto a : aa.arr3) {
		std::cout << "ARR3: " << a << std::endl;
    }

    return 0;
}

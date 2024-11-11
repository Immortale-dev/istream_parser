#include <string>
#include <sstream>

#include "parser.h"

struct Vec_count : ParseInterface {
	PARSE_SETUP;
	PARSE(int) int_size = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(int) int_arr = COUNT(int_size());
	SKIP(char) = SINGLE(';');
	PARSE(int) str_size = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(std::string) str_arr = COUNT(str_size());
	SKIP(char) = SINGLE(';');
	PARSE(int) rows = SINGLE();
	SKIP(char) = SINGLE('x');
	PARSE(int) cols = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(float) mat = CALC_COUNT({return rows()*cols();});
};

struct Part_mat : ParseInterface {
	PARSE_SETUP;
	PARSE(int) rows = SINGLE();
	SKIP(char) = SINGLE('x');
	PARSE(int) cols = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(double) mat = COUNT(rows()*cols());
};

struct Vec_with_mat : ParseInterface {
	PARSE_SETUP;
	PARSE(int) int_size = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(int) int_arr = COUNT(int_size());
	SKIP(char) = SINGLE(';');
	PARSE(int) str_size = SINGLE();
	SKIP(char) = SINGLE(':');
	PARSE(std::string) str_arr = COUNT(str_size());
	SKIP(char) = SINGLE(';');
	PARSE(Part_mat) mat = SINGLE();
};

struct Part_struct : ParseInterface {
	PARSE_SETUP;
	PARSE(int) num = SINGLE();
	SKIP(char) = SINGLE(',');
	PARSE(std::string) str = SINGLE();
	SKIP(char) = SINGLE(';');
};

struct Vec_struct : ParseInterface {
	PARSE_SETUP;
	PARSE(Part_struct) arr = COUNT(3);
};

SCENARIO_START

DESCRIBE("Given stream of <int>: vector<int>; <int>: vector<string>; <int>x<int>: vector<float>", {
	std::stringstream* s;
	BEFORE_EACH({
		s = new std::stringstream{"5: 3 4 5 6 7; \n 3: ab cd de ; \n 2x3: 1.5 2.5 3.5 \n 4.5 5.5 6.5"};
	});
	AFTER_EACH({
		delete s;
	});

	IT("should parse and compute vector lengths", {
		Vec_count p(*s);

		EXPECT(p.int_size()).toBe(5);
		EXPECT(p.int_arr).toBeIterableEqual({3,4,5,6,7});
		EXPECT(p.str_size()).toBe(3);
		EXPECT(p.str_arr).toBeIterableEqual({"ab", "cd", "de"});
		EXPECT(p.rows()).toBe(2);
		EXPECT(p.cols()).toBe(3);
		EXPECT(p.mat).toBeIterableEqual({1.5,2.5,3.5,4.5,5.5,6.5});
	});

	IT("should parse struct", {
		Vec_with_mat p(*s);

		EXPECT(p.mat().rows()).toBe(2);
		EXPECT(p.mat().cols()).toBe(3);
		EXPECT(p.mat().mat).toBeIterableEqual({1.5,2.5,3.5,4.5,5.5,6.5});
	});
});

DESCRIBE("Givem stream of vector<<int>, <string> ;>", {
	std::stringstream* s;
	BEFORE_EACH({
		s = new std::stringstream{"4, hello ; 5, world ; 6, woohoo ;"};
	});
	AFTER_EACH({
		delete s;
	});

	IT("should parse array of structs", {
		Vec_struct p(*s);
		auto copy = p;

		EXPECT(copy.arr.size()).toBe(3);
		EXPECT(copy.arr[0].num()).toBe(4);
		EXPECT(copy.arr[1].num()).toBe(5);
		EXPECT(copy.arr[2].num()).toBe(6);
		EXPECT(p.arr[0].str()).toBe("hello");
		EXPECT(p.arr[1].str()).toBe("world");
		EXPECT(p.arr[2].str()).toBe("woohoo");
	});
});

SCENARIO_END

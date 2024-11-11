#include <string>
#include <sstream>

#include "parser.h"

struct Vec_until : ParseInterface {
	PARSE_SETUP;
	PARSE(int) arr1 = UNTIL('|');
	SKIP(char) = SINGLE('|');
	PARSE(int) arr2 = THROUGH('\n');
	PARSE(int) arr3 = UNTIL_END();
};

struct Vec_skip_until : ParseInterface {
	PARSE_SETUP;
	SKIP(int) = THROUGH('|');
	PARSE(int) arr = THROUGH_LINE();
	SKIP(int) = UNTIL_END();
};

SCENARIO_START

DESCRIBE("Given stream of vector<int> | vector<int> \\n vector<int>", {
	std::stringstream* s;
	BEFORE_EACH({
		s = new std::stringstream{"1 2 3 4 5 | 6 7 8 9 10 \n 11 12 13"};
	});
	AFTER_EACH({
		delete s;
	});

	IT("should parse vectors using until", {
		Vec_until p(*s);

		EXPECT(p.arr1).toBeIterableEqual({1,2,3,4,5});
		EXPECT(p.arr2).toBeIterableEqual({6,7,8,9,10});
		EXPECT(p.arr3).toBeIterableEqual({11,12,13});
	});

	IT("should skip vectors when parsing", {
		Vec_skip_until p(*s);

		EXPECT(p.arr).toBeIterableEqual({6,7,8,9,10});
	});
});

SCENARIO_END

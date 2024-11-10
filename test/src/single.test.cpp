#include <string>
#include <sstream>

#include "parser.h"

struct IndVals : ParseInterface {
	PARSE_SETUP;
	PARSE(int) int_val = SINGLE();
	PARSE(float) float_val = SINGLE(13.5);
	PARSE(std::string) s1 = SINGLE();
	PARSE(std::string) s2 = SINGLE("world");
};

struct IndVals_fail : ParseInterface {
	PARSE_SETUP;
	PARSE(int) int_val = SINGLE();
	PARSE(float) float_val = SINGLE(33.33);
};

struct IndVals_wrong_type : ParseInterface {
	PARSE_SETUP;
	PARSE(int) int_val = SINGLE();
	PARSE(float) float_val = SINGLE(13.5);
	PARSE(int) wrong_type = SINGLE();
};

struct IndVals_skip : ParseInterface {
	PARSE_SETUP;
	SKIP(int) = SINGLE(42);
	SKIP(float) = SINGLE();
	SKIP(std::string) = SINGLE("hello");
	PARSE(std::string) val = SINGLE();
};

struct IndVals_skip_fail : ParseInterface {
	PARSE_SETUP;
	SKIP(int) = SINGLE(44);
};

struct IndVals_skip_fail_type : ParseInterface {
	PARSE_SETUP;
	SKIP(int) = SINGLE();
	SKIP(float) = SINGLE();
	SKIP(int) = SINGLE();
};

SCENARIO_START

DESCRIBE("Given stream of individual values", {
	std::stringstream* s;

	BEFORE_EACH({
		s = new std::stringstream{"42 13.5 hello world"};
	});

	IT("should parse individual values", {
		IndVals p(*s);

		EXPECT(p.int_val()).toBe(42);
		EXPECT(p.float_val()).toBe(13.5);
		EXPECT(p.s1()).toBe("hello");
		EXPECT(p.s2()).toBe("world");
	});

	IT("should throw if the single value doesn't match", {
		EXPECT([s](){
			IndVals_fail p(*s);
		}).toThrowError();
	});

	IT("should throw if the value cannot be parsed", {
		EXPECT([s](){
			IndVals_wrong_type p(*s);
		}).toThrowError();
	});

	IT("should skip values", {
		IndVals_skip p(*s);

		EXPECT(p.val()).toBe("world");
	});

	IT("should fail to skip if the value doesn't match", {
		EXPECT([s](){
			IndVals_skip_fail p(*s);
		}).toThrowError();
	});

	IT("should fail to skip if the type doesn't match", {
		EXPECT([s](){
			IndVals_skip_fail_type p(*s);
		}).toThrowError();
	});
});

SCENARIO_END

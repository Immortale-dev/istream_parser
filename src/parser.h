#ifndef CPP_ISTREAM_PARSER_H_
#define CPP_ISTREAM_PARSER_H_

#include <iostream>
#include <vector>
#include <functional>
#include <format>

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a ## b
#define PRIVATE_FIELD_NAME() CONCAT(_private_field_, __COUNTER__)

#define PARSE_SETUP \
using ParseInterface::ParseInterface

#define PARSE(t) public: \
parse_it<t>

#define SKIP(t) private: \
parse_it<t> PRIVATE_FIELD_NAME()

#define SINGLE(a) \
{this, a}

#define COUNT(a) \
{this, [this](){ return a; }}

#define CALC_COUNT(a) \
{this, [this]()a}

#define UNTIL(a) \
{this, parse_until(a)}

#define UNTIL_END() \
{this, parse_until('\0')}

#define THROUGH(a) \
{this, parse_until(a), true}

#define THROUGH_LINE() \
{this, parse_until('\n'), true}

template<typename U>
struct parse_until {
	parse_until(U val) : value(val) {}
	U value;
};

class ParseInterface {
	template<typename T_ > friend class parse_it;
	public:
	    ParseInterface(std::istream &stream) : source(&stream) {}
	    ParseInterface(const ParseInterface&) = default;

	private:
		std::istream* source;
};

template<typename T>
class parse_it {
	public:
		parse_it(ParseInterface* obj) : source(obj->source) { process(); }
		parse_it(ParseInterface* obj, T check) requires(!std::is_base_of_v<ParseInterface, T>) : source(obj->source) { process(); if(arr.back() != check) parse_unexpected_error(check, arr.back()); }
		parse_it(ParseInterface* obj, std::function<size_t()> calc_length) : source(obj->source), length(calc_length()) { for(size_t i=0;i<length;i++) process(); }
		template<typename U>
		parse_it(ParseInterface* obj, parse_until<U> until, bool through=false) : source(obj->source) { process_until(until, through); }
		parse_it(const parse_it<T>& other) : source(other.source), arr(other.arr), length(other.length) {}

		T& operator()(){ return arr[0]; }
		T& operator[](size_t index) {return arr[index]; }
		std::vector<T>& vector() { return arr; }
		size_t size() { return arr.size(); }
		decltype(auto) begin() { return arr.begin(); }
		decltype(auto) end() { return arr.end(); }

	private:
		bool is_empty_char(char c) { return c=='\n' || c=='\t' || c=='\r' || c==' '; };
		char get_char() { char c; source->get(c); return c; }
		void skip_empty(char but='.') { while(!source->eof() && source->peek() != but && is_empty_char(source->peek())) get_char(); }
		void parse() { size_t num = source->tellg(); arr.emplace_back(); *source >> arr.back(); if (source->fail()) parse_error(num); }
		void emplace() { arr.emplace_back(*source); }
		void parse_error(size_t num) { throw std::logic_error(std::format("Cannot parse \"{}\" at pos: {}", std::string(typeid(T).name()), std::to_string(num))); }
		void parse_unexpected_error(T& expected, T& actual) { throw std::logic_error(std::format("Expected: \"{}\" but got \"{}\"", expected, actual)); }
		void process() { if constexpr (!std::is_base_of_v<ParseInterface, T>) parse(); else emplace(); }
		template<typename U>
		void process_until(parse_until<U> &until, bool through) requires std::is_same_v<char, U> {
			while(1) {
				if constexpr (!std::is_same_v<char, T>) skip_empty(until.value);
				if (source->eof() || source->peek() == until.value) break;
				process();
			}
			if (through) get_char();
		}

		std::istream* source;
		std::vector<T> arr;
		size_t length = 1;
};

#endif // CPP_ISTREAM_PARSER_H_

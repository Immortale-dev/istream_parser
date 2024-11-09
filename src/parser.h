#ifndef CPP_ISTREAM_PARSER_H_
#define CPP_ISTREAM_PARSER_H_

#include <sstream>
#include <iostream>
#include <type_traits>
#include <vector>
#include <functional>
#include <format>

#define PARSE(t) \
parse_it<t>

#define SINGLE() \
this

#define PARSE_INTERFACE(name) \
struct name : ParseInterface

#define PARSE_SETUP \
using ParseInterface::ParseInterface

#define COUNT(a) \
{this, [this](){ return a;}}

#define CALC_COUNT(a) \
{this, [this]()a}

#define UNTIL(a) \
{this, parse_until(a)}

struct ParseInterface {
	ParseInterface() = default;
    ParseInterface(std::istream &stream) : source(&stream) {}
    ParseInterface(std::istream *stream) : source(stream) {}
    ParseInterface(const ParseInterface&) = default;

    std::istream* source;
};

template<typename U>
struct parse_until {
	parse_until(U val) : value(val) {}
	U value;
};

template<typename T>
struct parse_it {
	std::istream* source;
	std::vector<T> arr;
	size_t length = 1;

	T& operator()(){ return arr[0]; }
	T& operator[](size_t index) {return arr[index]; }
	std::vector<T>& vector() { return arr; }
	size_t size() { return arr.size(); }
	decltype(auto) begin() { return arr.begin(); }
	decltype(auto) end() { return arr.end(); }

	bool is_empty_char(char c) { return c=='\n' || c=='\t' ||  c==' '; };
	void skip_empty(char but='.') { char c;	while(!source->eof() && source->peek() != but && is_empty_char(source->peek())) source->get(c); }
	void parse() { size_t num = source->tellg(); arr.emplace_back(); *source >> arr.back(); if (source->fail()) parse_error(num); }
	void emplace() { arr.emplace_back(source); }
	void parse_error(size_t num) { throw std::logic_error(std::format("Cannot parse \"{}\" at pos: {}", std::string(typeid(T).name()), std::to_string(num))); }
	void process() { if constexpr (!std::is_base_of_v<ParseInterface, T>) parse(); else emplace(); }
	template<typename U>
	void process_until(parse_until<U> &until) requires std::is_same_v<char, U> {
		while(1) {
			if constexpr (!std::is_same_v<char, T>) skip_empty(until.value);
			if (source->eof() || source->peek() == until.value) break;
			process();
		}
	}

	parse_it(ParseInterface* obj) : source(obj->source) { process(); }
	parse_it(ParseInterface* obj, std::function<size_t()> calc_length) : source(obj->source), length(calc_length()) { for(size_t i=0;i<length;i++) process(); }
	template<typename U>
	parse_it(ParseInterface* obj, parse_until<U> until) : source(obj->source) { process_until(until); }

	parse_it(const parse_it<T>& other) : source(other.source), arr(other.arr), length(other.length) {}
};

#endif // CPP_ISTREAM_PARSER_H_

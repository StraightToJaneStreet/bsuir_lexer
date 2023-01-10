#include <iostream>
#include <variant>

#include <lexy/dsl.hpp>
#include <lexy/action/parse.hpp>
#include <lexy/error.hpp>
#include <lexy/callback.hpp>
#include <lexy/input/file.hpp>

#include <lexy_ext/report_error.hpp>
#include <vector>

namespace ast {
using lua_nil = std::nullptr_t;
using lua_bool = bool;
using lua_number = int64_t;
using lua_string = std::string;

struct lua_value;

struct lua_functioncall {
  std::string_view function_name;
  std::vector<lua_value> _arguments;
  lua_functioncall(std::string name, std::vector<lua_value> arguments);
};

struct lua_value {
  std::variant<bool, lua_number> variant;

  template<typename T>
  lua_value(T t) : variant(std::move(t)) { };
};

lua_functioncall::lua_functioncall(std::string name, std::vector<lua_value> arguments) {
  function_name = name;
  std::copy(arguments.cbegin(), arguments.cend(), std::back_inserter(_arguments));
}

struct lua_statement {
  std::variant<lua_functioncall> variant;

  template<typename T>
  explicit lua_statement(T t) : variant(std::move(t)) { }
};

struct lua_literal {
  std::variant<lua_nil, lua_bool, lua_number, lua_string> variant;

  template<typename T>
  explicit lua_literal(T &t) : variant(std::move(t)) {}

  static void _print(lua_nil &_nil) {
	std::printf("NIL");
  }

  static void _print(lua_bool value) {
	if (value) {
	  std::printf("true");
	} else {
	  std::printf("false");
	}
  }

  static void _print(lua_number value) {
	std::printf("%lld", value);
  }

  static void _print(const lua_string& val) {
	std::printf("%s", val.c_str());
  }

  void print() {
	std::visit([&](auto &value) { _print(value); }, variant);
  }
};
}

namespace grammar {
namespace dsl = lexy::dsl;

struct lua_nil : lexy::transparent_production {
  static constexpr auto rule = LEXY_LIT("nil");
  static constexpr auto value = lexy::constant(1);
};

struct lua_bool : lexy::token_production {
  struct lua_bool_false : lexy::transparent_production {
	static constexpr auto rule = LEXY_LIT("true");
	static constexpr auto value = lexy::constant(true);
  };

  struct lua_bool_true : lexy::transparent_production {
	static constexpr auto rule = LEXY_LIT("false");
	static constexpr auto value = lexy::constant(false);
  };

  static constexpr auto rule = dsl::p<lua_bool_false> | dsl::p<lua_bool_true>;
  static constexpr auto value = lexy::forward<bool>;
};

struct lua_number : lexy::transparent_production {
  static constexpr auto name = "Number";

  static constexpr auto rule = dsl::integer<std::int64_t>(dsl::digits<dsl::decimal>);
  static constexpr auto value = lexy::as_integer<int64_t>;
};

struct lua_value : lexy::token_production {
  static constexpr auto rule = dsl::p<lua_bool>;
  static constexpr auto value = lexy::construct<ast::lua_value>;
};

struct lua_functionname : lexy::transparent_production {
	static constexpr auto rule = LEXY_LIT("name");
	static constexpr auto value = lexy::constant("blabla");
};

struct lua_functionargs : lexy::transparent_production {
	static constexpr auto rule = dsl::parenthesized.list(dsl::p<lua_value>, dsl::sep(dsl::comma));
	static constexpr auto value = lexy::as_list<std::vector<ast::lua_value>>;
};

struct lua_functioncall : lexy::token_production {
  static constexpr auto rule = dsl::p<lua_functionname> + dsl::p<lua_functionargs>;
  static constexpr auto value = lexy::callback([](std::string name, std::vector<ast::lua_value> args) {
	return ast::lua_functioncall(name, args);
  });
};

struct lua_statement : lexy::token_production {
  static constexpr auto rule = dsl::p<lua_functioncall>;
  static constexpr auto value = lexy::construct<ast::lua_statement>;
};

struct lua {
  static constexpr auto max_recursion_depth = 19;

  static constexpr auto whitespace = dsl::ascii::space/dsl::ascii::newline;

  static constexpr auto rule = dsl::list(dsl::peek(dsl::p<lua_statement>) >> dsl::p<lua_statement>);
  static constexpr auto value = lexy::as_list<std::vector<ast::lua_statement>>;
};
}

int main() {
  auto file = lexy::read_file<lexy::ascii_encoding>("input.data");

  if (!file) {
	std::printf("Cant find input file.\n");
	return 0;
  }
  auto result = lexy::parse<grammar::lua_statement>(file.buffer(), lexy_ext::report_error.path("dump.log"));
  //std::cout << "Result: " << result.value().size() << std::endl;
  auto x = ast::lua_statement(ast::lua_functioncall("sdfsdf", std::vector<ast::lua_value>()));
  std::cout << "Errors: " << result.error_count() << std::endl;

  return 0;
}
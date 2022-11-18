#include <cstdio>
#include <string_view>

#include <lexy/action/parse.hpp>
#include <lexy/callback.hpp>
#include <lexy/dsl.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>

struct Color {
  std::uint8_t r, g, b;
};

struct expression_t {
};

namespace grammar {
namespace dsl = lexy::dsl;

struct nil_literal {
  static constexpr auto rule = LEXY_LIT("nil");

  static constexpr auto value = lexy::constant(10);
};

struct tail_literal {
  static constexpr auto rule = LEXY_LIT("...");

  static constexpr auto value = lexy::constant(20);
};

struct number_literal {
  static constexpr auto rule = dsl::digits<>;

  static constexpr auto value = lexy::constant(20);
};

struct bool_literal_true {
  static constexpr auto rule = LEXY_LIT("true") | LEXY_LIT("TRUE");

  static constexpr auto value = lexy::constant(20);
};

struct bool_literal_false {
  static constexpr auto rule = LEXY_LIT("false") | LEXY_LIT("FALSE");

  static constexpr auto value = lexy::constant(10);
};

struct bool_literal : dsl::rule_base {
  static constexpr auto rule
	  = dsl::p<::grammar::bool_literal_true>
		  | dsl::p<::grammar::bool_literal_false>;
  static constexpr auto value = lexy::constant(10);
};

struct exp {
  static constexpr auto rule
	  = dsl::p<::grammar::bool_literal>
		  | dsl::p<::grammar::nil_literal>
		  | dsl::p<::grammar::number_literal>;
  static constexpr auto value = lexy::constant(10);
};

struct explist : dsl::rule_base {
  static constexpr auto rule
	  = dsl::list(dsl::p<::grammar::exp>, dsl::sep(dsl::comma));

  static constexpr auto value = lexy::forward<int8_t>;

};
} // namespace grammar

int main() {
  auto input = lexy::string_input(std::string_view("#FFFFFF"));
  auto result = lexy::parse<grammar::explist>(input, lexy_ext::report_error);

  return result ? 0 : 1;
}
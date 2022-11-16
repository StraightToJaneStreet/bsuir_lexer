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

namespace grammar
{
namespace dsl = lexy::dsl;

struct nil_literal {
    static constexpr auto rule = LEXY_LIT("nil");
};

struct tail_literal {
    static constexpr auto rule = LEXY_LIT("...");
};

struct number_literal {
    static constexpr auto rule = dsl::digits<>;
};

struct bool_literal_true {
    static constexpr auto rule = LEXY_LIT("true") | LEXY_LIT("TRUE");
};

struct bool_literal_false {
    static constexpr auto rule = LEXY_LIT("false") | LEXY_LIT("FALSE");
};

struct bool_literal {
    static constexpr auto rule
        = dsl::p<::grammar::bool_literal_true>
          | dsl::p<::grammar::bool_literal_false>;
};

struct exp {
    static constexpr auto rule
        = dsl::p<::grammar::bool_literal>
          | dsl::p<::grammar::nil_literal>
          | dsl::p<::grammar::number_literal>;
};

struct explist {
    static constexpr auto rule
        = dsl::p<exp>;
};
} // namespace grammar

int main ()
{
  auto input = lexy::string_input (std::string_view ("#FFFFFF"));
  auto result = lexy::parse<grammar::color> (input, lexy_ext::report_error);

  if (result.has_value ())
    {
      auto color = result.value ();
      std::printf ("#%02x%02x%02x\n", color.r, color.g, color.b);
    }

  return result ? 0 : 1;
}
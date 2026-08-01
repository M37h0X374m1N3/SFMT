#include <print>

#include "Keywords.h"
#include "SourceBytes.h"
#include "Tokenizer.h"

void stdout_test(const MultiArray &arr);
void readable_flags(TokenFlags f);
void readable_tag(Tag t);

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::println(stderr, "usage: SFMT <file path>");
    return 2;
  }

  ++argv;

  auto sBytes = SourceBytes::from_file(*argv);

  if (sBytes.has_value()) {
    SourceBytes sb = sBytes.value();

    Tokenizer tk = Tokenizer(sb);

    size_t sbsz = sb.size();

    uint32_t upfront_alloc = sbsz / 2;

    MultiArray ma{upfront_alloc, upfront_alloc, upfront_alloc, upfront_alloc};
    tk.scan(ma);

    auto &[starts, ends, tags, flags] = ma;

    stdout_test(ma);
  }

  return 0;
}

void stdout_test(const MultiArray &arr) {
  auto arr_size = arr.size();
  for (size_t i{}; i < arr_size; ++i) {
    std::println("Full span: {}", arr.ends[i] - arr.starts[i]);
    readable_tag(arr.tags[i]);
  }
}

void readable_flags(TokenFlags f) {
  std::println("escaped: {}, follows space: {}, unterminated: {}, float: {}, "
               "overflowed: {}, error: {} ",
               f.has(TokenFlags::has_escape), f.has(TokenFlags::follows_space),
               f.has(TokenFlags::unterminated), f.has(TokenFlags::is_float),
               f.has(TokenFlags::has_overflow), f.has(TokenFlags::has_error));
  auto rad = std::to_underlying(f.radix());
  std::println("radix: {}", rad == 64    ? "Octal"
                            : rad == 128 ? "Hex"
                            : rad == 192 ? "Bin"
                                         : "Dec / Not a number");
}

void readable_tag(Tag t) {
  auto underlying = std::to_underlying(t);
  switch (underlying) {
  case 0:
    std::println("none");
    break;
  case 1:
    std::println("eof");
    break;
  case 2:
    std::println("string");
    break;
  case 3:
    std::println("number");
    break;
  case 4:
    std::println("identifier");
    break;
  case 5:
    std::println("plus");
    break;
  case 6:
    std::println("minus");
    break;
  case 7:
    std::println("slash");
    break;
  case 8:
    std::println("asterisk");
    break;
  case 9:
    std::println("equal");
    break;
  case 10:
    std::println("left angle bracket");
    break;
  case 11:
    std::println("right angle bracket");
    break;
  case 12:
    std::println("less than or equal");
    break;
  case 13:
    std::println("greater than or equal");
    break;
  case 14:
    std::println("left shift");
    break;
  case 15:
    std::println("right shift");
    break;
  case 16:
    std::println("left parenthesis");
    break;
  case 17:
    std::println("right parenthesis");
    break;
  case 18:
    std::println("left square bracket");
    break;
  case 19:
    std::println("right square bracket");
    break;
  case 20:
    std::println("left brace");
    break;
  case 21:
    std::println("right brace");
    break;
  case 22:
    std::println("colon");
    break;
  case 23:
    std::println("question mark");
    break;
  case 24:
    std::println("bang");
    break;
  case 25:
    std::println("comma");
    break;
  case 26:
    std::println("period");
    break;
  case 27:
    std::println("pipe");
    break;
  case 28:
    std::println("amp");
    break;
  case 29:
    std::println("tilde");
    break;
  case 30:
    std::println("hash");
    break;
  case 31:
    std::println("backtick");
    break;
  case 32:
    std::println("backslash");
    break;
  case 33:
    std::println("dollar sign");
    break;
  case 34:
    std::println("double dollar sign");
    break;
  case 35:
    std::println("double at sign");
    break;
  case 36:
    std::println("(back) at sign");
    break;
  case 37:
    std::println("(forward) at sign");
    break;
  case 38:
    std::println("percent");
    break;
  case 39:
    std::println("end of statement");
    break;
  default:
    std::println("<keyword> {}", kw_src[underlying - 40]);
    break;
  }
}

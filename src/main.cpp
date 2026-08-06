#include <print>

#include "Parser.h"
#include "SourceBytes.h"
#include "Tokenizer.h"

#ifdef DEBUG
void stdout_test(const MultiArray &arr, const SourceBytes &sb);
#endif

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::println(stderr, "usage: SFMT <file path>");
    return 2;
  }
  ++argv;

  auto srcRes = SourceBytes::from_file(*argv);
  if (!srcRes) {
    std::println(stderr, "err: code {}",
                 std::to_underlying(srcRes.error().code));
    return 1;
  }

#ifdef DEBUG

  const SourceBytes src{std::move(*srcRes)};
  const auto SourceSize{src.size()};
  Tokenizer tnizer{src};
  auto tokens{MultiArray::init_ceil64(SourceSize)};

  tnizer.scan(tokens);
  stdout_test(tokens, src);
  auto nodes{NodeArray::init_ceil64(SourceSize)};
  Parser parser(tokens, nodes);
  parser.parse();

#endif

  return 0;
}

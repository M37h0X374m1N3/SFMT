#include <print>

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

  const SourceBytes src{std::move(*srcRes)};
  Tokenizer tok{src};
  MultiArray mar{src.size()};

  tok.scan(mar);
#ifdef DEBUG
  stdout_test(mar, src);
#endif
  return 0;
}

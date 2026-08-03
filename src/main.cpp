#include <print>

#include "SourceBytes.h"
#include "Tokenizer.h"


#ifdef DEBUG

void stdout_test(const MultiArray &arr);

int main(int argc, const char **argv) {
  if (argc < 2) {
    std::println(stderr, "usage: SFMT <file path>");
    return 2;
  }
  ++argv;

  auto sBytesRes = SourceBytes::from_file(*argv);
  if (sBytesRes.has_value()) {
    auto sBytes = sBytesRes.value();
    auto tokenizer = Tokenizer(sBytes);
    uint32_t mArrayCap = sBytes.size() / 2; // conservative estimate of 2 bytes per token

    auto mArray = MultiArray(mArrayCap);
    tokenizer.scan(mArray); // careful - mutable structured bindings unpacking (use for tests only)
    stdout_test(mArray);
  }
  return 0;
}

#else

int main(int argc, const char **argv) {
    if (argc < 2) {
        std::println(stderr, "usage: SFMT <file path>");
        return 2;
    }
    ++argv;

    auto sBytesRes = SourceBytes::from_file(*argv);
    if (sBytesRes.has_value()) {
        auto sBytes = sBytesRes.value();
        auto tokenizer = Tokenizer(sBytes);
        uint32_t mArrayCap = sBytes.size() / 2;
        auto mArray = MultiArray(mArrayCap);
        tokenizer.scan(mArray);
    }
    return 0;
}

#endif

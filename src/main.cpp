#include <print>

#include "SourceBytes.h"
#include "Tokenizer.h"

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

      uint32_t sblen = sb.size() / 3;

      MultiArray ma { sblen, sblen, sblen };

      tk.scan(ma);

      for (size_t i {}; i < sb.size(); ++i) { 
          std::print("{}", sb.at(i)); 
      }

      for (size_t i {}; i < ma.flags.size(); ++i) {
          std::print("flags: {}, tags: {}\n", ma.flags[i].bits, std::to_underlying(ma.tags[i]));
      }

  }

  return 0;
}

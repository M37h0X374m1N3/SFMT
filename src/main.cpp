#include <print>

#include "SourceBytes.h"
#include "Tokenizer.h"

void stdout_test(const MultiArray& arr);

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

      uint32_t upfront_alloc = sbsz / 3;

      MultiArray ma { upfront_alloc, upfront_alloc, upfront_alloc };
      tk.scan(ma);

      stdout_test(ma);

  }

  return 0;
}

void readable_flags(TokenFlags f);

void stdout_test(const MultiArray& arr) {
    auto flags_sz = arr.flags.size();
    auto tags_sz = arr.tags.size();
    auto starts_sz = arr.starts.size();

    if (flags_sz == tags_sz && flags_sz == starts_sz) {
        for (size_t i {}; i < flags_sz; ++i) {
            // readable_flags(arr.flags[i]);
            std::print("Tag value: {}, ", std::to_underlying(arr.tags[i]));
            auto d = i < flags_sz - 1 ? arr.starts[i + 1] - arr.starts[i] : 0;
            std::println("Length: {}", d);
        }
    }

}

void readable_flags(TokenFlags f) {
    std::println(
            "has_escape: {}, follows_space: {}, unterminated: {}, is_float: {}, has_overflow: {}, has_error: {} ",
            f.has(TokenFlags::has_escape), f.has(TokenFlags::follows_space), f.has(TokenFlags::unterminated),
            f.has(TokenFlags::is_float), f.has(TokenFlags::has_overflow), f.has(TokenFlags::has_error)
            );
    auto rad = std::to_underlying(f.radix());
    std::println("radix: {}", rad == 64 ? "Octal" : rad == 128 ? "Hex" : rad == 192 ? "Bin" : "None / Dec");
}

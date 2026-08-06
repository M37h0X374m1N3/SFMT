#ifndef PARSER_H
#define PARSER_H

#include "Basics.h"
#include <utility>

consteval std::array<uint8_t, 256> make_prec() {
  std::array<uint8_t, 256> p{};
  p[std::to_underlying(Tag::pipe)] = 1;
  p[std::to_underlying(Tag::ampersand)] = 2;
  p[std::to_underlying(Tag::plus)] = 3;
  p[std::to_underlying(Tag::minus)] = 3;
  p[std::to_underlying(Tag::asterisk)] = 4;
  p[std::to_underlying(Tag::slash)] = 4;
  p[std::to_underlying(Tag::percent)] = 4;
  return p;
}
inline constexpr auto prec = make_prec();

class Parser {
public:
  Parser(const MultiArray &toks, NodeArray &out)
      : tags_{std::get<2>(toks.arrays())}, toks_{toks}, out_{out} {}

  Parser(Parser &&) = delete;
  Parser &operator=(Parser &&) = delete;

private:
  Tag peek(uint32_t i = 0) const {
    assert(i < max_lookahead);
    return tags_[pos_ + i];
  }

  void advance() { ++pos_; }

  std::span<const Tag> tags_;
  const MultiArray &toks_;
  NodeArray &out_;
  uint32_t pos_{};
};

#endif

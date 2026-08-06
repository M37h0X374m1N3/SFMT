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

  void parse() {
    while (peek() != Tag::eof) {
      advance();
    }
  }

  uint32_t parse_expr(uint8_t min_prec = 1) {
    uint32_t lhs{parse_primary()};
    while (true) {
      uint8_t p{prec[std::to_underlying(peek())]};
      if (p < min_prec)
          return lhs;
      uint32_t op_tok { pos_ };
      advance();
      uint32_t rhs { parse_expr(p + 1) };
      lhs = out_.add(NodeTag::binary, op_tok, lhs, rhs);
    }
  }

  uint32_t parse_primary() {
    uint32_t t{pos_};
    switch (peek()) {
    case Tag::number:
      advance();
      return out_.add(NodeTag::number, t, 0, 0);
    case Tag::identifier:
      advance();
      return out_.add(NodeTag::ident, t, 0, 0);
    case Tag::string:
      advance();
      return out_.add(NodeTag::string, t, 0, 0);
    case Tag::dollar_sign:
      advance();
      return out_.add(NodeTag::here, t, 0, 0);
    case Tag::double_dollar_sign:
      advance();
      return out_.add(NodeTag::section_base, t, 0, 0);
    default:
      return 0;
    }
  };

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

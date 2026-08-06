#ifndef PARSER_H
#define PARSER_H

#include "Basics.h"

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

private:
  Tag peek(uint32_t i = 0) const { return tags_[pos_ + i]; }
  void advance() { ++pos_; }

  std::span<const Tag> tags_;
  const MultiArray &toks_;
  NodeArray &out_;
  uint32_t pos_{};
};

#endif

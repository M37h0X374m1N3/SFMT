#ifndef PARSER_H
#define PARSER_H

#include <utility>

#include "Basics.h"

consteval std::array<uint8_t, 256> make_prec() {
    std::array<uint8_t, 256> p {};
    p[std::to_underlying(Tag::pipe)] = 1;
    p[std::to_underlying(Tag::ampersand)] = 2;
    p[std::to_underlying(Tag::l_angle_bracket)] = 3;
    p[std::to_underlying(Tag::r_angle_bracket)] = 3;
    p[std::to_underlying(Tag::plus)] = 4;
    p[std::to_underlying(Tag::minus)] = 4;
    p[std::to_underlying(Tag::asterisk)] = 5;
    p[std::to_underlying(Tag::slash)] = 5;
    p[std::to_underlying(Tag::percent)] = 5;
    
    return p;
}
inline constexpr auto prec = make_prec();

enum class ParseDiag : uint8_t {
    none,
    expected,
    fatal
};

class Parser {
  public:
    Parser(const MultiArray &toks, NodeArray &out)
        : tags_ {std::get<2>(toks.arrays())}, toks_ {toks}, out_ {out} {}

    Parser(Parser &&) = delete;
    Parser &operator=(Parser &&) = delete;

    void parse_root() {
        while (peek() != Tag::eof) {
            while (peek() == Tag::endstmt)
                advance();
            if (peek() == Tag::eof) break;
            const uint32_t root {parse_expr(1)};
            if (!root) {
                while (peek() != Tag::endstmt && peek() != Tag::eof)
                    advance();
                continue;
            }
            out_.push_extra(root);
        }
    }

    uint32_t parse_expr(uint8_t min_prec) {
        uint32_t lhs {parse_atom()};
        if (!lhs) return 0;
        for (;;) {
            const uint8_t p {prec[std::to_underlying(peek())]};
            if (p == 0 || p < min_prec) return lhs;

            const uint32_t main {pos_};
            advance();
            const uint32_t rhs {parse_expr(p + 1)};
            if (!rhs) return 0;
            lhs = out_.add(NodeTag::binary, main, lhs, rhs);
        }
    }

    uint32_t parse_atom() {
        const uint32_t tok {pos_};
        switch (peek()) {
            case Tag::number: advance(); return out_.add(NodeTag::number, tok, 0, 0);
            case Tag::identifier: advance(); return out_.add(NodeTag::ident, tok, 0, 0);
            case Tag::minus:
            case Tag::tilde:
            case Tag::bang: {
                advance();
                const uint32_t operand {parse_atom()};
                return operand ? out_.add(NodeTag::unary, tok, operand, 0) : 0;
            }
            case Tag::l_paren: {
                advance();
                const uint32_t inner {parse_expr(1)};
                if (peek() != Tag::r_paren) return 0;
                advance();
                return inner;
            }
            default: return 0;
        }
    }



  private:
    Tag peek(uint32_t i = 0) const {
        assert(i < max_lookahead);
        return tags_[pos_ + i];
    }

    void advance() { ++pos_; }

    std::span<const Tag> tags_;
    const MultiArray &toks_;
    NodeArray &out_;
    uint32_t pos_ {};
};

#endif

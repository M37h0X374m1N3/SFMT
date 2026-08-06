#ifndef BASICS_H
#define BASICS_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <tuple>

using std::uint64_t, std::uint32_t, std::uint16_t, std::uint8_t, std::size_t;

/* primary token identifier | keep to tokens and keywords only */
enum class Tag : uint8_t {
  none = 0,
  eof,
  string,
  number,
  identifier,
  plus,
  minus,
  slash,
  asterisk,
  equal,
  l_angle_bracket,
  r_angle_bracket,
  l_angle_equal,
  r_angle_equal,
  l_shift,
  r_shift,
  l_paren,
  r_paren,
  l_bracket,
  r_bracket,
  l_brace,
  r_brace,
  colon,
  question,
  bang,
  comma,
  dot,
  pipe,
  ampersand, // 28
  tilde,
  hash,
  backtick,
  backslash,
  dollar_sign,
  double_dollar_sign,
  at_at,
  at_back,
  at_forward,
  percent,
  endstmt, // 39
  // keywords
  kw_common = 40,
  kw_forward,
  kw_reverse,
  kw_local,
  kw_while,
  kw_repeat,
  kw_match,
  kw_macro,
  kw_struc,
  kw_restruc,
  kw_struct,
  kw_end,
  kw_if,
  kw_else,
  kw_include,
  kw_db,
  kw_dw,
  kw_dd,
  kw_dq,
  kw_dt,
  kw_rb,
  kw_rw,
  kw_rd,
  kw_rq,
  kw_byte,
  kw_word,
  kw_dword,
  kw_qword,
  kw_tword,
  kw_yword,
  kw_zword,
  kw_ptr,
  kw_equ,
  kw_section,
  kw_segment,
  kw_code,
  kw_data,
  kw_readable,
  kw_writeable,
  kw_executable,
  kw_entry,
  kw_extern,
  kw_public,
  kw_display,
  kw_org,
  kw_bits,
  kw_defined,
  kw_fix,
  kw_purge,
  kw_restore,
  kw_format,
  kw_console,
  kw_gui
};

/* associated with Tag and SourceBytes handle via shared index in MultiArray */
struct TokenFlags {
  uint8_t bits;

  static constexpr uint8_t has_escape = 0b0000'0001;
  static constexpr uint8_t follows_space = 0b0000'0010;
  static constexpr uint8_t unterminated = 0b0000'0100;
  static constexpr uint8_t is_float = 0b0000'1000;
  static constexpr uint8_t has_overflow = 0b0001'0000;
  static constexpr uint8_t has_error = 0b0010'0000;
  static constexpr uint8_t radix_mask = 0b1100'0000;

  enum Radix : uint8_t {
    oct = 0b0100'0000,
    hex = 0b1000'0000,
    bin = 0b1100'0000
  };

  bool has(uint8_t bit) const { return bits & bit; }
  void set(uint8_t bit) { bits |= bit; }
  void clear(uint8_t bit) { bits &= ~bit; }

  Radix radix() const { return static_cast<Radix>(bits & radix_mask); }
  void set_radix(Radix r) { bits = (bits & ~radix_mask) | r; }
};

/* contiguous allocated list */
template <class T> class SingleArray {
public:
  SingleArray(uint32_t n) : data_{std::make_unique_for_overwrite<T[]>(n)} {}

  SingleArray(SingleArray &&) = delete;
  SingleArray &operator=(SingleArray &&) = delete;

  const T &operator[](uint32_t index) const { return data_[index]; }

  friend class MultiArray;
  friend class NodeArray;

private:
  T *get() { return data_.get(); }
  const T *data() const { return data_.get(); }
  std::unique_ptr<T[]> data_;
};

class MultiArray {
public:
  MultiArray(uint32_t res)
      : starts_(res), ends_(res), tags_(res), flags_(res), cap_(res) {}

  MultiArray(MultiArray &&) = delete;
  MultiArray &operator=(MultiArray &&) = delete;

  void push(uint32_t s, uint32_t e, Tag t, TokenFlags f) {
    assert(size_ < cap_);
    starts_.get()[size_] = s;
    ends_.get()[size_] = e;
    tags_.get()[size_] = t;
    flags_.get()[size_] = f;
    ++size_;
  }

  uint32_t starts(uint32_t i) const {
    assert(i < size_);
    return starts_[i];
  }
  uint32_t ends(uint32_t i) const {
    assert(i < size_);
    return ends_[i];
  }
  Tag tags(uint32_t i) const {
    assert(i < size_);
    return tags_[i];
  }
  TokenFlags flags(uint32_t i) const {
    assert(i < size_);
    return flags_[i];
  }

  uint32_t size() const { return size_; }
  uint32_t cap() const { return cap_; }

  auto arrays() const {
    return std::tuple{std::span<const uint32_t>{starts_.data(), size_},
                      std::span<const uint32_t>{ends_.data(), size_},
                      std::span<const Tag>{tags_.data(), size_},
                      std::span<const TokenFlags>{flags_.data(), size_}};
  }

private:
  SingleArray<uint32_t> starts_, ends_;
  SingleArray<Tag> tags_;
  SingleArray<TokenFlags> flags_;
  uint32_t size_{};
  uint32_t cap_;
};

enum class NodeTag : uint8_t {
  none,
  root,
  label_def,
  instruction,
  data_def,
  reserve,
  equ,
  assign,
  fix,
  purge,
  restore,
  format,
  section,
  segment,
  org,
  entry,
  bits,
  d_extern,
  d_public,
  display,
  include,
  macro_def,
  struc_def,
  struct_def,
  if_stmt,
  else_stmt,
  while_stmt,
  repeat,
  match,
  local,
  common,
  forward,
  reverse,
  block,
  // expression
  number,
  string,
  ident,
  here,
  section_base,
  anon_label,
  unary,
  binary,
  mem,
  sized,
  seg_offset,
  dup,
  uninit
};

class NodeArray {

public:
  NodeArray(uint32_t nres, uint32_t eres)
      : tags_(nres), main_(nres), lhs_(nres), rhs_(nres), extra_(eres),
        cap_(nres), extra_cap_(eres) {
    add(NodeTag::none, 0, 0, 0);
  }

  NodeArray(NodeArray &&) = delete;
  NodeArray &operator=(NodeArray &&) = delete;

  uint32_t add(NodeTag t, uint32_t main, uint32_t lhs, uint32_t rhs) {
    assert(size_ < cap_);
    tags_.get()[size_] = t;
    main_.get()[size_] = main;
    lhs_.get()[size_] = lhs;
    rhs_.get()[size_] = rhs;
    return size_++;
  }

  void push_extra(uint32_t slot) {
    assert(extra_size_ < extra_cap_);
    extra_.get()[extra_size_++] = slot;
  }

  NodeTag tags(uint32_t i) const {
    assert(i < size_);
    return tags_[i];
  }
  uint32_t main(uint32_t i) const {
    assert(i < size_);
    return main_[i];
  }
  uint32_t lhs(uint32_t i) const {
    assert(i < size_);
    return lhs_[i];
  }
  uint32_t rhs(uint32_t i) const {
    assert(i < size_);
    return rhs_[i];
  }

  uint32_t extra(uint32_t k) const {
    assert(k < extra_size_);
    return extra_[k];
  }

  uint32_t size() const { return size_; }
  uint32_t cap() const { return cap_; }
  uint32_t extra_size() const { return extra_size_; }
  uint32_t extra_cap() const { return extra_cap_; }

  auto arrays() const {
    return std::tuple{std::span<const NodeTag>{tags_.data(), size_},
                      std::span<const uint32_t>{main_.data(), size_},
                      std::span<const uint32_t>{lhs_.data(), size_},
                      std::span<const uint32_t>{rhs_.data(), size_},
                      std::span<const uint32_t>{extra_.data(), extra_size_}};
  }

private:
  SingleArray<NodeTag> tags_;
  SingleArray<uint32_t> main_, lhs_, rhs_;
  SingleArray<uint32_t> extra_;
  uint32_t size_{}, cap_;
  uint32_t extra_size_{}, extra_cap_;
};

#endif

#include <print>

#include "Parser.h"
#include "SourceBytes.h"
#include "Tokenizer.h"

#ifdef DEBUG
static void dump_node(const NodeArray &n, const MultiArray &t, const SourceBytes &sb, uint32_t i) {
    const uint32_t tok {n.main(i)};
    const auto text {sb.substr(t.starts(tok), t.ends(tok) - t.starts(tok))};

    switch (n.tags(i)) {
        case NodeTag::number:
        case NodeTag::ident: std::print("{}", text); break;
        case NodeTag::unary:
            std::print("({} ", text);
            dump_node(n, t, sb, n.lhs(i));
            std::print(")");
            break;
        case NodeTag::binary:
            std::print("(");
            dump_node(n, t, sb, n.lhs(i));
            std::print(" {} ", text);
            dump_node(n, t, sb, n.rhs(i));
            std::print(")");
            break;
        default: std::print("?"); break;
    }
}

void dump_roots(const NodeArray &n, const MultiArray &t, const SourceBytes &sb) {
    for (uint32_t k {}; k < n.extra_size(); ++k) {
        dump_node(n, t, sb, n.extra(k));
        std::println("");
    }
}

#endif

int main(int argc, const char **argv) {
    if (argc < 2) {
        std::println(stderr, "usage: SFMT <file path>");
        return 2;
    }
    ++argv;

    auto srcRes = SourceBytes::from_file(*argv);
    if (!srcRes) {
        std::println(stderr, "err: code {}", std::to_underlying(srcRes.error().code));
        return 1;
    }

#ifdef DEBUG

    const SourceBytes src {std::move(*srcRes)};
    const auto SourceSize {src.size()};
    Tokenizer tnizer {src};
    auto tokens {MultiArray::init_ceil64(SourceSize)};

    tnizer.scan(tokens);
    auto nodes {NodeArray::init_ceil64(SourceSize)};
    Parser parser(tokens, nodes);
    parser.parse_root();

    dump_roots(nodes, tokens, src);

#endif

    return 0;
}

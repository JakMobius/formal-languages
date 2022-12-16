#pragma once

#include "../grammar.hpp"
#include "useless-symbol-remover.hpp"
#include "mixed-rule-remover.hpp"
#include "long-rule-remover.hpp"
#include "epsilon-creating-remover.hpp"
#include "chain-rule-remover.hpp"

class KNFConverter {
    Grammar &grammar;

public:
    KNFConverter(Grammar &grammar) : grammar(grammar) {}

    Grammar convert() {
        grammar = UselessSymbolRemover(grammar).remove();
        MixedRuleRemover(grammar).remove();
        LongRuleRemover(grammar).remove();
        EpsilonCreatingRemover(grammar).remove();
        ChainRuleRemover(grammar).remove();
        return grammar;
    }
};
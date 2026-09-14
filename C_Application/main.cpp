#include "GrammarExtraction.hpp"
#include "RegexParser.hpp"
#include <cassert>
#include <memory>
#include <iostream>

using namespace Regex;
using namespace CFG;

int main(int argc, char** argv) 
{
    auto l_regexParser = RegexParser();
    std::unique_ptr<IExtraction> l_extractor = std::make_unique<ExtractionCYK>();

    // ---------------------------------------------------------
    // 1. Basic Concatenation
    // ---------------------------------------------------------
    {
        // Infix: ab
        auto l_pattern = l_regexParser.constructRegexPattern("ab@");
        assert(l_extractor->matchPattern("ab", l_pattern));
        assert(!l_extractor->matchPattern("a", l_pattern));
        assert(!l_extractor->matchPattern("abc", l_pattern));
        assert(!l_extractor->matchPattern("b", l_pattern));
    }
    {
        // Infix: abc
        auto l_pattern = l_regexParser.constructRegexPattern("ab@c@");
        assert(l_extractor->matchPattern("abc", l_pattern));
        assert(!l_extractor->matchPattern("ab", l_pattern));
        assert(!l_extractor->matchPattern("bc", l_pattern));
        assert(!l_extractor->matchPattern("abcd", l_pattern));
    }

    // ---------------------------------------------------------
    // 2. Basic Alternation
    // ---------------------------------------------------------
    {
        // Infix: a|b
        auto l_pattern = l_regexParser.constructRegexPattern("ab|");
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("b", l_pattern));
        assert(!l_extractor->matchPattern("ab", l_pattern));
        assert(!l_extractor->matchPattern("c", l_pattern));
    }
    {
        // Infix: a|b|c
        auto l_pattern = l_regexParser.constructRegexPattern("ab|c|");
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("b", l_pattern));
        assert(l_extractor->matchPattern("c", l_pattern));
        assert(!l_extractor->matchPattern("abc", l_pattern));
        assert(!l_extractor->matchPattern("d", l_pattern));
    }

    // ---------------------------------------------------------
    // 3. Unary Plus (1 or more)
    // ---------------------------------------------------------
    {
        // Infix: a+
        auto l_pattern = l_regexParser.constructRegexPattern("a+");
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("aa", l_pattern));
        assert(l_extractor->matchPattern("aaaaa", l_pattern));
        assert(!l_extractor->matchPattern("b", l_pattern));
        assert(!l_extractor->matchPattern("ab", l_pattern));
    }
    {
        // Infix: a+b
        auto l_pattern = l_regexParser.constructRegexPattern("a+b@");
        assert(l_extractor->matchPattern("ab", l_pattern));
        assert(l_extractor->matchPattern("aab", l_pattern));
        assert(l_extractor->matchPattern("aaaab", l_pattern));
        assert(!l_extractor->matchPattern("b", l_pattern));
        assert(!l_extractor->matchPattern("aba", l_pattern));
    }

    // ---------------------------------------------------------
    // 4. Wildcard Operations
    // ---------------------------------------------------------
    {
        // Infix: . (any single char)
        auto l_pattern = l_regexParser.constructRegexPattern(".");
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("1", l_pattern));
        assert(l_extractor->matchPattern("Z", l_pattern));
        assert(!l_extractor->matchPattern("aa", l_pattern));
    }
    {
        // Infix: .. (any two chars)
        auto l_pattern = l_regexParser.constructRegexPattern("..@");
        assert(l_extractor->matchPattern("ab", l_pattern));
        assert(l_extractor->matchPattern("12", l_pattern));
        assert(!l_extractor->matchPattern("a", l_pattern));
        assert(!l_extractor->matchPattern("abc", l_pattern));
    }
    {
        // Infix: .+ (1 or more of any char)
        auto l_pattern = l_regexParser.constructRegexPattern(".+");
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("abcde", l_pattern));
        assert(l_extractor->matchPattern("12345", l_pattern));
    }

    // ---------------------------------------------------------
    // 5. Complex Combinations
    // ---------------------------------------------------------
    {
        // Infix: (ab)|(cd)
        auto l_pattern = l_regexParser.constructRegexPattern("ab@cd@|");
        assert(l_extractor->matchPattern("ab", l_pattern));
        assert(l_extractor->matchPattern("cd", l_pattern));
        assert(!l_extractor->matchPattern("ac", l_pattern));
        assert(!l_extractor->matchPattern("abcd", l_pattern));
    }
    {
        // Infix: (a|b)(c|d)
        auto l_pattern = l_regexParser.constructRegexPattern("ab|cd|@");
        assert(l_extractor->matchPattern("ac", l_pattern));
        assert(l_extractor->matchPattern("ad", l_pattern));
        assert(l_extractor->matchPattern("bc", l_pattern));
        assert(l_extractor->matchPattern("bd", l_pattern));
        assert(!l_extractor->matchPattern("ab", l_pattern));
        assert(!l_extractor->matchPattern("cd", l_pattern));
    }
    {
        // Infix: (a|b)+ (any sequence of 'a' and 'b')
        auto l_pattern = l_regexParser.constructRegexPattern("ab|+");
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("b", l_pattern));
        assert(l_extractor->matchPattern("aba", l_pattern));
        assert(l_extractor->matchPattern("bbbaa", l_pattern));
        assert(!l_extractor->matchPattern("c", l_pattern));
        assert(!l_extractor->matchPattern("abc", l_pattern));
    }
    {
        // Infix: a(.+)b (starts with a, ends with b, at least one char between)
        auto l_pattern = l_regexParser.constructRegexPattern("a.+@b@");
        assert(l_extractor->matchPattern("acb", l_pattern));
        assert(l_extractor->matchPattern("a123b", l_pattern));
        assert(l_extractor->matchPattern("a_xyz_b", l_pattern));
        assert(!l_extractor->matchPattern("_ab", l_pattern)); // .+ needs at least 1 char
        assert(!l_extractor->matchPattern("ac", l_pattern));
        assert(!l_extractor->matchPattern("cb", l_pattern));
    }
    {
        // Infix: (ab)+c
        auto l_pattern = l_regexParser.constructRegexPattern("ab@+c@");
        assert(l_extractor->matchPattern("abc", l_pattern));
        assert(l_extractor->matchPattern("ababc", l_pattern));
        assert(l_extractor->matchPattern("abababc", l_pattern));
        assert(!l_extractor->matchPattern("ac", l_pattern));
        assert(!l_extractor->matchPattern("ab", l_pattern));
        assert(!l_extractor->matchPattern("abab", l_pattern));
    }
    
    // User's original test case
    {
        // Infix: kuba|.+d
        auto l_pattern = l_regexParser.constructRegexPattern("ku@ba@@.+d@|");
        assert(l_extractor->matchPattern("kubad", l_pattern)); // Matches .+d
        assert(l_extractor->matchPattern("kuba", l_pattern));  // Matches kuba
        assert(!l_extractor->matchPattern("kubac", l_pattern));
        assert(l_extractor->matchPattern("123d", l_pattern));  // Matches .+d
    }
    // ---------------------------------------------------------
    // 6. Multiple Word Dictionary (cat | dog | bird | fish)
    // ---------------------------------------------------------
    {
        // Infix: cat|dog|bird|fish
        // RPN:
        // cat  -> ca@t@
        // dog  -> do@g@
        // bird -> bi@r@d@
        // fish -> fi@s@h@
        auto l_pattern = l_regexParser.constructRegexPattern("ca@t@do@g@|bi@r@d@|fi@s@h@|");
        
        assert(l_extractor->matchPattern("cat", l_pattern));
        assert(l_extractor->matchPattern("dog", l_pattern));
        assert(l_extractor->matchPattern("bird", l_pattern));
        assert(l_extractor->matchPattern("fish", l_pattern));
        
        assert(!l_extractor->matchPattern("catdog", l_pattern)); // No concatenation between them
        assert(!l_extractor->matchPattern("c", l_pattern));
        assert(!l_extractor->matchPattern("bir", l_pattern));
    }

    // ---------------------------------------------------------
    // 7. Vowel Matching (1 or more vowels)
    // ---------------------------------------------------------
    {
        // Infix: (a|e|i|o|u|y)+
        // RPN: ae|i|o|u|y|+
        auto l_pattern = l_regexParser.constructRegexPattern("ae|i|o|u|y|+");
        
        assert(l_extractor->matchPattern("a", l_pattern));
        assert(l_extractor->matchPattern("e", l_pattern));
        assert(l_extractor->matchPattern("ie", l_pattern));
        assert(l_extractor->matchPattern("aeiouy", l_pattern));
        assert(l_extractor->matchPattern("oouu", l_pattern));
        
        assert(!l_extractor->matchPattern("b", l_pattern));
        assert(!l_extractor->matchPattern("ab", l_pattern)); // Contains consonant
        assert(!l_extractor->matchPattern("ea t", l_pattern)); 
    }

    // ---------------------------------------------------------
    // 8. Chained Character Alternations
    // ---------------------------------------------------------
    {
        // Infix: (a|b)(c|d)(e|f)(g|h)
        // RPN: ab|cd|@ef|@gh|@
        auto l_pattern = l_regexParser.constructRegexPattern("ab|cd|@ef|@gh|@");
        
        assert(l_extractor->matchPattern("aceg", l_pattern));
        assert(l_extractor->matchPattern("bdfh", l_pattern));
        assert(l_extractor->matchPattern("bceh", l_pattern));
        assert(l_extractor->matchPattern("adfg", l_pattern));
        
        assert(!l_extractor->matchPattern("abcd", l_pattern)); // 'b' is in the first group, not second
        assert(!l_extractor->matchPattern("ace", l_pattern));  // Too short
        assert(!l_extractor->matchPattern("acegh", l_pattern)); // Too long
    }

    // ---------------------------------------------------------
    // 9. HTTP Methods (GET | PUT | POST | PATCH)
    // ---------------------------------------------------------
    {
        // Infix: GET|PUT|POST|PATCH
        // RPN:
        // GET   -> GE@T@
        // PUT   -> PU@T@
        // POST  -> PO@S@T@
        // PATCH -> PA@T@C@H@
        auto l_pattern = l_regexParser.constructRegexPattern("GE@T@PU@T@|PO@S@T@|PA@T@C@H@|");
        
        assert(l_extractor->matchPattern("GET", l_pattern));
        assert(l_extractor->matchPattern("PUT", l_pattern));
        assert(l_extractor->matchPattern("POST", l_pattern));
        assert(l_extractor->matchPattern("PATCH", l_pattern));
        
        assert(!l_extractor->matchPattern("DELETE", l_pattern));
        assert(!l_extractor->matchPattern("GETT", l_pattern));
        assert(!l_extractor->matchPattern("POS", l_pattern));
    }

    // ---------------------------------------------------------
    // 10. Highly Branching Complex Structure with Kleene Plus
    // ---------------------------------------------------------
    {
        // Infix: ((ab|cd)+(ef|gh)+)|xyz
        // Breakdown:
        // (ab|cd)+   -> ab@cd@|+
        // (ef|gh)+   -> ef@gh@|+
        // concat     -> @
        // | xyz      -> xy@z@|
        // RPN: ab@cd@|+ef@gh@|+@xy@z@|
        
        auto l_pattern = l_regexParser.constructRegexPattern("ab@cd@|+ef@gh@|+@xy@z@|");
        
        // Matches right side of the main OR
        assert(l_extractor->matchPattern("xyz", l_pattern));
        
        // Matches left side of the main OR (1 of each group)
        assert(l_extractor->matchPattern("abef", l_pattern));
        assert(l_extractor->matchPattern("cdgh", l_pattern));
        assert(l_extractor->matchPattern("abgh", l_pattern));
        
        // Matches left side with repetitions
        assert(l_extractor->matchPattern("abcdabefgh", l_pattern));
        assert(l_extractor->matchPattern("cdcdghefef", l_pattern));
        
        // Invalid matches
        assert(!l_extractor->matchPattern("ab", l_pattern));    // Missing second half (ef|gh)
        assert(!l_extractor->matchPattern("ef", l_pattern));    // Missing first half (ab|cd)
        assert(!l_extractor->matchPattern("xyzef", l_pattern)); // Cannot cross the main OR barrier
        assert(!l_extractor->matchPattern("abxyz", l_pattern)); 
    }

    // ---------------------------------------------------------
    // 11. Wildcard Fallback Dictionary
    // ---------------------------------------------------------
    {
        // Infix: start|end|(.+)
        // Effectively: Matches "start", "end", or literally ANY string 
        // (Because .+ matches any string of length 1 or more)
        // RPN: st@a@r@t@en@d@|.+|
        auto l_pattern = l_regexParser.constructRegexPattern("st@a@r@t@en@d@|.+|");
        
        assert(l_extractor->matchPattern("start", l_pattern));
        assert(l_extractor->matchPattern("end", l_pattern));
        assert(l_extractor->matchPattern("anything", l_pattern));
        assert(l_extractor->matchPattern("x", l_pattern));
        assert(l_extractor->matchPattern("12345", l_pattern));
        assert(!l_extractor->matchPattern("", l_pattern));
    }
    std::string l_left = "{.+@}@";
    std::string l_right = "if@(@.+@)@";
    std::string l_condition = l_left + l_right + "|!";
    std::string l_patternText = ".+" + l_condition + ".+@@";
    auto l_pattern = l_regexParser.constructRegexPattern(l_patternText);
    std::string l_text = "def main():\n\tif(l_x < 5) \n\t{\n\t\t std::cout << \"Hello\"; \n\t} //for test";
    auto l_interval = l_extractor->matchPattern(l_text, l_pattern);

    if(l_interval)
    {
        std::string l_before = l_text.substr(0, l_interval->first);
        std::string l_match = l_text.substr(l_interval->first, l_interval->second - l_interval->first);
        std::string l_after = l_text.substr(l_interval->second);


        std::cout << l_before << "\033[31m" << l_match << "\033[0m" << l_after << "\n";
    }
}
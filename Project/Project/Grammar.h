#pragma once
#define _SILENCE_STDEXT_HASH_LOWER_BOUND_DEPRECATION_WARNING
#include <string>
#include <vector>
#include <regex>
#include <random>
#include <fstream>
#include <iostream>
#include <string_view>
#include <algorithm>
#include <set>
#include <format>
#include <unordered_set>
#include <unordered_map>


using production = std::pair<std::string, std::string>;

class Grammar
{
private:

	const std::string lambda{ "$" };
	std::vector<char> m_nonterminals;
	std::vector<char> m_terminals;
	std::vector<production> m_productions;
	char m_startSymbol;

	char nextNonusedNonterminal();
	char lastNonusedNonterminal(std::vector<char> addedZ);
	bool verifyRule1() const;
	bool verifyRule2() const;
	bool verifyRule3() const;
	bool verifyRule4() const;
	bool verifyRule5() const;

	bool wordStillHasNonterminals(const std::string&) const;

	void eliminateLambdaProductions();
	void eliminateRenames();
	void eliminateNonGeneratingSymbols();
	void eliminateInaccessibleSymbols();

	bool validCombination(int k, std::vector<int>& currentCombination);

public:
	void FNCconvert();

	void FNGconvert();

	Grammar() = default;
	Grammar(const Grammar&) = default;
	~Grammar() = default;
	Grammar& operator=(const Grammar&) = default;

	void ReadGrammar(std::istream&);
	bool IsContextIndependent() const;
	std::string GenerateWord(bool = true) const;
	bool VerifyGrammar(std::ostream&) const;
	void PrintGrammar() const;	
	void simplifyGrammar();
};
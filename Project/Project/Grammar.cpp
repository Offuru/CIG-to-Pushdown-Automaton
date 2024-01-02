#include "Grammar.h"

bool Grammar::verifyRule1() const
{
	std::vector<char> terminalsCopy(m_terminals);
	std::vector<char> nonterminalsCopy(m_nonterminals);
	std::sort(terminalsCopy.begin(), terminalsCopy.end());
	std::sort(nonterminalsCopy.begin(), nonterminalsCopy.end());

	std::vector<char> intersection;

	std::set_intersection(terminalsCopy.begin(), terminalsCopy.end(),
		nonterminalsCopy.begin(), nonterminalsCopy.end(),
		std::back_inserter(intersection));

	return intersection.size() == 0;
}

bool Grammar::verifyRule2() const
{
	for (auto nonterminal : m_nonterminals)
		if (nonterminal == m_startSymbol)
			return true;
	return false;
}

bool Grammar::verifyRule3() const
{
	for (const auto& production : m_productions)
	{
		bool found = false;
		for (auto& nonterminal : m_nonterminals)
			if (std::find(production.first.begin(), production.first.end(), nonterminal) != production.first.end())
			{
				found = true;
				break;
			}

		if (!found)
			return false;
	}

	return true;
}

bool Grammar::verifyRule4() const
{
	std::string startSymbolStr(1, m_startSymbol);

	for (const auto& rules : m_productions)
		if (rules.first == startSymbolStr)
			return true;

	return false;
}

bool Grammar::verifyRule5() const
{
	for (const auto& rule : m_productions)
	{
		std::string leftMember = rule.first;
		std::string rightMember = rule.second;

		if (rightMember == lambda)
			rightMember.clear();
		if (leftMember == lambda)
			leftMember.clear();

		for (const auto& symbol : m_nonterminals)
		{
			leftMember.erase(std::remove_if(leftMember.begin(), leftMember.end(), [&symbol](const char& a) {
				return a == symbol; }));
			rightMember.erase(std::remove_if(rightMember.begin(), rightMember.end(), [&symbol](const char& a) {
				return a == symbol; }));
		}

		for (const auto& symbol : m_terminals)
		{
			leftMember.erase(std::remove_if(leftMember.begin(), leftMember.end(), [&symbol](const char& a) {
				return a == symbol; }));
			rightMember.erase(std::remove_if(rightMember.begin(), rightMember.end(), [&symbol](const char& a) {
				return a == symbol; }));
		}

		if (rightMember.size() != 0 || leftMember.size() != 0)
			return false;
	}

	return true;
}

bool Grammar::wordStillHasNonterminals(const std::string& word) const
{
	for (const auto& symbol : m_nonterminals)
		if (std::regex_search(word, std::regex(std::string{ symbol })))
			return true;
	return false;
}

void Grammar::eliminateLambdaProductions()
{
	std::unordered_set<char> Vn;

	for (auto production : m_productions)
	{
		if (production.second == lambda && production.first.size() == 1)
			Vn.insert(production.first[0]);
	}

	//B->A1A2..An
	bool modifiedVn = true;

	while (!modifiedVn)
	{
		modifiedVn = false;

		for (auto production : m_productions)
		{
			if (production.first.size() == 1 &&
				Vn.find(production.first[0]) == Vn.end())
			{
				//check if A1A2.. in Vn
				bool allRightInVn = true;
				for (auto elem : production.second)
				{
					if (Vn.find(elem) == Vn.end())
					{
						allRightInVn = false;
						break;
					}
				}

				if (allRightInVn)
				{
					Vn.insert(production.first[0]);
					modifiedVn = true;
				}
			}
		}
	}

	std::vector<production> P1;

	for (auto production : m_productions)
	{
		if (production.second != lambda)
		{
			P1.push_back({ production.first, production.second });
			std::string rightMember = production.second;
			for (int i = 0; i < rightMember.size(); ++i)
			{
				if (Vn.find(rightMember[i]) != Vn.end())
				{
					std::string copy = rightMember;
					copy.erase(i, 1);

					if (copy.size() != 0)
						P1.push_back({ production.first, copy });
				}
			}
		}
	}

	if (Vn.find(m_startSymbol) != Vn.end())
	{
		std::string aux;
		aux += m_startSymbol;
		P1.push_back({ "@", aux });
		P1.push_back({ "@", lambda });

		m_startSymbol = '@';
	}

	m_productions = P1;
}

void Grammar::simplifyGrammar()
{
	/*if (!IsContextIndependent())
	{
		std::cout << "Can't simplify grammar! It isn't context independent.\n";
		return;
	}*/

	//eliminate lambda-productions ????

	eliminateLambdaProductions();

}

void Grammar::ReadGrammar(std::istream& in)
{
	uint32_t n;

	in >> n;

	m_nonterminals.resize(n);

	for (uint32_t i = 0; i < n; ++i)
		in >> m_nonterminals[i];

	in >> n;

	m_terminals.resize(n);

	for (uint32_t i = 0; i < n; ++i)
		in >> m_terminals[i];

	in >> m_startSymbol;

	in >> n;

	m_productions.resize(n);

	for (uint32_t i = 0; i < n; ++i)
		in >> m_productions[i].first >> m_productions[i].second;
}

bool Grammar::IsContextIndependent() const
{
	//u1Au2 -> u1wu2
	for (auto production : m_productions)
	{
		if (!wordStillHasNonterminals(production.first))
			return false;
	}

	// S -> lambda
	for (auto production : m_productions)
	{
		if (production.second == lambda)
		{
			if (production.first.size() != 1)
				return false;
			if (production.first[0] != m_startSymbol)
				return false;

			for (auto production2 : m_productions)
				if (std::regex_search(production2.second, std::regex(std::string{ m_startSymbol })))
					return false;

		}
	}

	//A->w
	for (auto production : m_productions)
	{
		if (production.first.size() != 1)
			return false;

		if (!wordStillHasNonterminals(production.first))
			return false;
	}

	return true;
}

std::string Grammar::GenerateWord(bool print) const
{
	std::string currentWord{ m_startSymbol };
	std::string oldWord = currentWord;

	while (wordStillHasNonterminals(currentWord))
	{
		std::vector<std::pair<std::string, std::string>> availableRules;
		std::smatch match;

		std::random_device rd;
		std::mt19937 eng1(rd());

		for (const auto& rules : m_productions)
		{
			if (std::regex_search(currentWord, match, std::regex(rules.first)))
				availableRules.push_back(rules);
		}
		if (availableRules.empty())
		{
			if (print)
				std::cout << "There are no available rules to apply!\n";
			return "-1";
		}

		std::uniform_int_distribution<> distr1(0, availableRules.size() - 1);

		std::pair<std::string, std::string> ruleToApply = availableRules[distr1(eng1)];
		availableRules.clear();

		std::regex rule(ruleToApply.first);

		std::string currentWordCpy{ currentWord };
		std::vector<uint32_t> availablePositionsToReplace;

		while (regex_search(currentWordCpy, match, rule))
		{
			availablePositionsToReplace.emplace_back(match.position(0));
			currentWordCpy = match.suffix().str();
		}

		if (availablePositionsToReplace.size() == 0)
		{
			if (print)
				std::cout << "Can't apply any rules in the current state!\n";
			return "-1";
		}
		std::uniform_int_distribution<> distr2(0, availablePositionsToReplace.size() - 1);

		std::mt19937 eng2(rd());
		uint32_t posToReplace = availablePositionsToReplace[distr2(eng2)];

		if (ruleToApply.second == lambda)
			ruleToApply.second = "";


		currentWord.replace(currentWord.begin() + posToReplace,
			currentWord.begin() + posToReplace + ruleToApply.first.size(),
			ruleToApply.second);

		if (print)
		{
			std::string second = ruleToApply.second;
			if (second == "")
				second = "lambda";
			std::cout << std::format("{} => {}, by using rule {} -> {}\n", oldWord, currentWord, ruleToApply.first, second);
		}
		oldWord = currentWord;
	}

	return currentWord;
}

bool Grammar::VerifyGrammar(std::ostream& errOut) const
{
	bool verify = true;
	if (!verifyRule1())
		errOut << "Rule 1 not met!\n", verify = false;

	if (!verifyRule2())
		errOut << "Rule 2 not met!\n", verify = false;

	if (!verifyRule3())
		errOut << "Rule 3 not met!\n", verify = false;

	if (!verifyRule4())
		errOut << "Rule 4 not met!\n", verify = false;

	if (!verifyRule5())
		errOut << "Rule 5 not met!\n", verify = false;

	return verify;
}

void Grammar::PrintGrammar() const
{
	std::cout << "G = ({";
	for (int i = 0; i < m_nonterminals.size() - 1; ++i)
	{
		std::cout << m_nonterminals[i] << ", ";
	}
	std::cout << m_nonterminals[m_nonterminals.size() - 1] << "}, {";

	for (int i = 0; i < m_terminals.size() - 1; ++i)
	{
		std::cout << m_terminals[i] << ", ";
	}
	std::cout << m_terminals[m_terminals.size() - 1] << "}, " <<
		m_startSymbol << ", productions), where productions are:\n";

	uint32_t cnt = 0;
	for (const auto& prod : m_productions)
	{
		std::string second = prod.second;
		if (second == lambda)
			second = "lambda";
		std::cout << std::format("{}. {} -> {}\n", ++cnt, prod.first, second);
	}

}

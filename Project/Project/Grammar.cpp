#include "Grammar.h"

char Grammar::nextNonusedNonterminal()
{
	char next = 'A';
	while (std::find(m_nonterminals.begin(), m_nonterminals.end(), next) != m_nonterminals.end())
	{
		++next;
	}

	return next;
}

char Grammar::lastNonusedNonterminal(std::vector<char> addedZ)
{
	char next = 'Z';
	while (std::find(m_nonterminals.begin(), m_nonterminals.end(), next) != m_nonterminals.end() ||
		std::find(addedZ.begin(), addedZ.end(), next) != addedZ.end())
	{
		--next;
	}

	return next;
}

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
			leftMember.erase(std::remove(leftMember.begin(), leftMember.end(), symbol), leftMember.end());
			rightMember.erase(std::remove(rightMember.begin(), rightMember.end(), symbol), rightMember.end());


			/*leftMember.erase(std::remove_if(leftMember.begin(), leftMember.end(), [&symbol](const char& a) {
				return a == symbol; }));
			rightMember.erase(std::remove_if(rightMember.begin(), rightMember.end(), [&symbol](const char& a) {
				return a == symbol; }));*/
		}

		for (const auto& symbol : m_terminals)
		{
			leftMember.erase(std::remove(leftMember.begin(), leftMember.end(), symbol), leftMember.end());
			rightMember.erase(std::remove(rightMember.begin(), rightMember.end(), symbol), rightMember.end());
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
		if (production.second == lambda && production.first.size() == 1
			&& wordStillHasNonterminals(production.first))
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
	std::vector<char> lettersInVn(Vn.begin(), Vn.end());
	std::vector<int> currentCombination; //which nonterminals are lambda

	for (auto production : m_productions)
		if (production.second != lambda)
		{
			P1.push_back({ production.first, production.second });
		}

	for (int c = 1; c <= Vn.size(); ++c)
	{
		//Select all possible combinations of nonterminals from Vn

		currentCombination.resize(c + 1, 0);

		currentCombination[1] = 0;

		int k = 1;
		while (k > 0)
		{
			while (currentCombination[k] < Vn.size())
			{
				++currentCombination[k];
				if (validCombination(k, currentCombination))
				{
					if (k == c)
					{
						/*for (int i = 1; i <= c; ++i)
							std::cout << currentCombination[i] << " ";*/
							//se intampla magie aici

						for (auto production : m_productions)
						{
							if (production.second != lambda)
							{
								std::string rightMember = production.second;
								bool changedSomething = false;

								for (int j = 0; j < rightMember.size(); ++j)
								{
									for (int i = 1; i < currentCombination.size(); ++i)
									{
										if (lettersInVn[currentCombination[i] - 1] == rightMember[j])
										{
											rightMember.erase(j--, 1);
											changedSomething = true;
											break;
										}
									}
								}
								//std::cout << rightMember << '\n';
								if (rightMember.size() != 0 && changedSomething)
								{
									//verify if this production is not already in P1
									bool alreadyIn = false;
									for (auto prod : P1)
									{
										if (prod.first == production.first && prod.second == rightMember)
										{
											alreadyIn = true;
											break;
										}
									}
									if (!alreadyIn)
										P1.push_back({ production.first, rightMember });
								}
							}
						}
					}
					else
					{
						++k;
						currentCombination[k] = 0;
					}
				}
			}
			--k;
		}
	}


	if (Vn.find(m_startSymbol) != Vn.end())
	{
		std::string aux;
		aux += m_startSymbol;
		std::string newStartSymb;
		newStartSymb += nextNonusedNonterminal();
		P1.push_back({ newStartSymb, aux });
		P1.push_back({ newStartSymb, lambda });

		m_nonterminals.push_back(newStartSymb[0]);
		m_startSymbol = newStartSymb[0];
	}

	m_productions = P1;
}

void Grammar::eliminateRenames()
{
	std::unordered_multimap<char, std::string> newProductions;
	std::unordered_multimap<char, char> derivations;

	for (auto production : m_productions)
	{
		if (production.second.size() != 1 || !wordStillHasNonterminals(production.second)) // no renaming
		{
			newProductions.insert({ production.first[0], production.second });
		}
		else if (production.second.size() == 1 && wordStillHasNonterminals(production.second))
		{
			derivations.insert({ production.first[0], production.second[0] });
		}
	}

	for (auto derivation : derivations)
	{
		auto range = newProductions.equal_range(derivation.second);

		for (auto prod = range.first; prod != range.second; ++prod)
		{
			newProductions.insert({ derivation.first, prod->second });
		}
	}

	for (auto derivation : derivations)
	{
		//Ms -> Md
		auto range = derivations.equal_range(derivation.second);

		for (auto prod = range.first; prod != range.second; ++prod)
		{
			//Md -> alphai
			auto range2 = newProductions.equal_range(prod->second);

			for (auto prod2 = range2.first; prod2 != range2.second; ++prod2)
			{
				//verify if newProd doesn't already exist
				bool exists = false;
				auto range3 = newProductions.equal_range(derivation.first);

				for (auto prod3 = range3.first; prod3 != range3.second; ++prod3)
				{
					if (prod3->first == derivation.first && prod3->second == prod2->second)
					{
						exists = true;
						break;
					}
				}

				//Ms -> alphai
				if (!exists)
					newProductions.insert({ derivation.first, prod2->second });
			}
		}
	}

	m_productions.clear();

	for (auto production : newProductions)
	{
		std::string Ms;
		Ms += production.first;
		m_productions.push_back({ Ms, production.second });
	}
}

void Grammar::eliminateNonGeneratingSymbols()
{
	std::unordered_set<char> generatingSymbols;

	bool changed = true;

	while (changed)
	{
		changed = false;
		//A->alpha
		for (auto production : m_productions)
		{
			////check if alpha contains only {Vi-1 U Vt}*

			//check if A isn't already added
			if (generatingSymbols.find(production.first[0]) == generatingSymbols.end())
			{
				//check if alpha contains only {Vi-1 U Vt}*
				bool checkIfContains = true;
				for (auto var : production.second)
				{
					if (generatingSymbols.find(var) == generatingSymbols.end() &&
						std::find(m_terminals.begin(), m_terminals.end(), var) == m_terminals.end()
						&& production.second != lambda)
					{
						checkIfContains = false;
						break;
					}

				}

				if (checkIfContains)
				{
					generatingSymbols.insert(production.first[0]);
					changed = true;
				}
			}
		}
	}

	m_nonterminals = std::vector<char>(generatingSymbols.begin(), generatingSymbols.end());

	//eliminate productions that contain nongenerating symbols
	bool containsNongenerating;
	for (auto production = m_productions.begin(); production != m_productions.end();)
	{
		if (generatingSymbols.find(production->first[0]) == generatingSymbols.end())
		{
			production = m_productions.erase(production);
			continue;
		}
		//check if Md contains nongen symb
		containsNongenerating = false;

		for (auto elem : production->second)
		{
			if (generatingSymbols.find(elem) == generatingSymbols.end() &&
				std::find(m_terminals.begin(), m_terminals.end(), elem) == m_terminals.end() &&
				production->second != lambda)
			{
				containsNongenerating = true;
				break;
			}
		}

		if (containsNongenerating)
			production = m_productions.erase(production);
		else
			++production;
	}

	std::unordered_set<char> newNonterminals;

	for (auto prod : m_productions)
	{
		newNonterminals.insert(prod.first[0]);
	}

	m_nonterminals.clear();

	for (auto p : newNonterminals)
		m_nonterminals.push_back(p);
}

void Grammar::eliminateInaccessibleSymbols()
{
	std::unordered_set<char> Vn1;
	Vn1.insert(m_startSymbol);

	//repeat until no symbol was added to Vn
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (auto production : m_productions)
		{
			if (Vn1.find(production.first[0]) != Vn1.end())
			{
				for (auto elem : production.second)
				{
					//if elem is nonterminal
					if (std::find(m_nonterminals.begin(), m_nonterminals.end(), elem) != m_nonterminals.end())
					{
						//add if not already in Vn1
						if (Vn1.find(elem) == Vn1.end())
						{
							Vn1.insert(elem);
							changed = true;
						}
					}
				}
			}
		}
	}

	for (auto production = m_productions.begin(); production != m_productions.end();)
	{
		if (Vn1.find(production->first[0]) != Vn1.end())
		{
			if (production->second == lambda)
			{
				++production;
				continue;
			}
			else
			{
				//check if alpha in {Vn1 U Vt}*
				bool isInVn1UVt = true;
				for (auto elem : production->second)
				{
					if (Vn1.find(elem) == Vn1.end() &&
						std::find(m_terminals.begin(), m_terminals.end(), elem) == m_terminals.end())
					{
						isInVn1UVt = false;
						break;
					}
				}

				if (!isInVn1UVt)
				{
					production = m_productions.erase(production);
				}
				else
					++production;
			}
		}
		else production = m_productions.erase(production);
	}

	std::unordered_set<char> newNonterminals;

	for (auto prod : m_productions)
	{
		newNonterminals.insert(prod.first[0]);
	}

	m_nonterminals.clear();

	for (auto p : newNonterminals)
		m_nonterminals.push_back(p);
}

void Grammar::FNCconvert()
{
	if (!IsContextIndependent())
	{
		std::cout << "Can't simplify grammar! It isn't context independent.\n";
		return;
	}

	eliminateRenames();
	eliminateNonGeneratingSymbols();
	eliminateInaccessibleSymbols();

	std::unordered_map<char, char> newProductions; //Step 2 - Ci -> Bi but the key is Bi to find if a terminal is alr replaced by a nonterminal (Bi <- Ci)

	for (auto& production : m_productions)
	{
		if (production.second.size() >= 2)
		{
			for (auto elem : production.second)
			{
				if (std::find(m_terminals.begin(), m_terminals.end(), elem) != m_terminals.end())
				{
					if (newProductions.find(elem) == newProductions.end())
					{
						char nextNonterminal = nextNonusedNonterminal();
						m_nonterminals.push_back(nextNonterminal);
						newProductions.insert({ elem, nextNonterminal });
					}

					std::replace(production.second.begin(), production.second.end(), elem, newProductions[elem]);
				}
			}
		}
	}

	for (auto prod : newProductions)
	{
		std::string second;
		second += prod.second;
		std::string first;
		first += prod.first;
		m_productions.push_back({ second, first });
	}
	newProductions.clear();

	std::vector<production> step3Productions;

	for (auto production = m_productions.begin(); production != m_productions.end();)
	{
		if (production->second.size() >= 3)
		{
			std::string newMd;
			std::string lastNewNonterminal;
			std::string currentNewNonterminal(1, nextNonusedNonterminal());
			step3Productions.push_back({ production->first, production->second[0] + currentNewNonterminal });

			std::string tmpMd(production->second.begin() + 1, production->second.end());
			lastNewNonterminal = currentNewNonterminal;

			m_nonterminals.push_back(currentNewNonterminal[0]);

			while (tmpMd.size() >= 3)
			{
				currentNewNonterminal = std::string(1, nextNonusedNonterminal());
				step3Productions.push_back({ lastNewNonterminal, tmpMd[0] + currentNewNonterminal });
				lastNewNonterminal = currentNewNonterminal;
				m_nonterminals.push_back(currentNewNonterminal[0]);
				tmpMd = std::string(tmpMd.begin() + 1, tmpMd.end());
			}
			step3Productions.push_back({ lastNewNonterminal, tmpMd });
			m_nonterminals.push_back(currentNewNonterminal[0]);

			production = m_productions.erase(production);
		}
		else
		{
			++production;
		}
	}

	m_productions.insert(m_productions.end(), step3Productions.begin(), step3Productions.end());

	//std::sort(m_productions.begin(), m_productions.end(), [](const production& a, const production& b) {return a.first < b.first; });
}

void Grammar::FNGconvert()
{

	if (!IsContextIndependent())
	{
		std::cout << "Can't simplify grammar! It isn't context independent.\n";
		return;
	}

	FNCconvert();


	std::unordered_multimap<char, std::string> productions;
	for (auto prod : m_productions)
		productions.insert({ prod.first[0], prod.second });

	//x1, x2,..., xn depends on the order of m_nonterminals
	std::unordered_map<char, int> nonterminals; //key = x, int = index of x
	int cnt = 0;
	for (auto production : m_productions)
	{
		if (nonterminals.find(production.first[0]) == nonterminals.end() &&
			std::find(m_terminals.begin(), m_terminals.end(), production.first[0]) == m_terminals.end())
		{
			nonterminals[production.first[0]] = ++cnt;
		}
		for (auto elem : production.second)
		{
			if (nonterminals.find(production.first[0]) == nonterminals.end() &&
				std::find(m_terminals.begin(), m_terminals.end(), production.first[0]) == m_terminals.end())
			{
				nonterminals[elem] = ++cnt;
			}
		}

		if (nonterminals.size() == m_nonterminals.size())
		{
			break;
		}
	}

	//step1
	std::vector<char> addedZ;
	bool addedSmth = true;

	while (addedSmth)
	{
		addedSmth = false;
		std::unordered_multimap<char, std::string> aux;
		for (auto production = productions.begin(); production != productions.end();)
		{
			if (nonterminals.find(production->second[0]) != nonterminals.end())
			{
				if (nonterminals[production->first] > nonterminals[production->second[0]] &&
					std::find(addedZ.begin(), addedZ.end(), production->first) == addedZ.end())
				{
					auto its = productions.equal_range(production->second[0]);
					for (auto it = its.first; it != its.second; ++it) {
						if (*it != *production)
						{
							std::string newMd = production->second;
							newMd.replace(newMd.begin(), newMd.begin() + 1, it->second);

							aux.insert({ production->first, newMd });
							addedSmth = true;
						}
					}

					std::unordered_multimap<char, std::string> aux2;
					its = aux.equal_range(production->second[0]);
					for (auto it = its.first; it != its.second; ++it) {
						if (*it != *production)
						{
							std::string newMd = production->second;
							newMd.replace(newMd.begin(), newMd.begin() + 1, it->second);

							aux2.insert({ production->first, newMd });
							addedSmth = true;
						}
					}

					aux.insert(aux2.begin(), aux2.end());
					production = productions.erase(production);
					addedSmth = true;
					continue;
				}
				else if (nonterminals[production->first] == nonterminals[production->second[0]])
				{
					char currentZ = lastNonusedNonterminal(addedZ);
					nonterminals[currentZ] = ++cnt;
					addedZ.push_back(currentZ);
					std::unordered_multimap<char, std::string> aux2;

					for (auto p : aux)
					{
						if (p.first == production->first)
						{
							aux2.insert({ p.first, p.second + currentZ });
							addedSmth = true;
						}
					}

					aux.insert(aux2.begin(), aux2.end());

					for (auto p : productions)
					{
						if (p == *production)
						{
							std::string newMd(production->second.begin() + 1, production->second.end());
							aux.insert({ currentZ, newMd });
							aux.insert({ currentZ, newMd + currentZ });
							addedSmth = true;
						}
						else if (p.first == production->first)
						{
							aux.insert({ p.first, p.second + currentZ });
						}
					}

					production = productions.erase(production);
					continue;
				}
				else ++production;


			}
			else ++production;
		}
		productions.insert(aux.begin(), aux.end());
	}


	m_productions.clear();

	for (auto p : productions)
	{
		std::string aux;
		aux += p.first;
		m_productions.push_back({ aux, p.second });
	}

	//step 2

	std::sort(m_nonterminals.begin(), m_nonterminals.end(), [&](const char& a, const char& b) {return nonterminals[a] > nonterminals[b]; });
	std::unordered_multimap<char, std::string> aux;
	for (auto p : m_productions)
		if (p.first[0] == m_nonterminals[0])
			aux.insert({ p.first[0], p.second });


	for (auto elem = m_nonterminals.begin() + 1; elem != m_nonterminals.end(); ++elem)
	{
		for (auto prod : m_productions)
		{
			if (prod.first[0] == *elem)
			{
				if (nonterminals.find(prod.second[0]) != nonterminals.end())
				{
					if (nonterminals[prod.first[0]] < nonterminals[prod.second[0]])
					{
						std::unordered_multimap<char, std::string> aux2;
						auto its = aux.equal_range(prod.second[0]);
						for (auto it = its.first; it != its.second; ++it)
						{
							std::string newMd = prod.second;
							newMd.replace(newMd.begin(), newMd.begin() + 1, it->second);

							aux2.insert({ prod.first[0], newMd });
						}
						aux.insert(aux2.begin(), aux2.end());
					}
					
				}
				//de adaugat productii tip A -> a in aux pt ca alg de mai sus nu face asta
				for (auto p : m_productions)
				{
					if (p.first == prod.first)
					{
						auto its = aux.equal_range(p.first[0]);
						bool alreadyIn = false;
						for (auto it = its.first; it != its.second; ++it)
						{
							if (it->first == p.first[0] && it->second == p.second)
							{
								alreadyIn = true;
								break;
							}
						}
						if (!alreadyIn && nonterminals.find(p.second[0]) == nonterminals.end())
							aux.insert({ p.first[0] , p.second });
					}
				}
			}
		}
	}

	for (auto p : m_productions)
	{
		if (std::find(addedZ.begin(), addedZ.end(), p.first[0]) != addedZ.end())
		{
			aux.insert({ p.first[0] , p.second });
		}
		else if (nonterminals.find(p.first[0]) != nonterminals.end() &&
			nonterminals.find(p.second[0]) == nonterminals.end()) {
			
			auto its = aux.equal_range(p.first[0]);
			bool alreadyIn = false;
			for (auto it = its.first; it != its.second; ++it)
			{
				if (it->first == p.first[0] && it->second == p.second)
				{
					alreadyIn = true;
					break;
				}
			}
			if (!alreadyIn)
				aux.insert({ p.first[0] , p.second });
		}
	}

	m_productions.clear();

	for (auto p : aux)
	{
		std::string aux2;
		aux2 += p.first;
		m_productions.push_back({ aux2, p.second });
	}

	aux.clear();
	//step 3
	//Z -> Ai.....
	for (auto prod = m_productions.begin(); prod != m_productions.end();)
	{
		if (std::find(addedZ.begin(), addedZ.end(), prod->first[0]) != addedZ.end())
		{
			for (auto p : m_productions)
			{
				if (p.first[0] == prod->second[0])
				{
					std::string newMd = prod->second;
					newMd.replace(newMd.begin(), newMd.begin() + 1, p.second);
					aux.insert({ prod->first[0], newMd});
				}
			}
			prod = m_productions.erase(prod);
		}
		else
			++prod;
	}

	m_nonterminals.insert(m_nonterminals.end(), addedZ.begin(), addedZ.end());

	for (auto p : aux)
	{
		std::string aux2;
		aux2 += p.first;
		m_productions.push_back({ aux2, p.second });
	}
}

bool Grammar::validCombination(int k, std::vector<int>& nonterminals)
{
	if (nonterminals[k] > nonterminals[k - 1])
		return true;
	return false;
}

void Grammar::simplifyGrammar()
{
	/*if (!IsContextIndependent())
	{
		std::cout << "Can't simplify grammar! It isn't context independent.\n";
		return;
	}*/

	for (auto production : m_productions)
	{
		if (production.first.size() != 1 && wordStillHasNonterminals(production.first))
		{
			std::cout << "Can't simplify grammar! One or more productions have more than one elements in the left member.\n";
			return;
		}
	}

	eliminateLambdaProductions(); //it brings the grammar in CIG form
	eliminateRenames();
	eliminateNonGeneratingSymbols();
	eliminateInaccessibleSymbols();
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

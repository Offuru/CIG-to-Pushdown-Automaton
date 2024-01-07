#pragma once
#include <algorithm>
#include <iostream>
#include <queue>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <format>
#include <stack>
#include <iterator>
#include "Grammar.h"

class APD;

using transition = std::tuple<std::string, char, std::string>;
using statePair = std::pair<std::string, std::vector<std::string>>;

struct TransitionCompare
{
	bool operator()(const transition& transition1, const transition& transition2) const
	{
		const auto& [state1, letter1, stackState1] = transition1;
		const auto& [state2, letter2, stackState2] = transition2;
		if (state1 < state2)
			return true;
		else if (state1 == state2)
		{
			if (letter1 < letter2)
				return true;
			else if (letter1 == letter2)
				return stackState1 < stackState2;
		}
		return false;
	}
};

class APD
{
public:

	static const char lambda = '$';


	APD() = default;
	APD(const Grammar& grammar);

	friend std::ostream& operator<<(std::ostream& out, const APD& apd);
	bool checkWord(const std::string& word) const;
	bool isDeterministic() const;

	APD idcGrammarToAPD(const Grammar& grammar);

private:

	std::vector<std::string> m_states;
	std::vector<char> m_alphabet;
	std::vector<char> m_stackAlphabet;
	std::multimap<transition, statePair, TransitionCompare> m_transitions;
	std::string m_initialState;
	std::string m_initialStackState;
	std::vector<std::string> m_finalStates;

};


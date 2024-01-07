#pragma once
#include <algorithm>
#include <iostream>
#include <queue>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <format>
#include "Grammar.h"

class APD
{
public:

	static const char lambda = '$';

	using transition = std::tuple<std::string, char, std::string>;
	using statePair = std::pair<std::string, std::vector<std::string>>;

	APD(const Grammar& grammar);

	friend std::ostream& operator<<(std::ostream& out, const APD& apd);
	bool validWord(const std::string& word) const;
	bool isDeterministic() const;

private:

	std::vector<std::string> m_states;
	std::vector<char> m_alphabet;
	std::vector<char> m_stackAlphabet;
	std::multimap<transition, statePair> m_transitions;
	std::string m_initialState;
	std::string m_initialStackState;
	std::vector<std::string> m_finalStates;

};


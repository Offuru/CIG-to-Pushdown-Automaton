#include "APD.h"

APD::APD(const Grammar& grammar)
{
	*this = idcGrammarToAPD(grammar);
}

bool APD::checkWord(const std::string& word) const
{
	if (word.size() == 1 && word[0] == lambda)
	{
		transition tr = { m_initialState, lambda, m_initialStackState };
		auto foundTr = std::find_if(m_transitions.cbegin(), m_transitions.cend(), [&tr](const auto& other)
			{
				const auto& [state1, letter1, stackState1] = tr;
				const auto& [state2, letter2, stackState2] = other.first;
				if (state1 == state2 &&
					letter1 == letter2 &&
					stackState1 == stackState2)
					return true;

				return false;
			});
		if (foundTr == m_transitions.cend()) return false;
		
		if (foundTr->second.first == m_initialState && foundTr->second.second[0] == std::string(1,lambda))
			return true;
		return false;
	}

	using wordStack = std::tuple<std::string, std::string, std::stack<std::string>>;
	std::queue<wordStack> totalAlphabetMemory;

	wordStack wS;

	auto& [initialStateWS, initialWordWS, initialAlphabetStackWS] = wS;
	initialStateWS = m_initialState;
	initialWordWS = word;
	initialAlphabetStackWS.push(m_initialStackState);

	totalAlphabetMemory.push(wS);

	bool validWord = false;

	while (!totalAlphabetMemory.empty())
	{
		wordStack currentWordStack = totalAlphabetMemory.front();
		auto& [currentStateWS, currentWordWS, currentAlphabetStackWS] = currentWordStack;

		if (!currentWordWS.size() || currentAlphabetStackWS.empty())
		{
			totalAlphabetMemory.pop();
			continue;
		}

		transition currentTransition{ currentStateWS, currentWordWS[0], currentAlphabetStackWS.top() };
		currentAlphabetStackWS.pop();
		auto& [state, chr, alphabet] = currentTransition;
		auto itrStart = m_transitions.lower_bound(currentTransition);
		auto itrEnd = m_transitions.upper_bound(currentTransition);

		if (itrStart == itrEnd)
		{
			totalAlphabetMemory.pop();
			continue;
		}

		while (itrStart != itrEnd)
		{
			wordStack newWordStack;
			auto& [stateWS, wordWS, alphabetStackWS] = newWordStack;
			stateWS = state;
			if (currentWordWS.size() > 0)
				wordWS = currentWordWS.substr(1, currentWordWS.size() - 1);
			alphabetStackWS = currentAlphabetStackWS;
			
			for (auto chr = itrStart->second.second.crbegin(); chr != itrStart->second.second.crend(); ++chr)
			{
				if (*chr != std::string(1,lambda))
					alphabetStackWS.push(*chr);
			}
			if (alphabetStackWS.empty() && !wordWS.size()) return true;
			totalAlphabetMemory.push(newWordStack);
			++itrStart;
		}
		totalAlphabetMemory.pop();
	}

	return false;
}

bool APD::isDeterministic() const
{
	for (auto& transition : m_transitions)
	{
		auto itrStart = m_transitions.lower_bound(transition.first);
		auto itrEnd = m_transitions.upper_bound(transition.first);
		--itrEnd;
		if (itrStart != itrEnd)
		{
			return false;
		}
	}

	return true;
}

APD APD::idcGrammarToAPD(const Grammar& grammar) 
{
	APD automaton;
	Grammar fngGrammar = grammar;

	if (!grammar.IsContextIndependent())
	{
		std::cout << "Grammar isn't context independent.\n";
		return APD();
	}

	fngGrammar.FNGconvert();

	automaton.m_initialState = "q0";
	automaton.m_states.push_back(automaton.m_initialState);

	automaton.m_alphabet = fngGrammar.getTerminals();
	automaton.m_stackAlphabet = fngGrammar.getNonTerminals();
	automaton.m_initialStackState = fngGrammar.getStartSymbol();

	for (const auto& production : fngGrammar.getProductions())
	{
		const auto& [left, right] = production;
		transition newTransition = { automaton.m_states[0], right[0], left};
		statePair newStatePair = { automaton.m_states[0] , {} };

		for (uint32_t index = 1; index < right.size(); ++index)
			newStatePair.second.push_back(std::string(1, right[index]));

		if (right.size() == 1)
			newStatePair.second.push_back(std::string(1, lambda));
		
		automaton.m_transitions.insert({ newTransition, newStatePair });
	}

	return automaton;
}

std::ostream& operator<<(std::ostream& out, const APD& apd)
{
	out << std::format("States:\n");
	for (const auto& state : apd.m_states)
		out << std::format("{} ", state);
	out << "\n\n";

	out << std::format("Alphabet:\n");
	for (char letter : apd.m_alphabet)
		out << std::format("{} ", letter);
	out << "\n\n";

	out << std::format("Stack symbols:\n");
	for (char letter : apd.m_stackAlphabet)
		out << std::format("{} ", letter);
	out << "\n\n";

	out << std::format("Initial state:\n{}\n\n", apd.m_initialState);

	out << std::format("Initial stack state:\n{}\n\n", apd.m_initialStackState);

	out << std::format("Final states:\n");
	for (const auto& state : apd.m_finalStates)
		out << std::format("{} ", state);
	out << "\n\n";

	for (const auto& transition : apd.m_transitions)
	{
		const auto& [state, letter, stackState] = transition.first;

		out << std::format("({},{},{}) -> ({},", state, letter, stackState, transition.second.first);
		for (const auto& nextStackState : transition.second.second)
			out << std::format("{}", nextStackState);
		out << ")\n";
	}
	return out;
}
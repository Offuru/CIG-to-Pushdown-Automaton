#include "APD.h"

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
		out << "\n";
	}
	return out;
}

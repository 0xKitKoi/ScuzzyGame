#pragma once
#ifndef MAGIC_H
#define MAGIC_H

#include <string>
#include <cstdio>
//#include "Source/GameState.hpp"


class GameState;

class Magic {
public:
	std::string m_abilityName = "Default Magic Name";
	int m_TensionCost = 0;
	virtual void Cast() {
		printf("Using base Magic class Cast(). This should be overridden.\n");
	}
	virtual ~Magic() = default; // Virtual destructor is important!
};



class DoubleOrNothing : public Magic {
public:
	DoubleOrNothing() {
		m_abilityName = "Double or Nothing";
		m_TensionCost = 0;
	}
	void Cast() override;
};

class Heal : public Magic {
public:
	Heal() {
		m_abilityName = "Heal";
		m_TensionCost = 5;
	}
	void Cast() override;
};

class Scuzzy : public Magic {
public:
	Scuzzy() {
		m_abilityName = "Scuzzy";
		m_TensionCost = 10;
	}
	void Cast() override;
};

#endif // !Magic

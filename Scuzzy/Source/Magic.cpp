
#include "Source/Magic.hpp"
#include "Source/GameState.hpp"

//class DoubleOrNothing : public Magic {
//public:
//	DoubleOrNothing() {
//		m_abilityName = "Double or Nothing";
//		m_TensionCost = 0;
//	}
//	void Cast() override {
//		printf("Casting Double or Nothing!\n");
//		// this will make the heart glow. 
//		// this is intended as a huge story plot, the caster is absorbed by their soul and becomes powerful, but is also extemely vulnerable.
//		// this is a last resort move, it will increase the tension meter by a large amount, but the caster will die on any damage taken.
//		// this effect lasts for the duration of the fight, and cannot be undone. if the caster wins, they get more powerful.
//		gameState.TensionMeter += 100; // increase tension meter by a large amount
//		gameState.doubleOrNothingActive = true; // set the flag to indicate the effect is active
//
//	}
//};
//
//class Heal : public Magic {
//public:
//	Heal() {
//		m_abilityName = "Heal";
//		m_TensionCost = 5;
//	}
//	void Cast() override {
//		printf("Casting Heal!\n");
//		gameState.HP += 5;
//		gameState.TensionMeter -= m_TensionCost;
//		if (gameState.HP > 10) gameState.HP = 10;
//	}
//};
//
//class Scuzzy : public Magic {
//public:
//	Scuzzy() {
//		m_abilityName = "Scuzzy";
//		m_TensionCost = 10;
//	}
//	void Cast() override {
//		printf("Casting Scuzzy!\n");
//		gameState.enemy->HP -= 10;
//		gameState.TensionMeter -= m_TensionCost;
//	}
//};


void DoubleOrNothing::Cast() {
	printf("Casting Double or Nothing!\n");
	// this will make the heart glow. 
	// this is intended as a huge story plot, the caster is absorbed by their soul and becomes powerful, but is also extemely vulnerable.
	// this is a last resort move, it will increase the tension meter by a large amount, but the caster will die on any damage taken.
	// this effect lasts for the duration of the fight, and cannot be undone. if the caster wins, they get more powerful.
	gameState.TensionMeter += 100; // increase tension meter by a large amount
	gameState.doubleOrNothingActive = true; // set the flag to indicate the effect is active
}

void Heal::Cast() {
	printf("Casting Heal!\n");
	gameState.HP += 5;
	gameState.TensionMeter -= m_TensionCost;
	if (gameState.HP > 10) gameState.HP = 10;
}

void Scuzzy::Cast() {
	printf("Casting Scuzzy!\n");
	gameState.enemy->HP -= 10;
	gameState.TensionMeter -= m_TensionCost;
}
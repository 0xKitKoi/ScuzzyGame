#include "Source/Entity.hpp"
#include "Source/Enemy.hpp"
#include "Source/GameState.hpp"
#include "Source/Projectile.hpp"

extern std::shared_ptr<LTexture> getTexture(const std::string& filename);


// Polymorphism time bitches

class BoxOfFuckYou : public Enemy {
public:
	BoxOfFuckYou(std::shared_ptr<Entity> entity) : Enemy(entity) {
		m_AttackDamage = 1;
		m_EnemyDialogue = { "The Box Full of \"Fuck You\" Appeared!",
			"The Box of fuck you said ... \"Fuck you\"", 
			"You opened the box. There was \"fuck you\" inside." };

		m_Actions = { "info", "sit", "kick.?" };

		m_ActionResponse = { "STATUS: .. its a box..?",
			"You sat on the box, it left a dent in it.", 
			"WHAM! you left a big dent in its fleshy cardboard." };

		m_EnemyFightSpriteSheet = getTexture("data/box_fuck_u_ari_1.png");

		m_EnemySpriteClips =  { { 0,0,128,128 }, { 128,0,128,128 }, { 128 * 2,0,128,128 }, { 128 * 3,0,128,128 } };

		FRAME_COUNT = 2;

		m_EnemyProjectile = std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 1);
	}

	std::string FightActionResponse(int actionIndex) override {
		// by default, return the action response at the given index.
		if (actionIndex < 0 || actionIndex >= m_ActionResponse.size()) {
			return "Invalid action.";
		}
		if (gameState.SillyMeter >= 5) {
			// I want the actions and responses to change here
			return "Custom silly String here! fuck you !!!1! ";
		}
		// increase silly mode?
		if (actionIndex == 1) {
			printf("Silly mode INCREASED!!!!!!!!!!");
			gameState.SillyMeter += 5;
		}
		return m_ActionResponse[actionIndex];
	}
};



class DooDooMartBox : public Enemy {
public:
	DooDooMartBox(std::shared_ptr<Entity> entity) : Enemy(entity) {
		m_AttackDamage = 3;
		m_EnemyDialogue = { "The DOODOOMART Box ran at you!",
			"The DooDoo Mart Box has a buncha doodoo init",
			"The doodoomart box gave you a negative coupon. you are now in even more doodoo debt." };

		m_Actions = { "info", "dissassemble", "turn into shitbox" };

		m_ActionResponse = { "STATUS: .. its a box..?",
			"You flattened the box. It took Heavy Damage", 
			"my actual pc" };

		m_EnemyFightSpriteSheet = getTexture("data/DooDooMart_StorageBox-Sheet.png");

		m_EnemySpriteClips = { { 0,0,128,128 }, { 128,0,128,128 }, { 128 * 2,0,128,128 }, { 128 * 3,0,128,128 }, { 128 * 4 ,0,128,128 }, { 128 * 5 ,0,128,128 } };

		FRAME_COUNT = 5;
	}

	std::string FightActionResponse(int actionIndex) override {

		printf("DooDooMartBox FightActionResponse called with actionIndex: %d\n", actionIndex);
		// by default, return the action response at the given index.
		if (actionIndex < 0 || actionIndex >= m_ActionResponse.size()) {
			return "Invalid action.";
		}
		if (gameState.SillyMeter >= 5) {
			// I want the actions and responses to change here
			return "What the fuck!...  is a Chungas? ???!?!?!?! ";
		}
		// increase silly mode?
		if (actionIndex == 1) {
			printf("Silly mode INCREASED!!!!!!!!!!");
			gameState.SillyMeter += 5;
		}
		return m_ActionResponse[actionIndex];
	}
};


#include "Source/Entity.hpp"
#include "Source/Enemy.hpp"
#include "Source/GameState.hpp"
#include "Source/Projectile.hpp"

#include <random>

extern std::shared_ptr<LTexture> getTexture(const std::string& filename);

/*int randomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}*/
extern int randomInt(int min, int max);


// Polymorphism time bitches

/*
class CustomEnemy : public Enemy {
public:
	CustomEnemy(std::shared_ptr<Entity> entity) : Enemy(entity) {
		// Custom initialization for this enemy type
		m_EnemySoulSpriteSheet = getTexture("data/EnemySoul.png");

		m_AttackDamage = 1; // Projectiles have their own damage value. Disregard this for now.

		m_EnemyDialogue = { "Dialogue Option 1", // These are shown in fight dialogue.
			"Dialogue Option 2",
			"Dialogue Option 3" };

		m_Actions = { "info", "Action1", "Action2" }; // Actions shown to player in fight.

		m_ActionResponse = { "Info: Default Enemy.", // Responses to player actions. See FightActionResponse function for custom behavior.
			"Response to action 1",
			"Response to action 2" };

		m_EnemyFightSpriteSheet = getTexture("data/Error.png"); // Replace with Sprite sheet.

		m_EnemySpriteClips = { { 0,0,128,128 } }; // default sprite rect. Replace with dimension of single sprite in sprite sheet.

		FRAME_COUNT = 1; // number of frames in animation.

		m_EnemyProjectile = std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{ 0,0,10,10 }, Vector2f(0, 0), Vector2f(200, 200), 1);
		m_projectileCount = 10;
	}
	std::string FightActionResponse(int actionIndex) override {
		// Custom behavior for this enemy type
		return "Custom response for action index " + std::to_string(actionIndex);
		// Enemy.cpp has a default implementation you can use as a reference.
	}
	void ResetProjectiles() override {
		// Custom projectile reset behavior
		m_EnemyProjectiles.clear();
		// Initialize projectiles as needed using the m_EnemyProjectile as a template or create new ones.
		// Enemy.cpp has a default implementation you can use as a reference.
	}
};
*/


class BoxOfFuckYou : public Enemy {
public:
	BoxOfFuckYou(std::shared_ptr<Entity> entity) : Enemy(entity) {
		m_EnemySoulSpriteSheet = getTexture("data/EnemySoul2.png");
		m_Name = "Box of Fuck You";
		m_AttackDamage = 1;
		m_EnemyDialogue = { "The Box Full of \"Fuck You\" Appeared!",
			"The Box of fuck you said ... \"Fuck you\"",
			"You opened the box. There was \"fuck you\" inside." };

		m_Actions = { "info", "sit", "kick.?" };

		m_ActionResponse = { "It's a box. The label says it's full of \"Fuck You\". I wonder what's inside? It's probably full of \"Fuck You\".",
			"You sat on the box with your whole ass. Your voluminous cheeks left a deep imprint.",
			"WHAM! you left a big dent in its fleshy cardboard." };

		m_EnemyFightSpriteSheet = getTexture("data/box_fuck_u_ari_1.png");

		m_EnemySpriteClips = { { 0,0,128,128 }, { 128,0,128,128 }, { 128 * 2,0,128,128 }, { 128 * 3,0,128,128 }, { 128 * 4,0,128,128 } };

		FRAME_COUNT = 4;

		m_EnemyProjectile = std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{ 0,0,10,10 }, Vector2f(0, 0), Vector2f(200, 200), 1);
		m_projectileCount = 10;
		m_layer1 = 323;
		m_layer2 = 194;
		/*
		for (int i = 0; i < m_projectileCount; i++) {
			m_EnemyProjectiles.push_back(std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 1));
		}
		*/
	}

	std::string FightActionResponse(int actionIndex) override {
		printf("BoxOfFuckYou FightActionResponse called with actionIndex: %d\n", actionIndex);
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
			//m_Entity->m_SpriteRect = { 128*4, 0, 128, 128 };
			this->currentFrameCount = 4;
			this->m_animationDone = true;
			if (gameState.DebugMode) {
				printf("Silly mode INCREASED!!!!!!!!!!");
			}
			gameState.SillyMeter += 5;
		}
		return m_ActionResponse[actionIndex];
	}

};

class DooDooMartBox : public Enemy {
public:
	DooDooMartBox(std::shared_ptr<Entity> entity) : Enemy(entity) {
		m_EnemySoulSpriteSheet = getTexture("data/EnemySoul2.png");
		m_Name = "DooDooMart Box";
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

		FRAME_COUNT = 6;

		m_EnemyProjectile = std::make_shared<FallingProjectile>(getTexture("data/box.png"), SDL_Rect{0,0,20,20}, Vector2f(0,0), Vector2f(200,200), 1);
		m_projectileCount = 10;
		m_layer1 = 284;
		m_layer2 = 194;
	}

	std::string FightActionResponse(int actionIndex) override {
		if (gameState.DebugMode) {
			printf("DooDooMartBox FightActionResponse called with actionIndex: %d\n", actionIndex);
		}
		// by default, return the action response at the given index.
		if (actionIndex < 0 || actionIndex >= m_ActionResponse.size()) {
			return "Invalid action.";
		}
		if (gameState.SillyMeter >= 5) {
			// I want the actions and responses to change here
			return "What the fuck!...  is a Chungas????!?!?!?! ";
		}
		// increase silly mode?
		if (actionIndex == 1) {
			if (gameState.DebugMode) {
				printf("Silly mode INCREASED!!!!!!!!!!");
			}
			gameState.SillyMeter += 5;
		}
		return m_ActionResponse[actionIndex];
	}

	void ResetProjectiles() override {
    	m_EnemyProjectiles.clear();
                        // init the projectiles
        for (int i = 0; i < m_projectileCount; i++) {
		    //m_EnemyProjectiles.push_back(std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 1));
            // using the m_EnemyProjectile as a template, create new projectiles
            float subx = float(randomInt(0, gameState.screenwidth));
            float suby = float(randomInt(0, gameState.screenheight));
            m_EnemyProjectiles.push_back(std::make_shared<FallingProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f( subx, suby ), Vector2f(200,200), 1));
            // randomize vector2f(x,y) position:
        }
        float subx = float(randomInt(0, gameState.screenwidth));
        float suby = float(randomInt(0, gameState.screenheight));
        m_EnemyProjectiles[0] = std::make_shared<HomingProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f(subx, suby), Vector2f(200, 200), 1);
	}

};


class DrPebba : public Enemy {
public:
	DrPebba(std::shared_ptr<Entity> entity) : Enemy(entity) {
		m_EnemySoulSpriteSheet = getTexture("data/EnemySoul2.png");
		m_Name = "Evil ass Dr. Pebba";
		m_AttackDamage = 1;
		m_EnemyDialogue = { "The aluminum can ran at you!",
			"The soda can sizzles in anger..!",
			"The soda can ponders a moment, watching your every move." };

		m_Actions = { "info", "crush", "empty the can" };

		m_ActionResponse = { "STATUS: .. its a can..?",
			"You flattened the box. It took Heavy Damage",
			"my actual pc" };

		m_EnemyFightSpriteSheet = getTexture("data/drpebba.png");

		m_EnemySpriteClips = { { 0,0,128,128 } , { 0,0,128,128 } };

		FRAME_COUNT = 1;

		m_EnemyProjectile = std::make_shared<AluminumCanProjectile>(getTexture("data/soda_bullet.png"), SDL_Rect{ 0,0,20,20 }, Vector2f(0, 0), Vector2f(200, 200), 1);
		m_projectileCount = 25;
		m_layer1 = 4;
		m_layer2 = 0;

	}

	std::string FightActionResponse(int actionIndex) override {
		if (gameState.DebugMode) {
			printf("DrPebba FightActionResponse called with actionIndex: %d\n", actionIndex);
		}
		// by default, return the action response at the given index.
		if (actionIndex < 0 || actionIndex >= m_ActionResponse.size()) {
			return "Invalid action.";
		}
		if (gameState.SillyMeter >= 5) {
			// I want the actions and responses to change here
			return "What the fuck!...  is a Chungas????!?!?!?! ";
		}
		// increase silly mode?
		if (actionIndex == 1) {
			if (gameState.DebugMode) {
				printf("Silly mode INCREASED!!!!!!!!!!");
			}
			gameState.SillyMeter += 5;
		}
		return m_ActionResponse[actionIndex];
	}

	void ResetProjectiles() override {
		m_EnemyProjectiles.clear();
		// init the projectiles
		for (int i = 0; i < m_projectileCount; i++) {
			//m_EnemyProjectiles.push_back(std::make_shared<Projectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 1));
			// using the m_EnemyProjectile as a template, create new projectiles
			float subx = float(randomInt(0, gameState.screenwidth));
			float suby = float(randomInt(0, gameState.screenheight));
			m_EnemyProjectiles.push_back(std::make_shared<FallingProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f(subx, suby), Vector2f(200, 200), 1));
			// randomize vector2f(x,y) position:
		}
		float subx = float(randomInt(0, gameState.screenwidth));
		float suby = float(randomInt(0, gameState.screenheight));
		m_EnemyProjectiles[0] = std::make_shared<HomingProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f(subx, suby), Vector2f(200, 200), 1);
	}

};


// TODO: i want the wizard enemy specifically to be the tutorial, and it needs to talk to the player, directing them on how to use the FightSystem.

class WizardEnemy : public Enemy {
public:
	WizardEnemy(std::shared_ptr<Entity> entity) : Enemy(entity) {
		m_EnemySoulSpriteSheet = getTexture("data/EnemySoul2.png");
		m_Name = "Wizard";
		m_AttackDamage = 0;
		m_EnemyDialogue = {
			"If you ever get in a fight, your soul will be forced to face the enemies soul.",
			"You can fight back, cast soul magic, or act to try to resolve the fight without violence.",
			"Lets try dodging. That heart is YOUR soul, and you should really take good care of it.",
			"Dodge the projectiles to avoid taking damage. Use the arrow keys to move your soul around the screen."
		};

		m_Actions = { "Info", "Talk" };

		m_ActionResponse = { "This will contain info about the enemy you are fighting. You can feel the wizards soul across from you, but you cant see it.",
			"Good, you can usually take actions in combat. Sometimes doing something special will make something happen. Try this often. Maybe you don't have to kill whatever you are fighting."};

		m_EnemyFightSpriteSheet = getTexture("data/wizard.png");

		m_EnemySpriteClips = { { 0,0,128,128 } };

		FRAME_COUNT = 1;

		m_EnemyProjectile = std::make_shared<TutorialProjectile>(getTexture("data/boolet.png"), SDL_Rect{0,0,10,10}, Vector2f(0,0), Vector2f(200,200), 0);
		m_projectileCount = 10;
		m_layer1 = 284;
		m_layer2 = 194;
	}

	std::string FightActionResponse(int actionIndex) override {
		if (gameState.DebugMode) {
			printf("WizardEnemy FightActionResponse called with actionIndex: %d\n", actionIndex);
		}
		// by default, return the action response at the given index.
		if (actionIndex < 0 || actionIndex >= m_ActionResponse.size()) {
			return "Invalid action.";
		}
		if (actionIndex == 1 && m_tutorialStep == TutorialStep::ACT_ACTION) {
			m_tutorialStep = TutorialStep::MAGIC_MENU;
		}
		return m_ActionResponse[actionIndex];
	}

	bool IsTutorialFight() const { return true; }

	void ResetFightTutorial() {
		m_tutorialStep = TutorialStep::DODGE;
		m_tutorialPromptLine = 0;
	}

	bool ShouldStartTutorialDodge() const { return m_tutorialStep == TutorialStep::DODGE; }

	void OnTutorialDodgeComplete() {
		if (m_tutorialStep == TutorialStep::DODGE) {
			m_tutorialStep = TutorialStep::ACT_MENU;
			m_tutorialPromptLine = 0;
		}
	}

	/*
			// custom fight update for the wizard enemy. 
		// needs a couple of trigger flags, for the intro dialouge, 
		// and directing the player to use each part of the fight system.
		// this will be a tutorial fight, so the wizard will not attack the player,
		// but projectiles will still be spawned, and the player will need to dodge them.
		// It will be impossible to die here, and impossible to kill the wizard.
		// Fight will end on SillyMeter reaching 10.
		if (!m_introDialogueDone) {
			// tell the player what this screen is. 
			// "If you ever get in a fight, your soul will be forced to face the enemies soul."
			// "You can fight back, cast soul magic, or act to try to resolve the fight without violence."
			// "Lets try dodging. That heart is YOUR soul, and you should really take good care of it."
			// "Dodge the projectiles to avoid taking damage. Use the arrow keys to move your soul around the screen."

			m_introDialogueDone = true;
			return; // fall into the actions dialouge.
		}
		if (!m_actionDialogueDone) {
			// tell the player to use the action button. 
			
			//"Lets try acting first. Select Actions with your soul, and pick something to do. Often, theres something special you can do to make something happen."
			
			m_actionDialogueDone = true;
			return;
		}
		if (!m_magicDialogueDone) {
			// tell the player to use the magic button.
			
			//"Next, lets try casting some soul magic. Select Magic with your soul, and pick an ability to cast."
			//"Since you got here, you've already reached into your soul. Try reaching into it again."
			//" You'll learn more abilities as your soul changes. Sometimes the shape of your sould will effect what abilities you have."
			//" Youll have to figure out your own soul. No one can help you with that."
			//"Try casting something."
			// after the player has casted the only spell available, say this:
			//"Ah, that ability is called DoubleOrNothing. It will double your attack damage, but it will also make you extremely fragile."
			//"When an enemy is about to die, the usually reach into their souls, much like you just did to get here..."
			//"when that happens they usually get a burst of extreme power. But as strong as they are, they are also extremely fragile and will die in a single hit. 
			//"If you can see their soul, you can probably kill them."
			//"That being said, you should be careful when casting DoubleOrNothing. You'll also be extremely fragile, but powerful."
			
			m_magicDialogueDone = true;
			return;
		}
		if (!m_FightActionComplete) {
			// tell the player to use the fight button.
			
			//"okay good. Lets try fighting."
			//"when its your turn to fight, your soul will show you an aiming meter. try to hit Z at the right time to hit them hard."
			//"The more you hurt an enemy, the more you can see their soul coming out of them. If you can see their soul, you can probably kill them."
			//"Fighting doesnt really require your soul, just punch me or something, idk."
			// after the player has used the fight button, say this:

			
			
			m_FightActionComplete = true;
			return;
		}
		if (!m_tutorialComplete) {
			// End the Fight, and set the tutorial complete flag.
			
			//"okay good, you can fight now. Try not to die. Remember, if you can see your soul, you are probably near an enemy. "
			//"As long as you know the shape of your own soul, you'll be alright."
			
			m_tutorialComplete = true;
			return;
		}
	*/

	std::string GetTutorialInstruction() const {
		const std::vector<std::string> lines = GetTutorialPromptLines();
		return m_tutorialPromptLine < lines.size() ? lines[m_tutorialPromptLine] : "";
	}

	// Returns true only when the current group of tutorial dialogue is finished.
	bool AdvanceTutorialPrompt() {
		const std::vector<std::string> lines = GetTutorialPromptLines();
		if (m_tutorialPromptLine + 1 < lines.size()) {
			++m_tutorialPromptLine;
			return false;
		}

		if (m_tutorialStep == TutorialStep::MAGIC_RESULT) {
			m_tutorialStep = TutorialStep::FIGHT_MENU;
			m_tutorialPromptLine = 0;
			return false;
		}
		if (m_tutorialStep == TutorialStep::COMPLETE) {
			m_tutorialPromptLine = lines.size();
		}
		return true;
	}

	int TutorialRequiredMainMenuSelection() const {
		switch (m_tutorialStep) {
		case TutorialStep::ACT_MENU: return 1;   // Actions
		case TutorialStep::MAGIC_MENU: return 2; // Magic
		case TutorialStep::FIGHT_MENU: return 0; // Fight
		default: return -1;
		}
	}

	int TutorialRequiredActionSelection() const {
		return m_tutorialStep == TutorialStep::ACT_ACTION ? 1 : -1; // Talk
	}

	void OnTutorialMainMenuSelected(int selected) {
		if (m_tutorialStep == TutorialStep::ACT_MENU && selected == 1) {
			m_tutorialStep = TutorialStep::ACT_ACTION;
		}
		else if (m_tutorialStep == TutorialStep::MAGIC_MENU && selected == 2) {
			m_tutorialStep = TutorialStep::MAGIC_ACTION;
		}
		else if (m_tutorialStep == TutorialStep::FIGHT_MENU && selected == 0) {
			m_tutorialStep = TutorialStep::FIGHTING;
		}
	}

	void OnTutorialMagicSelected(int /*selected*/) {
		if (m_tutorialStep == TutorialStep::MAGIC_ACTION) {
			m_tutorialStep = TutorialStep::MAGIC_RESULT;
			m_tutorialPromptLine = 0;
		}
	}

	void OnTutorialFightResolved() {
		if (m_tutorialStep == TutorialStep::FIGHTING) {
			m_tutorialStep = TutorialStep::COMPLETE;
			m_tutorialPromptLine = 0;
		}
	}

	bool IsTutorialComplete() const { return m_tutorialStep == TutorialStep::COMPLETE && m_tutorialPromptLine >= GetTutorialPromptLines().size(); }

	std::string GetFightEndText() const {
		return "";
	}

	void ResetProjectiles() override {
    	m_EnemyProjectiles.clear();
                        // init the projectiles
		for (int i = 0; i < m_projectileCount; i++) {
            float subx = float(randomInt(0, gameState.screenwidth));
            float suby = float(randomInt(0, gameState.screenheight));
			m_EnemyProjectiles.push_back(std::make_shared<TutorialProjectile>(m_EnemyProjectile->m_SpriteSheet, m_EnemyProjectile->m_SpriteClip, Vector2f(subx, suby), Vector2f(200, 200), 0));
        }
	}

	private:
	enum class TutorialStep { DODGE, ACT_MENU, ACT_ACTION, MAGIC_MENU, MAGIC_ACTION, MAGIC_RESULT, FIGHT_MENU, FIGHTING, COMPLETE };
	std::vector<std::string> GetTutorialPromptLines() const {
		switch (m_tutorialStep) {
		case TutorialStep::ACT_MENU:
			return { "Okay, Now lets try Acting. Select Actions with your soul, and pick something to do. Often, theres something special you can do to make something happen." };
		case TutorialStep::MAGIC_MENU:
			return { "Next, lets try casting some soul magic. Select Magic with your soul, and pick an ability to cast.",
				"Since you got here, you've already reached into your soul. Try reaching into it again.",
				"You'll learn more abilities as your soul changes. Sometimes the shape of your soul will effect what abilities you have.",
				"Youll have to figure out your own soul. No one can help you with that.",
				"Try casting something." };
		case TutorialStep::MAGIC_RESULT:
			return { "Ah, that ability is called DoubleOrNothing. It will double your attack damage, but it will also make you extremely fragile.",
				"When an enemy is about to die, they will usually reach into their souls, much like you just did to get here...",
				"when that happens they usually get a burst of extreme power. But as strong as they are, they are also extremely fragile and will die in a single hit.",
				"If you can see their soul, you can probably kill them.",
				"That being said, you should be careful when casting DoubleOrNothing. You'll also be extremely fragile, but powerful." };
		case TutorialStep::FIGHT_MENU:
			return { "okay good. Lets try fighting.",
				"when its your turn to fight, your soul will show you an aiming meter. try to hit Z at the right time to hit them hard.",
				"The more you hurt an enemy, the more you can see their soul coming out of them. If you can see their soul, you can probably kill them.",
				"Fighting doesnt really require your soul, just punch me or something, idk." };
		case TutorialStep::COMPLETE:
			return { "okay good, you can fight now. Try not to die. Remember, if you can see your soul, you are probably near an enemy.",
				"As long as you know the shape of your own soul, you'll be alright." };
		default:
			return {};
		}
	}
	TutorialStep m_tutorialStep = TutorialStep::DODGE;
	size_t m_tutorialPromptLine = 0;
	bool m_TutorialComplete = false;
};

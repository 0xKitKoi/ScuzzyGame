#include <player.hpp>
#include <LTexture.hpp>
#include <vector>
#include <math.h>

bool checkCollision(SDL_Rect a, SDL_Rect b);
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
//extern LTexture gSpriteSheetTexture;

//enum class State {
//	Idle,
//	Walking,
//	Running,
//	// Add more states as needed
//};
//
//enum class Direction {
//	Up,
//	Down,
//	Left,
//	Right,
//	// Add more directions if needed
//};

int FRAME_COUNT = 4; // Assume each animation has 4 frames

// Each SDL_Rect points to a specific frame in the sprite sheet, the int is the frame count.
std::vector<SDL_Rect> UpWalking;
std::vector<SDL_Rect> idleDown;
std::vector<SDL_Rect> DownWalking;
std::vector<SDL_Rect> LeftWalking;
std::vector<SDL_Rect> RightWalking;
std::vector<SDL_Rect> Emotes;



Player::Player(Vector2f initPos) {
	m_PosX = initPos.x;
	m_PosY = initPos.y;
	m_Collider.w = SpriteWidth;
	m_Collider.h = SpriteHeight;
	m_VelX = 0;
	m_VelY = 0;
	currentState = State::Idle;
	currentDirection = Direction::Down;
	currentFrame = 0;

	SDL_Rect gSpriteClips[6*6];

	int pixelsize = 128; // size of each frame in sprite sheet. offset.

	//Load sprite sheet texture
	if (!SpriteSheet.loadFromFile("noss cartman 128.png"))
	{
		printf("Failed to load sprite sheet texture!\n");
		
	}
	else
	{
		// SPRITE SHEET MAPPING

		// Looking up not moving [0]
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = pixelsize;
		gSpriteClips[0].h = pixelsize;
		UpWalking.push_back(gSpriteClips[0]);

		// Looking up moving 1 [1]
		gSpriteClips[1].x = pixelsize;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = pixelsize;
		gSpriteClips[1].h = pixelsize;
		UpWalking.push_back(gSpriteClips[1]);

		// Looking up moving 2 [2]
		gSpriteClips[2].x = pixelsize * 2;
		gSpriteClips[2].y = 0;
		gSpriteClips[2].w = pixelsize;
		gSpriteClips[2].h = pixelsize;
		UpWalking.push_back(gSpriteClips[2]);

		// Looking up moving 3 [3]
		gSpriteClips[3].x = pixelsize * 3;
		gSpriteClips[3].y = 0;
		gSpriteClips[3].w = pixelsize;
		gSpriteClips[3].h = pixelsize;
		UpWalking.push_back(gSpriteClips[3]);

		// Looking up moving 4 [4]
		gSpriteClips[4].x = pixelsize * 4;
		gSpriteClips[4].y = 0;
		gSpriteClips[4].w = pixelsize;
		gSpriteClips[4].h = pixelsize;
		UpWalking.push_back(gSpriteClips[4]);

		// Looking down not moving [5]
		gSpriteClips[5].x = pixelsize * 5;
		gSpriteClips[5].y = 0;
		gSpriteClips[5].w = pixelsize;
		gSpriteClips[5].h = pixelsize;
		DownWalking.push_back(gSpriteClips[5]);

		// Looking down moving 1 [6]
		gSpriteClips[6].x = 0;
		gSpriteClips[6].y = pixelsize;
		gSpriteClips[6].w = pixelsize;
		gSpriteClips[6].h = pixelsize;
		DownWalking.push_back(gSpriteClips[6]);

		// Looking down moving 2 [7]
		gSpriteClips[7].x = pixelsize;
		gSpriteClips[7].y = pixelsize;
		gSpriteClips[7].w = pixelsize;
		gSpriteClips[7].h = pixelsize;
		DownWalking.push_back(gSpriteClips[7]);

		// Looking down moving 3 [8]
		gSpriteClips[8].x = pixelsize * 2;
		gSpriteClips[8].y = pixelsize;
		gSpriteClips[8].w = pixelsize;
		gSpriteClips[8].h = pixelsize;
		DownWalking.push_back(gSpriteClips[8]);

		// Looking down moving 4 [9]
		gSpriteClips[9].x = pixelsize * 3;
		gSpriteClips[9].y = pixelsize;
		gSpriteClips[9].w = pixelsize;
		gSpriteClips[9].h = pixelsize;
		DownWalking.push_back(gSpriteClips[9]);

		// Looking Left not moving [10]
		gSpriteClips[10].x = pixelsize * 4;
		gSpriteClips[10].y = pixelsize;
		gSpriteClips[10].w = pixelsize;
		gSpriteClips[10].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[10]);

		// Looking Left moving 1 [11]
		gSpriteClips[11].x = pixelsize * 5;
		gSpriteClips[11].y = pixelsize;
		gSpriteClips[11].w = pixelsize;
		gSpriteClips[11].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[11]);

		// Looking Left moving 2 [12]
		gSpriteClips[12].x = 0;
		gSpriteClips[12].y = pixelsize * 2;
		gSpriteClips[12].w = pixelsize;
		gSpriteClips[12].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[12]);

		// Looking Left moving 3 [13]
		gSpriteClips[13].x = pixelsize;
		gSpriteClips[13].y = pixelsize * 2;
		gSpriteClips[13].w = pixelsize;
		gSpriteClips[13].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[13]);

		// Looking Left moving 4 [14]
		gSpriteClips[14].x = pixelsize * 2;
		gSpriteClips[14].y = pixelsize * 2;
		gSpriteClips[14].w = pixelsize;
		gSpriteClips[14].h = pixelsize;
		LeftWalking.push_back(gSpriteClips[14]);

		// Looking Right not moving [15]
		gSpriteClips[15].x = pixelsize * 3;
		gSpriteClips[15].y = pixelsize * 2;
		gSpriteClips[15].w = pixelsize;
		gSpriteClips[15].h = pixelsize;
		RightWalking.push_back(gSpriteClips[15]);

		// Looking Right moving 1 [16]
		gSpriteClips[16].x = pixelsize * 4;
		gSpriteClips[16].y = pixelsize * 2;
		gSpriteClips[16].w = pixelsize;
		gSpriteClips[16].h = pixelsize;
		RightWalking.push_back(gSpriteClips[16]);

		// Looking Right moving 2 [17]
		gSpriteClips[17].x = pixelsize * 5;
		gSpriteClips[17].y = pixelsize * 2;
		gSpriteClips[17].w = pixelsize;
		gSpriteClips[17].h = pixelsize;
		RightWalking.push_back(gSpriteClips[17]);

		// Looking Right moving 3 [18]
		gSpriteClips[18].x = 0;
		gSpriteClips[18].y = pixelsize * 3;
		gSpriteClips[18].w = pixelsize;
		gSpriteClips[18].h = pixelsize;
		RightWalking.push_back(gSpriteClips[18]);

		// Looking Right moving 4 [19]
		gSpriteClips[19].x = pixelsize;
		gSpriteClips[19].y = pixelsize * 3;
		gSpriteClips[19].w = pixelsize;
		gSpriteClips[19].h = pixelsize;
		RightWalking.push_back(gSpriteClips[19]);

		// Looking Down not moving  [20]
		gSpriteClips[20].x = pixelsize * 2;
		gSpriteClips[20].y = pixelsize * 3;
		gSpriteClips[20].w = pixelsize;
		gSpriteClips[20].h = pixelsize;
		DownWalking.push_back(gSpriteClips[20]);

		// Looking Down suprised  [21]
		gSpriteClips[21].x = pixelsize * 3;
		gSpriteClips[21].y = pixelsize * 3;
		gSpriteClips[21].w = pixelsize;
		gSpriteClips[21].h = pixelsize;
		Emotes.push_back(gSpriteClips[21]);

		// Looking Right Suprised  [22]
		gSpriteClips[22].x = pixelsize * 4;
		gSpriteClips[22].y = pixelsize * 3;
		gSpriteClips[22].w = pixelsize;
		gSpriteClips[22].h = pixelsize;
		Emotes.push_back(gSpriteClips[22]);

		// Looking Left Suprised  [23]
		gSpriteClips[23].x = pixelsize * 5;
		gSpriteClips[23].y = pixelsize * 3;
		gSpriteClips[23].w = pixelsize;
		gSpriteClips[23].h = pixelsize;
		Emotes.push_back(gSpriteClips[23]);

		// Looking Up Suprised  [24]
		gSpriteClips[24].x = 0;
		gSpriteClips[24].y = pixelsize * 4;
		gSpriteClips[24].w = pixelsize;
		gSpriteClips[24].h = pixelsize;
		Emotes.push_back(gSpriteClips[24]);

		// Idle 1  [25]
		gSpriteClips[25].x = pixelsize;
		gSpriteClips[25].y = pixelsize * 4;
		gSpriteClips[25].w = pixelsize;
		gSpriteClips[25].h = pixelsize;
		Emotes.push_back(gSpriteClips[25]);

		// Idle 2 [26]
		gSpriteClips[26].x = pixelsize * 2;
		gSpriteClips[26].y = pixelsize * 4;
		gSpriteClips[26].w = pixelsize;
		gSpriteClips[26].h = pixelsize;
		Emotes.push_back(gSpriteClips[26]);

		// Idle 3  [27]
		gSpriteClips[27].x = pixelsize * 3;
		gSpriteClips[27].y = pixelsize * 4;
		gSpriteClips[27].w = pixelsize;
		gSpriteClips[27].h = pixelsize;
		Emotes.push_back(gSpriteClips[27]);

		// Looking Down Confused  [28]
		gSpriteClips[28].x = pixelsize * 4;
		gSpriteClips[28].y = pixelsize * 4;
		gSpriteClips[28].w = pixelsize;
		gSpriteClips[28].h = pixelsize;
		Emotes.push_back(gSpriteClips[28]);

		// Looking Down goober  [29]
		gSpriteClips[29].x = pixelsize * 5;
		gSpriteClips[29].y = pixelsize * 4;
		gSpriteClips[29].w = pixelsize;
		gSpriteClips[29].h = pixelsize;
		Emotes.push_back(gSpriteClips[29]);

		// Looking down Pissed  [30]
		gSpriteClips[30].x = 0;
		gSpriteClips[30].y = pixelsize * 5;
		gSpriteClips[30].w = pixelsize;
		gSpriteClips[30].h = pixelsize;
		Emotes.push_back(gSpriteClips[30]);

		// POG Left  [31]
		gSpriteClips[31].x = pixelsize;
		gSpriteClips[31].y = pixelsize * 5;
		gSpriteClips[31].w = pixelsize;
		gSpriteClips[31].h = pixelsize;
		Emotes.push_back(gSpriteClips[31]);

		// POG Right  [32]
		gSpriteClips[32].x = pixelsize * 2;
		gSpriteClips[32].y = pixelsize * 5;
		gSpriteClips[32].w = pixelsize;
		gSpriteClips[32].h = pixelsize;
		Emotes.push_back(gSpriteClips[32]);

		// DED  [33]
		gSpriteClips[33].x = pixelsize * 4;
		gSpriteClips[33].y = pixelsize * 5;
		gSpriteClips[33].w = pixelsize;
		gSpriteClips[33].h = pixelsize;
		Emotes.push_back(gSpriteClips[33]);
	}
}

void Player::Update(Uint32 deltaTime) {
	// Handle state and direction based on input...
	// (e.g., update `currentState` and `currentDirection`)

	// Advance animation frames
	lastFrameTime += deltaTime;
	if (lastFrameTime >= frameDuration) {
		currentFrame = (currentFrame + 1) % FRAME_COUNT;
		lastFrameTime = 0;
	}
}

void Player::move(SDL_Rect& wall) {
	m_PosX += m_VelX;
	m_Collider.x = m_PosX;

	//If the Player collided or went too far to the left or right
	if ((m_PosX < 0) || (m_PosX + SpriteWidth > SCREEN_WIDTH) || checkCollision(m_Collider, wall))
	{
		//Move back
		m_PosX -= m_VelX;
		m_Collider.x = m_PosX;
	}

	m_PosY += m_VelY;
	m_Collider.y = m_PosY;

	//If the Player collided or went too far up or down
	if ((m_PosY < 0) || (m_PosY + SpriteHeight > SCREEN_HEIGHT) || checkCollision(m_Collider, wall))
	{
		//Move back
		m_PosY -= m_VelY;
		m_Collider.y = m_PosY;
	}

}

void Player::handleEvent(SDL_Event& e) {
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: m_VelY -= MaxVelocity; currentState = State::Walking; currentDirection = Direction::Up; break;
		case SDLK_DOWN: m_VelY += MaxVelocity; currentState = State::Walking; currentDirection = Direction::Down; break;
		case SDLK_LEFT: m_VelX -= MaxVelocity; currentState = State::Walking; currentDirection = Direction::Left;  break;
		case SDLK_RIGHT: m_VelX += MaxVelocity; currentState = State::Walking; currentDirection = Direction::Right; break;
		}
	}

	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: m_VelY += MaxVelocity; currentState = State::Idle; break;
		case SDLK_DOWN: m_VelY -= MaxVelocity; currentState = State::Idle; break;
		case SDLK_LEFT: m_VelX += MaxVelocity; currentState = State::Idle; break;
		case SDLK_RIGHT: m_VelX -= MaxVelocity; currentState = State::Idle; break;
		}
	}
}

void Player::render() {
	//gSpriteSheetTexture.render(m_PosX, m_PosY);
	SDL_Rect srcRect;
	if (currentState == State::Idle && currentDirection == Direction::Down) {
		srcRect = DownWalking[0];
	}
	else if (currentState == State::Idle && currentDirection == Direction::Up) {
		srcRect = UpWalking[0];
	}
	else if (currentState == State::Idle && currentDirection == Direction::Left) {
		srcRect = LeftWalking[0];
	}
	else if (currentState == State::Idle && currentDirection == Direction::Right) {
		srcRect = RightWalking[0];
	}

	else if (currentState == State::Walking && currentDirection == Direction::Down) {
		if (currentFrame > DownWalking.size()) {
			//srcRect = idleDown[idleDown.size()];
			srcRect = DownWalking[DownWalking.size()];
		}
		else {
			srcRect = DownWalking[currentFrame];
		}
		
	}
	else if (currentState == State::Walking && currentDirection == Direction::Right) {
		if (currentFrame > RightWalking.size()) {
			srcRect = RightWalking[RightWalking.size()];
		}
		else {
			srcRect = RightWalking[currentFrame];
		}
		
	}
	else if (currentState == State::Walking && currentDirection == Direction::Left) {
		if (currentFrame > LeftWalking.size()) {
			srcRect = LeftWalking[LeftWalking.size()];
		}
		else {
			srcRect = LeftWalking[currentFrame];
		}
		
	}
	else if (currentState == State::Walking && currentDirection == Direction::Up) {
		if (currentFrame > UpWalking.size()) {
			srcRect = UpWalking[UpWalking.size()];
		}
		else {
			srcRect = UpWalking[currentFrame];
		}
		
	}
	// Add more conditions for other states and directions...

	// Define the destination rectangle on the screen
	//SDL_Rect destRect = { /* Define position and size */ };


	// hey dickhead tell the Ltexture to render with the rect you have!!!
	SpriteSheet.render(m_PosX, m_PosY, &srcRect);


	// Render the current frame
	//SDL_RenderCopy(renderer, SpriteSheet, &srcRect, &destRect);
	//SDL_RenderCopy(renderer, , &srcRect, &srcRect); // HUH
	//SpriteSheet.render();
}

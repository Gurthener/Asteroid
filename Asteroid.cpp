#include "Asteroid.h"

int desktopWidth;
int desktopHeight;
std::vector<Asteroid> v_Asteroids;
const int count = 10;

// Makes a window transparent by setting a transparency color.
bool MakeWindowTransparent(SDL_Window* window, COLORREF colorKey) {
	// Get window handle (https://stackoverflow.com/a/24118145/3357935)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);  // Initialize wmInfo
	SDL_GetWindowWMInfo(window, &wmInfo);
	HWND hWnd = wmInfo.info.win.window;

	// Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	// Set transparency color
	return SetLayeredWindowAttributes(hWnd, colorKey, 0, LWA_COLORKEY);
}

int main(int argc, char** argv) {
	bool quit = false;
	// Get resolution of primary monitor
	desktopWidth = GetSystemMetrics(SM_CXSCREEN);
	desktopHeight = GetSystemMetrics(SM_CYSCREEN);

	for (size_t i = 0; i < count; i++) {
		v_Asteroids.push_back(Asteroid({rand() % desktopWidth, rand() % desktopHeight},
									   {rand() % 1000 - 500, rand() % 1000 - 500},
									   {0,0},
									   50,
									   1000));
	}

	SDL_Window* window = SDL_CreateWindow("Asteroid",
										  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										  desktopWidth, desktopHeight, SDL_WINDOW_BORDERLESS);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture* asteroid = IMG_LoadTexture(renderer, "asteroidSprite.png");
	if (asteroid == NULL) quit = true;
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(asteroid, SDL_BLENDMODE_BLEND);

	auto oldTicks = SDL_GetTicks();
	auto newTicks = oldTicks;
	auto diffTicks = newTicks - oldTicks;
	while (!quit) {
		newTicks = SDL_GetTicks();
		diffTicks = newTicks - oldTicks;
		oldTicks = newTicks;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		MakeWindowTransparent(window, RGB(0, 0, 0));

		glm::ivec2 imousePosition;
		SDL_GetGlobalMouseState(&imousePosition.x, &imousePosition.y);
		glm::vec2 mousePosition = imousePosition;

		for (auto &rock : v_Asteroids) {
			rock.move(diffTicks);

			auto sprite = rock.getSpriteRect();
			auto pos = rock.getPosition();
			SDL_Rect rect1 = {pos.x - 64, pos.y - 64,
			128, 128};
			SDL_RenderCopy(renderer, asteroid, &sprite, &rect1);
		}
		
		for (auto& rock : v_Asteroids) {
			rock.setAcceleration({0,0});

			for (auto& target : v_Asteroids) {
				if (target.getPosition() != rock.getPosition()) {
					auto vecToTarget = target.getPosition() - rock.getPosition();
					float multiplier = 100000.0f / /*pow(*/(glm::length(vecToTarget) * 5)/*, 2)*/;
					vecToTarget = glm::normalize(vecToTarget);
					vecToTarget *= multiplier;
					rock.setAcceleration(rock.getAcceleration() + vecToTarget);
				}
			}
		}

		//auto vecToMouse = mousePosition - rock.getPosition();
		//float multiplier = 10000000.0f / pow(glm::length(vecToMouse), 2);
		//vecToMouse = glm::normalize(vecToMouse);
		//vecToMouse *= multiplier;
		//rock.setAcceleration(vecToMouse);

		SDL_RenderPresent(renderer);

		SDL_Event event;
		if (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
	}

	SDL_DestroyWindow(window);
	IMG_Init(IMG_INIT_PNG);
	SDL_Quit();
	return 0;
}

Asteroid& Asteroid::setPosition(glm::vec2 pos) {
	position = pos;
	return *this;
}

Asteroid& Asteroid::setSpeed(glm::vec2 spd) {
	speed = spd;
	return *this;
}

Asteroid& Asteroid::setAcceleration(glm::vec2 acc) {
	acceleration = acc;
	return *this;
}

Asteroid& Asteroid::setTexture(std::string tex) {
	texture = tex;
	return *this;
}

Asteroid& Asteroid::setSize(float sz) {
	size = sz;
	return *this;
}

Asteroid& Asteroid::setAnimationSpeed(float animSpd) {
	animationSpeed = animSpd;
	return *this;
}

glm::vec2 Asteroid::getPosition() {
	return position;
}

glm::vec2 Asteroid::getSpeed() {
	return speed;
}

glm::vec2 Asteroid::getAcceleration() {
	return acceleration;
}

std::string Asteroid::getTexture() {
	return texture;
}

float Asteroid::getSize() {
	return size;
}

float Asteroid::getAnimationSpeed() {
	return animationSpeed;
}

Asteroid& Asteroid::move(Uint32 diffTicks) {
	speed += acceleration * (diffTicks / 1000.0f);
	if (!((speed.x == 0) && (speed.y == 0)))
		setAnimationSpeed(glm::length(speed));

	position += speed * (diffTicks / 1000.0f);

	bool collision = false;
	if (position.x - size < 0) {
		speed.x = abs(speed.x) * 0.5;
		position.x = size;
		//collision = true;
	}

	if (position.x + size > desktopWidth) {
		speed.x = -abs(speed.x) * 0.5;
		position.x = desktopWidth - size;
		//collision = true;
	}

	if (position.y - size < 0) {
		speed.y = abs(speed.y) * 0.5;
		position.y = size;
		//collision = true;
	}

	if (position.y + size > desktopHeight) {
		speed.y = -abs(speed.y) * 0.5;
		position.y = desktopHeight - size;
		//collision = true;
	}

	return *this;
}


SDL_Rect Asteroid::getSpriteRect() {
	auto currentTicks = SDL_GetTicks();

	while ((animationSpeed >= 1) && (lastFrameTick + animationSpeed < currentTicks)) {
		frame++;
		lastFrameTick += animationSpeed;
	}

	int frameCol = frame % 8;
	int frameRow = (frame / 8) % 4 + 4;

	return {128 * frameCol, 128 * frameRow, 128, 128};
}
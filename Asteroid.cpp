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
									   {0, 0},
									   //{rand() % 1000 - 500, rand() % 1000 - 500},
									   {0,0},
									   rand() % 128 + 1,
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

		for (auto& rock : v_Asteroids) {
			rock.move(diffTicks);

			auto size = rock.getRadius();
			auto sprite = rock.getSpriteRect();
			auto pos = rock.getPosition();
			SDL_Rect rect1 = {pos.x - size, pos.y - size,
			size * 2, size * 2};
			SDL_RenderCopy(renderer, asteroid, &sprite, &rect1);
		}

		for (auto& rock : v_Asteroids) {
			
			//auto vecToMouse = mousePosition - rock.getPosition();
			//float force = 10000.0f / /*pow(*/(glm::length(vecToMouse))/*, 2)*/;
			//vecToMouse = glm::normalize(vecToMouse);
			//vecToMouse *= force;
		
			auto mass = rock.getMass();
			auto position = rock.getPosition();
			auto acceleration = rock.setAcceleration({0,0}).getAcceleration();

			for (auto& target : v_Asteroids) {
				auto vecToTarget = target.getPosition() - position;
				if (vecToTarget != glm::vec2(0,0)) {
					float force = 1 * /*M_GravConst * */ mass * target.getMass() / (glm::length(vecToTarget) * glm::length(vecToTarget));
					float acc = force / mass;

					vecToTarget = glm::normalize(vecToTarget);
					vecToTarget *= acc;

					acceleration += vecToTarget;
					//rock.setAcceleration(rock.getAcceleration() + vecToTarget);
				}
			}
			rock.setAcceleration(acceleration);
		}

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

Asteroid& Asteroid::setMass(float ms) {
	mass = ms;
	return *this;
	// TODO: вставьте здесь оператор return
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

Asteroid& Asteroid::setRadius(float sz) {
	radius = sz;
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

float Asteroid::getRadius() {
	return radius;
}

float Asteroid::getAnimationSpeed() {
	return animationSpeed;
}

float Asteroid::getMass() {
	return mass;
}

Asteroid& Asteroid::move(Uint32 diffTicks) {
	speed += acceleration * (diffTicks / 1000.0f);
	if (!((speed.x == 0) && (speed.y == 0)))
		setAnimationSpeed(glm::length(speed));

	position += speed * (diffTicks / 1000.0f);

	if (position.x - radius < 0) {
		speed.x = abs(speed.x) * 0.5;
		position.x = radius;
	}
	
	if (position.x + radius > desktopWidth) {
		speed.x = -abs(speed.x) * 0.5;
		position.x = desktopWidth - radius;
	}
	
	if (position.y - radius < 0) {
		speed.y = abs(speed.y) * 0.5;
		position.y = radius;
	}
	
	if (position.y + radius > desktopHeight) {
		speed.y = -abs(speed.y) * 0.5;
		position.y = desktopHeight - radius;
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
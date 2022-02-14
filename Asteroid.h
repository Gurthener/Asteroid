#pragma once
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"
#include "glm.hpp"

class Asteroid {
private:
	glm::vec2 position;
	glm::vec2 speed;
	glm::vec2 acceleration;
	std::string texture;
	float size;
	float animationSpeed = 100;
	uint32_t frame = 0;
	uint32_t lastFrameTick = 0;

public:
	Asteroid(glm::vec2 pos, glm::vec2 spd, glm::vec2 acc, float sz, float animSpd){
		setPosition(pos);
		setSpeed(spd);
		setAcceleration(acc);
		setSize(sz);
		setAnimationSpeed(animSpd);
	}

	Asteroid& setPosition(glm::vec2 pos);
	Asteroid& setSpeed(glm::vec2 spd);
	Asteroid& setAcceleration(glm::vec2 acc);
	Asteroid& setTexture(std::string tex);
	Asteroid& setSize(float sz);
	Asteroid& setAnimationSpeed(float animSpd);

	glm::vec2 getPosition();
	glm::vec2 getSpeed();
	glm::vec2 getAcceleration();
	std::string getTexture();
	float getSize();
	float getAnimationSpeed();

	Asteroid& move(Uint32 diffTicks);
	SDL_Rect getSpriteRect();
};


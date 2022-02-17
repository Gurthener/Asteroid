#pragma once
#include <string>
#include <vector>

#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_image.h"
#include "glm.hpp"

const float M_GravConst = 6.67430151515151515e-11;

class Asteroid {
private:
	glm::vec2 position;
	glm::vec2 speed;
	glm::vec2 acceleration;
	float radius;
	float mass;
	float animationSpeed = 100;
	uint32_t frame = 0;
	uint32_t lastFrameTick = 0;

public:
	Asteroid(glm::vec2 pos, glm::vec2 spd, glm::vec2 acc, float rad, float animSpd){
		setPosition(pos);
		setSpeed(spd);
		setAcceleration(acc);
		setRadius(rad);
		setAnimationSpeed(animSpd);
		setMass(4/3 * M_PI * pow(rad, 3));
	}

	Asteroid& setPosition(glm::vec2 pos);
	Asteroid& setSpeed(glm::vec2 spd);
	Asteroid& setAcceleration(glm::vec2 acc);
	Asteroid& setRadius(float rad);
	Asteroid& setAnimationSpeed(float animSpd);
	Asteroid& setMass(float mss);

	glm::vec2 getPosition();
	glm::vec2 getSpeed();
	glm::vec2 getAcceleration();
	float getRadius();
	float getAnimationSpeed();
	float getMass();

	Asteroid& move(Uint32 diffTicks);
	SDL_Rect getSpriteRect();
};


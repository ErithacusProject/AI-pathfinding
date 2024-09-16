#pragma once
// there is probably a much better way to use this class
// Using an Object Oriented Design Pattern - e.g. a Vehicle HAS-A ForceHelper. 
// currently it is simply a simple force calculator which doesn't take into account existing velocity

#include "Vector2D.h"
#include <vector>
#include <string>

using namespace std;

/*
typedef struct attractor {
	Vector2D attractorPoint;
	string name; // purely to help you debug
};

typedef vector<attractor> vecAttractor;

class ForceHelper
{
public:
	ForceHelper();
	~ForceHelper();

public:
	void startForceCalculations(); // reset the force calculations (important! must be called at the beginning of each frame)
	void addForce(const Vector2D& force);

	// returns a new position based on the force calculations, desired speed, and current position
	Vector2D calculateNewPosition(const float deltaTime, const float speed, Vector2D currentPostion);

	void clearAttractors() { m_vecAttractors.clear(); }
	void clearDetractors() { m_vecDetractors.clear(); }

	void addAttractor(const Vector2D v2d, const string name);
	void addDetractor(const Vector2D v2d, const string name);

	void removeAttractor(const string name);
	void removeDetractor(const string name);

protected:
	vecAttractor	m_vecAttractors;
	vecAttractor	m_vecDetractors;

	Vector2D m_totalForce;
};

*/
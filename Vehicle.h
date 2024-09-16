#pragma once

#include "DrawableGameObject.h"
#include "WaypointManager.h"
#include "Vector2D.h"
#include "Collidable.h"
#include "ForceMotion.h"
#include "Waypoint.h"
#include <vector>
#include <iostream>

#define VEHICLE_MASS 0.00005f
#define SEEK_MESSAGE "SEEK"
#define SEEK_RADIUS 10


typedef struct MessagePosition
{
	Vector2D position;
	string name;
};

enum States
{
	WANDER,
	SEEK,
	FLEE,
	ARRIVE,
	PURSUIT,
	WAIT
};

enum CarMode 
{
	TAXI,
	SABOTAGE
};


enum class carColour
{
	redCar,
	blueCar,
};

class Vehicle : public DrawableGameObject, public Collidable
{
public:
	Vehicle();

public:
	virtual HRESULT initMesh(ID3D11Device* pd3dDevice, carColour colour);
	virtual void update(const float deltaTime);

	void setPosition(Vector2D position); // the current position
	void setWaypointManager(WaypointManager* wpm);
	void hasCollided() {}
	

	ForceMotion* getForceMotion() { return &m_forceMotion; }

	void applyForceToPosition(const Vector2D& positionTo, string name = "");
	Waypoint* targetPosition;
	void SetState(States newState) { state = newState; }
	States GetState() { return state; }
	void SetOtherCar(Vehicle* followCar) { m_otherCar = followCar; }
	void SetTargetPosition(Vector2D target);
	void SetTargetPosition(Waypoint* target);
	void SetCarMode(CarMode newMode) { m_carMode = newMode; }
	void SetPassengerPos(Vector2D newpos) { m_passengerPosition = newpos; }
	void SetFuelPos(Vector2D newpos) { m_fuelPosition = newpos; }
	void SetSpeedPos(Vector2D newpos) { m_speedPosition = newpos; }

	void AddFuel();
	void AddSpeed();
		
protected: // protected methods
	Vector2D* getPositionAddress() { return &m_currentPosition; }

	
	void updateMessages(const float deltaTime);
	void messageReceived(MessagePosition message);
	void addMessage(MessagePosition message);
	void updateState();
	void TaxiUpdate();
	void SabotageUpdate();
	void CreateWhisker();
	void UpdateWhisker(int whiskerIndex, Vector2D direction);
	void Seek();
	void Flee();
	void Arrive();
	void Wander(WaypointManager* waypoints);
	void Pursuit();
	Waypoint* GetRandomWaypoint();
	void FindPath(Waypoint* target);
	vecWaypoints m_path;

	float FindDistance(Vector2D point1, Vector2D point2);


protected: // protected properties
	Vector2D m_currentPosition;
	Vector2D m_lastPosition;
	WaypointManager* m_waypointManager;
	ForceMotion m_forceMotion;
	vector<Vector2D> m_whiskers;
	States state;
	States m_previousState;
	Vehicle* m_otherCar;
	float m_speed;
	CarMode m_carMode;
	float m_fuel;

	Vector2D m_passengerPosition;
	Vector2D m_fuelPosition;
	Vector2D m_speedPosition;

	list<MessagePosition> m_vecMessages;

};


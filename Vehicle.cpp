#include "Vehicle.h"
#include <vector>
#include <cstdlib>

#define VEHICLE_MASS 0.00005f
#define SEEK_MESSAGE "SEEK"
#define SEEK_RADIUS 10

Vehicle::Vehicle() : m_forceMotion(VEHICLE_MASS, getPositionAddress())
{
	m_currentPosition = Vector2D(0,0);
	m_lastPosition = Vector2D(0, 0);
	//m_waypointManager = nullptr;
	m_otherCar = nullptr;
	m_speed = 1;
	m_fuel = 100;
	m_path.clear();

	//create whiskers and set to 0 at start
	//the position will change on update whiskers
	int noWhiskers = 1;
	for (int i = 0; i < noWhiskers; i++) 
	{
		m_whiskers.push_back(Vector2D(0, 0));
	}
}

HRESULT	Vehicle::initMesh(ID3D11Device* pd3dDevice, carColour colour)
{
	m_scale = XMFLOAT3(30, 20, 1);

	if (colour == carColour::redCar)
	{
		setTextureName(L"Resources\\car_red.dds");
	}
	else if (colour == carColour::blueCar)
	{
		setTextureName(L"Resources\\car_blue.dds");
	}

	HRESULT hr = DrawableGameObject::initMesh(pd3dDevice);

	setPosition(Vector2D(0, 0));

	m_lastPosition = Vector2D(0, 0);

	return hr;
}

void Vehicle::update(const float deltaTime)
{
	updateMessages(deltaTime);
	/*switch (m_carMode) 
	{
		case TAXI:
		{
			TaxiUpdate();
			break;
		}
		case SABOTAGE:
		{
			SabotageUpdate();
			break;
		}
	}*/
	updateState();

	m_forceMotion.update(deltaTime);

	// rotate the object based on its last & current position
	Vector2D diff = m_currentPosition - m_lastPosition;
	for (int i = 0; i < m_whiskers.size(); i++) 
	{
		UpdateWhisker(i, diff);
	}
	if (diff.Length() > 0) { // if zero then don't update rotation
		diff.Normalize();
		m_radianRotation = atan2f((float)diff.y, (float)diff.x); // this is used by DrawableGameObject to set the rotation
	}
	m_lastPosition = m_currentPosition;

	// set the current poistion for the drawablegameobject
	setPosition(m_currentPosition);

	if (m_fuel > 0) {
		m_fuel -= 0.02;
	}

	if (m_fuel <= 0) {
		m_fuel = 0;
		m_speed = 1;
	}

	std::cout << m_fuel;

	DrawableGameObject::update(deltaTime);
}

void Vehicle::TaxiUpdate() 
{
	Vector2D cardis = m_otherCar->getPosition() - m_currentPosition;
	Vector2D passengeris = m_passengerPosition - m_currentPosition;
	//handles state trantions for taxi mode
	if (m_fuel < 25) {
		state = SEEK;
		targetPosition = m_waypointManager->getNearestWaypoint(m_fuelPosition);
	}
	else if (abs(cardis.x) < 10|| abs(cardis.y) < 10) {
		int random = rand() % 4;
		if (random == 1) {
			state = PURSUIT;
		}
		else {
			state = FLEE;
		}
	}
	else if (abs(passengeris.x) < 20|| abs(passengeris.y) < 20) {
		state = ARRIVE;
		targetPosition = m_waypointManager->getNearestWaypoint(m_passengerPosition);
	}
	else 
	{
		state = WANDER;
	}

}
void Vehicle::SabotageUpdate()
{
	//this will need to check some things havent been called before 
	Vector2D cardis = m_otherCar->getPosition() - m_currentPosition;
	Vector2D speedis = m_speedPosition - m_currentPosition;
	//handles state trantions for sabo mode
	if (m_fuel < 50) {
		state = SEEK;
		targetPosition = m_waypointManager->getNearestWaypoint(m_fuelPosition);
	}
	else if (abs(cardis.x) < 10 || abs(cardis.y) < 10)
	{
		int random = rand() % 4;
		if (random == 1)
		{
			state = FLEE;
		}
		else
		{
			state = PURSUIT;
		}
	}
	else if (abs(speedis.x) < 20 || abs(speedis.y) < 20) {
		state = SEEK;
		targetPosition = m_waypointManager->getNearestWaypoint(m_speedPosition);
	}
	else
	{
		state = WANDER;
	}
}


void Vehicle::updateState()
{
	switch (state) 
	{
		case WANDER:
		{
			//call every x frames 
			break;
		}
		case SEEK:
		{
			Vector2D carpos = getPosition();
			Vector2D targetspos = getPosition();
			if (FindDistance(getPosition(), targetPosition->getPosition()) > 0.1f)
			{
				Seek();
			}
			if (FindDistance(getPosition(), targetPosition->getPosition()) < 0.1f)
			{
				m_path.erase(m_path.begin());
				if (m_path.empty() == true)
				{
					state = WAIT;
				}
				else 
				{
					targetPosition = m_path[0];
				}
			}
			break;
		}
		case FLEE:
		{
			//need a limit on flee or it goes forever
			if (state != m_previousState)
			{
				Flee();
			}
			break;
		}
		case ARRIVE:
		{
			if (getPosition() != targetPosition->getPosition()) {
				Arrive(); //jittering issue
			}
			else {
				state = WAIT;
			}
			break;
		}
		case PURSUIT:
		{
			Pursuit();
			break;
		}
		case WAIT:
		{
			getForceMotion()->clearForce();
			break;
		}
	}
	m_previousState = state;
}

// set the current position
void Vehicle::setPosition(Vector2D position)
{
	m_currentPosition = position;
	DrawableGameObject::setPosition(position);
}

void Vehicle::applyForceToPosition(const Vector2D& positionTo, string name)
{

	// create a vector from the position to, and the current car position
	Vector2D posFrom = getPosition();
	Vector2D force = positionTo - posFrom;

	// normalise this (make it length 1)
	force.Normalize();
	force = force * m_speed;

	getForceMotion()->applyForce(force);

	// Tutorial todo
	// create a message called 'SEEK' which detects when the car has reached a certain point
	// note: this has been done for you in the updateMessages function. 
	MessagePosition message;
	message.name = name;
	message.position = positionTo;
	addMessage(message);

	//addMessage(message);
}

void Vehicle::setWaypointManager(WaypointManager* wpm)
{
	m_waypointManager = wpm;
}


// -------------------------------------------------------------------------------
// a really rubbish messaging system.. there is clearly a better way to do this...


void Vehicle::addMessage(MessagePosition message)
{
	m_vecMessages.push_back(message);
}

void Vehicle::updateMessages(const float deltaTime)
{
	// create an iterator to iterate the message list
	list<MessagePosition>::iterator messageIterator = m_vecMessages.begin();

	// loop while the iterator is not at the end
	while (messageIterator != m_vecMessages.end())
	{
		MessagePosition msg = *messageIterator;
		if (msg.name.compare(SEEK_MESSAGE) == 0)
		{
			Vector2D differenceVector = getPosition() - msg.position;
			// WARNING - when testing distances, make sure they are large enough to be detected. Ask a lecturer if you don't understand why. 10 *should* be about right
			if (differenceVector.Length() < SEEK_RADIUS)
			{
				messageReceived(msg);

				// delete the message. This will also assign(increment) the iterator to be the next item in the list
				messageIterator = m_vecMessages.erase(messageIterator);
				continue; // continue the next loop (we don't want to increment below as this will skip an item)
			}
		}
		messageIterator++; // incremenet the iterator
	}

	
}

void Vehicle::messageReceived(MessagePosition message)
{
	if (message.name.compare(SEEK_MESSAGE) == 0)
	{
		// Tutorial Todo
		m_forceMotion.clearForce();
	}
}

void Vehicle::Seek()
{
	applyForceToPosition(targetPosition->getPosition(), SEEK_MESSAGE);
}

//Flee
void Vehicle::Flee()
{
	Vector2D force = (getPosition() - targetPosition->getPosition());
	force.Normalize();
	getForceMotion()->applyForce(force);
}

//arrive
void Vehicle::Arrive()
{
	//needs to slow down sooner
	Vector2D force = (targetPosition->getPosition() - getPosition()) * m_speed;
	if (abs(force.x) > 1 || abs(force.y) > 1)
	{
		force.Normalize();
		//force += force * m_speed;
	}
	getForceMotion()->applyForce(force);
	if (abs(force.x) < 0.2 && abs(force.y) < 0.2) {
		setPosition(targetPosition->getPosition());
		state = WAIT;
	}
}

void Vehicle::Wander(WaypointManager* waypoints) 
{
	SetTargetPosition(GetRandomWaypoint());
	//chooses a waypoint to move to, but after a second or two (unless it reaches the waypoint first), it then chooses another waypoint.
}

void Vehicle::Pursuit() 
{
	//finds and follows the other car, always staying behind it.
}

void Vehicle::UpdateWhisker(int whiskerIndex, Vector2D direction)
{
	//whisker = vehicle pos + vehicle dir
	m_whiskers[whiskerIndex] = (getPosition() + direction);
	//std::cout << m_whiskers[whiskerIndex];
}

void Vehicle::AddFuel() {
	m_fuel += 25;
	if (m_fuel > 100) 
	{
		m_fuel = 100;
	}
}

void Vehicle::AddSpeed() {
	if (m_fuel > 0) 
	{
		//add extra speed for x seconds
	}
}

void Vehicle::FindPath(Waypoint* target)
{
	//Open List is a collection of all possible waypoints.
	vecWaypoints openList;
	openList.clear();
	for (int i = 0; i < m_waypointManager->getWaypointCount(); i++) {
		openList.push_back(m_waypointManager->getWaypoint(i));
	}
	//closed list is a collection of all visited nodes, it is added to when a node is visited.
	vecWaypoints closedList;

	//this is the path the AI will follow
	vecWaypoints pathToFollow;

	//find current nearest waypoint from position
	Waypoint* CurrentNode = m_waypointManager->getNearestWaypoint(this->getPosition());

	//add current closest node to path and closed list
	closedList.push_back(CurrentNode);
	pathToFollow.push_back(CurrentNode);

	//get neighboring waypoints and get the closest one until you reach the target node
	while (CurrentNode != target)
	{
		vecWaypoints neighbors = m_waypointManager->getNeighbouringWaypoints(CurrentNode); //getting neighboring waypoints
		Waypoint* nextNode = nullptr; //current waypoint with shortest path
		for (int i = 0; i < neighbors.size(); i++) //loop to find which neighbor has shortest path
		{
			if (std::count(openList.begin(), openList.end(), neighbors[i])) { //checks that current neigthbor is not already in closed
				if (nextNode == nullptr) {
					nextNode = neighbors[i];
				}
				else
				{
					if (CurrentNode->distanceToWaypoint(neighbors[i]) + neighbors[i]->distanceToWaypoint(target) < CurrentNode->distanceToWaypoint(nextNode) + nextNode->distanceToWaypoint(target))
					{
						nextNode = neighbors[i];
					}
				}
			}
		}
		pathToFollow.push_back(nextNode);
		closedList.push_back(nextNode);
		openList.erase(std::remove(openList.begin(), openList.end(), nextNode), openList.end());
		CurrentNode = nextNode;
	}
	m_path = pathToFollow;
}

float Vehicle::FindDistance(Vector2D point1, Vector2D point2)
{
	float a = (point1.x - point2.x) * (point1.x - point2.x);
	float b = (point1.y - point2.y) * (point1.y - point2.y);
	float c = abs(a + b) /100;
	return c;
}

Waypoint* Vehicle::GetRandomWaypoint() {
	int randomNumber = rand() % m_waypointManager->getWaypointCount();
	Waypoint* Waypoint = m_waypointManager->getWaypoint(randomNumber);
	return Waypoint;
}

void Vehicle::SetTargetPosition(Vector2D target) 
{ 
	FindPath(m_waypointManager->getNearestWaypoint(target)); 
}

void Vehicle::SetTargetPosition(Waypoint* target) 
{
	FindPath(target);
}
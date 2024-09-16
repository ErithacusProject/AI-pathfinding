#include "AIManager.h"
#include "Vehicle.h"
#include "DrawableGameObject.h"
#include "PickupItem.h"
#include "Waypoint.h"
#include "main.h"
#include "constants.h"
#include "ForceHelper.h"

// AI Manager

AIManager::AIManager()
{
    m_pRedCar = nullptr;
	m_pBlueCar = nullptr;
}

AIManager::~AIManager()
{
	release();
}

void AIManager::release()
{
	clearDrawList();

	for (PickupItem* pu : m_pickups)
	{
		delete pu;
	}
	m_pickups.clear();

	delete m_pRedCar;
    m_pRedCar = nullptr;
	delete m_pBlueCar;
	m_pBlueCar = nullptr;
}

HRESULT AIManager::initialise(ID3D11Device* pd3dDevice)
{
	m_waypointManager = new WaypointManager();
	m_waypointManager->createWaypoints(pd3dDevice);

    // create the vehicle 
    float xPos = -500;
    float yPos = 300;

    m_pRedCar = new Vehicle();
    HRESULT hr = m_pRedCar->initMesh(pd3dDevice, carColour::redCar);
	m_pRedCar->setWaypointManager(m_waypointManager);
    m_pRedCar->setPosition(Vector2D(xPos, yPos));
    m_pRedCar->SetCarMode(SABOTAGE);
    if (FAILED(hr))
        return hr;

	m_pBlueCar = new Vehicle();
	hr = m_pBlueCar->initMesh(pd3dDevice, carColour::blueCar);
	m_pBlueCar->setWaypointManager(m_waypointManager);
	m_pBlueCar->setPosition(Vector2D(xPos, yPos));
    m_pBlueCar->SetCarMode(TAXI);
	m_pBlueCar->targetPosition = m_waypointManager->getNearestWaypoint(m_pBlueCar->getPosition());

    // setup the waypoints
    

    // create a passenger pickup item
    PickupItem* pPickupPassenger = new PickupItem();
    hr = pPickupPassenger->initMesh(pd3dDevice, pickuptype::Passenger);
    m_pickups.push_back(pPickupPassenger);

	//create a fuel pickup item
	PickupItem* pPickupFuel = new PickupItem();
	hr = pPickupFuel->initMesh(pd3dDevice, pickuptype::Fuel);
	m_pickups.push_back(pPickupFuel);

	//create a speed pickup item
	PickupItem* pPickupSpeed = new PickupItem();
	hr = pPickupSpeed->initMesh(pd3dDevice, pickuptype::SpeedBoost);
	m_pickups.push_back(pPickupSpeed);

    // (needs to be done after waypoint setup)
    setRandomPickupPosition(pPickupPassenger);

    return hr;
}


void AIManager::update(const float fDeltaTime)
{
    for (unsigned int i = 0; i < m_waypointManager->getWaypointCount(); i++) {
        m_waypointManager->getWaypoint(i)->update(fDeltaTime);
        //AddItemToDrawList(m_waypointManager.getWaypoint(i)); // if you uncomment this, it will display the waypoints
    }

    for (int i = 0; i < m_waypointManager->getQuadpointCount(); i++)
    {
        Waypoint* qp = m_waypointManager->getQuadpoint(i);
        qp->update(fDeltaTime);
        //AddItemToDrawList(qp); // if you uncomment this, it will display the quad waypoints
    }

    // update and display the pickups
    for (unsigned int i = 0; i < m_pickups.size(); i++) {
        m_pickups[i]->update(fDeltaTime);
        AddItemToDrawList(m_pickups[i]);
    }

	// draw the waypoints nearest to the red car
	/*
    Waypoint* wp = m_waypointManager.getNearestWaypoint(m_pRedCar->getPosition());
	if (wp != nullptr)
	{
		vecWaypoints vwps = m_waypointManager.getNeighbouringWaypoints(wp);
		for (Waypoint* wp : vwps)
		{
			AddItemToDrawList(wp);
		}
	}
    */

    // update and draw the red car (and check for pickup collisions)
	if (m_pRedCar != nullptr)
	{
        m_pRedCar->update(fDeltaTime);
		for (int x = 0; x < m_pickups.size(); x++) 
		{
			checkForCollisions(x, m_pRedCar);
		}
		AddItemToDrawList(m_pRedCar);
	}
	if (m_pBlueCar != nullptr)
	{
		m_pBlueCar->update(fDeltaTime);
		for (int x = 0; x < m_pickups.size(); x++)
		{
			checkForCollisions(x, m_pBlueCar);
		}
		AddItemToDrawList(m_pBlueCar);
	}
}

void AIManager::mouseUp(int x, int y)
{
    // HINT you will find this useful later on...
	//Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
	//if (wp == nullptr)
	//	return;

    // Tutorial todo here
    m_pRedCar->applyForceToPosition(Vector2D(x, y), SEEK_MESSAGE);
}

void AIManager::keyUp(WPARAM param)
{
    const WPARAM key_a = 65;
    switch (param)
    {
    case key_a:
    {
        OutputDebugStringA("a Up \n");
        break;
    }
    }
}

void AIManager::keyDown(WPARAM param)
{
    // hint 65-90 are a-z
    const WPARAM key_a = 65;
    const WPARAM key_s = 83;
    const WPARAM key_t = 84;
	const WPARAM key_f = 70;
    const WPARAM key_w = 87;
    const WPARAM key_p = 80;

    switch (param)
    {
    case VK_NUMPAD0:
    {
        OutputDebugStringA("0 pressed \n");
        break;
    }
    case VK_NUMPAD1:
    {
        OutputDebugStringA("1 pressed \n");
        break;
    }
    case VK_NUMPAD2:
    {
        OutputDebugStringA("2 pressed \n");
        break;
    }
    case key_a:
    {
        int index = rand() % m_waypointManager->getWaypointCount();
        m_pBlueCar->SetTargetPosition(m_waypointManager->getWaypoint(index));
        m_pBlueCar->SetState(ARRIVE);
        break;
    }
    case key_s:
    {
		//random point 
        int index = rand() % m_waypointManager->getWaypointCount();
        m_pBlueCar->SetTargetPosition(m_waypointManager->getWaypoint(index));
        m_pBlueCar->SetState(SEEK);
        break;
    }
	case key_f:
	{
        m_pRedCar->SetTargetPosition(m_pBlueCar->getPosition());
		m_pRedCar->SetState(FLEE);
		break;
	}
    case key_w: 
    {
        // red car wander
        m_pRedCar->SetState(WANDER);
        break;
    }
    case key_p: 
    {
        //blue car finds and follows red car, always staying behind it 
        m_pBlueCar->SetOtherCar(m_pRedCar);
        m_pBlueCar->SetState(PURSUIT);
        break;
    }
    case key_t:
    {
        break;
    }
    // etc
    default:
        break;
    }
}

void AIManager::setRandomPickupPosition(PickupItem* pickup)
{
    if (pickup == nullptr)
        return;

    int x = (rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2);
    int y = (rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2);

    Waypoint* wp = m_waypointManager->getNearestWaypoint(Vector2D(x, y));
    if (wp) {
        pickup->setPosition(wp->getPosition());
    }
}

/*
// hello. This is hopefully the only time you may need to use and alter directx code 
// the relevant #includes are already in place, but if you create your own collision class (or use this code anywhere else) 
// make sure you have the following:

#include <d3d11_1.h> // this has the appropriate directx structures / objects
#include <DirectXCollision.h> // this is the dx collision class helper
using namespace DirectX; // this means you don't need to put DirectX:: in front of objects like XMVECTOR and so on. 
*/

bool AIManager::checkForCollisions(int pickupNo, Vehicle* car)
{
    if (m_pickups.size() == 0)
        return false;

    XMVECTOR dummy;

    // get the position and scale of the car and store in dx friendly xmvectors
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose(
        &carScale,
        &dummy,
        &carPos,
        XMLoadFloat4x4(car->getTransform())
    );

    // create a bounding sphere for the car
    XMFLOAT3 scale;
    XMStoreFloat3(&scale, carScale);
    BoundingSphere boundingSphereCar;
    XMStoreFloat3(&boundingSphereCar.Center, carPos);
    boundingSphereCar.Radius = scale.x;

    // do the same for a pickup item
    // a pickup - !! NOTE it is only referring the first one in the list !!
    // to get the passenger, fuel or speedboost specifically you will need to iterate the pickups and test their type (getType()) - see the pickup class
    XMVECTOR puPos;
    XMVECTOR puScale;
    XMMatrixDecompose(
        &puScale,
        &dummy,
        &puPos,
        XMLoadFloat4x4(m_pickups[pickupNo]->getTransform())
    );

    // bounding sphere for pickup item
    XMStoreFloat3(&scale, puScale);
    BoundingSphere boundingSpherePU;
    XMStoreFloat3(&boundingSpherePU.Center, puPos);
    boundingSpherePU.Radius = scale.x;

    // does the car bounding sphere collide with the pickup bounding sphere?
    if (boundingSphereCar.Intersects(boundingSpherePU))
    {
        OutputDebugStringA("pickup passenger collision\n");
        m_pickups[pickupNo]->hasCollided();
        setRandomPickupPosition(m_pickups[pickupNo]);

        // you will need to test the type of the pickup to decide on the behaviour
        // m_pRedCar->dosomething(); ...
		if (m_pickups[pickupNo]->getType() == pickuptype::Passenger) 
		{
			car->SetPassengerPos(m_pickups[pickupNo]->getPosition());
		}
		else if (m_pickups[pickupNo]->getType() == pickuptype::Fuel) 
		{
			car->AddFuel();
			car->SetFuelPos(m_pickups[pickupNo]->getPosition());
		}
		else if (m_pickups[pickupNo]->getType() == pickuptype::SpeedBoost) 
		{
			car->AddSpeed();
			car->SetSpeedPos(m_pickups[pickupNo]->getPosition());
		}

        return true;
    }

    return false;
}
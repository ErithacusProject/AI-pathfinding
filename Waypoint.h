#pragma once
#include "DrawableGameObject.h"
class Waypoint :
    public DrawableGameObject
{
public:
	
    virtual HRESULT init(ID3D11Device* pd3dDevice, bool isOnTrack, bool checkPointID);

    bool    isOnTrack()     { return m_isOnTrack; }
    void    setVisible(const bool visible) { m_visible = visible; }
    bool    isVisible()     { return m_visible;}
    int     getCheckpointID() { return m_checkpointID; }
	float   distanceToWaypoint(Waypoint* waypoint);
    

private:
    bool m_isOnTrack;
    bool m_visible;
    int  m_checkpointID;
};


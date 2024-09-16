#pragma once

#include "Vector2D.h"



class ForceMotion {

private:
    float      m_mass; // object's mass
    Vector2D      m_velocity; // object's velocity
    Vector2D      m_force; // force applied to the object
    Vector2D*   m_pPosition;

public:
    // constructor
    ForceMotion(float m, Vector2D* position) {
        m_mass = m;
        m_velocity = Vector2D(0, 0);
        m_force = Vector2D(0, 0);
        m_pPosition = position;
    }


    // method to clear force - do this each frame
    void clearForce() {
        m_force = Vector2D(0,0);
    }

    // method to apply force
    void applyForce(Vector2D f) {
        m_force = (f);
    }

    // method to accummulate force
    void accummulateForce(Vector2D f) {
        m_force += (f);
    }

    void update(double dt) {
        updateVelocity(dt);
        updatePosition(dt);
    }

    // method to update object's velocity
    void updateVelocity(double dt) {
        m_velocity = (m_force / m_mass) * dt;
    }

    // method to update object's position
    void updatePosition(double dt) {
        *m_pPosition += m_velocity * dt;
    }

    // getter for velocity
    Vector2D getVelocity() {
        return m_velocity;
    }
};


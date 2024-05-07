#ifndef EMITTER_H_
#define EMITTER_H_

#include <iostream>
#include <cmath>
#include <vector>
#include <string_view>
#include "Particle.h"
#include <ngl/Vec3.h>
#include <ngl/AbstractVAO.h>
#include <memory>
#include <ngl/Util.h>
#include <ngl/Mat4.h>


class Emitter
{
public :
  Emitter(int _numParticles, int _maxAlive, float _rectWidth = 150.0f, float _rectHeight = 150.0f);
  void update();
  void render() const;
  void writeToGeo(std::string_view _fname);
private :
    ngl::Vec3 randomVectorOnSphere();
  void createDefaultParticle(Particle &_p,  float xPos, float zPos);
  void createZeroParticle(Particle &_p);
  std::vector<Particle> m_particles;
  ngl::Vec3 m_position={0,0,0};
  float m_spread = 15.0f;
  ngl::Vec3 m_emitDir = {0,20.0f,0};
  int m_maxAlive;
  std::unique_ptr<ngl::AbstractVAO> m_vao;

    float m_rectWidth;    // Width of the rectangle
    float m_rectHeight;   // Height of the rectangle
};

class Camera
{
public :
    Camera(ngl::Vec3 position, ngl::Vec3 direction, ngl::Vec3 orientation):m_position(position), m_direction(direction), m_orientation(orientation) {}

    ngl::Mat4 getViewMatrix() const
    {
        return ngl::lookAt(m_position, m_direction, m_orientation);
    }
    ngl::Vec3 getPosition() const
    {
        return m_position;
    }
    ngl::Vec3 getDirection() const
    {
        return m_direction;
    }
    ngl::Vec3 getOrientation() const
    {
        return m_orientation;
    }
private :
    ngl::Vec3 m_position;
    ngl::Vec3 m_direction;
    ngl::Vec3 m_orientation;
};

class DoubleCone
{
public :
    DoubleCone(float cone_height, float cone_radius) :cone_height(cone_height), cone_radius(cone_radius){}

    std::vector<ngl::Vec3> generateVertices(const Camera &camera) {
        std::vector<ngl::Vec3> vertices;

        // Get camera position and orientation
        ngl::Vec3 camera_position = camera.getPosition();
        ngl::Vec3 camera_direction = camera.getDirection();
        ngl::Vec3 camera_orientation = camera.getOrientation();

        // Calculate the position of the double cone relative to the camera
        ngl::Vec3 cone_position = camera_position + camera_direction * camera_cone_distance;

        // Generate vertices for the upper cone
        for (int angle = 0; angle < 360; angle += 10) {
            float x = cone_radius * cos(angle * M_PI / 180) + cone_position.m_x;
            float y = cone_radius * sin(angle * M_PI / 180) + cone_position.m_y;
            float z = cone_height / 2 + cone_position.m_z;
            vertices.emplace_back(x, y, z);
        }

        // Generate vertices for the lower cone
        for (int angle = 0; angle < 360; angle += 10) {
            float x = cone_radius * cos(angle * M_PI / 180) + cone_position.m_x;
            float y = cone_radius * sin(angle * M_PI / 180) + cone_position.m_y;
            float z = -cone_height / 2 + cone_position.m_z;
            vertices.emplace_back(x, y, z);
        }

        return vertices;
    }
private :
    float cone_height;
    float cone_radius;
    float camera_cone_distance;

};


#endif
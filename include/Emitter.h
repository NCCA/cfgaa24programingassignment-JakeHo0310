#ifndef EMITTER_H_
#define EMITTER_H_

#include <vector>
#include <string_view>
#include "Particle.h"
#include <ngl/Vec3.h>
#include <ngl/AbstractVAO.h>
#include <memory>

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


#endif
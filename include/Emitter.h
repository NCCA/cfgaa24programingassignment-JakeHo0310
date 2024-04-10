#ifndef EMITTER_H_
#define EMITTER_H_

#include <vector>
#include <string_view>
#include "Particle.h"
#include "Vec3.h"

class Emitter
{
public :
  Emitter(int _numParticles, int _maxAlive, float _rectWidth = 1.0f, float _rectHeight = 1.0f);
  void update();
  void render() const;
  void writeToGeo(std::string_view _fname);
private :
  void createDefaultParticle(Particle & _p,float xPos, float zPos);
  void createZeroParticle(Particle &_p,  float _rectWidth, float _rectHeight);
  std::vector<Particle> m_particles;
  Vec3 m_position={0,0,0};
  float m_spread = 15.0f;
  Vec3 m_emitDir = {0,20.0f,0};
  int m_maxAlive;

    float m_rectWidth;    // Width of the rectangle
    float m_rectHeight;   // Height of the rectangle
};


#endif

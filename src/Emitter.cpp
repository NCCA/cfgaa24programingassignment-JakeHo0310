#include "Emitter.h"
#include <ngl/Random.h>
#include <fmt/format.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ngl/Util.h>
#include <ngl/Transformation.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/VAOFactory.h>

Emitter::Emitter(int _numParticles, int _maxAlive)
{
    m_numParticles=_numParticles;
    m_maxAlive=_maxAlive;
    pos.resize(m_numParticles);
    dir.resize(m_numParticles);
    colour.resize(m_numParticles);
    life.resize(m_numParticles);
    //size.resize(m_numParticles);
    isAlive.resize(m_numParticles);
    for(size_t i=0; i<m_numParticles; ++i)
    {
        createZeroParticle(i);
    }



  //m_particles.resize(_numParticles);
  //m_maxAlive=_maxAlive;
  //for(auto &p : m_particles)
  //{
    //createZeroParticle(p);
  //}
  //m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO,GL_POINTS);
    m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_POINTS));
    m_vao->bind();
    m_vao->setData(ngl::MultiBufferVAO::VertexData(0,0)); //positions
    m_vao->setData(ngl::MultiBufferVAO::VertexData(1,0)); //colours
    m_vao->unbind();


}

void Emitter::createZeroParticle(size_t _p)
{
   // pos[_p].set(0.0f,0.0f,0.0f,0.01f);
   // pos[_p].set(0.0f,0.0f,0.0f);
    isAlive[_p] = false;
}

void Emitter::createDefaultParticle(size_t _p)
{
  pos[_p]=m_position + randomVectorOnRectangle();
  //dir[_p]=m_emitDir * ngl::Random::randomPositiveNumber() +randomVectorOnSphere() * m_spread;
  dir[_p].m_y = -2.0;
  colour[_p] = ngl::Vec3(1.0f, 1.0f, 1.0f);
  life[_p] = static_cast<int>(500.0f+ngl::Random::randomPositiveNumber(400));
  pos[_p].m_y= 40.0f;
  pos[_p].m_w= 4.0;
  isAlive[_p] = true;
}


ngl::Vec3 Emitter::randomVectorOnSphere()
{
  auto phi = ngl::Random::randomPositiveNumber(M_PI * 2.0f);
  auto costheta = ngl::Random::randomNumber();
  auto theta = acosf(costheta);
  auto u = ngl::Random::randomPositiveNumber();
  auto r = 1.0f *std::cbrt(u);
  return ngl::Vec3(r*sinf(theta) *cosf(phi),
                   r* sinf(theta) * sinf(phi),
                   r*cosf(theta));
}

FlowField::FlowField(int _width, int _height, int _depth)
        : m_width(_width), m_height(_height), m_depth(_depth)
{
    // Initialize libnoise Perlin modules with appropriate parameters
    m_perlinModuleX.SetOctaveCount(6);
    m_perlinModuleY.SetOctaveCount(6);
    m_perlinModuleZ.SetOctaveCount(6);
}

ngl::Vec3 FlowField::getFlowDirection(float _x, float _y, float _z)
{

    // Scale the input coordinates to adjust the frequency
    _x *= 50.0f;
    _y *= 50.0f;
    _z *= 50.0f;

    // Use libnoise Perlin modules to generate flow direction
    float flowX = m_perlinModuleX.GetValue(_x, _y, _z);
    float flowY = m_perlinModuleY.GetValue(_x, _y, _z);
    float flowZ = m_perlinModuleZ.GetValue(_x, _y, _z);

    // Normalize flow direction vector

    ngl::Vec3 flowDirection(flowX, flowY, flowZ);
    flowDirection += ngl::Vec3(1.0f, 1.0f, 1.0f);
    flowDirection *= 0.5f;
    flowDirection.normalize();

//    std::cout << "FlowX: " << flowX << std::endl;
//    std::cout << "FlowY: " << flowY << std::endl;
//    std::cout << "FlowZ: " << flowZ << std::endl;
    return flowDirection;
}

void Emitter::render() const
{
    //ngl::Mat4 look=ngl::lookAt({0,130,130},{0,0,0},{0,1,0});
    //ngl::Mat4 persp=ngl::perspective(45.0f,1.0,0.1,200);
    //glPointSize(4);
    //ngl::ShaderLib::setUniform("MVP", persp*look);

  glEnable(GL_PROGRAM_POINT_SIZE);
  m_vao->bind();
  m_vao->setData(ngl::AbstractVAO::VertexData(m_numParticles*sizeof(ngl::Vec4),pos[0].m_x));
  m_vao->setVertexAttributePointer(0,4,GL_FLOAT,0,0);

  m_vao->setData(ngl::AbstractVAO::VertexData(m_numParticles*sizeof(ngl::Vec3),colour[0].m_x));
  m_vao->setVertexAttributePointer(1,3,GL_FLOAT,0,0);

  m_vao->setNumIndices(m_numParticles);
  m_vao->draw();
  m_vao->unbind();
  //  ngl::Transformation tx;
//  for(auto p : m_particles)
//  {
//    tx.setPosition(p.pos);
//    tx.setScale(p.size,p.size,p.size);
//    ngl::ShaderLib::setUniform("MVP", persp*look*tx.getMatrix());
//    ngl::ShaderLib::setUniform("Colour",p.colour.m_r,p.colour.m_g,p.colour.m_b,1.0f);
//    ngl::VAOPrimitives::draw(ngl::bunny);//"sphere");
//    //    fmt::print("{} ,{}, {} \n",p.pos.m_x,
//            p.pos.m_y, p.pos.m_z);
//
//
//
//  }
}

void Emitter::update()
{
  //std::cout<<"update\n";
//// dir += gravity * _dt * 0.5f
////  pos += p.dir * _dt
float _dt= 0.3f;
FlowField flowField(50.0f,40.0f,50.0f);
ngl::Vec3 gravity(0,-0.7, 0);
//static int numP =0;
//// choose number to birth
// find first not alive and set as new particle
int numberToBirth=1000+ngl::Random::randomPositiveNumber(3000);

for(int i=0; i<numberToBirth; ++i)
{
    size_t index=0;
    for(auto a: isAlive)
    {
        if (!a)
            break;
        else
            ++index;
    }
    createDefaultParticle(index);
}

    for(size_t p=0;p<m_numParticles; ++p)
    {
        if(isAlive[p])
        {
            // Generate a random value between -0.05 and 0.05 for horizontal change
            //float horizontalVelocity = -1.0f + 8.0f * ngl::Random::randomPositiveNumber();
            ngl::Vec3 currentPosition = ngl::Vec3(pos[p].m_x, pos[p].m_y, pos[p].m_z);

            // Get flow direction at the current position from the flow field
            ngl::Vec3 flowDirection = flowField.getFlowDirection(currentPosition.m_x, currentPosition.m_y, currentPosition.m_z);

            // Update particle direction based on flow direction
            dir[p] = flowDirection * _dt + gravity;
            //dir[p].m_x = horizontalVelocity;
            //dir[p] += gravity * _dt;
            pos[p] += dir[p] * _dt ;
            //pos[p].m_w += 0.05; //change size of particle
            if (--life[p] == 0 )
            {
                createZeroParticle(p);
            }
        }
    }
}


  //for(auto &p : m_particles)
  //{
    //if (p.isAlive == true)
    //{
      //p.dir += gravity * _dt * 0.5;
      //p.pos += p.dir * _dt;
      //p.size += 0.01f;

      //if (--p.life == 0 || p.pos.m_y <= 0.0)
      //{
         //createZeroParticle(p);
      //}
    //}
  //}
//}











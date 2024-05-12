#include "Emitter.h"
#include <ngl/Random.h>
#include <fmt/format.h>
#include <ngl/Util.h>
#include <ngl/Transformation.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/VAOFactory.h>

Emitter::Emitter(int _numParticles, int _maxAlive, int _colourType)
{
    m_numParticles=_numParticles;
    m_maxAlive=_maxAlive;
    m_colourType=_colourType;
    pos.resize(m_numParticles);
    dir.resize(m_numParticles);
    colour.resize(m_numParticles);
    life.resize(m_numParticles);
    isAlive.resize(m_numParticles);
    for(size_t i=0; i<m_numParticles; ++i)
    {
        createZeroParticle(i);
    }
    m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_POINTS));
    m_vao->bind();
    m_vao->setData(ngl::MultiBufferVAO::VertexData(0,0)); //positions
    m_vao->setData(ngl::MultiBufferVAO::VertexData(1,0)); //colours
    m_vao->unbind();
}

void Emitter::createZeroParticle(size_t _p)
{
    isAlive[_p] = false;
}

void Emitter::createDefaultParticle(size_t _p)
{
  pos[_p]=m_position + randomVectorOnRectangle();
  dir[_p].m_y = -2.0;
    life[_p] = static_cast<int>(500.0f+ngl::Random::randomPositiveNumber(750));
  pos[_p].m_y= 40.0f;
  pos[_p].m_w= 5.0f+ngl::Random::randomPositiveNumber(15);
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
    // Initialize Perlin modules with appropriate parameters
    m_perlinModuleX.SetOctaveCount(8); // Increase octave count for X component
    m_perlinModuleX.SetPersistence(0.6); // Adjust persistence for X component
    m_perlinModuleY.SetOctaveCount(8); // Increase octave count for Y component
    m_perlinModuleY.SetPersistence(0.6); // Adjust persistence for Y component
    m_perlinModuleZ.SetOctaveCount(8); // Increase octave count for Z component
    m_perlinModuleZ.SetPersistence(0.6); // Adjust persistence for Z component
}

ngl::Vec3 FlowField::getFlowDirection(float _x, float _y, float _z)
{


    // Use libnoise Perlin modules to generate flow direction
    float flowX = m_perlinModuleX.GetValue(_x, _y, _z);
    float flowY = m_perlinModuleY.GetValue(_x, _y, _z);
    float flowZ = m_perlinModuleZ.GetValue(_x, _y, _z);

    // Scale the input coordinates to adjust the frequency

 //  simple smoothing by averaging neighboring samples
    constexpr int numSamples = 1.2;
    float smoothX = 0.0f;
    float smoothY = 0.0f;
    float smoothZ = 0.0f;

    for (int dx = -numSamples; dx <= numSamples; ++dx)
    {
        for (int dy = -numSamples; dy <= numSamples; ++dy)
        {
            for (int dz = -numSamples; dz <= numSamples; ++dz)
            {
                smoothX += m_perlinModuleX.GetValue(_x + dx, _y + dy, _z + dz);
                smoothY += m_perlinModuleY.GetValue(_x + dx, _y + dy, _z + dz);
                smoothZ += m_perlinModuleZ.GetValue(_x + dx, _y + dy, _z + dz);
            }
        }
    }

    // Normalize the smoothed values
    flowX = smoothX / pow(2 * numSamples + 1, 3);
    flowY = smoothY / pow(2 * numSamples + 1, 3);
    flowZ = smoothZ / pow(2 * numSamples + 1, 3);

    // Normalize flow direction vector
    ngl::Vec3 flowDirection(flowX, flowY, flowZ);
    flowDirection.normalize();
    return flowDirection;
}

void Emitter::render() const
{
  glEnable(GL_PROGRAM_POINT_SIZE);
  m_vao->bind();
  m_vao->setData(ngl::AbstractVAO::VertexData(m_numParticles*sizeof(ngl::Vec4),pos[0].m_x));
  m_vao->setVertexAttributePointer(0,4,GL_FLOAT,0,0);

  m_vao->setData(ngl::AbstractVAO::VertexData(m_numParticles*sizeof(ngl::Vec3),colour[0].m_x));
  m_vao->setVertexAttributePointer(1,3,GL_FLOAT,0,0);

  m_vao->setNumIndices(m_numParticles);
  m_vao->draw();
  m_vao->unbind();
// Calculate background gradient color based on particle positions
    // Interpolate between top and bottom gradient colors based on particle's y-coordinate
//    float gradientFactor;
//    for(size_t p = 0; p < m_numParticles; ++p)
//    {
//        if(isAlive[p])
//        {
//            // Calculate gradient factor based on particle's y-coordinate
//            gradientFactor = (pos[p].m_y + 50.0f) / 100.0f; // Assuming the scene height is 100 units
//
//            // Interpolate between top and bottom gradient colors
//            ngl::Vec4 backgroundGradientColor = m_gradientTopColor * (1.0f - gradientFactor) + m_gradientBottomColor * gradientFactor;
//
//            // Render the background color at the particle's position
//            renderBackgroundAtPosition(backgroundGradientColor, pos[p]);
//
//            // Now render particles or any other scene geometry on top of the background
//            // ...
//        }
//    }
}

void Emitter::update()
{
  //std::cout<<"update\n";
//// dir += gravity * _dt * 0.5f
////  pos += p.dir * _dt
float _dt= 0.3f;
FlowField flowField(75.0f,40.0f,75.0f);
ngl::Vec3 gravity(0,-0.7, 0);
//static int numP =0;
//// choose number to birth
// find first not alive and set as new particle
int numberToBirth=1000+ngl::Random::randomPositiveNumber(1500);

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
    if(index<m_numParticles)
    {
        createDefaultParticle(index);
    }

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
            if(m_colourType == 0)
            {
                colour[p] = ngl::Vec3(0.8f, 0.8f, 1.0f);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            }
            if(m_colourType == 1)
            {
                colour[p] = ngl::Vec3(1.0f,1.0f,1.0f);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            }
            if(m_colourType == 2)
            {
                colour[p] = ngl::Vec3(1.0f,0.0f,0.0f);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            }
            if(m_colourType == 3)
            {
                colour[p] = ngl::Vec3(0.3f+0.7f*ngl::Random::randomPositiveNumber(),
                                      0.3f+0.7f*ngl::Random::randomPositiveNumber(),
                                      0.3f+0.7f*ngl::Random::randomPositiveNumber());
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            }
            if(m_colourType == 4)
            {
                colour[p] = ngl::Vec3(0.0f,0.0f,0.0f);
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            }
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






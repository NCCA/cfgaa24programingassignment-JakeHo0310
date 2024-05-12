#ifndef EMITTER_H_
#define EMITTER_H_

#include <vector>
#include <string_view>
#include <ngl/Vec3.h>
#include <ngl/MultiBufferVAO.h>
#include <memory>
#include <ngl/Util.h>
#include <ngl/Mat4.h>
#include <ngl/Random.h>
#include <algorithm>
#include <noise/noise.h>
#include <QObject>



class Emitter
{
public :
  Emitter(int _numParticles, int _maxAlive, int _colourType);
  void update();
  void render() const;
  //void writeToGeo(std::string_view _fname);
private :
  ngl::Vec3 randomVectorOnSphere();
  void createDefaultParticle(size_t _p);
  void createZeroParticle(size_t _p);
  //void createMetorite(size_t _p);
  void renderBackgroundAtPosition(const ngl::Vec4& color, const ngl::Vec4& position) const;

    size_t m_numParticles;
    int m_colourType;
  ngl::Vec3 m_position={0,0,0};
  ngl::Vec3 m_emitDir = {0,20.0f,0};
  int m_maxAlive;
  std::unique_ptr<ngl::MultiBufferVAO> m_vao;

    std::vector<ngl::Vec4> pos;
    std::vector<ngl::Vec3>  dir;
    std::vector<ngl::Vec3>  colour;
    std::vector<int> life;
    std::vector<int> isAlive;
    ngl::Vec3 windDir;

    float m_rectWidth = 75.0f;    // Width of the rectangle
    float m_rectHeight = 75.0f;   // Height of the rectangle

    // Define gradient colors
    // ngl::Vec4 m_gradientTopColor = ngl::Vec4(0.0f, 0.0f, 1.0f, 1.0f);   // Blue
    // ngl::Vec4 m_gradientBottomColor = ngl::Vec4(0.0f, 0.0f, 0.0f, 1.0f); // Black

    ngl::Vec3 randomVectorOnRectangle() const
    {
        float xPos = ngl::Random::randomNumber() * m_rectWidth - m_rectWidth / 2.0f;
        float zPos = ngl::Random::randomNumber()* m_rectHeight - m_rectHeight / 2.0f;

        return {xPos, 0.0f, zPos};
    }

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




class FlowField
        {
                public:
                FlowField(int _width, int _height, int _depth);
                ngl::Vec3 getFlowDirection(float _x, float _y, float _z); // Get flow direction at a given position

                private:
                // variables for generating the flow field
                noise::module::Perlin m_perlinModuleX;
                noise::module::Perlin m_perlinModuleY;
                noise::module::Perlin m_perlinModuleZ;

                // flow field dimensions
                int m_width;
                int m_height;
                int m_depth;
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

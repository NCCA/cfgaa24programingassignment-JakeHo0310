#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Util.h>
#include <ngl/ShaderLib.h>
#include <iostream>
#include <ngl/Texture.h>
#include <ngl/Obj.h>


NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Snow Particles");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f,float(_w)/_h,0.1f,200);
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			   // Black Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  m_emitter=std::make_unique<Emitter>(500,400,0);
  ngl::ShaderLib::loadShader("ParticleShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");
  ngl::ShaderLib::use("ParticleShader");
  // Import snow texture
  ngl::Texture t("texture/p2.png");
  m_ptexture = t.setTextureGL();

    // passing in camera parameters from the camera class
    ngl::Vec3 camera_position(5.0f, 0.0f, 5.0f);
    ngl::Vec3 camera_direction(0.0f, 0.0f, 0.0f);
    ngl::Vec3 camera_orientation(0.0f, 1.0f, 0.0f);
    Camera camera(camera_position, camera_direction, camera_orientation);
    m_view = camera.getViewMatrix();

    ngl::Obj tree("lp_xmas_tree_w_presents.obj");
    tree.createVAO();
    ngl::VAOPrimitives::loadObj("tree","lp_xmas_tree_w_presents.obj");
    ngl::ShaderLib::loadShader("TreeShader","shaders/ParticleVertex.glsl","shaders/ParticleFragment.glsl");
    ngl::ShaderLib::use("TreeShader");

    //float cone_height = 20.0f; // Adjust height as needed
    //float cone_radius = 10.0f; // Adjust radius as needed
    //DoubleCone doubleCone(cone_height, cone_radius);
       // Generate vertices for the double cone
       //std::vector<ngl::Vec3> coneVertices = doubleCone.generateVertices(camera);

  //m_view = ngl::lookAt({0,100,100},{0,0,0},{0,1,0});
  startTimer(10);
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
  m_emitter->update();
  update();
}

void NGLScene::loadMatricesToShader()
{
    ngl::Mat4 MVP =m_project*m_view * m_mouseGlobalTX;
    ngl::ShaderLib::setUniform("MVP",MVP);
}
void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);

    auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);
  auto mouseRotation = rotX * rotY;
  mouseRotation.m_m[3][0] = m_modelPos.m_x;
  mouseRotation.m_m[3][0] = m_modelPos.m_y;
  mouseRotation.m_m[3][0] = m_modelPos.m_z;

  loadMatricesToShader();
  ngl::VAOPrimitives::draw("tree");
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*mouseRotation);
  glPointSize(2);
  glBindTexture(GL_TEXTURE_2D, m_ptexture);
  m_emitter->render();
  //ngl::VAOPrimitives::draw("sphere");

}

//----------------------------------------------------------------------------------------------------------------------
//void renderBackgroundAtPosition(const ngl::Vec4& color, const ngl::Vec4& position) const
//{
//    // Enable depth testing
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);
//
//    // Disable depth writing
//    glDepthMask(GL_FALSE);
//
//    // Set up the shader program for rendering
//    // Assuming you have a shader program named "backgroundShader"
//    ngl::ShaderLib::use("backgroundShader");
//
//    // Set uniform for background color
//    ngl::ShaderLib::setUniform("backgroundColor", color);
//
//    // Set uniform for position
//    ngl::ShaderLib::setUniform("backgroundPosition", position);
//
//    // Render a fullscreen quad using modern OpenGL
//
//    // Define vertex positions for a fullscreen quad
//    static const GLfloat quadVertices[] = {
//            -1.0f, -1.0f, 0.0f, // Bottom-left corner
//            1.0f, -1.0f, 0.0f, // Bottom-right corner
//            1.0f,  1.0f, 0.0f, // Top-right corner
//            -1.0f,  1.0f, 0.0f  // Top-left corner
//    };
//
//    // Bind and enable VAO for rendering
//    GLuint vao;
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);
//
//    // Generate vertex buffer object (VBO) for vertex positions
//    GLuint vbo;
//    glGenBuffers(1, &vbo);
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
//
//    // Set up vertex attribute pointers
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//
//    // Draw the quad
//    glDrawArrays(GL_QUADS, 0, 4);
//
//    // Clean up
//    glDeleteBuffers(1, &vbo);
//    glDeleteVertexArrays(1, &vao);
//
//    // Clean up
//    glDepthMask(GL_TRUE);
//    glDisable(GL_DEPTH_TEST);
//}

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
  break;

  case Qt::Key_1 :
      m_emitter=std::make_unique<Emitter>(500,400,0);
  break;

  case Qt::Key_2 :
      m_emitter=std::make_unique<Emitter>(500,400,1);
  break;

  case Qt::Key_3 :
      m_emitter=std::make_unique<Emitter>(500,400,2);
  break;

  case Qt::Key_4 :
      m_emitter=std::make_unique<Emitter>(500,400,3);
  break;

  case Qt::Key_5 :
      m_emitter=std::make_unique<Emitter>(500,400,4);
  break;
  
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

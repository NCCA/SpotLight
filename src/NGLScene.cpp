#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Random.h>


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  m_animate=true;

  setTitle("ngl::SpotLight demo");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(QResizeEvent *_event)
{
  m_width=_event->size().width()*devicePixelRatio();
  m_height=_event->size().height()*devicePixelRatio();
  // now set the camera size values as the screen size has changed
  m_cam.setShape(45.0f,(float)width()/height(),0.05f,350.0f);
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_cam.setShape(45.0f,(float)_w/_h,0.05f,350.0f);
  m_width=_w*devicePixelRatio();
  m_height=_h*devicePixelRatio();
}

void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0,4,10);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_cam.set(from,to,up);
  // set the shape using FOV 45 Aspect Ratio based on Width and Height
  // The final two are near and far clipping planes of 0.5 and 10
  m_cam.setShape(45,(float)720.0/576.0,0.5,150);
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // we are creating a shader called Spotlight
  shader->createShaderProgram("Spotlight");
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader("SpotlightVertex",ngl::ShaderType::VERTEX);
  shader->attachShader("SpotlightFragment",ngl::ShaderType::FRAGMENT);
  // attach the source
  shader->loadShaderSource("SpotlightVertex","shaders/SpotlightVert.glsl");
  shader->loadShaderSource("SpotlightFragment","shaders/SpotlightFrag.glsl");
  // compile the shaders
  shader->compileShader("SpotlightVertex");
  shader->compileShader("SpotlightFragment");
  // add them to the program
  shader->attachShaderToProgram("Spotlight","SpotlightVertex");
  shader->attachShaderToProgram("Spotlight","SpotlightFragment");

  // now we have associated this data we can link the shader
  shader->linkProgramObject("Spotlight");
  // and make it active ready to load values
  (*shader)["Spotlight"]->use();
  shader->setShaderParam1f("Normalize",1);
  glEnable(GL_DEPTH_TEST); // for removal of hidden surfaces
  // the shader will use the currently active material and light0 so set them
  ngl::Material m(ngl::STDMAT::GOLD);
  // load our material values to the shader into the structure material (see Vertex shader)
  m.loadToShader("material");
  ngl::VAOPrimitives *prim =  ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",30,30,180,180,ngl::Vec3(0,1,0));
  // create the lights
  createLights();
  // start some timers for the lights
  m_lightChangeTimer=startTimer(30);
  m_lightParamChangeTimer=startTimer(10);
}


void NGLScene::loadMatricesToShader()
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // these are used to hold our pre-computed matrix values
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  // our model view matrix
  MV=m_transform.getMatrix()
     *m_mouseGlobalTX*m_cam.getViewMatrix() ;
  // our model view ;rojection matrix
  MVP=MV*m_cam.getProjectionMatrix();
  // calculate normal matrix by getting the top 3x3 of the MV
  normalMatrix=MV;
  // then calculate the inverse
  normalMatrix.inverse();
  // now load them to the current shader
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_width,m_height);
  // Rotation based on the mouse position for our global
  // transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;
  // grab an instance of the primitives for drawing
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // Rotation based on the mouse position for our global
  // transform
  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Spotlight"]->use();
  // load the values to the shader and draw the plane
  m_transform.reset();
  loadMatricesToShader();
  prim->draw("plane");

  for (int z=-8; z<8; z+=2)
  {
    for (int x=-8; x<8; x+=2)
    {
      // set the teapot position and roation
      m_transform.setRotation(0,(x*z)*20,0);
      m_transform.setPosition(x,0.49,z);
      // load the current transform to the shader
      loadMatricesToShader();
      prim->draw("teapot");
   }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    update();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();

   }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Space : changeSpotParams(); break;
  case Qt::Key_A : toggleAnimation(); break;

  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}




//----------------------------------------------------------------------------------------------------------------------
void NGLScene::createLights()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  (*shader)["Spotlight"]->use();
  ngl::Real x=0;
  ngl::Real z=0;
  // loop an set the spot values
  ngl::Random *rand=ngl::Random::instance();
  rand->setSeed(time(NULL));
  for(int i=0; i<8; ++i)
  {
    // create a random position for the spot
    x=rand->randomNumber(3);
    z=rand->randomNumber(3);
    ngl::Vec3 pos(x,3,z);
    // shine down
    ngl::Vec3 dir(0,-1,0);
    // now load this to the spot data class
    SpotData d;
    d.m_aimCenter=rand->getRandomPoint(x*4,0,z*4);
    d.m_radiusX=rand->randomNumber(2)+0.5;
    d.m_radiusZ=rand->randomNumber(2)+0.5;
    d.m_startColour =rand->getRandomColour();
    d.m_endColour =rand->getRandomColour();
    d.m_startColour.clamp(0.4,1.0);
    d.m_endColour.clamp(0.4,1.0);

    d.m_mix=0.0;
    d.m_time=rand->randomPositiveNumber(4)+0.6;
    m_spotData.push_back(d);
    m_spots.push_back(ngl::SpotLight(pos,dir,d.m_startColour));
  }
  // get the inverse view matrix and load this to the light shader
  // we use this as we do the light calculations in eye space in the shader
  ngl::Mat4 iv=m_cam.getViewMatrix();
  iv.transpose();

  for(int i=0; i<8; ++i)
  {
    // set the spot values
    m_spots[i].setSpecColour(ngl::Colour(1,1,1,1));
    m_spots[i].setCutoff(rand->randomPositiveNumber(24)+0.5);
    m_spots[i].setInnerCutoff(rand->randomPositiveNumber(12)+0.1);
    m_spots[i].setExponent(rand->randomPositiveNumber(2)+1);
    m_spots[i].setAttenuation(1.0,0.0,0.0);
    m_spots[i].enable();
    m_spots[i].setTransform(iv);

    QString light=QString("light[%1]").arg(i);
    m_spots[i].loadToShader(light.toStdString());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::changeSpotParams()
{
  ngl::Random *rand=ngl::Random::instance();
  rand->setSeed(time(NULL));
  // change the spot positions

  for(int i=0; i<8; ++i)
  {
    float x=rand->randomNumber(3);
    float z=rand->randomNumber(3);
    m_spots[i].setPosition(ngl::Vec3(x,4,z));
    m_spots[i].setCutoff(rand->randomPositiveNumber(24)+0.5);
    m_spots[i].setInnerCutoff(rand->randomPositiveNumber(12)+0.1);

    // now we update the spot values
    m_spotData[i].m_aimCenter=rand->getRandomPoint(x*4,0,z*4);
    m_spotData[i].m_radiusX=rand->randomNumber(2)+0.5;
    m_spotData[i].m_radiusZ=rand->randomNumber(2)+0.5;
    m_spotData[i].m_time=rand->randomPositiveNumber(4)+0.6;
    m_spotData[i].m_startColour =rand->getRandomColour();
    m_spotData[i].m_endColour =rand->getRandomColour();
    m_spotData[i].m_mix=0.0;
  }
}
void NGLScene::timerEvent(QTimerEvent *_event )
{
  if(_event->timerId() == m_lightChangeTimer)
  {
   if (m_animate !=true)
   {
    return;
   }
   static float time=0.0;
   // create pointers to the begining and end of the spot vector
    int size=m_spots.size();
    for(int i=0; i<size; ++i)
    {
      float pointOnCircleX= cos(time+m_spotData[i].m_time)*m_spotData[i].m_radiusX;
      float pointOnCircleZ= sin(time+m_spotData[i].m_time)*m_spotData[i].m_radiusZ;
      // get the points value we need
      ngl::Vec4 p(m_spotData[i].m_aimCenter[0]+pointOnCircleX,0,m_spotData[i].m_aimCenter[2]+pointOnCircleZ,0.0);
      m_spots[i].setColour(trigInterp(m_spotData[i].m_startColour,m_spotData[i].m_endColour,m_spotData[i].m_mix ));
      // do the colour mixing
      m_spotData[i].m_mix+=0.05;
      if (m_spotData[i].m_mix >=1.0)
      {
        m_spotData[i].m_mix=0.0;
      }
      // set spot aim
      m_spots[i].aim(p);
      // now load to shader
      QString light=QString("light[%1]").arg(i);
      m_spots[i].loadToShader(light.toStdString());
    }
    time+=0.2;
    update();

  }
}


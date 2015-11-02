#ifndef NGLSCENE_H__
#define NGLSCENE_H__
#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/SpotLight.h>
#include <ngl/Transformation.h>
#include <ngl/Text.h>
#include <QOpenGLWindow>
class SpotData
{
public :
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the time offset for the spot
  //----------------------------------------------------------------------------------------------------------------------
  float m_time;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief x radius of ellipse
  //----------------------------------------------------------------------------------------------------------------------
  float m_radiusX;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief z radius of the ellipse
  //----------------------------------------------------------------------------------------------------------------------
  float m_radiusZ;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief current mix factor of the light for Lerp of colours
  //----------------------------------------------------------------------------------------------------------------------
  float m_mix;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief aim center of the spot where the ellipse is centered
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_aimCenter;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief start colour of light interpolated to end colour
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Colour m_startColour;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief end colour of the light
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Colour m_endColour;
};


//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we resize
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(QResizeEvent *_event);

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a method to change the spot light parameters
    //----------------------------------------------------------------------------------------------------------------------
    void changeSpotParams();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief toggle the Animation of the lights called from main window
    //----------------------------------------------------------------------------------------------------------------------
    inline void toggleAnimation(){m_animate^=true;}

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the x rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinXFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the y rotation mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_spinYFace;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_rotate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the Right mouse button is pressed when dragging
    //----------------------------------------------------------------------------------------------------------------------
    bool m_translate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value
    //----------------------------------------------------------------------------------------------------------------------
    int m_origY;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous x mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origXPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the previous y mouse value for Position changes
    //----------------------------------------------------------------------------------------------------------------------
    int m_origYPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief window width
    //----------------------------------------------------------------------------------------------------------------------
    int m_width;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief window height
    //----------------------------------------------------------------------------------------------------------------------
    int m_height;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief used to store the global mouse transforms
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_mouseGlobalTX;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Our Camera
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Camera m_cam;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the model position for mouse movement
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_modelPos;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief timer to change the light
    //----------------------------------------------------------------------------------------------------------------------
    int m_lightChangeTimer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief light parameter change timer
    //----------------------------------------------------------------------------------------------------------------------
    int m_lightParamChangeTimer;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our transform stack for drawing the teapots
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Transformation m_transform;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dynamic array of spotlights for drawing
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <ngl::SpotLight> m_spots;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a class to contain the spot attributes
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <SpotData> m_spotData;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if animation is active or not
    //----------------------------------------------------------------------------------------------------------------------
    bool m_animate;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to load transform matrices to the shader
    //----------------------------------------------------------------------------------------------------------------------
    void loadMatricesToShader();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event );

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief create the initial lights
    //----------------------------------------------------------------------------------------------------------------------
    void createLights();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the timer event triggered from the timers
    /// @param _even the event of the timer triggered by Qt
    //----------------------------------------------------------------------------------------------------------------------
    void timerEvent( QTimerEvent *_event );


};



#endif

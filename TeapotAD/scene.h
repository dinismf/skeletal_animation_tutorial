#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <GLFW/glfw3.h>
#include "QuatCamera.h"

class Scene
{
public:
	Scene() : m_animate(true) { }
	
    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initScene(QuatCamera camera) = 0;

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update(long long f_startTime, float f_Interval) = 0;

    /**
      Draw your scene.
      */
    virtual void render(QuatCamera camera) = 0;

    /**
      Called when screen is resized
      */
    virtual void resize(QuatCamera camera,int, int) = 0;
    
    void animate( bool value ) { m_animate = value; }
    bool animating() { return m_animate; }
    
protected:
	bool m_animate;
};

#endif // SCENE_H

//
//  MainWindow.h
//  jSpike
//
//  Created by Stuart Layton on 12/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef jSpike_MainWindow_h
#define jSpike_MainWindow_h
#include "JuceHeader.h"
#include "ArteOpenGLComponent.cpp"

//==============================================================================
class MainWindow  : public DocumentWindow
{
public:
    //==============================================================================
    MainWindow();
    ~MainWindow();
    
    //==============================================================================
    // called when the close button is pressed or esc is pushed
    void closeButtonPressed();
    
    // the command manager object used to dispatch command events
    ApplicationCommandManager commandManager;
    
private:
    ScopedPointer<Component> taskbarIcon;
    ArteOpenGLComponent *oglc1;
    ArteOpenGLComponent *oglc2;
    ArteOpenGLComponent *oglc3;
    ArteOpenGLComponent *oglc4;
    ArteOpenGLComponent *oglc5;
    ArteOpenGLComponent *oglc6;

    
    
};



#endif

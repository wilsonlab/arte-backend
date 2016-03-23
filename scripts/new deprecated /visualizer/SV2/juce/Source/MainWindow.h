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
#include "vector"


//==============================================================================
class MainWindow  : public DocumentWindow,
                    public KeyListener//,
//                    public ButtonListener
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

    std::vector<ArteOpenGLComponent*> tPlots;
    std::vector<Button*> cfgButtons;
    void initializeComponents();
    bool keyPressed (const KeyPress &key, Component *originatingComponent);
    void zoomAllPlots(int zoomval);
    void panAllPlots(int zoomval);
    
//    void buttonClicked(juce::Button *btn);
    
};



#endif

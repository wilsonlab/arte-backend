//
//  MainWindow.cpp
//  jSpike
//
//  Created by Stuart Layton on 12/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "MainWindow.h"

MainWindow::MainWindow()
: DocumentWindow ("jSpike",
                  Colours::azure,
                  DocumentWindow::allButtons,
                  true)
{
    setUsingNativeTitleBar(true);
    setResizable (true, false); // resizability is a property of ResizableWindow
    setResizeLimits (400, 300, 8192, 8192);

    setVisible (true);
    oglc1 = new ArteOpenGLComponent("6300"); 
    oglc2 = new ArteOpenGLComponent("6301"); 
    oglc3 = new ArteOpenGLComponent("6302"); 
    oglc4 = new ArteOpenGLComponent("6303"); 
    oglc5 = new ArteOpenGLComponent("6304"); 
    oglc6 = new ArteOpenGLComponent("6305"); 

    //    oglc->initialize("6300");

    addAndMakeVisible(oglc1);
    addAndMakeVisible(oglc2);
    addAndMakeVisible(oglc3);
    addAndMakeVisible(oglc4);
    addAndMakeVisible(oglc5);
    addAndMakeVisible(oglc6);
    
    oglc1->setBounds(30,  50, 300, 300);
    oglc2->setBounds(345, 50, 300, 300);
    oglc3->setBounds(660, 50, 300, 300);
    oglc4->setBounds(30,  380, 300, 300);
    oglc5->setBounds(345, 380, 300, 300);
    oglc6->setBounds(660, 380, 300, 300);
    
}

MainWindow::~MainWindow()
{
    deleteAllChildren();
}

void MainWindow::closeButtonPressed()
{
    // The correct thing to do when you want the app to quit is to call the
    // JUCEApplication::systemRequestedQuit() method.
    
    // That means that requests to quit that come from your own UI, or from other
    // OS-specific sources (e.g. the dock menu on the mac) all get handled in the
    // same way.
    
    JUCEApplication::getInstance()->systemRequestedQuit();
}
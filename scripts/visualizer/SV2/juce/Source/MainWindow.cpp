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
                  Colours::darkgrey,
                  DocumentWindow::allButtons,
                  true)

{
    setUsingNativeTitleBar(true);
    setResizable (true, false); // resizability is a property of ResizableWindow
    setResizeLimits (400, 300, 8192, 8192);

    setVisible (true);
    
    initializeComponents();
        grabKeyboardFocus();
    addKeyListener(this);

}

MainWindow::~MainWindow()
{
    deleteAllChildren();
}

void MainWindow::initializeComponents(){
    
    char *ports[] = {"6300", "6301", "6302", "6303", "6304", "6305"};
    int nPlots = 6;
    int x = 30;
    int dx = 320;
    int y = 20;
    int dy = 310;
    int width = 300;
    int height = 300;
    // Initialize the plots
    for (int i=0; i<nPlots; i++){
        std::cout<<"Creating plot:"<<i<<" using port:"<<ports[i]<<" ";
        std::cout<<"At location:"<<x + (dx * (i%3))<<","<< y + (dy * (i/3))<<std::endl;
        
        ArteOpenGLComponent* tmp = new ArteOpenGLComponent(ports[i]);
        addAndMakeVisible(tmp);
        tmp->setBounds(x + (dx * (i%3)), y + (dy * (i/3)), width, height);       
        
        tPlots.push_back(tmp);

    }
    
    //Initialize the cfg buttons
    for (int i=0; i<nPlots; i++){
        Button* tmp = new TextButton("");
        addAndMakeVisible(tmp);
        tmp->setBounds(x + (dx * (i%3)) + width, y + (dy * (i/3)), 15, 15);
        tmp->setColour(TextButton::buttonColourId, Colours::white);
        tmp->setConnectedEdges(Button::ConnectedOnLeft);
        
        cfgButtons.push_back(tmp);
    }
    

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

bool MainWindow::keyPressed (const KeyPress &key, Component *originatingComponent){
    int size = tPlots.size();
    for (int i=0; i<size; i++)
        tPlots[i]->processKeyPress(key);

    return true;
}




//void MainWindow::buttonClicked(Button *button){
//    std::cout<<"Btn clicked"<<std::endl;
//}


//
//  AppDelegate.m
//  SummitEngine
//
//  Created by Mes on 20/11/2018.
//  Copyright © 2018 Kubovcik. All rights reserved.
//

#import "AppDelegate.h"
#import <AppKit/Appkit.h>

#include <Engine/Engine.h>
#include <PAL/FileSystem/File.h>


#include "SummitDemo.h"

@interface AppDelegate ()
{
    std::unique_ptr<Demo::SummitDemo> demo;
}
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {    
    Summit::EngineServiceLocator::Provide(Summit::CreateEngineService());
    
    auto& engine = *Summit::EngineServiceLocator::Service();
    engine.Initialize();
    
    demo = std::make_unique<Demo::SummitDemo>(engine);
    demo->PushToEngine(engine);
    
    engine.Run();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    
    NSLog(@"Application will terminate!");
}


@end

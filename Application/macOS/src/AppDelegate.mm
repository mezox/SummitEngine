//
//  AppDelegate.m
//  SummitEngine
//
//  Created by Mes on 20/11/2018.
//  Copyright © 2018 Kubovcik. All rights reserved.
//

#import "AppDelegate.h"
#import <AppKit/Appkit.h>

#include <Logging/LoggingService.h>
#include <Engine/Engine.h>
#include "SummitDemo.h"

#ifdef LOG_MODULE_ID
#   undef LOG_MODULE_ID
#   define LOG_MODULE_ID LOG_MODULE_4BYTE('D','E','M','O')
#endif

#ifdef LOGGER_ID
#   undef LOGGER_ID
#   define LOGGER_ID "Application"
#endif

@interface AppDelegate ()
{
    std::unique_ptr<Demo::SummitDemo> demo;
}
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {    
    Summit::EngineService::Provide(Summit::CreateEngineService());
    
    auto& engine = Summit::EngineService::Get();
    engine.Initialize();
    
    Logging::LoggingServiceLocator::Service().AddLogger(std::make_unique<Logging::Logger>(LOGGER_ID));
    LOG(Information) << "SummitApp did finish launching!";
    
    demo = std::make_unique<Demo::SummitDemo>(engine);
    demo->PushToEngine(engine);
    
    engine.Run();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    Summit::EngineService::Get().DeInitialize();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application
{
    return YES;
}

@end

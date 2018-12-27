//
//  AppDelegate.m
//  SummitEngine
//
//  Created by Mes on 20/11/2018.
//  Copyright © 2018 Kubovcik. All rights reserved.
//

#import "AppDelegate.h"
#import <AppKit/Appkit.h>

#include <Logging/Logger.h>
#include <Logging/LoggingService.h>

#include <Engine/Engine.h>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#ifdef LOGGER_ID
#undef LOGGER_ID
#define LOGGER_ID "Application"
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

@interface AppDelegate ()
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    Engine::EngineServiceLocator::Provide(Engine::CreateEngineService());
    
    auto appLogger = std::make_unique<Logging::Logger>(LOGGER_ID);
    Logging::LoggingServiceLocator::Service()->AddLogger(std::move(appLogger));
    //LOG_INFORMATION << "SummitApp did finish launching!";
    
    Engine::EngineServiceLocator::Service()->Initialize();
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    
    NSLog(@"Application will terminate!");
}


@end

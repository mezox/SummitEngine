//
//  AppDelegate.m
//  SummitEngine
//
//  Created by Mes on 20/11/2018.
//  Copyright © 2018 Kubovcik. All rights reserved.
//

#import "AppDelegate.h"
#import <AppKit/Appkit.h>

#import "SummitWindow.h"
#import "SummitView.h"

#include <Logging/Logger.h>
#include <Logging/LoggingService.h>
#include <Logging/ConsoleWriter.h>

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
{
    id windowDelegate;
    id window;
    id view;
}
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    Engine::EngineServiceLocator::Provide(Engine::CreateEngineService());
    
    auto consoleWriterService = std::make_unique<Logging::ConsoleWriter>("", "");
    auto appLogger = std::make_unique<Logging::Logger>(LOGGER_ID);
    appLogger->AddWriter(std::move(consoleWriterService));
    Logging::LoggingServiceLocator::Service()->AddLogger(std::move(appLogger));
    LOG_INFORMATION("SummitApp did finish launching!")
    
    windowDelegate = [[SummitWindowDelegate alloc] init];
    
    NSRect contentRect = NSMakeRect(0, 0, 1280, 720);
    window = [[SummitWindow alloc] initWithContentRect:contentRect styleMask:NSWindowStyleMaskTitled
                         backing:NSBackingStoreBuffered
                         defer:NO];
    
    view = [[SummitRenderView alloc] init];
    
    [window setContentView:view];
    [window makeFirstResponder:view];
    [window setTitle:[NSString stringWithUTF8String:"SummitEngine"]];
    [window setDelegate:windowDelegate];
    [window setAcceptsMouseMovedEvents: YES];
    
    [window orderFront:nil];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    
    NSLog(@"Application will terminate!");
}


@end

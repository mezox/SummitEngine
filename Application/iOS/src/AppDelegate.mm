//
//  AppDelegate.m
//  example-app
//
//  Created by Alexander Widerberg on 2017-02-03.
//  Copyright © 2017 example. All rights reserved.
//

#import "AppDelegate.h"

#include <Logging/Logger.h>

#include <Engine/Engine.h>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
//    auto consoleWriterService = std::make_unique<Logging::ConsoleWriter>("", "");
//    Logging::Logger::getLogger().AddWriter(std::move(consoleWriterService));
    
    Engine::EngineServiceLocator::Provide(Engine::CreateEngineService());

    //Logging::LoggingServiceLocator::Service()->AddLogger(std::move(appLogger));
    //LOG_INFORMATION << "SummitApp did finish launching!";
    
    Engine::EngineServiceLocator::Service()->Initialize();
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    //LOG_INFORMATION("Application become active")
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end

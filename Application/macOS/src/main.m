//
//  main.m
//  SummitEngine
//
//  Created by Mes on 20/11/2018.
//  Copyright © 2018 Kubovcik. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    [NSApplication sharedApplication];
    [NSApp setDelegate: [AppDelegate new]];
    
    return NSApplicationMain(argc, argv);
}

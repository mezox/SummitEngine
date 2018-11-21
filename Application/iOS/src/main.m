//
//  main.m
//  example-app
//
//  Created by Alexander Widerberg on 2017-02-03.
//  Copyright © 2017 example. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[])
{
    @autoreleasepool
    {
        int res = 0;
        @try
        {
            res = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
        }
        @catch (NSException * e) {
            NSLog(@"Exception: %@", e);
        }
        @finally {
            NSLog(@"finally");
        }
    }
}

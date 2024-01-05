//
//  main.m
//  mshared
//
//  Created by Jithin Renji on 1/5/24.
//

#include "mshared.hpp"

#import <Foundation/Foundation.h>

int main(int argc, const char *argv[]) {
  int err = -1;
  @autoreleasepool {
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *urls = [fm URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];

    if ([urls count] < 1) {
      return err;
    }

    NSURL *appSupportDir = [urls objectAtIndex:0];
    NSURL *appDir = [appSupportDir URLByAppendingPathComponent:@"MShare"];

    NSLog(@"%@", appDir.path);

    NSString *appDirPath = appDir.path;
    err = mshare_start([appDirPath UTF8String]);
  }

  return err;
}

#import <Cocoa/Cocoa.h>

void MakeApplicationActive(void)
{
  [NSApp activateIgnoringOtherApps:YES];
}

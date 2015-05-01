#include "ResourcePath.h"
#import <Cocoa/Cocoa.h>

std::string getResourcePath(std::string name, std::string fileEnding) {
    NSString *nameString = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
    NSString *endingString = [NSString stringWithCString:fileEnding.c_str() encoding:NSUTF8StringEncoding];
    NSString *resourcePath = [[NSBundle mainBundle] pathForResource:nameString ofType:endingString];
    return std::string([resourcePath cStringUsingEncoding:NSUTF8StringEncoding]);
}

#import <Foundation/Foundation.h>

bool osx_get_resource_path( char * buffer, int size )
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init] ;

    NSBundle * bundle = [NSBundle mainBundle] ;
    NSString * path = [bundle resourcePath] ;

    bool success = ( [path getFileSystemRepresentation: buffer maxLength: size] == YES ) ;

    [pool drain] ;
    return success ;
}


bool osx_get_app_support_path( char * buffer, int size )
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init] ;

    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *path = [paths objectAtIndex:0];

    bool success = ( [path getFileSystemRepresentation:buffer maxLength: size] == YES );

    [pool drain] ;
    return success;
}



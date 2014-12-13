#import <Foundation/Foundation.h>

bool osx_get_resource_path( char * buffer, int size )
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init] ;
    bool result = false ;

        NSBundle * bundle = [NSBundle mainBundle] ;
        NSString * ns_string = [bundle resourcePath] ;
        result = ( [ns_string getFileSystemRepresentation: buffer maxLength: size] == YES ) ;

    [pool drain] ;
    return result ;
}


bool osx_get_app_support_path( char * buffer, int size )
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init] ;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory ,NSUserDomainMask, YES);
    NSString *path = [paths objectAtIndex:0];


    bool success = ( [path getFileSystemRepresentation:buffer maxLength: size] == YES );

    [pool drain] ;
    
    return success;
}



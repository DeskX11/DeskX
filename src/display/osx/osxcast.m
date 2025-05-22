
#import <Foundation/Foundation.h>
#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <AVFoundation/AVFoundation.h>
#import <display/osx/osxcast.h>

struct ctrl {
	dispatch_semaphore_t sig;
	SCStream *stream;
	void *buff;
	CGSize res;
}	*ptr = NULL;

@interface ScreenStream: NSObject <SCStreamOutput>
@end

@implementation ScreenStream
- (void)stream:(SCStream *)stream didOutputSampleBuffer:(CMSampleBufferRef)buff ofType:(SCStreamOutputType)type {
	NSLog(@"buffer received!");
	dispatch_semaphore_wait(ptr->sig, DISPATCH_TIME_FOREVER);
}
@end

void *
osxcast_init(int *status) {
	if (@available(macOS 11.0, *)) {
		if (!CGPreflightScreenCaptureAccess()) {
			*status = 0;
			return NULL;
		}
	}

	dispatch_semaphore_t sig = dispatch_semaphore_create(0);
	__block SCDisplay *disp = NULL;
	NSArray *empty = @[];
	ptr = (struct ctrl *)malloc(sizeof(struct ctrl));
	DIE(!ptr);

	[SCShareableContent getShareableContentWithCompletionHandler:^(SCShareableContent * _Nullable content, NSError * _Nullable error) {
		disp = content.displays[0];
		dispatch_semaphore_signal(sig);
	}];
	dispatch_semaphore_wait(sig, DISPATCH_TIME_FOREVER);

	SCContentFilter *filter = [[SCContentFilter alloc] initWithDisplay:disp excludingWindows:empty];
	SCStreamConfiguration *config = [[SCStreamConfiguration alloc] init];
	ScreenStream *scr = [[ScreenStream alloc] init];
	DIE(!config || !filter || !scr);

	ptr->res = CGDisplayBounds(CGMainDisplayID()).size;
	ptr->sig = dispatch_semaphore_create(0);
	config.pixelFormat = kCVPixelFormatType_32BGRA;
	config.colorSpaceName = kCGColorSpaceSRGB;
	config.width  = ptr->res.width;
	config.height = ptr->res.height;
	config.showsCursor = false;
	config.queueDepth = 8;

	ptr->stream = [[SCStream alloc] initWithFilter:filter configuration:config delegate:nil];
	DIE(!ptr->stream);

	if (![ptr->stream addStreamOutput:scr type:SCStreamOutputTypeScreen sampleHandlerQueue:nil error:nil]) {
		*status = 1;
		return NULL;
	}
	[ptr->stream startCaptureWithCompletionHandler:nil];
	return ptr;
}

void *
oscast_iterate(void *val) {
	struct ctrl *ptr = (struct ctrl *)val;

	ptr->buff = NULL;
	dispatch_semaphore_signal(ptr->sig);
	while (!ptr->buff) { NSLog(@"AAAAAAA"); }
	return ptr->buff;
}

void
osxcast_close(void *ptr) {

}

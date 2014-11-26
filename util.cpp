/*
	Utility function reside here.
*/
#include "util.h"

void reset_buffer(char *buf, int size)
{
	memset(buf, '\0', size);
}

int get_screen_size(int *width, int *height)
{

	int num_sizes;
	Rotation original_rotation;

	Display *display = XOpenDisplay(NULL);
	Window root = RootWindow(display, 0);
	XRRScreenSize *xrrs = XRRSizes(display, 0, &num_sizes);
	XRRScreenConfiguration *conf = XRRGetScreenInfo(display, root);
	SizeID original_size_id = XRRConfigCurrentConfiguration(conf, &original_rotation);

	*width = xrrs[original_size_id].width;
	*height = xrrs[original_size_id].height;

	XCloseDisplay(display);
	return 0;
}

/*
	Why is this needed?
	json objects from server come like this:
		{"coord_y":1090,"coord_x":556,"mtype":"ACTION"}{"coord_y":1035,"coord_x":579,"mtype":"ACTION"}
	This function breaks this string into balanced json string objects.
	Somehow btoutputstream.flush() is not working on Android,
	hence the need for this.

	It assumes:
	1. length of json string per object is <= 512.

	@return
		# of objects parsed
*/
stack<int> si;
int defragment_json(const char *buf, char objects[10][1024]) 
{
	int i, j, k, l=0, len = strlen(buf);
	bool isnew = true;

	for( i = 0; i < len; i++ ) {

		if ( buf[i] == '{' ) {
			si.push(i);
			if ( isnew ) j = i;
		} else if ( buf[i] == '}' ) {
			si.pop();
			if ( si.empty() ) {
				k = i;
				if ( l < 10 )
					strncpy( objects[l++], &buf[j], k-j+1 );
			}
		}
		isnew = si.empty();
	}

	return l;
}
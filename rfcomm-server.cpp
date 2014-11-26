#include <libnotify/notify.h>
#include <libnotify/notification.h>

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
/*
	Keep rapidjson files above X11 headers
	to successfully compile.
*/
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "util.h"
#include "registersdp.h"

extern "C" {
#include <xdo.h>	
}

#define BUFFER_SIZE 1024

using namespace rapidjson;
using namespace std;

xdo_t *xdo_instance;
NotifyNotification *nn = notify_notification_new("This is summary",
												 "Bluetooth mouse is connected",
												 NULL);

class HostConfig {
public:

	HostConfig() {
		printf("Getting host configuration...\n");
		get_screen_size(&screen_width, &screen_height);
		mouse_sensitivity = 0.8;
	}

	int screen_width;
	int screen_height;

	/* If pointer on touchscreen moves x in X direction,
	   move (mouse_sensitivity * x) on host screen. */
	float mouse_sensitivity;
};
HostConfig hostconfig;

class ActionMessage {
public:

	ActionMessage(){}
	~ActionMessage(){}

};

class InitConnMessage {
public:

	InitConnMessage(int screen_width, int screen_height){
		_screen_height = screen_height;
		_screen_width = screen_width;
	}	

	~InitConnMessage() {}

    string Serialize() const {

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);

        writer.StartObject();

		writer.String("screen_width");
        writer.Uint(this->_screen_width);

        writer.String("screen_height");
        writer.Uint(this->_screen_height);

        writer.String("mtype");
        writer.String(this->mtype);

        writer.EndObject();

        return buffer.GetString();
    }

private:
	int _screen_height;
	int _screen_width;
	const char *mtype = "INIT_CONN";
};

void wait_for_init_conn(int client)
{
	char buf[BUFFER_SIZE] = {0};
	int bytes_read;
		
	bytes_read = read(client, buf, sizeof(buf));
	if ( bytes_read > 0 ) {
		Document d;
		d.Parse(buf);
		string mtype = d["mtype"].GetString();

		// if conn initiation is requested,
		// accept on security conditions
		if ( mtype == "INIT_CONN" ) {
			// everyting is fine, send host info to phone
			printf("INIT_CONN message received, sending hostconfig\n");
			InitConnMessage initmessage = InitConnMessage(hostconfig.screen_width,
														  hostconfig.screen_height);
			string response = initmessage.Serialize();
			write(client, response.c_str(), strlen(response.c_str()));
		}
		else {
			// init conditions are not set
			// break the connection
			// write(client,);
			exit(1);
		}

		memset(buf, '\0', BUFFER_SIZE);
	} else {
		printf("Failed to read initconn message\n");
		exit(1);
	}
}

bool event_loop(int client) 
{
	// dive into infinite loop
	char buf[BUFFER_SIZE] = {0};
	int bytes_read, d;
	Document jsondoc = NULL;

	// wait for INII_CONN from client
	wait_for_init_conn(client);

	int prev_x = -1, prev_y = -1,
		pointer_x, pointer_y,
		x_delta, y_delta, poc;

	printf("Ready to receive events for client %d\n", client);
	while(true) {
		bytes_read = read( client, buf, sizeof( buf ) );

		if (bytes_read>0) {
			// printf("RECEIVED : %s\n", buf);

			if ( strlen( buf ) >= BUFFER_SIZE ) {
				printf("BUFFER_SIZE exceeded\n");
				exit(1);
			}

			char objects[10][BUFFER_SIZE] = {'\0'};
			poc = defragment_json( buf, objects );

			for (int i = 0; i < poc; ++i)
			{
				Document d; d.Parse(objects[i]);
				const char *mtype = d["mtype"].GetString();

				printf("%s\n", mtype);
				if ( strcmp("1P_M", mtype )  == 0 ){
					// 1 pointer move

					pointer_x = d["coord_x"].GetInt();
					pointer_y = d["coord_y"].GetInt();
					x_delta = (int)(hostconfig.mouse_sensitivity * (float)( pointer_x - prev_x ));
					y_delta = (int)(hostconfig.mouse_sensitivity * (float)( pointer_y - prev_y ));

					xdo_move_mouse_relative( xdo_instance, x_delta, y_delta );

					prev_x = pointer_x;
					prev_y = pointer_y;
				}
				else if ( strcmp( "1P_D", mtype ) == 0 ) {
					// 1 pointer down
					prev_x = d["coord_x"].GetInt();
					prev_y = d["coord_y"].GetInt();
				}
				else if ( strcmp( "1P_U", mtype ) == 0 ) {
					prev_x = -1;
					prev_y = -1;
				}
				else if ( strcmp( "1P_T", mtype ) == 0 ) {
					// 1 pointer tap
					printf("mtype: 1P_T\n");
					xdo_click_window( xdo_instance, CURRENTWINDOW, 1 );
				}
				else if ( strcmp( "1P_DT", mtype) == 0 ) {
					// 1 pointers double tap
					printf("mtype: 1P_DT\n");
					xdo_click_window_multiple( xdo_instance, CURRENTWINDOW, 1 , 2, 500);
				}
				else if ( strcmp( "2P_M_UP", mtype ) == 0 ) { // 2 pointers : scroll up
					printf("mtype: %s\n", mtype);
					xdo_click_window(xdo_instance, CURRENTWINDOW, 4);
				}
				else if ( strcmp( "2P_M_DOWN", mtype ) == 0 ) { // 2 pointers : scroll down
					printf("mtype: %s\n", mtype);
					xdo_click_window(xdo_instance, CURRENTWINDOW, 5);
				}
				else if ( strcmp( "2P_M_LEFT", mtype ) == 0 ) {
					printf("mtype: %s\n", mtype);
				}
				else if ( strcmp( "2P_M_RIGHT", mtype ) == 0 ) {
					printf("mtype: %s\n", mtype);
				}

				else if ( strcmp( "3P_M", mtype ) == 0 ) {
					// 3 pointers move
					printf("mtype: 3P_M\n");
				}
				else if ( strcmp( "REINIT_CONN", mtype ) == 0 ) {
					// intent to reconnect
					return 1;
				}
				else if ( strcmp( "CLOSE_CONN", mtype ) == 0 ) {
					return 1; // start listening for conn again
				}
				else {
					printf("NOTHING\n");
				}
			}

		} else {
			printf("Failed to read bytes [%d], going to accept state.\n", bytes_read);
			return 1; // start listening for conn again
		}
		memset(buf, '\0', BUFFER_SIZE);
	}
}

void setup()
{
	// enable notify system
	notify_set_app_name("Musendrid");
	notify_init("Musendrid");
	notify_notification_set_app_name(nn, "Musendrid");
	notify_notification_set_timeout(nn, 3000);

	// init xdo instance
	xdo_instance = xdo_new(NULL);

    sdp_session_t* session = register_service();
}

int main(int argc, char const *argv[])
{

    // init local address and remote address
    sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

	setup();

	int s, client, bytes_read;
	bool ctn = false; // ctn to listen
	char buf[1024] = {0};
	socklen_t opt = sizeof(rem_addr);

	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// bind socket to first available port of
	// local bluetooth adapter
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = *BDADDR_ANY;
	loc_addr.rc_channel = (uint8_t) 1;
	bind( s, (struct sockaddr *)&loc_addr, sizeof(loc_addr) );

	while(true) {
		// put socket into listening mode
		printf("Listening for connection\n");
		listen(s, 1);

		// accept one connection
		client = accept(s, (struct sockaddr*)&rem_addr, &opt);

		ba2str(&rem_addr.rc_bdaddr, buf);
		fprintf(stderr, "accepted connection from %s\n", buf);

		notify_notification_update(nn, "Musendrid phone is connected", NULL, NULL);
		notify_notification_show(nn, NULL);

		ctn = event_loop(client);
		close(client);

		notify_notification_update(nn, "Musendrid phone is disconnected", NULL, NULL);
		notify_notification_show(nn, NULL);

		/* if intent is not to contiue, break*/
		if ( !ctn ) break;

	}

	printf("Closing bluetooth conn.\n");
	close(s);
	xdo_free(xdo_instance);

	return 0;
}
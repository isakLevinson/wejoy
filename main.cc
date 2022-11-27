
#include <iostream>
#include <unistd.h> //sleep
#include <thread>   //thread
#include <mutex>
#include <string>

//#include "CKeyboard.h"
#include "linuxtrack.h"
#include "CVirtualJoy.h"

bool bPoll = true;
std::mutex mtx;
CVirtualJoy*  vJoy;

void updateThreadJoysticks(void)
{
	int retVal;
	//linuxtrack_state_type state;
	float fh, fp, fr, fx, fy, fz;
	int   h, p, r, x, y, z;
	unsigned int counter;

	//Sleep one second to give the X11 system time to adapt.
	sleep(1);

	while (bPoll) {
		usleep(1000);
		//mtx.lock();
		//mtx.unlock();

		//state = linuxtrack_get_tracking_state();
		retVal = linuxtrack_get_pose(&fh, &fp, &fr, &fx, &fy, &fz, &counter);
		retVal = 1;
		if (retVal) {
			x = 100 * fx;
			y = 100 * fy;
			z = 100 * fz;
			r = 100 * fr;
			p = 100 * fp;
			h = 100 * fh;

			printf("ltr: %6d %6d %6d %6d %6d %6d\n", h, p, r, x, y, z);

			vJoy->send_axis_event(0, x);
			vJoy->send_axis_event(1, y);
			vJoy->send_axis_event(2, z);
			vJoy->send_axis_event(3, h);
			vJoy->send_axis_event(4, p);
			vJoy->send_axis_event(5, r);

		} else {
			printf("...\n");
		}
	}//while
}

bool intialize_tracking(void)
{
	linuxtrack_state_type state;
	//Initialize the tracking using Default profile
	state = linuxtrack_init(NULL);
	if (state < LINUXTRACK_OK) {
		printf("%s\n", linuxtrack_explain(state));
		return false;
	}
	int timeout = 0;
	//wait up to 20 seconds for the tracker initialization
	while (timeout < 200) {
		state = linuxtrack_get_tracking_state();
		printf("Status: %s\n", linuxtrack_explain(state));
		//std::cout << "Status: ", linuxtrack_explain(state), "\n";
		if ((state == RUNNING) || (state == PAUSED)) {
			return true;
		}
		usleep(100000);
		++timeout;
	}
	std::cout << "Linuxtrack doesn't work right!\n";
	std::cout << "Make sure it is installed and configured correctly.\n";
	return false;
}


int main(int argc, char** argv)
{
	//TODO I need to search for information on which buttons and axes is required to correctly be found in applications, as sometimes i.e. axes are found in system, but not by applications.

	std::cout << "WeJoy v0.2 by Johannes Bergmark\n";

	if (!intialize_tracking()) {
		std::cout << "intialize_tracking failed\n";
		return 1;
	}

	std::cout << "intialize_tracking started!\n";

	vJoy = new CVirtualJoy(1, 6);

	std::cout << "Press 'q' and then 'ENTER' to quit!\n";

	std::thread threadUpdateJoysticks(updateThreadJoysticks);

	while (getchar() != 'q');
	bPoll = false,
	threadUpdateJoysticks.join();
	sleep(1);

	//stop the tracker
	linuxtrack_shutdown();

	return 0;
}

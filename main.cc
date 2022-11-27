#include <unistd.h> //sleep
#include <thread>   //thread
#include <mutex>

#include "LuaScript.h"
#include "global.h"
#include "CKeyboard.h"
#include "linuxtrack.h"

bool bPoll = true;
std::mutex mtx;

void updateThreadJoysticks(LuaScript &lScript)
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

			lScript.call_device_function("ltr_x_event", x);
			lScript.call_device_function("ltr_y_event", y);
			lScript.call_device_function("ltr_z_event", z);
			lScript.call_device_function("ltr_h_event", h);
			lScript.call_device_function("ltr_p_event", p);
			lScript.call_device_function("ltr_r_event", r);

		} else {
			printf("...\n");
		}
	}//while
}

//Called from user via lua script
int l_send_vjoy_button_event(lua_State* _L)
{
	unsigned int id = lua_tonumber(_L, 1);
	int type = lua_tonumber(_L, 2);
	int value = lua_tonumber(_L, 3);

	if (id >= GLOBAL::vJoyList.size()) {
		std::cout << "ERROR send_vjoy_button_event: Virtual device " << id << " does not exist.\n";
		return 0;
	}//if
	GLOBAL::vJoyList[id]->send_button_event(type, value);
	return 0;
}

//Called from user via lua script
int l_send_vjoy_axis_event(lua_State* _L)
{
	unsigned int id = lua_tonumber(_L, 1);
	int type = lua_tonumber(_L, 2);
	int value = lua_tonumber(_L, 3);

	if (id >= GLOBAL::vJoyList.size()) {
		std::cout << "ERROR send_vjoy_axis_event: Virtual device " << id << " does not exist.\n";
		return 0;
	}//if
	GLOBAL::vJoyList[id]->send_axis_event(type, value);

	return 0;
}

//Called from user via lua script
int l_get_vjoy_button_status(lua_State* L)
{
	unsigned int id = lua_tonumber(L, 1);
	int type = lua_tonumber(L, 2);
	if (id >= GLOBAL::vJoyList.size()) {
		std::cout << "ERROR get_vjoy_button_status: Virtual device " << id << " does not exist.\n";
		lua_pushnumber(L, -1);
		return 1;
	}
	int status = GLOBAL::vJoyList[id]->get_button_status(type);
	lua_pushnumber(L, status);
	return 1;
}


//Called from user via lua script
int l_get_vjoy_axis_status(lua_State* L)
{
	unsigned int id = lua_tonumber(L, 1);
	int type = lua_tonumber(L, 2);
	if (id >= GLOBAL::vJoyList.size()) {
		std::cout << "ERROR get_vjoy_axis_status: Virtual device " << id << " does not exist.\n";
		lua_pushnumber(L, -1);
		return 1;
	}
	int status = GLOBAL::vJoyList[id]->get_axis_status(type);
	lua_pushnumber(L, status);
	return 1;
}


//Populate a list of physical devices defined in user lua file
bool populate_devices(LuaScript &lScript)
{
	//Get the data from the user lua file
	std::vector<std::array<int, 2>> dList;
	std::array<int, 2> val;
	int cIndex = 0;
	while (1) {
		bool noerr;
		val[0] = lScript.get<int>("devices.d" + std::to_string(cIndex) + ".vendorid", noerr);
		if (!noerr) {
			break;
		}
		val[1] = lScript.get<int>("devices.d" + std::to_string(cIndex) + ".productid", noerr);
		if (!noerr) {
			break;
		}

		dList.push_back(val);

		cIndex++;
	}//for

	return true;
}

//Populate a alist of virtual devices defined in user lua file
bool populate_virtual_devices(LuaScript &lScript)
{
	std::vector<std::array<int, 2>> dList;
	std::array<int, 2> val;
	int cIndex = 0;
	while (1) {
		bool noerr;

		val[0] = lScript.get<int>("v_devices.v" + std::to_string(cIndex) + ".buttons", noerr);
		if (!noerr) {
			break;
		}
		val[1] = lScript.get<int>("v_devices.v" + std::to_string(cIndex) + ".axes", noerr);
		if (!noerr) {
			break;
		}

		dList.push_back(val);

		cIndex++;
	}//for

	//Create and populate the list of user defined virtual devices
	for (unsigned int i = 0; i < dList.size(); i++) {
		CVirtualJoy* vJoy = new CVirtualJoy(dList[i][0], dList[i][1]);
		if (!vJoy->isOpen()) {
			return false;
		}
		GLOBAL::vJoyList.push_back(vJoy);
	}//for

	return true;
}

//Initialize lua functions
void link_lua_functions(LuaScript &lScript)
{
	lScript.pushcfunction(l_send_vjoy_button_event, "send_button_event");
	lScript.pushcfunction(l_send_vjoy_axis_event,   "send_axis_event");
	lScript.pushcfunction(l_get_vjoy_button_status, "get_vjoy_button_status");
	lScript.pushcfunction(l_get_vjoy_axis_status,   "get_vjoy_axis_status");
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

	if (argc < 2) {
		std::cout << "Please type the path of your lua file.\n";
		std::cout << "I.e. 'wejoy config.lua'\n";
		return 0;
	}

	if (!intialize_tracking()) {
		std::cout << "intialize_tracking failed\n";
		return 1;
	}

	std::cout << "intialize_tracking started!\n";

	//Open the user lua file.
	LuaScript lScript(argv[1]);
	if (!lScript.isOpen()) {
		return 0;
	}

	if (!populate_devices(lScript)) {
		exit(0);
	}
	if (!populate_virtual_devices(lScript)) {
		exit(0);
	}
	link_lua_functions(lScript);

	std::cout << "Press 'q' and then 'ENTER' to quit!\n";

	std::thread threadUpdateJoysticks(updateThreadJoysticks, std::ref(lScript));

	while (getchar() != 'q');
	bPoll = false,
	threadUpdateJoysticks.join();
	sleep(1);

	//stop the tracker
	linuxtrack_shutdown();

	return 0;
}

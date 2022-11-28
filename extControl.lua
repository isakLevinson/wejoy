--Physical devices to use (type lsusb in terminal to list your connected devices)
devices = 
   {
      d0 = --Thrustmaster Warthog Joystick
	 {
	    vendorid = 0x0738,
       productid = 0xa215,
	    --productid = 0x2215,
	 },

      kbd0 = "/dev/input/by-id/usb-04d9_USB_Keyboard-event-kbd",  -- keyboard device (try to find a suitable device by listing input devices by typing 'ls /dev/input/by-id/' )
      kbd1 = "/dev/input/by-id/usb-Aqua_Computer_GmbH___Co._KG_aquaero_07538-20376-event-kbd" -- another keyboard device example
   }

-- Send method for keyboard. Key is given, i.e. KEY_G (check reference document for more supported key-codes), and state is given, i.e. 0 for release or 1 for press.
-- send_keyboard_event(key, state)

-- Send methods for virtual devices. Index of virtual device is given, i.e. 0 for v0, 1 for v1 and so on. Button/axis is given, i.e. 0 for button 0, 1 for button 1 and so on.
-- send_button_event(vjoy, button, state)
-- send_axis_event(vjoy, axis, pos)			//pos is the axis position (-32767 >= pos <= 32767)

-- Get methods for physical devices. Arguments applies in the same way as for above methods.
-- get_button_status(joy, button) 	//Returns 0 or 1 if button is pressed or not.
-- get_axis_status(joy, axis)		//Returns the axis position value

-- Get methods for virtual devices, applies in the same way as for physical devices.
-- get_vjoy_button_status(vjoy, button)
-- get_vjoy_axis_status(vjoy, axis)

function kbd0_pressed(value)
   if value == KEY_W then
      send_button_event(0, 1, 1)
   end
end

function kbd0_released(value)
   if value == KEY_W then
      send_button_event(0, 1, 0)
   end
end

-- Send a button 0 event to virtual device 0 when button 0 on physical device 0 is pressed and released.
function d0_b0_event(value)
   if value == 1 then
      send_button_event(0, 0, 1)
   else
      send_button_event(0, 0, 0)
   end
end

-- When button 1 on device 0 is pressed, invert virtual axes 0 and 1 on virtual device 0, otherwise these axes is as the first two axes on physical device 0.
-- Send a button 1 event to virtual device 0 when button 1 on physical device 0 is pressed and released.
function d0_b1_event(value)
   if value == 1 then
      send_button_event(0, 1, 1)
      x = get_axis_status(0, 0)
      y = get_axis_status(0, 1)
      send_axis_event(0, 0, -x)
      send_axis_event(0, 1, -y)
   else
      send_button_event(0, 1, 0)
      x = get_axis_status(0, 0)
      y = get_axis_status(0, 1)
      send_axis_event(0, 0, x)
      send_axis_event(0, 1, y)
   end
end

local gainShift    = 300;
local gainRotation = 100;

-- Send a button 2 event to virtual device 0 when button 2 on physical device 0 is pressed and released.
function d0_b31_event(value)
   if value == 1 then
      gainRotation   = 100;
      gainShift      = 300;
   end
end

function d0_b32_event(value)
   if value == 1 then
      gainRotation   = 0;
      gainShift      = 0;
      ltr_recenter(0, 0, 0)
   end
end

function d0_b33_event(value)
   if value == 1 then
      gainRotation   = 20;
      gainShift      = 60;
   end
end

function ltr_x_event(value)
   send_axis_event(0, 0, -gainShift * value)
end

function ltr_y_event(value)
   send_axis_event(0, 1, gainShift * value)
end

function ltr_z_event(value)
   send_axis_event(0, 2, gainShift * value)
end

function ltr_rx_event(value)
   send_axis_event(0, 3, -gainRotation * value)
end

function ltr_ry_event(value)
   send_axis_event(0, 4, -gainRotation * value)
end

function ltr_rz_event(value)
   send_axis_event(0, 5, gainRotation * value)
end

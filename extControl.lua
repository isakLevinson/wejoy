--Physical devices to use (type lsusb in terminal to list your connected devices)
devices = 
{
   d0 = -- throttle
	{
	    vendorid = 0x0738,
       productid = 0xa215,
   },

   d1 = -- stick
	{
	    vendorid = 0x0738,
	    productid = 0x2215,
	},

--  kbd0 = "/dev/input/by-id/usb-04d9_USB_Keyboard-event-kbd",  -- keyboard device (try to find a suitable device by listing input devices by typing 'ls /dev/input/by-id/' )
--  kbd1 = "/dev/input/by-id/usb-Aqua_Computer_GmbH___Co._KG_aquaero_07538-20376-event-kbd" -- another keyboard device example

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


local gainShift    = 300;
local gainRotation = 100;
local offsetZ      = 0;

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
      ltr_recenter()
   end
end

function d0_b33_event(value)
   if value == 1 then
      gainRotation   = 20;
      gainShift      = 60;
   end
end

function d0_a5_event(value)
   offsetZ = value;
end

function ltr_event(x, y, z, rx, ry, rz)
   send_axis_events(
      -gainShift * x,
      gainShift * y,
      gainShift * z + offsetZ,
      -gainRotation * rx,
      -gainRotation * ry,
      gainRotation * rz)

end
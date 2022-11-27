
--Virtual devices to create, current limit is maximum 53 (0 to 52) buttons and 19 (0 to 18) axes. Note that not every button or axis is fully tested to work.
--Creating more than one virtual devices is possible, making room for more buttons and axes.
v_devices = 
   {
      v0 = 
	 {
	    buttons = 1,
	    axes = 6
	 }
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

function ltr_x_event(value)
   send_axis_event(0, 0, value)
end

function ltr_y_event(value)
   send_axis_event(0, 1, value)
end

function ltr_z_event(value)
   send_axis_event(0, 2, value)
end

function ltr_h_event(value)
   send_axis_event(0, 3, value)
end

function ltr_p_event(value)
   send_axis_event(0, 4, value)
end

function ltr_r_event(value)
   send_axis_event(0, 5, value)
end


function ltr_xxx_event(value)
   send_axis_event(0, 6, value)
end


****************************************************************
** Group 22-010 Seven Hills Fire Alarm Code
** Members: Steven Maroni, Imane Hankour, Vyto Morkunas, Hammad Shafiq
** Write up by: Steven Maroni
****************************************************************

//// Intro //////////////////

This Is a repository for all Arduino Code made for the 22-010 Capstone group. Code is sorted in the following ways:

1. Testing Code - This code is code that used to learn how to use the arduino IDE. This snippets are NOT meant for production use and are not a part of the project itself.

2. Partial Code - This is Handmade code that will be used later in the file project. These programs will vary in use
   and size, but will all be the baseline for what the final code will be.

3. Combined Code - This code is a combination of previous Partial Code as well as signifigant improvements to them. Full code is the latest test in this folder.

4. Final_FireAlarm_Code - This is the completed Fire Alarm detection code. Webhook may not be the same as the client's for security.


//// Running programs/////////

To run the code, install the arduino IDE and then open the .ino file. You will need to have it in a folder with a
corrisponding name for it to load.

After opening the file, You will need to have the correct correct libraries installed. Confirm that these libraries on on your IDE:

	Adafriut TouchScreen
	Adafruit_GFX
	Adafruit_Hx8357

Make sure that in the Tools Tab, these settings are correct:

	Board: "Arduino Uno WiFi Rev2"
	Registers emulation: "None(ATEMEGAA4809)"
	Port: "COM3(Arduino Uno WiFi Rev2)"

Once all settings are confirmed, you can run the code by pressing the Upload ("->") button. This will send the code to
the arduino and it will run continously.

All other information is contained in the user manual which will be included with the device.

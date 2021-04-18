WiFiController v0.4.4
Copyright (c)2007 David Rudie (d.rudie@gmail.com)
http://imk.cx/psp/wificontroller/


READ ME.  READ ALL OF ME!


LICENSE
 This application uses the BSD license and a copy of the included Copyright
 LICENSE file can be found in the root of the zip file.


ABOUT THIS PROJECT
 Mikko Monone wrote the original controller program for the PSP and the server
 for the PC. After using his program on the PSP it seemed it was missing some
 things. The start button exited the program and select didn't work. It had a
 display for the buttons you would press, and while it was functional, I like
 a little more detail to be displayed. I then wrote my own controller from
 scratch and only borrowed a small portion of his code (the packet creation).
 Here is a link to the thread that started this all in a way for me.


AFFILIATION NOTE
 I don't know Mikko Monone or the authors of PPJoy. There is no affiliation
 with them at all. I thank them for their programs.


WHAT THIS IS AND WHAT THIS DOES
 This basically turns your PSP into a controller for your PC (running
 Windows). How it works is that PPJoy provides a way to create a virtual
 joystick. We can then communicate with this virtual joystick as though it
 were a real joystick. This is accomplished by running a server in Microsoft
 Windows. The server just runs in the background waiting for a connection.
 When you start the WiFiController program on your Sony PSP it will establish
 a network connection over wireless using infrastructure mode. Once it
 establishes a connection and obtains an IP address it will connect to the
 server running on your PC. Once it has connected to your PC it will act just
 like a regular joystick.


NOTES
 Although I have not comfirmed this myself, I have heard that this works on
 v2.60 PSP's as well. I originally wrote this program for v1.00 and v1.50 only.
 Recently, I have added official support for v3.xx OE.  When using v3.xx, make
 sure you run it with the v3.xx kernel.  If you use CWCheat and the v3.xx
 version of WiFiController, hold down the right trigger when launching the
 program.  If you do not, WiFiController will crash.


INSTALLATION
 I'm going to attempt to explain how to get this working. These steps are all
 necessary and you should pay close attention to them.

 1) Open the PSP directory and edit the proper wifi.cfg for your PSP version
    and set the IP of the computer you are going to use this on. If you don't
    know your IP address. Go to your "Start" menu. Click on "Run..." When the
    box opens up type "cmd.exe" without the quotes. When the command prompt
    opens type "ipconfig" without the quotes. It will report your IP address.

 2) Copy the correct director(y|ies) to your PSP depending on whether you have
    a v1.00, v1.50 or v3.xx OE PSP.

 3) Download and install PPJoy.
    http://www.geocities.com/deonvdw/Docs/PPJoyMain.htm

 4) After it is installed open up your "Control Panel" and click on "Parallel
    Port Joysticks".

 5) When the window comes up click "Add..." at the bottom. On the next screen
    change "Parallel port" to "Virtual joysticks" and then click "Add."

 6) It will want to install some drivers so either have it scan automatically
    or point it to the directory you installed PPJoy to.

 7) After it is done installing, click the "PPJoy Virtual joystick 1" in the
    menu and click "Mapping..."

 8) Choose "Set a custom mapping for this controller" and click "Next."

 9) Set these options for best results.
    Axes = 2 (X Axis and Y Axis)
    Buttons = 9
    POV hats = 1

 10) X Axis = Analog 0
     Y Axis = Analog 1

 11) These buttons are up to you to set. You should leave "Button 1" set to
     "nothing" otherwise you may have problems using the Triangle button on
     your PSP later.
     Button 1 = nothing
     Button 2 = Digital 0
     Button 3 = Digital 1
     Button 4 = Digital 2
     Button 5 = Digital 3
     Button 6 = Digital 4
     Button 7 = Digital 5
     Button 8 = Digital 11
     Button 9 = Digital 10

 12) Directional button POV
     North = Digital 6
     East = Digital 7
     West = Digital 9
     South = Digital 8

 13) Repeat steps 5 to 12 to add up to 4 joysticks (this is the maximum amount
     that WiFiController will support at the moment).

 14) Inside the PC directory, you will find a WiFiServer.exe.  If you are
     just using one PSP to play games, you can run this file directly.  If,
     however, you are or will be using more than one PSP, you will need to
     run one of the batch files in the directory.  Simply double-click the
     batch file for the appropriate controller.  You must have a server
     running for each controller you wish to use.  The server(s) must also be
     running at all times while using the PSP as a joystick.  When you are
     done using it, you can safely exit the program by pressing CTRL+C or
     clicking the close button for the window.

     Example:
       Two players will be playing a game.  You will need to start the batch
       files for controller 1 and 2.  Player 1 will choose Controller 1 from
       WiFiController and player 2 will choose Controller 2.

     Note:
       If the window opens and closes immediately, something went wrong and
       you'll need to either contact me or figure it out yourself.  However,
       make sure you RETRY all the steps before contacting me.  Also try
       reading over the WiFiController website and the PPJoy website for help.

 15) Now start the program on your PSP, choose your connection and your
     controller.  It should connect to your computer and basically be working.
     You'll find out in the next step.

 16) Go back to your Control Panel and go to Game Controllers. Click on "PPJoy
     Virtual joystick 1" and choose Properties. On the screen that comes up
     you should be able to press buttons on the PSP and have them show as
     working on the screen. I recommend calibrating the Analog Stick too.

 17) Enjoy!

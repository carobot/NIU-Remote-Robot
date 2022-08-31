The web controller is a set of files to be used as a web page interface for controlling the robot remotely. 

The index.html file is the core of the page and its layout. It makes the title, embedded stream, and each of the control buttons. 

The app.js file is the inner workings of the controller which listens for key strokes, changes the direction values in a firebase database, and makes a visual indication of the key pressed on the webpage.

Finally the style.css file determines the details of each object used in the index.html file. This includes things like the button shape, background color, and container spacing. 

When this webpage is run on a server (for instance the VS code extension: Live Server) and the website is accessed through the IP address it allows for sending data to the online database and receiving the live stream from the USB cameras. 

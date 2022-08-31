import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import os
import subprocess
import shlex
import time
import serial

# Fetch the service account key JSON file contents
cred = credentials.Certificate('/path/to/database/jsonconfig')

# )
# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://quick-woodland-357917-default-rtdb.firebaseio.com/'
})

#setting up the serial output
ser = serial.Serial(
        port='/dev/ttyAMC0', 
        baudrate = 115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

rightVal = 0
leftVal = 0
shootVal = 0
camera = 0
ltCam = 0

#Runs the parameter command in the Raspberry Pi terminal - command kills running ffmpeg processes so only one webcam is being encoded/streamed at a time 
os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")
os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")


#below is the looping function
while True:
    #Getting data from the firebase database
    ref = db.reference('data')
    snap = ref.get()
    if shootVal != snap['shoot']:
        ser.write(bytearray(b'.\0x02\0x71\0x03'))
    rightVal = snap['right']
    leftVal = snap['left']
    shootVal = snap['shoot']
    camera = snap['cam']
    curDir = 0
    lastDir = 0
    
    #------------------------------------------------------------------------------------------------
    #Toggle between streaming cameras when there is a change in the cam value on the database
    if camera == 1 and ltCam == 0:
        ltCam = 1
        os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")
        os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")        
        print("switching to video2")
        #start encoding and streaming webcam to specified youtube channel/key
        subprocess.Popen(["ffmpeg -f v4l2 -i /dev/video2 -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero -acodec aac -ab 128k -strict experimental -s 1280x720 -vcodec h264 -pix_fmt yuv420p -g 10 -vb 128k -profile:v baseline -r 5 -f flv 'rtmp://a.rtmp.youtube.com/live2/streamkey'"], shell=True)

    
    elif camera == 2 and ltCam == 1:
        ltCam = 2
        os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")
        os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")
        print("switching to video4")
        subprocess.Popen(["ffmpeg -f v4l2 -i /dev/video4 -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero -acodec aac -ab 128k -strict experimental -s 1280x720 -vcodec h264 -pix_fmt yuv420p -g 10 -vb 128k -profile:v baseline -r 5 -f flv 'rtmp://a.rtmp.youtube.com/live2/streamkey'"], shell=True)
    
    elif camera == 0 and ltCam == 2:
        ltCam = 0
        os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")
        os.system("ps ax  |  grep 'ffmpeg'  |  awk '{print $1}'  |  xargs sudo kill -9")
        print("switching to video0")
        subprocess.Popen(["ffmpeg -f v4l2 -i /dev/video0 -ar 44100 -ac 2 -acodec pcm_s16le -f s16le -ac 2 -i /dev/zero -acodec aac -ab 128k -strict experimental -s 1280x720 -vcodec h264 -pix_fmt yuv420p -g 10 -vb 128k -profile:v baseline -r 5 -f flv 'rtmp://a.rtmp.youtube.com/live2/streamkey'"], shell=True)
    #------------------------------------------------------------------------------------------------
    #Sending data to the arduino through serial depending on values seen in the database

    if rightVal==100 and leftVal ==100:
        curDir = 'w'
        if curDir != lastDir:
            ser.write(bytearray(b'.\0x02\0x77\0x03'))
            lastDir = curDir
    elif rightVal==100 and leftVal==-100:
        curDir = 'a'
        if curDir != lastDir:
            ser.write(bytearray(b'.\0x02\0x61\0x03'))
            lastDir = curDir
    elif rightVal ==-100 and leftVal ==-100:
        curDir = 's'
        if curDir != lastDir:
            ser.write(bytearray(b'.\0x02\0x73\0x03'))
            lastDir = curDir
    elif rightVal == -100 and leftVal == 100:
        curDir = 'd'
        if curDir != lastDir:
            ser.write(bytearray(b'.\0x02\0x64\0x03'))
            lastDir = curDir
    

    

import RPi.GPIO as GPIO

import time

import socket

import sys

HOST, PORT = "192.168.193.1", 27015


# GPIO Pins set up
GPIO.setmode(GPIO.BOARD)
# 7,11,13,15 for motor control
GPIO.setup(7,  GPIO.OUT)

GPIO.setup(11, GPIO.OUT)

GPIO.setup(13, GPIO.OUT)

GPIO.setup(15, GPIO.OUT)

// 12 for LED 
GPIO.setup(12,GPIO.OUT)


flag = False


def forward():

    GPIO.output(7, 0)

    GPIO.output(11,1)

    GPIO.output(13,1)

    GPIO.output(15,0)

    time.sleep(0.75)



def back():

    a=5

    #if(flag):

        #GPIO.output(12,1)

	#time.sleep(2)

	#GPIO.output(12,0)

    while(a!= 0):

       	GPIO.output(7, 1)

       	GPIO.output(11,0)

   	GPIO.output(13,0)

   	GPIO.output(15,1)

   	time.sleep(0.2)

	a=a-1

	#GPIO.output(12,0)



def left():

    
    #while(a!=0):

        GPIO.output(7, 1)

        GPIO.output(11,0)

        GPIO.output(13,1)

        GPIO.output(15,0)

        time.sleep(0.85)

        #a=a-1




def right():

    #b=20

    #while(b != 0):

        GPIO.output(7, 0)

        GPIO.output(11,1)

        GPIO.output(13,0)

	GPIO.output(15,1)

        time.sleep(0.85)

        #b=b-1



def glow_led():

	GPIO.output(12,1)

	time.sleep(2)

	GPIO.output(12,0)



def spl_stop():
	
	time.sleep(5)



def stop():

    GPIO.output(7, 1)

    GPIO.output(11,1)

    GPIO.output(13,1)

    GPIO.output(15,1)

    time.sleep(0.55)



forward()

stop()

left()

stop()

right()

stop()



s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


s.connect((HOST, PORT))

s.send(b'rpi')

print("Initiating server connection...")

s.send('rpi\n')
print(str(s.recv(20)))

while(1)	:

       s.send('waiting\n')

       cmd=str(s.recv(2))

       if cmd=='fw':

		 print("moving forward...!!")

                 forward()

                 stop()

       if cmd=='lf':
		 print("moving left....!!")
                 left()
                 stop()
		 forward()
		 stop()
       if cmd=='rf':
		 print("moving right...!!")
                 right()
                 stop()
 		 forward()
		 stop()
       if cmd=='lt':
	 	 #flag=true
		 #forward()
		 #flag=false
		 #stop()
		 left()
		 stop()
       if cmd=='rt':
		 #flag=true
 		 right()
		 stop()
#		 forward()
#		 stop()
#		 flag=false
       if cmd== 'gf':
		print("gf")
		glow_led()
		stop()
		forward()
		stop()
       if cmd == 'lg':
		print("lg")
		left()
		stop()
		glow_led()
		stop()
		forward()
		stop()
       if cmd == 'rg':
		print("rg")
		right()
		stop()
		glow_led()
		stop()
		forward()
		stop()
       if cmd == 'sf':
		print("sf")
		spl_stop()
		forward()
		stop()
       if cmd == 'sr':
		print("sr")
		right()
		stop()
		spl_stop()
		forward()
		stop()
       if cmd == 'sl':
		print("sl")
		left()
		stop()
		spl_stop()
		forward()
		stop()
		
GPIO.cleanup()

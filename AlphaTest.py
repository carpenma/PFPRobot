import pygame, time, sys, serial

#PyGame Setup
pygame.init()
js = pygame.joystick.Joystick(0)
js.init()

#Axis Numbers
L_stick = 1
R_stick = 2

#Serial Setup
myport = serial.Serial('/dev/ttyACM0', 9600)
myport.timeout = 1

x=0
while (x<250):
	#print '-=-=-=-=-=-=-=-=-=- X: '+str(x)
	#print 'Axis '+ str(i) +': '+ str(curval)

	left_loc = js.get_axis(L_stick)*128+128
	right_loc = js.get_axis(R_stick)*128+128
	#Calculate Values to send
	left_pos = int(left_loc - ((left_loc/255) % 255))
	right_pos = int(right_loc - ((right_loc/255) % 255))
	#Left Axis
	print left_pos
	left_send = '1,'+chr(left_pos)
	#Right Axis
	right_send = '2,'+chr(right_pos)


	#If Disable Button Not Clicked
	
	print 'Bytes written for left: '+str(myport.write(left_send))+' they were: '+left_send
	myport.write(right_send)
	#Else
	
	#Debugging code
	print 'Bytes waiting: '+str(myport.inWaiting())
	
	pygame.event.pump()
	time.sleep(.05)
	x = x + 1

myport.close()
sys.exit(0)

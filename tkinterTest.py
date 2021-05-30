from tkinter import *
from pyfirmata2 import *
import pyfirmata2.util as util
from random import randint
import time

shouldFlash = False
timeOfLastFlash = 1001

board = Arduino('/dev/rfcomm0')

def sendCommand() :
    for x in box.get():
        board.send_sysex(0x71, util.str_to_two_byte_iter(x))
    
    box.delete(0,"end")


def flashCallback(event):
    global timeOfLastFlash
    if (time.time() - timeOfLastFlash) > 0.25 :
        timeOfLastFlash = time.time()
        toSend="l"+ str(randint(0,255)) + "r" + str(randint(0,255)) + "g" + str(randint(0,255)) + "b."
        for x in toSend:
            board.send_sysex(0x71, util.str_to_two_byte_iter(x))



def RGBsend():
    toSend="l"+ R.get() + "r" + G.get() + "g" + B.get() + "b."
    for x in toSend:
        board.send_sysex(0x71, util.str_to_two_byte_iter(x))

window = Tk()

button = Button(window, text="Send raw command", command=sendCommand)
button.grid(column=3,row=0, sticky='e')


box = Entry(window,width=61)
box.grid(column=0,row=0, columnspan=3, sticky='w')




R = Entry(window)
R.grid(column=0,row=2)

G = Entry(window)
G.grid(column=1,row=2)

B = Entry(window)
B.grid(column=2,row=2)

RGBsubmit = Button(window, text="RGB", command=RGBsend)
RGBsubmit.grid(column=3,row=2)

window.bind("f",flashCallback)







window.mainloop()


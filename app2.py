from pyfirmata2 import *

board = Arduino('/dev/ttyUSB0')

while True:
    board.send_sysex(0x71, util.str_to_two_byte_iter(input(":")))

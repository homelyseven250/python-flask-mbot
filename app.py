import os
from flask import Flask
from flask.helpers import send_from_directory
from flask.templating import render_template
from flask_socketio import SocketIO
import serial
from math import log2, pow


async_mode = None
app = Flask(__name__)
socket_ = SocketIO(app, async_mode=async_mode)
mbot = serial.Serial("/dev/ttyUSB0", 115200)
numCounts = [128, 64, 32, 16, 8, 4, 2, 1]
canvasArray = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

@app.route('/')
def index():
    return render_template("index.html")


@app.route('/manifest.json')
def manifest():
    return render_template("manifest.json")


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'), 'mbot.png', mimetype='image/png')



def renderToDraw(toDraw) :
    global canvasArray, numCounts
    drawTemp = [0, 0, 0, 0, 0, 0, 0, 0]
    
    for x in range(8):
        for y in range(16):
            if (toDraw['x'] == y and toDraw['y'] == x):
                drawTemp[x] = y + numCounts[x]
                print(drawTemp)
    
    mbot.write(('<r,').encode('utf-8'))
    for z in drawTemp:
        mbot.write((str(z)+',').encode('utf-8'))
    mbot.write(('>').encode('utf-8'))
            

@socket_.on('json')
def socketRecieve(dict):
    command = dict["data"]
    if command == "buzz":
        mbot.write('<b>'.encode('utf-8'))
        mbotResponse = mbot.readline()
        mbotResponse = mbotResponse.decode('utf-8')
        socket_.emit('response', mbotResponse)
    elif command == "move":
        mbot.write(('<m,' + str(dict["dir"]) + ',' +
                   str(dict["speed"]) + ">").encode('utf-8'))
        mbotResponse = mbot.readline()
        mbotResponse = mbotResponse.decode('utf-8')
        socket_.emit('response', mbotResponse)


@socket_.on('lightSensorPoll')
def lightSensor():
    mbot.write(('<s>').encode('utf-8'))
    mbotResponse = mbot.readline()
    mbotResponse = mbotResponse.decode('utf-8')
    socket_.emit('lightSensor', mbotResponse)


@socket_.on('led')
def ledSend(dict):
    for coords in dict['coords']:
        # mbot.write(('<l,'+str(coords['x'])+',' +
        #            str(coords['y'])+'>').encode('utf-8'))
        # mbotResponse = mbot.readline()
        # mbotResponse = mbotResponse.decode('utf-8')
        # socket_.emit('response', mbotResponse)
        renderToDraw(coords)

@socket_.on('clearScreen')
def clearScreen():
    mbot.write('<c>'.encode('utf-8'))

@socket_.on('sendText')
def sendTXT(dict):
    print(('<'+dict['text']+'>').encode('utf-8'))
    mbot.write(('<'+dict['text']+'>').encode('utf-8'))




# def queryMbotResponse():
#     mbotResponse = mbot.readline()
#     mbotResponse = mbotResponse.decode('utf-8')
#     if mbotResponse[1] == 'r':
#         # mbotResponse = mbotResponse[2:]
#         # socket_.emit('response', mbotResponse)
#         pass
#     else:
#         socket_.emit('lightSensor', mbotResponse)


if __name__ == '__main__':
    socket_.run(app)

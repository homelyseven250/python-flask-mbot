from flask import Flask, request
from flask.templating import render_template
import asyncio
import serial_asyncio
app = Flask(__name__)

toRespond = ""

class Output(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
        print('port opened', transport)
        transport.serial.rts = False

    def data_received(self, data):
        toRespond = repr(data)

    def connection_lost(self, exc):
        print('port closed')
        self.transport.loop.stop()

    def pause_writing(self):
        print('pause writing')
        print(self.transport.get_write_buffer_size())

    def resume_writing(self):
        print(self.transport.get_write_buffer_size())
        print('resume writing')

loop = asyncio.get_event_loop()
coro = serial_asyncio.create_serial_connection(loop, Output, '/dev/rfcomm5', baudrate=115200)
loop.run_until_complete(coro)
loop.run_forever()
loop.close()

@app.route('/')
def index():
    return render_template("index.html")


@app.route('/move')
def move():
    Output.transport.write(('<m,' + request.args.get('direction','') + ','+ request.args.get('speed','') + '>').encode('utf-8'))
    print("Command sent to mBot")
    return "Moving at speed " + str(request.args.get('speed')) + " in direction " + str(request.args.get('direction'))

@app.route('/buzz')
def buzz():
    Output.transport.write('<b>'.encode('utf-8'))
    print("Command sent to mBot")
    return "Buzzed"

@app.route('/stop')
def stop():
    Output.transport.write('<m,1,0>'.encode('utf-8'))
    return "Stopped"
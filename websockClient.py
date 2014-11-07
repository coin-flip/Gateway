__author__ = 'shams'
from websocket import create_connection
from jFlip import *
from services import *
import sys

ws = create_connection("ws://54.200.254.179/coinflip/socket.yaws")
connect = jFlip.connectToCloud("2Qsdfd34234",["Assal","Nawar","Mohsen","Shams"],"Shams_Laptop")
ws.send(connect)
rec = jFlip.receive(ws.recv())
myservices = services("services.ini")
services = myservices.getServices()

if(rec["returnCode"] == "Failed"):
    print "Failed reason: " + rec["failReason"]
    sys.exit()

while (True):
    rec = ws.recv()
    dec = jFlip.receive(rec)
    print dec
    if dec["action"] == action.getServices:
        s = []
        for service in services:  #should be in the database!
            x = jFlip.makeService(service["serviceName"],service["parameterCount"],service["parameters"],service["returnType"])
            s.append(x)
        y = jFlip.getServicesSuccess(s,dec["userID"])
        print y
        ws.send(y)
    elif dec["action"] == action.execService:
        arg = []
        call={}
        print dec["parameters"]
        print len(dec["parameters"])
        for param in  dec["parameters"]:
            arg.append(param["parameterValue"])

        for serv in services: #makesure
            if(serv["serviceName"]==dec["serviceName"]):
               call =  serv
               break
        assert(call!={})
        call["parameters"] = arg
        ret = myservices.executeService(call)
        s = jFlip.execServiceSuccess(str(dec["userID"]),str(dec["serviceName"]),str(call["returnType"]),str(ret))
        print s
        ws.send(s)

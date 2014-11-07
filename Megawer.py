import socket
import sys

host = '10.40.0.89' 
port = 8888 
size = 1024 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
s.connect((host,port)) 
s.send(sys.argv[1]) 
s.close() 

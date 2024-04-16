import socket
import struct

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the server's address and port
server_address = ('192.168.0.242', 8080)  # Use the IP address of the ESP32 device
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

try:
    # Receive the image size
    data = sock.recv(4)
    imageSize = struct.unpack('<I', data)[0]
    print('received image size: {}'.format(imageSize))

    # Receive the image data
    with open('received_image.jpg', 'wb') as f:
        while imageSize > 0:
            data = sock.recv(2048)
            f.write(data)
            imageSize -= len(data)

finally:
    print('closing socket')
    sock.close()
import socket
import keyboard
import time

# ESP32 IP-adresse
ESP32_IP = "192.168.137.91" 
ESP32_PORT = 80

def send_key_to_esp32(key):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((ESP32_IP, ESP32_PORT))
            s.sendall(key.encode())
    except Exception as e:
        print(f"Failed to send {key}: {e}")

key_mapping = {
    'up': "UP",
    'down': "DOWN",
    'left': "LEFT",
    'right': "RIGHT",
    'esc': "ESC" ,
    'hjem' : "HJEM"
}

print("Press arrow keys to send commands to ESP32. Press ESC to exit.")
har_kjort = []
while True:
    try:
        if keyboard.is_pressed('up'):
            send_key_to_esp32(key_mapping['up'])
            print("Sent: UP")
            har_kjort.insert(0, 'up')
            time.sleep(0.1)
        if keyboard.is_pressed('down'):
            send_key_to_esp32(key_mapping['down'])
            print("Sent: DOWN")
            har_kjort.insert(0, 'down')
            time.sleep(0.1)
        if keyboard.is_pressed('left'):
            send_key_to_esp32(key_mapping['left'])
            print("Sent: LEFT")
            har_kjort.insert(0, 'right')
            time.sleep(0.1)
        if keyboard.is_pressed('right'):
            send_key_to_esp32(key_mapping['right'])
            print("Sent: RIGHT")
            har_kjort.insert(0, 'left')
            time.sleep(0.1)                                 # Legger inn at båten har kjørt left, right for right, left fordi 
        if keyboard.is_pressed('esc'):                      # når den kjører tilbake må den kjøre speilvendt.
            send_key_to_esp32(key_mapping['esc'])
            print("Sent: ESC")
            break
        if keyboard.is_pressed('space'):
            send_key_to_esp32(key_mapping['hjem'])
            print("Sent: HJEM")
            time.sleep(4)
            print(har_kjort)
            for i in range(0, len(har_kjort)):
                send_key_to_esp32(key_mapping[har_kjort[i]])

    except Exception as e:
        print(f"Error: {e}")
        break

    time.sleep(0.1)
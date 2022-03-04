import socket
import os
from pathlib import Path
import subprocess
import sys

SERVER_PORT = 23
SERVER_HOST_BYTE = 111

def is_windows():
    return os.name == 'nt'

def get_ssid():
    if is_windows():
        network_Information = str(subprocess.check_output(["netsh","wlan","show","network"]))
        network_Information = network_Information.replace('\\r','')
        network_Information = network_Information.replace("b' ",'')
        network_Information = network_Information.replace(":",'\n')
        network_Information = network_Information.replace("\\n",'\n')
        network_Information = network_Information.splitlines()
        return network_Information[6][1:]
    else:
        return os.popen("sudo iwgetid -r").read()[:-1]
        #  subprocess.check_output(['sudo', 'iwgetid']).split('"')[1]
    #  print("Connected Wifi SSID: " + output.split('"')[1])

def get_subnet():
    # JANK CODE TO GET SUBNET
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    subnet = s.getsockname()[0].split('.')[2]
    s.close()
    return subnet


def get_firmware_network_info_path():
    dir_path = Path(os.path.dirname(os.path.realpath(__file__)))
    firmware_path = dir_path / '..' / 'mte380_main'
    network_info_path = firmware_path / 'network_info.h'
    return network_info_path

def copy_constants_into_firmware(subnet):
    path = get_firmware_network_info_path()
    lines = ['#ifndef NETWORK_INFO_H',
             '#define NETWORK_INFO_H',
             '',
             f'#define SERVER_PORT {SERVER_PORT}',
             f'#define SERVER_HOST_BYTE {SERVER_HOST_BYTE}',
             f'#define SERVER_SUBNET {subnet}'
             '',
             '#endif']
    with open(path, 'w') as f:
        f.write('\n'.join(lines) + '\n')
        

def network_setup():
    ssid = get_ssid()
    if ssid != 'EMU':
        print(f"Wrong network! {ssid} should be EMU")
        sys.exit()

    subnet = get_subnet()
    copy_constants_into_firmware(subnet)

    return "192.168.{subnet}.{SERVER_HOST_BYTE}"

import socket
import os
from pathlib import Path
import subprocess
import sys
import time

from util import is_windows

SERVER_PORT = 23

SERVER_HOST_BYTE = 111
if os.path.exists(os.path.join(os.getcwd(),'.use_alt_ip')):
    print('using alt ip')
    SERVER_HOST_BYTE = 222

PASSWORDS = {'EMU': '11111111', 'anavacadothanks': '42069666'}
ALLOWED_SSIDS = list(PASSWORDS.keys())
SKIP_SSIDS = ('eduroam',)

def get_ssid():
    try:
        if is_windows():
            current_network = subprocess.check_output(['netsh', 'wlan', 'show', 'interfaces']).decode('utf-8').split('\n')
            ssid_line = [x for x in current_network if 'SSID' in x and 'BSSID' not in x]
            if ssid_line:
                ssid_list = ssid_line[0].split(':')
                return (ssid_list[1].strip(), True)
        else:
            ssid = os.popen("sudo iwgetid -r").read()[:-1]
            if not ssid:
                # ethernet
                ssid = 'EMU'
            elif ssid not in ALLOWED_SSIDS:
                if not is_windows():
                    try:
                        print(f'Changing to EMU')
                        out = os.popen("nmcli dev wifi connect EMU")
                        time.sleep(6)
                        ssid = os.popen("sudo iwgetid -r").read()[:-1]
                    except Exception as e:
                        print(f'e: {e}')
                        return '', False
            return ssid, True

    except Exception as e:
        print(f'error getting SSID: {e}')
        proceed = input(f"Continue anyway? o: offline | n: no")
        if not proceed in ('o', 'e'):
            sys.exit()
        return ('', False)

def get_byte_n(n):
    # JANK CODE TO GET SUBNET
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    byte = s.getsockname()[0].split('.')[n]
    s.close()
    return byte


def get_firmware_network_info_path():
    dir_path = Path(os.path.dirname(os.path.realpath(__file__)))
    firmware_path = dir_path / '..' / 'mte380_main'
    network_info_path = firmware_path / 'network_info.h'
    return network_info_path

def copy_constants_into_firmware(ip_bytes, ssid):
    if ssid in SKIP_SSIDS:
        return
    path = get_firmware_network_info_path()
    lines = ['#ifndef NETWORK_INFO_H',
             '#define NETWORK_INFO_H',
             '']
    for i in range(4):
        lines.append(f'#define SERVER_BYTE{i} {ip_bytes[i]}')

    try:
        lines += [f'#define SERVER_PORT {SERVER_PORT}',
                  f'#define WIFI_SSID "{ssid}"',
                  f'#define WIFI_PASSWORD "{PASSWORDS[ssid]}"',
                 '#endif']
    except:
        return
    with open(path, 'w') as f:
        f.write('\n'.join(lines) + '\n')
        

def network_setup():
    ssid, online = get_ssid()
    if not online:
        return
    if ssid not in ALLOWED_SSIDS:
        proceed = input(f"Wrong network! {ssid} should be in {ALLOWED_SSIDS}. Continue offline? <y/n/e>")
        if proceed == 'e':
            ssid = 'anavacadothanks'
            online = True
        elif not proceed == 'y':
            sys.exit()
        else:
            return

    ip_bytes = [get_byte_n(i) for i in range(3)]
    ip_bytes.append(SERVER_HOST_BYTE)
    ip_bytes = [str(i) for i in ip_bytes]
    copy_constants_into_firmware(ip_bytes, ssid)

    return ".".join(ip_bytes)

import serial
import io
import logging
from enum import Enum
import threading
import time
import datetime
from settings import Settings

from dash_data_container import DashData

# notes: 
# super broken (from old project) but useful starting point
# listing ports: python -m serial.tools.list_ports will print a list of available ports. It is also possible to add a regexp as first argument and the list will only include entries that matched.

# see documentation https://pythonhosted.org/pyserial/pyserial_api.html#classes
# screw with ports, id, parity, stopbits, rtscts
# device.in_waiting and .out_waiting gives num bytes in input and output buffers

class UartThread(threading.Thread):
    def __init__(self, testing, port='/dev/ttyACM0'):
        super().__init__()
        self.testing = testing
        self.settings = Settings()
        if not testing:
            self.device = serial.Serial(port, baudrate=9600, timeout=5, write_timeout=3)
            self.connect()
        self.logger = logging.Logger('logger')

        self.data = DashData()

    def connect(self):
        try:
            self.device.open()
        except serial.SerialException:
            #  self.logger.log("Error: Failed to open serial port.", print_line=True)
            return False
        return True

    def disconnect(self):
        if self.device.is_open:
            self.device.close()

    def decode_assign(self, string):
        try:
            component_id, val = string.split('=')
            timestamp = datetime.datetime.now()
            self.data.update(component_id, val, timestamp)
            #  plot_dict['data']['Y'].append(plots[name]['type'](val))

            #  plot_dict['data']['X'].append(timestamp)
        except Exception as e:
            print(f'error: could not parse serial data:\n {string}\n{e}')


    def run(self):
        if not self.testing:
            while(True):
                if (self.device.in_waiting):
                    line = self.device.readline().decode().rstrip('\r\n')
                    self.decode_assign(line)
        else: # testing
            while(True):
                for component_id, component_obj in self.data.mappings.items():
                    next_val = component_obj.generate_next_test_value()
                    self.decode_assign(f'{component_id}={next_val}')
                time.sleep(1/self.settings.arduino_hz)
    
    def serial_out(self, string):
        string += "\n"
        self.device.write(string_.encode())

if __name__ == "__main__":
    uart_thread = UartThread(True)
    uart_thread.start()
    uart_thread.join()

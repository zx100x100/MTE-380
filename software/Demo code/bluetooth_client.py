import asyncio
from bleak import BleakScanner
from bleak import discover
from bleak import BleakClient
import logging
import sys

#  async def main():
    #  devices = await BleakScanner.discover()
    #  for d in devices:
        #  print(d)

#  asyncio.run(main())


devices_dict = {}
devices_list = []
receive_data = []

class Bluetooth:
    def __init__(self):
        self.connection_enabled = False
        self.log = logging.getLogger(__name__)

        self.log.setLevel(logging.DEBUG)
        h = logging.StreamHandler(sys.stdout)
        h.setLevel(logging.DEBUG)
        self.log.addHandler(h)

    async def run(self, address, debug=False):
        #  async with BleakClient(address) as client:
            #  x = await self.client.is_connected()
            #  self.log.info("Connected: {0}".format(x))

        for service in self.client.services:
            self.log.info("[Service] {0}: {1}".format(service.uuid, service.description))
            for char in service.characteristics:
                if "read" in char.properties:
                    try:
                        value = bytes(await self.client.read_gatt_char(char.uuid))
                        print('hiiiiiiii')
                    except Exception as e:
                        value = str(e).encode()
                else:
                    value = None
                self.log.info(
                    "\t[Characteristic] {0}: (Handle: {1}) ({2}) | Name: {3}, Value: {4} ".format(
                        char.uuid,
                        char.handle,
                        ",".join(char.properties),
                        char.description,
                        value,
                    )
                )
                for descriptor in char.descriptors:
                    value = await self.client.read_gatt_descriptor(descriptor.handle)
                    self.log.info(
                        "\t\t[Descriptor] {0}: (Handle: {1}) | Value: {2} ".format(
                            descriptor.uuid, descriptor.handle, bytes(value)
                        )
                    )

                #Characteristic uuid
                CHARACTERISTIC_UUID = "cba1d466-344c-4be3-ab3f-189f80dd7518"

                await self.client.start_notify(CHARACTERISTIC_UUID, notification_handler)
                await asyncio.sleep(5.0)
                await self.client.stop_notify(CHARACTERISTIC_UUID)

    async def connect_to_device(self):
        while True:
            print(f'hi {self.connection_enabled}')
            if self.connection_enabled:
                try:
                    await self.client.connect()
                    self.connected = await self.client.is_connected()
                    self.log.info(f'hi2: {self.connected}')
                    if self.connected:
                        self.log.info("Connected to Device")
                        self.client.set_disconnected_callback(self.on_disconnect)
                        await self.client.start_notify(
                            self.notify_characteristic, self.notify_callback,
                        )
                        while True:
                            if not self.connected:
                                break
                            await asyncio.sleep(1.0)
                    else:
                        self.log.info("Failed to connect")
                except Exception as e:
                    print(e)
            else:
                await asyncio.sleep(1.0)

#To discover BLE devices nearby 
async def scan():
    dev = await discover()
    for i in range(0,len(dev)):
        #Print the devices discovered
        print("[" + str(i) + "]" + dev[i].address,dev[i].name,dev[i].metadata["uuids"])
        #Put devices information into list
        devices_dict[dev[i].address] = []
        devices_dict[dev[i].address].append(dev[i].name)
        devices_dict[dev[i].address].append(dev[i].metadata["uuids"])
        devices_list.append(dev[i].address)

#An easy notify function, just print the recieve data
def notification_handler(sender, data):
    print(', '.join('{:02x}'.format(x) for x in data))


if __name__ == "__main__":
    print("Scanning for peripherals...")

    #Build an event loop
    loop = asyncio.get_event_loop()
    #Run the discover event
    loop.run_until_complete(scan())

    #let user chose the device
    #  index = input('please select device from 0 to ' + str(len(devices_list)) + ":")
    #  index = devices_list.index('EMU')
    print(f'devices_list: {devices_list}')
    desired_val = next((k for k,v in devices_dict.items() if v[0] == 'EMU'), None)
    print(desired_val)
    index = devices_list.index(desired_val)
    address = devices_list[index]

    print("Address is " + address)

    #Run notify event
    #  loop = asyncio.get_event_loop()
    bt = Bluetooth()
    bt.client = BleakClient(address, loop=loop)
    asyncio.ensure_future(bt.connect_to_device(), loop=loop)
    loop.set_debug(True)
    bt.connection_enabled = True
    loop.run_until_complete(bt.run(address, True))

import pexpect
import sys
DEVICE = "B8:F0:09:CD:38:AA"   # address of your device
if len(sys.argv) == 2:
  DEVICE = str(sys.argv[1])
# Run gatttool interactively.
child = pexpect.spawn("gatttool -I")

# Connect to the device.
print("Connecting to:"),
print(DEVICE)
NOF_REMAINING_RETRY = 3
while True:
  try:
    child.sendline("connect {0}".format(DEVICE))
    child.expect("Connection successful", timeout=5)
  except pexpect.TIMEOUT:
    NOF_REMAINING_RETRY = NOF_REMAINING_RETRY-1
    if (NOF_REMAINING_RETRY>0):
      print("timeout, retry...")
      continue
    else:
      print("timeout, giving up.")
      break
  else:
    print("Connected!")
    child.sendline("char-read-hnd 0x0001") # put the handle you want to read
    child.expect("Characteristic value/descriptor: ", timeout=5)
    child.expect("\r\n", timeout=5)
    print("Presence Sensor: "),
    print(child.before),
    print(chr(int(child.before[0:2],16)))
    break

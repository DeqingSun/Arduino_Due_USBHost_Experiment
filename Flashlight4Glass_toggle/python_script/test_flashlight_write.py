import sys
sys.path.append('usb')

import usb.core
import usb.util

# find our device
dev = usb.core.find(idVendor=0x4207, idProduct=0x20A0)

# was it found?
if dev is None:
	raise ValueError('Device not found')
	

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration()

## get an endpoint instance
#cfg = dev.get_active_configuration()
#interface_number = cfg[(0,0)].bInterfaceNumber
#alternate_settting = usb.control.get_interface(interface_number)
#intf = usb.util.find_descriptor(
#    cfg, bInterfaceNumber = interface_number,
#    bAlternateSetting = alternate_setting
#)

for cfg in dev:
    print(str(cfg.bConfigurationValue) + '\n')
    for intf in cfg:
        print('\t' + \
                         str(intf.bInterfaceNumber) + \
                         ',' + \
                         str(intf.bAlternateSetting) + \
                         '\n')
        for ep in intf:
            print('\t\t' + \
                             str(ep.bEndpointAddress) + \
                             '\n')


intf = cfg[(0,0)]
ep = intf[1]
dev.write(1, 'FT', 0)
#dev.write(1, '\x80', 0)
#dev.write(1, '\x81', 0)
#ret = dev.read(0x81, 2, 0, 100)
#print ret
#ep.write('\x80')
							 
							 

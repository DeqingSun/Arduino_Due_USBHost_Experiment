#include "testUSB.h"

EndpointAccess::EndpointAccess(USBHost *p):
  pUsb(p), ready(false)
{
  // Initialize endpoint data structures
  for (uint32_t i = 0; i < EA_MAX_ENDPOINTS; ++i)
  {
    epInfo[i].deviceEpNum	= 0;
    epInfo[i].hostPipeNum	= 0;
    epInfo[i].maxPktSize	= (i) ? 0 : 8;
    epInfo[i].epAttribs		= 0;
    epInfo[i].bmNakPower  	= (i) ? USB_NAK_NOWAIT : USB_NAK_MAX_POWER;
  }

  if (pUsb)
  {
    pUsb->RegisterDeviceClass(this);
  }
}

uint32_t EndpointAccess::Init(uint32_t parent, uint32_t port, uint32_t lowspeed)
{

  uint8_t	buf[sizeof(USB_DEVICE_DESCRIPTOR)];
  uint32_t	rcode = 0;
  UsbDevice	*p = NULL;
  EpInfo	*oldep_ptr = NULL;
  uint32_t	num_of_conf = 0;

  // Get memory address of USB device address pool
  AddressPool	&addrPool = pUsb->GetAddressPool();

  printf("EndpointAccess::Init\r\n");

  // Check if address has already been assigned to an instance
  if (bAddress)
  {
    return USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE;
  }

  // Get pointer to pseudo device with address 0 assigned
  p = addrPool.GetUsbDevicePtr(0);

  if (!p)
  {
    return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;
  }

  if (!p->epinfo)
  {
    return USB_ERROR_EPINFO_IS_NULL;
  }

  // Save old pointer to EP_RECORD of address 0
  oldep_ptr = p->epinfo;

  // Temporary assign new pointer to epInfo to p->epinfo in order to avoid toggle inconsistence
  p->epinfo = epInfo;

  p->lowspeed = lowspeed;

  // Get device descriptor
  rcode = pUsb->getDevDescr(0, 0, sizeof(USB_DEVICE_DESCRIPTOR), (uint8_t*)buf);

  // Restore p->epinfo
  p->epinfo = oldep_ptr;

  if (rcode)
  {
    goto FailGetDevDescr;
  }

  // Allocate new address according to device class
  bAddress = addrPool.AllocAddress(parent, false, port);

  // Extract Max Packet Size from device descriptor
  epInfo[0].maxPktSize = (uint8_t)((USB_DEVICE_DESCRIPTOR*)buf)->bMaxPacketSize0;

  // Assign new address to the device
  rcode = pUsb->setAddr(0, 0, bAddress);
  if (rcode)
  {
    p->lowspeed = false;
    addrPool.FreeAddress(bAddress);
    bAddress = 0;
    printf("EndpointAccess::Init : setAddr failed with rcode %lu\r\n", rcode);
    return rcode;
  }

  printf("EndpointAccess::Init : device address is now %lu\r\n", bAddress);

  p->lowspeed = false;

  //get pointer to assigned address record
  p = addrPool.GetUsbDevicePtr(bAddress);
  if (!p) return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;
  p->lowspeed = lowspeed;

  // Assign epInfo to epinfo pointer - only EP0 is known
  rcode = pUsb->setEpInfoEntry(bAddress, 1, epInfo);
  if (rcode)
  {
    goto FailSetDevTblEntry;
  }

  if (((USB_DEVICE_DESCRIPTOR*)buf)->idVendor == EA_VID &&
      (((USB_DEVICE_DESCRIPTOR*)buf)->idProduct == EA_PID))
  {
    printf("EndpointAccess::Init : device ID MATCH\r\n");
    ready = true;
  } else {
    printf("EndpointAccess::Init : device ID MISMATCH\r\n");
  }
  return 0;

  // Diagnostic messages
FailGetDevDescr:
  printf("EndpointAccess::Init getDevDescr : ");
  goto Fail;

FailSetDevTblEntry:
  printf("EndpointAccess::Init setDevTblEn : ");
  goto Fail;

Fail:
  printf("error code: %lu\r\n", rcode);
  Release();
  return rcode;
}

uint32_t EndpointAccess::Release()
{
  printf("EndpointAccess::Release\r\n");

  // Free allocated USB address
  pUsb->GetAddressPool().FreeAddress(bAddress);

  // Must have to be reset to 1
  bNumEP = 1;

  bAddress = 0;
  ready = false;

  return 0;
}

uint32_t EndpointAccess::Poll()
{
  //Serial.println("ADK::Poll\r\n");

  uint32_t rcode = 0;

  /*if (!bPollEnable)
  	return 0;

  if (qNextPollTime <= millis())
  {
  	qNextPollTime = millis() + 10;

  	const uint32_t const_buff_len = 16;
  	uint8_t buf[const_buff_len];

  	uint32_t read = epInfo[epInterruptInIndex].maxPktSize;

  	rcode = pUsb->inTransfer(bAddress, epInfo[epInterruptInIndex].deviceEpNum, &read, buf);

  	if (rcode)
  	{
  		return rcode;
  	}

  	if (pRptParser)
  		pRptParser->Parse((HID*)this, 0, (uint32_t)read, buf);
  }*/

  return rcode;
}

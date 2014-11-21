#include "DueHost.h"

const uint32_t EndpointAccess::epDataInIndex  = 1;
const uint32_t EndpointAccess::epDataOutIndex = 2;

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

    /* Go through configurations, find first bulk-IN, bulk-OUT EP, fill epInfo and quit */
    num_of_conf = ((USB_DEVICE_DESCRIPTOR*)buf)->bNumConfigurations;

    printf("EndpointAccess::Init : number of configuration is %lu\r\n", num_of_conf);

    for (uint32_t i = 0; i < num_of_conf; ++i)
    {
      ConfigDescParser<0, 0, 0, 0> confDescrParser(this);

      delay(1);
      rcode = pUsb->getConfDescr(bAddress, 0, i, &confDescrParser);


      if (rcode)
      {
        goto FailGetConfDescr;
      }

      if (bNumEP > 2)
      {
        break;
      }
    }

    if (bNumEP == 3)
    {
      // Assign epInfo to epinfo pointer - this time all 3 endpoins
      rcode = pUsb->setEpInfoEntry(bAddress, 3, epInfo);
      if (rcode)
      {
        goto FailSetDevTblEntry;
      }
    }

    // Set Configuration Value
    rcode = pUsb->setConf(bAddress, 0, bConfNum);
    if (rcode)
    {
      goto FailSetConf;
    }

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

FailGetConfDescr:
  printf("EndpointAccess::Init getConf : ");
  goto Fail;

FailSetConf:
  printf("EndpointAccess::Init setConf : ");
  goto Fail;

Fail:
  printf("error code: %lu\r\n", rcode);
  Release();
  return rcode;
}

void EndpointAccess::EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *pep)
{
  if (bNumEP == 3)
  {
    return;
  }

  bConfNum = conf;

  uint32_t index = 0;
  uint32_t pipe = 0;

  if ((pep->bmAttributes & 0x02) == 2)
  {
    index = ((pep->bEndpointAddress & 0x80) == 0x80) ? epDataInIndex : epDataOutIndex;
  }

  // Fill in the endpoint info structure
  epInfo[index].deviceEpNum = pep->bEndpointAddress & 0x0F;
  epInfo[index].maxPktSize = pep->wMaxPacketSize;

  printf("EndpointAccess::EndpointXtract : Found new endpoint\r\n");
  printf("EndpointAccess::EndpointXtract : deviceEpNum: %lu\r\n", epInfo[index].deviceEpNum);
  printf("EndpointAccess::EndpointXtract : maxPktSize: %lu\r\n", epInfo[index].maxPktSize);
  printf("EndpointAccess::EndpointXtract : index: %lu\r\n", index);

  if (index == epDataInIndex)
    pipe = UHD_Pipe_Alloc(bAddress, epInfo[index].deviceEpNum, UOTGHS_HSTPIPCFG_PTYPE_BLK, UOTGHS_HSTPIPCFG_PTOKEN_IN, epInfo[index].maxPktSize, 0, UOTGHS_HSTPIPCFG_PBK_1_BANK);
  else if (index == epDataOutIndex)
    pipe = UHD_Pipe_Alloc(bAddress, epInfo[index].deviceEpNum, UOTGHS_HSTPIPCFG_PTYPE_BLK, UOTGHS_HSTPIPCFG_PTOKEN_OUT, epInfo[index].maxPktSize, 0, UOTGHS_HSTPIPCFG_PBK_1_BANK);

  // Ensure pipe allocation is okay
  if (pipe == 0)
  {
    printf("EndpointAccess::EndpointXtract : Pipe allocation failure\r\n");
    // Enumeration failed, so user should not perform write/read since isConnected will return false
    return;
  }

  epInfo[index].hostPipeNum = pipe;

  bNumEP++;
}

uint32_t EndpointAccess::Release()
{
  printf("EndpointAccess::Release\r\n");

  // Free allocated host pipes
  UHD_Pipe_Free(epInfo[epDataInIndex].hostPipeNum);
  UHD_Pipe_Free(epInfo[epDataOutIndex].hostPipeNum);

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
  uint32_t rcode = 0;
  return rcode;
}

uint32_t EndpointAccess::read(uint32_t *nreadbytes, uint32_t datalen, uint8_t *dataptr)
{
  *nreadbytes = datalen;
  return pUsb->inTransfer(bAddress, epInfo[epDataInIndex].deviceEpNum, nreadbytes, dataptr);
}

uint32_t EndpointAccess::write(uint32_t datalen, uint8_t *dataptr)
{
  return pUsb->outTransfer(bAddress, epInfo[epDataOutIndex].deviceEpNum, datalen, dataptr);
}


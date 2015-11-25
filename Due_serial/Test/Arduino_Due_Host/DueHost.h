#ifndef DUEHOST_H_
#define DUEHOST_H_

#include <Usb.h>
#include <Arduino.h>
#include <confdescparser.h>

#define EA_VID   0x03EB
#define EA_PID   0x204F

#define EA_MAX_ENDPOINTS 3 //endpoint 0, bulk_IN, bulk_OUT

class EndpointAccess : public USBDeviceConfig, public UsbConfigXtracter
{
  protected:
    static const uint32_t epDataInIndex;			// DataIn endpoint index
    static const uint32_t epDataOutIndex;			// DataOUT endpoint index

    /* Mandatory members */
    USBHost		*pUsb;
    uint32_t	bAddress;							// Device USB address
    uint32_t	bConfNum;							// configuration number

    uint32_t	bNumEP;								// total number of EP in the configuration
    bool		ready;

    /* Endpoint data structure describing the device EP */
    EpInfo		epInfo[EA_MAX_ENDPOINTS];


  public:
    EndpointAccess(USBHost *p);

    // Methods for receiving and sending data
    uint32_t read(uint32_t *nreadbytes, uint32_t datalen, uint8_t *dataptr);
    uint32_t write(uint32_t datalen, uint8_t *dataptr);

    // USBDeviceConfig implementation
    virtual uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed);
    virtual uint32_t Release();
    virtual uint32_t Poll();

    virtual uint32_t GetAddress() {
      return bAddress;
    };

    virtual bool isReady() {
      return ready;
    };

    // UsbConfigXtracter implementation
    virtual void EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt, uint32_t proto, const USB_ENDPOINT_DESCRIPTOR *ep);
};

#endif


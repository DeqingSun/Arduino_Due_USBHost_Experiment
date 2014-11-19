#ifndef TESTUSB_H_
#define TESTUSB_H_

#include <Usb.h>
#include <Arduino.h>

#define EA_VID   0x4207
#define EA_PID   0x20A0

#define EA_MAX_ENDPOINTS 3 //endpoint 0, bulk_IN, bulk_OUT

class EndpointAccess : public USBDeviceConfig
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

};

#endif


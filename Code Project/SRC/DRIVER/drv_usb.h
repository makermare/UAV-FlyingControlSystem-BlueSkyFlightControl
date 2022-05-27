#ifndef _DRV_USB_H
#define	_DRV_USB_H

#include "board.h"

void Usb_Init(void);
void Usb_Send(uint8_t *dataToSend, uint8_t length);

#endif

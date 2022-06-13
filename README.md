# FakeTag
firmware for nRF51 chips that is coincidentally compatible with the FindMy (AirTag) ecosystem.

You can read about this project [here](https://hackaday.com/2022/05/30/check-your-mailbox-using-the-airtag-infrastructure/).

It sends out advertisement keys defined in [keys.h](keys.h), rotating keys every hour.

Also, an input pin can be defined. Whenever that input pin is triggered, the status byte increments by one.
The first two bits of the status byte contain battery information, the other six bits contain the counter in my code.

I use this for my Mailbox-sensor.
There is a vibraion sensor attached to the board, and whener the Mailbox is opened the vibration sensor triggers, which 
makes the chip send out a different status byte.
That status byte, traveling through the FindMy network and [OpenHaystack](https://github.com/seemoo-lab/openhaystack), reaches my smarthome and I get a notification.

![PXL_20220504_162815409](https://user-images.githubusercontent.com/26143255/167520287-0ad64cf0-5481-4109-9714-0aaf7d0d3763.jpg)

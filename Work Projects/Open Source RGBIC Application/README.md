# Open Source RGB LED with Embedded IC
When the ISELED (Intelligent Smart Embedded LED) Alliance was announced in 2016 for BMW, other automotive OEMs also began sourcing for an alternative solution to the ISELED packages.

Since the ISELED packages are based on proprietary technology, a lot of the finer details of the ISELED devices are hidden from its users, allowing for less customisability as everything has been wrapped around an API.

An alternative solution which Everlight bring forward is the Open Source RGB LED with Embedded IC. This type of package is called the Open Source Solution as this type of package is not using proprietary transmission protocol, instead the protocol is based on common communication protocol such as UART and SPI. It offers the user more room to design their modules, with less restriction on the control over the driver IC.

## Overview of Open Source RGB LED Demo Module
One of the key difference between this type of package and the ISELED solution is the flexibility of transmission speed, as the user can modify the transmission speed based on their use. An example can be seen below where 30pcs of said package are connected sequentially and it is being operated at 12MHz. The clip is captured in slow motion to visualise what is actually happening.

![Alt Text](./src/Demo.gif)

In actual practice, 12Mhz may sound a bit extreme for applications in interior lighting, however, based on the roadmap provided by many automotive OEMs, where each car will use up to 300~400 RGB LEDs with embedded IC, higher transmission speed will prevent frame lags from happening (where the end of the LED chain is still operating on the previous frame and the new frame has already begun at the start of the chain).

## Summary
This project contains a UAVCAN example project for STM32F4DISCOVERY board. It
communicates UAVCAN protocol version 0.8 and can be conditionally compiled to
be a server or a client.
Features:
- Uses free Atollic True Studio IDE from STM (v9.3.0)
- Project uses C++11
- Uses UAVCAN version 0.8
- Distributed under MIT style license
    - Generic Atollic and STM project contents licensed under STM copyright (MIT)
    - UAVCAN library and generic example contents licensed under Zubax copyright (MIT)
    - Custom code of the project licensed under Janaka S. copyright (MIT)

## Hardware requirements
- STM32F4DISCOVERY board
- CAN bus Transceiver (E.g.: TJA1050)
- Terminated CAN bus.  E.g.:
    - USB to CAN dongle with termination resistors
    - 2nd STM32F4DISCOVERY board using different CAN address and with a termination resistors
    - Other CAN bus with termination resistors
- [Recommended Hardware]
    - Zubax Uavcan sniffer dongle with Uavcan GUI Tool

### How to connect Hardware
TODO:####

## How to compile
- TODO ####
- Enjoy!
	
## TODOs
- Upgrade to use UAVCAN 1.0 or later
- Add FreeRTOS to the project

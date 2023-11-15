# lwIOlinkMaster
**Library is still under development**

A light weight IO link master stack for the arduino framework. Integrate basic port functionality and extract data from IO-link devices.

## Description

This library allows for basic communication from an ESP32 processor to a TIOL11X master PHY to extract data from IO-link devices.  The library is structured to allow for the interface protocol between the ESP32 and the PHY to be abstracted for future growth.  Additionally, handles for changing the port mode to a DI or DO mode vs. standard SIO (IO-Link communication).

## Getting Started

### Dependencies

* arduino-esp32 [library](https://github.com/espressif/arduino-esp32)

## Roadmap

- [ ] Implement master.begin(); is called initialize all ports based on their settings passing through a pointer to the master
- [ ] Implement master.refresh() - Detect and update any changes to a port configuration. Update the IO-link device subscriptions.
- [ ] Implement m-sequence control for SIO interface with probes.
- [ ] Handle Fault Logic from PHY
- [ ] Hardware validation

## References

- IO-link References:
    - IO link 1.1.3 Specification: [here](https://io-link.com/share/Downloads/Package-2020/IOL-Interface-Spec_10002_V113_Jun19.pdf)
    - IO-link system description: [here](https://io-link.com/share/Downloads/At-a-glance/IO-Link_System_Description_eng_2018.pdf)
    - Maxim IO-link handbook: [here](https://www.maximintegrated.com/content/dam/files/design/technical-documents/handbooks/io-link-handbook.pdf)
- Texas Instruments References: 
    - IO-Link Master Example Project: [here](https://software-dl.ti.com/mcu-plus-sdk/esd/AM243X/08_04_00_17/exports/docs/industrial_protocol_docs/am243x/iolink_master/page_quickstart.html)
    - PHY TIOL112:
        - Spec Sheet: [here](https://www.ti.com/lit/ds/symlink/tiol112.pdf)
        - TIOX1X2X Eval Module: [here](https://www.ti.com/lit/ug/sllu344/sllu344.pdf)
- Espressif References:
    - ESP32S3:
        - Spec Sheet: [here](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)


## Authors

Contributors:
- [unref-ptr](https://github.com/unref-ptr)

## Version History

* 0.0.1
    * Initial Release

## License

This project is licensed under the GNU GENERAL PUBLIC LICENSE License - see the LICENSE.md file for details
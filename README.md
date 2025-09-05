# 🔌 Remote Relay Project

This project involves the development of a **remote actuation system** for electrical loads and equipment, based on the **WT32-ETH01** module. The module integrates an **ESP32-S** microcontroller with a **LAN8720 Ethernet PHY**, enabling wired network connectivity via the IEEE 802.3 standard.

The system communicates with a **TCA9555 digital I/O expander** over an **I²C** bus, providing scalable control of multiple digital outputs. Communication with a remote **MQTT broker** is performed via Ethernet, leveraging the lightweight publish/subscribe messaging protocol to ensure efficient and reliable data exchange in IoT applications.

📍 **Deployment Location**: Pico dos Dias Observatory, Itajubá, Minas Gerais, Brazil  
🔧 **Application**: Infrastructure automation and remote equipment control  
📡 **Technologies**: ESP32, LAN8720, MQTT, I2C, Ethernet, RMII

---

## 🌐 Ethernet PHY Overview

The image below illustrates a comparison between the **ISO/OSI Ethernet model** and internet protocols such as HTTP:

<img alt="OSI model vs Internet Protocols" src="./img/internet_model.png" width="500"/>

The **MAC (Media Access Controller)** is integrated into the ESP32 and manages the **Data Link Layer**. The **PHY (Physical Layer Transceiver)** handles the electrical signaling and physical connection to the network, interfacing with the MAC via standard protocols.

The ESP32 communicates with the LAN8720 using one of the following interfaces:

- **MII** (Media Independent Interface)
- **RMII** (Reduced Media Independent Interface) ← _used by this project_

---

### 📡 MII – Media Independent Interface

MII includes two main components:

- **Data Interface** – For sending/receiving Ethernet frames  
- **PHY Management Interface (MDIO / MIIM)** – For configuration and monitoring

#### Hardware Signals:
- `MDC`: Management Data Clock (driven by MAC)
- `MDIO`: Management Data I/O (bi-directional)

<img alt="MII Interface Diagram" src="./img/MII.png" width="500"/>

---

### ⚡ RMII – Reduced Media Independent Interface

RMII is the interface used by the WT32-ETH01 and LAN8720. Features include:

- ✅ Supports **10 Mbps** and **100 Mbps**
- ✅ Requires **50 MHz reference clock**
- ✅ Shared clock between MAC and PHY
- ✅ Independent 2-bit TX and RX data paths

<img alt="RMII Interface Diagram" src="./img/RMII.png" width="500"/>

---

## 🧩 WT32-ETH01 Ethernet Hardware

The **WT32-ETH01** with **LAN8720** includes an external **50 MHz oscillator**. The **ESP32's GPIO0** is used to input the clock to synchronize MAC and PHY layers.

However, **GPIO0** also controls **boot mode**:  
If held low during reset → programming mode  
If high or floating → normal boot mode

> ⚠️ **Conflict Resolution**:  
> To avoid conflicts, the oscillator is disabled at reset using **GPIO16**, and later re-enabled by software after boot.

### 🧠 GPIO Summary

| Function         | GPIO Pin | Description                                  |
|------------------|----------|----------------------------------------------|
| MDC              | GPIO23   | Management Data Clock                        |
| MDIO             | GPIO18   | Management Data I/O                          |
| Clock Input      | GPIO0    | Receives 50 MHz from oscillator              |
| Oscillator Enable| GPIO16   | Enables oscillator after boot                |
| PHY Reset        | —        | Automatically reset at startup (not GPIO)    |

---

## 🖥️ WT32-ETH01 Pinout

<img alt="WT32-ETH01 Pinout Diagram" src="./img/wt32_eth01_proj_pinout.jpg" width="800"/>

---

## ⚠️ Important Pin Usage Notes

> These GPIOs have restrictions during **ESP32 boot process**. Misuse can prevent boot or cause undefined behavior.

- **GPIO0**: Must be LOW to program; used for 50 MHz clock after boot. Avoid reusing.
- **GPIO1**: Serial TX (used for programming). Avoid reusing.
- **GPIO2**: Must be LOW/floating during boot. Usable post-boot.
- **GPIO3**: Serial RX (used for programming). Avoid reusing.
- **GPIO5, GPIO15**: Affects serial debug and SD interface at boot. Use with caution.
- **GPIO12**: Must be LOW/floating at boot to avoid voltage issues.
- **GPIO35, GPIO36, GPIO39**: Input-only. Safe for sensing applications.

✅ **Wi-Fi** functionality remains available and unaffected.

---

## 📚 References

- 🔗 [esp32-ethernet-lan8720 by Zelmoghazy](https://github.com/Zelmoghazy/esp32-ethernet-lan8720)
- 🔗 [WT32-ETH01-LAN-8720-RJ45 by ldijkman](https://github.com/ldijkman/WT32-ETH01-LAN-8720-RJ45-)
- 🔗 [wt32-eth01 by egnor](https://github.com/egnor/wt32-eth01)

---

## 📌 License

This project is open-source. Please check the [LICENSE](./LICENSE) file for more information.
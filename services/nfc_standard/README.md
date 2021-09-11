# NFC<a name="EN-US_TOPIC_0000001133207781"></a>

-   [Introduction](#section13587125816351)
-   [Architecture](#section13587185873516)
-   [Directory Structure](#section161941989596)
-   [Usage](#section1312121216216)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section13587125816351"></a>

Near-field communication \(NFC\) is a non-contact identification and interconnection technology for short-distance wireless communication between mobile devices, consumer electronic products, PCs, and smart devices.

The NFC module provides NFC control, tag reading and writing, secure element access, card emulation, and NFC notification.

The  **NfcController**  class provides features such as NFC control and status query. Tag reading and writing are provided by a variety of tag subclasses that match different technologies or protocols. The  **SEService**  class provides access to secure elements on a device. The  **CardEmulation**  class provides methods for enabling and disabling card emulation.

## Architecture<a name="section13587185873516"></a>

**Figure  1**  NFC architecture<a name="fig4460722185514"></a>  


![](figures/en-us_image_0000001086731550.gif)

## Directory Structure<a name="section161941989596"></a>

The main code directory structure of Intelligent Soft Bus is as follows:

```
/foundation/communication/nfc
├── interfaces         # API code
│   └── kits           # APIs
│       └── nifckit    # NFC APIs
│           ├── java   # Implementation code for Java APIs
│           └── test   # Test code for Java APIs
├── services           # Service
│   ├── nfc_standard   # NFC services
│   │   ├── etc        # NFC service startup configuration
│   │   ├── include    # Header files
│   │   └── src        # Source code

```

## Usage<a name="section1312121216216"></a>

（1） NFC control

Applications or other modules can implement the following functions related to card emulation through corresponding APIs:

1.  Check whether a device supports NFC.
2.  Enable or disable NFC on a device.

（2）Reading the tag type

1.  Obtain  **TagInfo**  from  **Intent**  and initialize the  **TagInfo**  instance.
2.  Call the  **getTagSupportedProfiles\(\)**  method to obtain the technologies or protocols supported by the tag.
3.  Call the  **isProfileSupported\(int profile\)**  method to check whether NFC-A, ISO-DEP, and MIFARE Classic are supported. If they are supported, use the  **TagInfo**  instance to create an NFC-A, ISO-DEP, or MIFARE Classic tag.
4.  Call different methods to access tags based on the tag technology type.


## Repositories Involved<a name="section1371113476307"></a>

hmf/communication/nfc


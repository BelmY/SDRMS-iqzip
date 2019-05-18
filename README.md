## iqzip: A library to facilitate the creation of signal databases.

The iqzip library provides a simple API to facilitate the compression/decompression of raw IQ signal recordings and their characterization through
metafile generation.

## Introduction
Efficiently storing and processing signal recordings has always been a challenging problem for scientist and signal analysts, due to their
excessive storage requirements. In addition, the emerging interest in combined applications of different scientific fields, as well
space telecommunications, require a robust and portable across multiple platforms specification to describe databases of signal recordings.

The iqzip library provides the required functionallity to efficiently store IQ data by incorporating the compression scheme proposed by the CCSDS
121.0-B-2 "Lossless Data Compression" [1]. Moreover, given a simplified set of functions, there is the
ability to fully describe the compressed signal recordings, by generating appropriate metafiles that conform the Signal Meta File (SigMF) [2] specification format.

## Compression

The iqzip library wraps and extends the libaec [3], a C implementation of the compressing Adaptive Entropy
coder algorithm proposed by the CCSDS 121.0-B standard, to provide a simplified API responsible for compressing and decompressing. 

Moreover, the iqzip library implements the Compression Identification Packet (CIP) that is an optional structure that shall contain information
that would allow the Adaptive Entropy decompressor to be automatically configured to decompress an iqzip-compressed signal recording without the need
of a-priori mission specific information. 
The important mission specific configuration for the Adaptive Entropy Coder refers to:
- l, the number of CDSes that exist in a packet
- r, the reference sample interval
- n, the sample resolution in bits
- J, the number of samples per block
- whether the Basic or Restricted set of codes is used

![cip](https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip/wikis/uploads/b4a2fb65f29c6126040fd8b58e4f3226/cip.png)

Fig 1. Compression Identification Packet Structure from [CCSDS 121.0-B-2](https://public.ccsds.org/Pubs/121x0b2ec1.pdf)

### IQzip Extension Header

The CCSDS 121.0-B standard strictly restricts the values of some critical parameters of the Adaptive Entropy
Coder (e.g the number of samples per processing block). However, the analysis [4] of the results for the process of testing the Adaptive Entropy Coder with different values for these parameters, reveals that some non-standard parameter values lead to increased compression performance. 

In that direction, the iqzip library appropriately extends the libaec implementation alongside with the CIP packet described in the CCSDS 121.0-B standard, in order to optionally support non-standard configuration. Consequently, an extension compression header is injected automatically between the standard CIP packet header and the
actual compressed data, whenever non-standard configuration is selected.

## Metafile generation
Regarding the metafile generation for the signal recordings, the iqzip makes use of the libsigmf
[5]. The libsigmf is a powerful and generic library that enables the generation of JSON metafiles that
conform to the SigMF specification for the core or custom namespaces.  

The iqzip provides an abstraction layer between the libsigmf and the user by giving a simplified set of functions
for the creation or parsing of SigMF metafiles. Currently, the iqzip metafile API only supports the core SigMF namespace.

## Requirements
* GNU Radio ( > 3.7.7 )
* CMake ( > 2.6)
* G++ (with C++14 support)
* libaec (will be dowloaded automatically during building)
* libsigmf (will be dowloaded automatically during building)
* libtar

## Installation

1. `git clone https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip.git`
2. `cd iqzip`
3. `mkdir build && cd build`
4. `cmake ..`
5. `make`
6. `sudo make install`

If this is the first time you are building the iqzip library run
`sudo ldconfig`

### Advanced
By default, the **iqzip** library will use the default installation prefix.
This highly depends on the Linux distribution. You can use the `CMAKE_INSTALL_PREFIX`
variable to alter the default installation path.
E.g:

`cmake -DCMAKE_INSTALL_PREFIX=/usr ..`

Another common control option is the library sugffix of the Linux distribution.
There are distributions like Fedora, openSUSE, e.t.c that the their 64-bit version
use the `lib64` folder to store the 64-bit versions of their dynamic libraries.
On the other hand, distributions like Ubuntu do the exact opposite. They use
`lib` directory for the libraries of the native architecture and place the 32-bit versions
on the `lib32` directory. In any case the correct library directory suffix
can be specified with the `LIB_SUFFIX` variable. For example:

`cmake -DLIB_SUFFIX=64 -DCMAKE_INSTALL_PREFIX=/usr`

will install the libraries at the `/usr/lib64` directory.

## Usage
Please mind the tools that exist under the /apps directory.

## Documentation

For more information about the iqzip library implementation, visit the Doxygen HTML pages [here](https://librespacefoundation.gitlab.io/sdrmakerspace/iqzip).

## License
The **iqzip** library is implemented in the scope of [SDR Makerspace](https://sdrmaker.space/), an initiative of the [European Space Agency](https://esa.int) and [Libre Space Foundation](https://libre.space). 


&copy; 2018 [Libre Space Foundation](http://librespacefoundation.org) under the [GPLv3](LICENSE).

![sdrmakerspace](https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip/wikis/uploads/5b652716f9ae39c444009ccdb8e337ba/sdrmakerspace.png)
![esa](https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip/wikis/uploads/a51802d6b007ce52f67dc6ad58fb022b/esa.gif)
![lsf](https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip/wikis/uploads/0efd910f4adfb62a67d6d048dbcac3ea/lsf.png)

## References
1. ##### [https://public.ccsds.org/Pubs/121x0b2ec1.pdf](https://public.ccsds.org/Pubs/121x0b2ec1.pdf)
2. ##### [https://github.com/gnuradio/SigMF/blob/master/sigmf-spec.md](https://github.com/gnuradio/SigMF/blob/master/sigmf-spec.md)
3. ##### [https://gitlab.dkrz.de/k202009/libaec](https://gitlab.dkrz.de/k202009/libaec)
4. ##### [https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip/wikis/comparison](https://gitlab.com/librespacefoundation/sdrmakerspace/iqzip/wikis/comparison)
5. ##### [https://github.com/deepsig/libsigmf](https://github.com/deepsig/libsigmf)

LDA-ICPC-2014
=============

Code and materials from the presentation "Understanding LDA in Source Code Analysis" given at ICPC 2014

### Prerequisites

* Linux/OSX
* perl
* python (3 or higher)
* tcsh
* bash
* gcc
* R (version 2.1 or higher)
* [TXL](http://www.txl.ca/ndownload.html)
* [NiCad Clone Detector](http://www.txl.ca/nicaddownload.html)

#### Special Instructions for Installing NiCad

Once TXL is installed on your system, you'll need to download and install NiCad. 
If your machine is 64 bit, you'll need to edit tools/Makefile in the NiCad directory
and uncomment out the `MACHINESIZE = -m64` line and comment out the `MACHINESIZE = -m32` line

Then, type `make` in the root of the NiCad directory as you normally would.

### Usage

See the README files in each directory. Additionally, there are several "FIX ME" placeholders
throughout the code. The list below gives more context on how to replace these with your system specific
paths

#### Replace FIX ME with the location of this repository:

* topic-models/aabbcc-models/n-study
* topic-models/aabbcc-models/do-aabbcc
* topic-models/aabbcc-models/all-betas
* topic-models/aabbcc-models/cook-aabbcc
* topic-models/topic-count/do-lots
* topic-models/bin/csv2heatmap
* topic-models/bin/run-Rs-lda
 
#### Replace FIX ME with an output directory of your choice:

* topic-models/topic-count/do

<div align="center">
    <img alt="3ccode logo" src="resources/code3c.png" style="transform: scale(90%)" />
</div>

---
# 3C-Code: Graphical Communication Protocol
> **3C-CODE** - _Colored and Compressed Circular Code_

The 3C-Code library and `code3c generator` CLI software are part of the 
<b>Graphical Communication Protocol: Network II</b> project as a practical works 
due for my 6th bachelor semester.

## Inspiration and Conception

Inspired by the QR Code, the 3C-Code is intended to produce a graphical way to communicate
information through a <i>circular</i> matrix and using colours to increase <b>pixel unit</b> 
capacity:

<ul>
    <li><b>WB (White & Black)</b>: 1 bpu (bit per unit)</li>
    <li><b>WB-2C (White, Black & 2 colors)</b>: 2 bpu</li>
    <li><b>WB-6C (White, Black & 6 colors)</b>: 3 bpu</li>
</ul>

The project uses some features to improve error coverage and available stocking space:
<ul>
    <li><b>Hamming Code</b> (7,4,3) or (3,1,3) to cover from 14% up to 33% errors</li>
    <li>pre-built <b>Huffman table</b> to compress data without loss</li>
</ul>

<br/>

[![pipeline](https://gitlab.isima.fr/rinbaudelet/uca-l3_graphicalprot/badges/main/pipeline.svg)](https://gitlab.isima.fr/rinbaudelet/uca-l3_graphicalprot/-/pipelines?page=1&scope=all&ref=main)
[![Tests](https://gitlab.isima.fr/rinbaudelet/uca-l3_graphicalprot/-/badges/release.svg)](https://gitlab.isima.fr/rinbaudelet/uca-l3_graphicalprot/-/releases)
[![lgpl-v3](https://img.shields.io/github/license/madeshiro/3c-code?color=darkgreen&logo=license)](https://www.gnu.org/licenses/lgpl-3.0.html)
[![gpl-v3](https://img.shields.io/badge/license%20(CLI)-GPL--3.0-darkred)](https://www.gnu.org/licenses/gpl-3.0.html)

# `code3c` generator CLI

## Arguments

Here a list of available arguments for the `code3c generator` CLI.
If there isn't any input stream specified, the software will ask the user to input data in the console.
Many stream channels are available: through **command arguments** or **pipe**.

### `-o, --output <file.png>`  
Specify the output file. If null, no file will be saved or default name will be `code3c.png`
### `-h, --huffman=<{NO, ASCII, LATIN1}>`
Set up the Huffman compressing method. Per default, no compression method is set 
### `-f, --file <input_file>`
Specify an input file to generate 3C-Code
### `-m, --model=<{WB, WB2C, WB6C}>`
Specify the 3C-Code model. Per default, WB2C is set
### `-e, --err={Hamming743, Hamming313}`
Specify the error model. Per default, Hamming743 is set (14% error coverage)
### `-t, --text "input text"`
Specify an input text to generate 3C-Code

# `htfgen` CLI
### `-t, --table <.htf file>`  
Read a .htf file and ouput the result in the console output stream
### `-g, --file <file>`
Generate a Huffman Table from a text's file
### `-s, --text <"text">`
Generate a Huffman Table from a specified text

---
<div align="center">

[![uca](https://www.uca.fr/uas/ksup/LOGO_CLAIR/UCA__Logo_head.png)](https://www.uca.fr)
[![lgpl-v3](https://www.gnu.org/graphics/lgplv3-with-text-154x68.png)](https://www.gnu.org/licenses/lgpl-3.0.html)
[![gpl-v3](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)](https://www.gnu.org/licenses/gpl-3.0.html)

</div>
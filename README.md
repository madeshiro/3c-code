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

---
<div align="center">

[![uca](https://www.uca.fr/uas/ksup/LOGO_CLAIR/UCA__Logo_head.png)](https://www.uca.fr)
[![lgpl-v3](https://www.gnu.org/graphics/lgplv3-with-text-154x68.png)](https://www.gnu.org/licenses/lgpl-3.0.html)
[![gpl-v3](https://www.gnu.org/graphics/gplv3-with-text-136x68.png)](https://www.gnu.org/licenses/gpl-3.0.html)

</div>
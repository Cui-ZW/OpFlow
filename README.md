<div align="center">
  <img width="800px" src="https://github.com/OpFlow-dev/OpFlow/blob/master/doc/assets/opflow_banner_slice.png" alt="OpFlow banner">
</div>

[![Latest Release](https://img.shields.io/github/v/release/OpFlow-dev/OpFlow?color=blue&label=Latest%20Release)](https://github.com/OpFlow-dev/OpFlow/releases/latest)
[![License](https://img.shields.io/github/license/OpFlow-dev/OpFlow?color=green&label=License)](https://github.com/OpFlow-dev/OpFlow/blob/master/LICENSE)
[![Downloads](https://img.shields.io/github/downloads/OpFlow-dev/OpFlow/total?color=blue&label=Downloads)](https://github.com/OpFlow-dev/OpFlow/releases/latest)

[![Format](https://img.shields.io/github/workflow/status/OpFlow-dev/OpFlow/AutoFormat?color=green&label=Format)](https://github.com/OpFlow-dev/OpFlow/actions/workflows/AutoFormat.yml)
[![Build](https://img.shields.io/github/workflow/status/OpFlow-dev/OpFlow/Build?color=green&label=Build)](https://github.com/OpFlow-dev/OpFlow/actions/workflows/Build.yml)

## Overview

**OpFlow** (运筹) is an embedded domain specific language (EDSL) for partial differential equation (PDE) solver composing.
It adopts the meta programming facilities provided by modern C++ extensively to provide an expressive front-end user
interface. With **expression templates** and **stencil auto-generation**, OpFlow is able to perform both **explicit** expression
evaluation and **implicit** linear system solving. With isolated expression, operator & evaluation engine's implementation,
OpFlow makes the algorithm, data structure and execution scheduler fully decoupled. OpFlow's advantages are:

- **Fully static**. No dynamic dispatching on the critical path
- **Zero-cost abstraction**. All operations are well-defined at compile time and force inlined
- **Automatic parallelization**. Partition & evaluation of expressions are automatically parallelized by user defined strategies
- **Implicit equation solving**. OpFlow can solve arbitrary user provided implicit equations as long as they are well-defined
- **Header only & C++ embedded**. OpFlow itself is header-only and embedded in C++. It's straight forward to integrate
  OpFlow into existing numerical codes
- **Mathematical API notations**. The front-end interface of OpFlow is very similar to mathematical notations. Little
  language noise will occur while writing equations
- **Proper level of abstraction**. OpFlow tries to eliminate the need to write bare schemes while keeping all the
  transformations & operations performed at the front-end. It's straight forward to see how each term is discretized,
  while keeping all expressions in a unified fashion. This is different from packing all things into modules, or
  looping over each element on the front stage.

## Examples

<a href="https://github.com/OpFlow-dev/OpFlow/blob/master/examples/CONV1D/CONV1D.cpp"><img src="https://github.com/OpFlow-dev/OpFlow/blob/master/doc/03_Examples/assets/weno.gif" height="192px"></a>
<a href="https://github.com/OpFlow-dev/OpFlow/blob/master/examples/FTCS2D/FTCS-OMP.cpp"> <img src="https://github.com/OpFlow-dev/OpFlow/blob/master/doc/03_Examples/assets/heattransfer.gif" height="192px"></a>
<a href="https://github.com/OpFlow-dev/OpFlow/blob/master/examples/LidDriven/LidDriven2D.cpp"><img src="https://github.com/OpFlow-dev/OpFlow/blob/master/doc/03_Examples/assets/liddriven.gif" height="192px"></a>
<a href="https://github.com/OpFlow-dev/OpFlow/blob/amr/examples/LevelSet/AMRLS.cpp"><img src="https://github.com/OpFlow-dev/OpFlow/blob/master/doc/03_Examples/assets/amrls.gif" height="192px"></a>

## Installation

Please refer to the documentation for installation instructions.
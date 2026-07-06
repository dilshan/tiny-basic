# Introduction

Welcome to **Tiny Basic for Arduino**, an open-source project designed to bring the simplicity and rapid prototyping power of the classic BASIC language to modern microcontrollers.

> **Terminology Note**: Throughout this documentation, the terms *Tiny Basic for Arduino* and *Tiny Basic* are used interchangeably for brevity.

## About the BASIC Language

BASIC (*Beginners' All-purpose Symbolic Instruction Code*) was originally developed in the [mid-1960s](https://www.youtube.com/watch?v=WYPNjSoDrqw) to make computing accessible to non-scientists. Unlike low-level assembly or compiled languages like C/C++, BASIC was built around readability, simple English-like commands, and an interactive layout. Historically, it pioneered the concept of an immediate-execution mode (often referred to as a "direct command prompt"), allowing developers to write, edit, and run code line-by-line in real time without waiting for external translation or linker phases.

## What is Tiny Basic for Arduino?

Tiny Basic for Arduino is engineered specifically to run on mid-range microcontrollers (MCUs) that offer sufficient RAM and flash memory space. While traditional microcontrollers often require a lengthy write-compile-upload cycle for every minor change, Tiny Basic for Arduino streamlines this workflow significantly. The key objective of this project is to provide a highly interactive, rapid development environment to test ideas, manipulate hardware peripherals, and run scripts directly on the MCU with a minimum amount of code and external reference materials.

## Hardware & Architecture

* The Reference Platform: The default, recommended development board for Tiny Basic for Arduino is the [Arduino Due](https://docs.arduino.cc/hardware/due/). Its 32-bit ARM Cortex-M3 architecture provides the ideal balance of memory and processing power to host the environment seamlessly.

* This compiler is written from scratch using standard C/C++. If you plan on rebuilding or modifying the interpreter from source, we highly recommend using a Linux-based environment for the best toolchain compatibility.

* The entire project is structured, configured, and built using the [PlatformIO](https://platformio.org/) ecosystem, making it cross-platform and incredibly easy to clone, tweak, and flash.

## Historical Roots

The Tiny Basic for Arduino project draws significant inspiration from two foundational milestones in personal computing:

* The Original [Tiny BASIC Project](https://en.wikipedia.org/wiki/Tiny_BASIC): Conceived by *Dennis Allison* in the mid-1970s as a lightweight, accessible alternative to resource-heavy interpreters.

* *[Commodore BASIC](https://www.c64-wiki.com/wiki/BASIC)*: The ubiquitous, immediate-execution environment that served as the primary interface for an entire generation of 8-bit systems.
# Digital-Phase-Locked-Loop
Output phase from quadrature sine/cos signals - Relevant for magnetic encoder or power grid synchronization.

1. Introduction

  Phase-locked-loop (PLL) is a technique to locked the phase of input signals. The input can be single, quadrature sinusoidal signals. PLL has wide application in tracking phase on communication, power grid synchronization. The advantage of PLL is real time tracking phase without delay time. Moreover, the computation of PLL is small which is very necessary in real time application. While, PLL has good perfomance on noise and distorted signals.

  For more information: https://en.wikipedia.org/wiki/Phase-locked_loop

  The project implements PLL on ARM STM32F405RG. The inputs are quadrature sine/cosine signals. Of course, the output is the phase of input's signals.

2. Summury of PLL transfer function and digital PLL

  Firstly, we consider the structure of PLL including: Phase Detector (PD), Loop Filter (LF) and Voltage Control Oscillator (VCO). The PD detects the phase error, LF related to noise reduction as well as time acquisition time, and VCO is a integral.

  The detail in:

3. Project structure

  The main files of project are organized
  
  - ADC_DMA.c/ADC_DMA.h - for reading ADC
  - TimerSample.c       - Configuration for running PLL at every sampling time
  - AADPLL.c            - contain Digital PLL implementation 
  - uart.c              - RS232 DMA to transmit the result to computer
  - main.c

PartiSim Alpha
==============

Dust like particles fly around by the flick of your mouse

Particles are not self-interacting.
This was written as a proof of concept of high quantities of particles in a real time enviroment mixed with online synchronisation.

PartiSim solution is setup for x86 and x64 compilation, it is written entirely from scratch using only directX and windows standard API's so there is no extra dependencies you need to build it.

Work in progress
================

Default launch parameters:
==========================
  +r filename (for reading and benchmarking a perviously recorded file)

  +w filename (for recording session, this can be combind with +h or +c to record a multiuser session)

  +t samples/second (leave 0 for real time, this sets the time between samples, example 1000 at 1000fps is real time)

  +h samples/second (for hosting a server)

  +c hostname (for connecting to a server)

Default controls:
=================
  Left mouse button: pull in particles
  
  Right mouse button: push away particles
  
  Spacebar: Stop particles
  
  R: Reset

System requirements:
====================
CPU: As operating system recommends

Graphics: Minimum: DirectX 11 with 256MB video memory, Recommended: HD6000 series AMD, GTX500 series Nvidia

Memory: As operating system recommends

Current version requires Windows Vista* or higher

*Vista service spack 3 for x86 or service pack 2 for x64 for the requirement of directX 11.


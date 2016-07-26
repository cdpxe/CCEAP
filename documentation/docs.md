# A Brief CCEAP Documentation

## Installation

The tool requires gcc and Linux (or similar). Run `make` to build the two components of the tool: **client** and **server**.

## Teaching Process in a Nutshell

The lecturing part is split into two parts. First, fundamentals are explained and secondly, exercises are performed and evaluated.

#### 1. Preparing Students to Use CCEAP

The lecturer has to introduce
- the CCEAP protocol (**Note:** Initial publication under review for a conference; it will be released after acceptance), and
- Hiding patterns as they are described on our [network information hiding patterns website](http://ih-patterns.blogspot.de/p/introduction.html) that also points to several of our recent publications in which we introduce the topic.

#### 2. Exercises

In the exercises, students should try create covert channels or determine the type for a given covert channel. Therefore, the lecturer can perform one of two exercises:

- The lecturer can ask the *students establish a hidden communication that represents a given pattern*. The pattern can be one of the known patterns of the available [pattern collection](http://ih-patterns.blogspot.de/p/test.html), e.g. [ValueModulation](http://ih-patterns.blogspot.de/p/references-1-s.html) or [Reserved/Unused](http://ih-patterns.blogspot.de/p/p3-add-redundancy-pattern.html). Therefore, the students need to find out how to create a covert channel for the particular pattern using the CCEAP protocol by analyzing the protocol structure and the `client` tool's command line parameters.
- Alternatively, the lecturer can ask the students to *determine the hiding pattern for a given traffic recording or for given tool parameters* that were used to run the client.



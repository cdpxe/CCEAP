# CCEAP: Covert Channel Educational Analysis Protocol

The *Covert Channel Educational Analysis Protocol* (CCEAP) is a simple network protocol designed for teaching covert channels (network steganography) to professionals and students. It is also an easy-to-use covert channel traffic generator.

The protocol is explicitly vulnerable against several [hiding patterns](http://ih-patterns.blogspot.de/p/introduction.html), i.e. patterns that represent hiding methods (steganographic methods that create covert channels, see [here](https://github.com/cdpxe/Network-Covert-Channels-A-University-level-Course/) for my GitHub online class on network steganography/covert channels). The protocol's structure is simple and self-explanatory and its implementation is kept at a minimum level of code lines to make it especially accessible to students. There is also a [documentation](https://github.com/cdpxe/CCEAP/tree/master/documentation) and an *academic publication* (published under the umbrella of ACM CCS 2016, download link is also in the documentation) available.

Please send requests and feedback to the author: Steffen Wendzel, [www.wendzel.de](http://www.wendzel.de) (wendzel (at) hs-worms (dot) de).

## Kickstart - a few Examples:

#### 1. A First Start
First, start the server, e.g. on your local host: `./server -P 9999`. This lets the server run on IP 127.0.0.1, port 9999.

Then, connect with the client: `./client -D 127.0.0.1 -P 9999`. This will make the client send ten simple standard messages to your server. The server should display the received packets' meta-data.

#### 2. Covert Timing Channel
Now, let us create a simple covert timing channel that we use to transfer the file `/etc/hosts`. And we want to encode 1 and 0 bits with 500ms and 1000ms. Therefore, we start the server again, and then run the client as follows using CCEAP's `iat_encode` tool which encodes files into inter-arrival times (this would represent the [Inter-arrival Time](http://ih-patterns.blogspot.de/p/blog-page_40.html) pattern):

`./client -D 127.0.0.1 -P 9999 -t ´./iat_encode /etc/hosts 500 1000´`

#### 3. Simple Covert Channel Traffic Generator
Of course, one could also use `dd` together with `/dev/random` as a source of randomness to create a file with random bits and use this file instead of `/etc/hosts`.
Similarly, the [Manipulated Message Ordering](http://ih-patterns.blogspot.de/p/p10-pdu-order-pattern.html) pattern can be represented using `./seq_encode /etc/hosts 256 2` instead of `./iat_encode /etc/hosts 500 1000`. Check the [documentation](https://github.com/cdpxe/CCEAP/tree/master/documentation) for details.

More parameters of `client` can be obtained by running `./client -h`.

#### More Examples

Additional examples can be found in the [sample_exercises](https://github.com/cdpxe/CCEAP/tree/master/sample_exercises) directory and in the documentation.

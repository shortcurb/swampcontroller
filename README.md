# swampcontroller
Hardware and software for controlling evaporative coolers with MQTT and relays

I live in a climate where using evaporative (swamp) coolers to cool dwellings is a generally effective possibility. I have two swamp coolers. I like them because they are cheap to run, cheap and easy to maintain, and if one breaks, I can almost certainly fix it myself, rather than calling a contractor.

My swamp coolers electrical components consist of a water pump and a 1/4HP blower fan with high and low speed settings. I designed this PCB to use an ESP8266 to control three off-board 12V automotive relays, which was easier than trying to figure out how to manage 10A on my little 1oz copper PCB traces.

If anyone is interested, I have 3 remaining unpopulated PCB boards I would be happy to sell/ship somewhere

Hardware features:
* ESP8266 header
* LM2596 (eBay/Amazon THT variant) 4V-40V input
* power header
* 3 relay-control headers

Hardware Problems:
* The pins that control speedheader and fanheader are currently wired to pins that control the flash on an ESP8266. Need to cut the trace and install a jumper wire somewhere else to control them correctly.

Software features:
* State, command, and response MQTT feeds
* State feed publishes the state of the swamper regularly
* Subscribes to command feed to listen for commands
* Response feed is for sending responses to commands
* 5s delay after receiving a command, to prevent changing motor/pump electricity too quickly
Software Problems:
* If the MQTT broker goes down, or Wifi goes down, the device will eventually crash and turn off the blower and fan. This is intended as a purely wifi-controllable device, so its user preference to decide whether failing off is better than failing on. This is kind of an odd edge case, in that my wifi/broker generally only go down if my house electricity goes down, and if my house doesn't have electricity, the swamper isn't running either.

Closing thoughts:
* I assembled the components and relays and a 12V wall wart adapter and cut up some old extension wire and wired everything up in a waterproof junction box and mounted it just below my swamp cooler, powered via an external outlet. Its been working great for the last month!
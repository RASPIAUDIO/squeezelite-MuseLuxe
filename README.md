This is a quick tutorial to burn on the [ESPMUSE LUXE SPEAKER](http://https://raspiaudio.com/produit/esp-muse-luxe "ESPMUSE LUXE SPEAKER") to do multiroom audio. Multiroom is great at home when you want to have the same music synchronized in you living room, kitchen, bathroom without the need of moving a bluetooth speaker and a phone with you, or worst having a very loud distorted music in one room hoping to hear in every room.

[Full tutorial could be found here](https://forum.raspiaudio.com/t/muse-luxe-speaker-with-squeezlite-logitech-media-server/300 "Full tutorial could be found here")

Our forked GIT version of squeezelite is here what we do in this fork is to add the audio codec that is used in the LUXE. In the future the goal is to ask to the orginal creator of the main branch of squeezligth to merge with our version so we will be able to follow updates easily. All credits for that project goes to the original creators Sle118 and Philippe44.

there is also a tutorial for [ESPMUSE PROTO here ](https://raspiaudio.com/produit/muse-proto "ESPMUSE PROTO here ")the small mono version of the LUXE without the case and speaker.

Squeezelite-esp32 is an audio software suite made to run on espressif’s ESP32 wifi (b/g/n) and bluetooth chipset. It offers the following capabilities

Stream your local music and connect to all major on-line music providers (Spotify, Deezer, Tidal, Qobuz) using Logitech Media Server - a.k.a LMS and enjoy multi-room audio synchronization. LMS can be extended by numerous plugins and can be controlled using a Web browser or dedicated applications (iPhone, Android). It can also send audio to UPnP, Sonos, ChromeCast and AirPlay speakers/devices.*
Stream from a Bluetooth device (iPhone, Android)*
Stream from an AirPlay controller (iPhone, iTunes …) and enjoy synchronization multiroom as well (although it’s AirPlay 1 only)*
It is sometime difficult to start with Squeeze Light so II have made a pre-confirgured .bin file to burn on the ESPMUSE LUXE SPEAKER 5 you[ can download it here](https://github.com/RASPIAUDIO/squeezelite-MuseLuxe/raw/main/squeezeliteML.bin " can download it here")

With Linux or ESPTOOLS use the following command :

esptool.py -p /dev/ttyUSB0 write_flash 0x0 my_image_to_burn.bin

UnderWindows, It must be loaded using the ESP32 flash downloading tool :

https://www.espressif.com/sites/default/files/tools/flash_download_tool_3.9.0_0.zip

....

[Full tutorial could be found here](https://forum.raspiaudio.com/t/muse-luxe-speaker-with-squeezlite-logitech-media-server/300 "Full tutorial could be found here")

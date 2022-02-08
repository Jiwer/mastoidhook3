# mastoidhook3
Internal Hearts Of Iron 4 Cheat For Linux & Windows. Uses https://github.com/kubo/funchook and https://github.com/ocornut/imgui

![alt text](https://i.imgur.com/ItcnC7g.png)

This isn't so much of a cheat, but more of a collection of exploits and tools.

# Features & Info
1. Anti-Ban. This makes you unbannable and unkickable. This feature must be disabled prior to connecting to a lobby and then re-enabled after connecting in order to use properly.

2. Join Game As Ghost. This makes you invisible inside lobbies and does not alert the other players when you join.

3. Hide Steam Name. Hides your steam name in the lobby screen.

4. DOS Host. Floods the host's game with packets through HOI4. This makes the host's network consume around 80mib/s average (peak 120mib/s) if your cpu and upload speed is powerful enough to support that output. (mid tier cpu at best + at least 12mib/s upload speed)

5. Freeze Host. Freezes the host's game indefinitely. Works similar to DOS Host, however it's extremely unlikely that this feature would be able to take down a host's internet connection.

6. Unrestricted File Transfer. This feature only works as host. It looks for the specified file inside of your HOI4 save games folder and transfers it to all of the other players in the game. The received file is also deposited inside the target's save games folder. In order for this feature to work properly you need to first attempt to load a .hoi4 save game through HOI4 one time (DO NOT USE THE UPLOAD BUTTON FOR THIS). Once you've done this you're good to use the upload button freely with any file name and extension for the rest of the duration that you have HOI4 open.

# Windows Compiling And Usage
Coming soon.

# Linux Compiling And Usage
In order to load mastoidhook3.so into HOI4 you'll need to enter the following command into your terminal:
LD_PRELOAD=/path/to/libfunchook.so.1.1.0:/path/to/mastoidhook3.so ./path/to/hoi4 -steam

An example of this would be:
LD_PRELOAD=/home/esther/mastoidhook3/libfunchook.so.1.1.0:/home/esther/mastoidhook3/mastoidhook3.so ./hoi4 -steam

Assuming your terminal is already inside of the HOI4 folder and you placed both libfunchook.so.1.1.0 and mastoidhook3.so inside of /home/esther/mastoidhook3/

In order to compile this you'll need to download and install funchook. After you've done that, use the following commands to compile:

g++ -fPIC -c main.cpp

g++ -fPIC -shared -rdynamic -o mastoidhook3.so main.o -ldl

The Linux version of this repository was my first time working with g++ and programming on Linux, so if there's anything that's wrong or I can correct, please submit an issue and I will respond to it.

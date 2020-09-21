# DriverBlock
This is a repository containing the source code from a blog post that I wrote on blocking kernel driver loads to prevent forensics tools from performing remote forensics.

## What's this all about?
You can find more information in my blog post here, but I'll provide a brief synopsis. Several open source forensics toolkits give investigators the option to perform remote memory captures. This is a very powerful forensics mechanism, and the remote aspect is very important now that administrators can't simply drive to the office and plug a write blocker into something hosted in a datacenter across the country. Some of these forensics tools utilize a driver, specifically winpmem, to perform the actual memory capture. By combining a kernel driver and userland process it's possible to inspect new processes and inject a hooking mechanism into them to watch for driver loads. By blocking a potential forensics driver load, it can inform an operator that this host is burnt or be used to perform automated cleanup.

To be clear, this is NOT a rootkit. It's a simple proof of concept for monitoring processes and injecting into them from userland to perform hooking on driver loads. 

## Why kernel and user space?
In some cases it may be possible to do this without a kernel driver, such as if the tool in use has a delay before the driver is loaded (1-2 seconds should be enough). However winpmem will immediately load the driver and beats out any sort of usermode monitor using WMI or ETW. The solution to this was to turn to a kernel driver which monitors for new processes and slightly delay it, giving our usermode process the opportunity to inject a DLL. You could accomplish this purely with a driver, however DLL injection from a driver requires an entirely different technique and I'm not very good at C++. There are a few POC projects out there for this though.

## What all is in this repository?

### ProcessMonitorDriver
This is a kernel driver that implements a CreateProcessNotify routine. It will suspend any processes matching a hardcoded string, such as "DriverLoader", and sleep for a set amount of time to allow for DLL injection into that process. It will then relay that process' PID over a named pipe so that another program can be used to perform the DLL injection.

### DriverHooker
This is the DLL to be injected into target processes. It is currently written for the purposes of injecting into an x86 process and hooking CreateServiceA, which is used to load kernel drivers. It then checks for a hardcoded name driver name, which will be blocked if found.

### CppInjector
This is a very basic DLL injection process written in C++. It will rapidly poll over a named pipe for updates from ProcessMonitorDriver for PIDs to inject into, and then perform that injection. Pipe name, sleep times, and DLL path are hardcoded in the application. I'll be adding command line switches very soon.

Usage: CppInjector.exe 

### DriverLoader
This is a very simple program that will load a driver on the target, wait for the user to press enter, and then unload the driver. I'll add a command line option to indivudually load/unload soon.

Usage: DriverLoader.exe C:\Path\To\Driver

### TargetDriver
This is a very simple "Hello World" driver that can be used in conjunction with DriverLoader for very basic driver loading/unloading. Literally just prints hello world.

## Notes
The architectures in this project are pretty finicky. You will need to use ProcessMonitorDriver in x64, DriverHooker in x86, and CppInjector in x86. TargetDriver and DriverLoader are both x64.

## Disclaimer
This code is so poorly written and spaghetti'd that you would be hard pressed to use it maliciously, but please do not try. This was created for educational purposes only.

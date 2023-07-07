# Sadeed-NASA-APOD
NASA Astronomy Picture of the Day on GTK 3

## Compilation
### Installing Dependencies
On Fedora,
```
sudo dnf install gtk3-devel libcurl-devel json-c-devel
```

### Compiling
Use GCC (the GNU Compiler Collection) and remember to link against the GTK 3, cURL, JSON-C libraries.  
```
gcc ./Sadeed_NASA_APOD.cpp -o ./Sadeed_NASA_APOD `pkg-config --cflags --libs gtk+-3.0` -lcurl -ljson-c
```

## Usage
Run the program from the terminal.
```
./Sadeed_NASA_APOD
```
**or** Double-click on the file.  

An image file named **Latest_APOD_Image.jpg** will be created in the directory in which the program is running. So, make sure the program or the current user has sufficient permission to do so.  

--- 

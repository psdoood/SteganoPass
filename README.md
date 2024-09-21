# SteganoPass - Hidden in Plain Sight

## Overview
This is a lightweight, privacy focused password concealer that uses steganography to hide encrypted passwords within image files. This approach provides an additional layer of security by hiding sensitive information in plain sight.

## Features
- **Encryption** - Encrypts the passwords using user defined key before being hidden in images.
- **Steganography** - Hides the passwords within images using LSB steganography.
- **Multiple Format Support** - Compatible with image formats such as JPG, BMP, PNG, TGA, and PSD.
- **Simple GUI** - Intuitive interface for hiding and extracting passwords from images easily.
- **File Explorer** - Built in file navigation system for easy image selection.
  
![noImageGUI](https://github.com/user-attachments/assets/ca3edf41-5d21-49cc-aaa2-abfbc42d6af4)
![loadedImageGUI](https://github.com/user-attachments/assets/59fe9675-04e5-4f93-aa35-277982de914e)

## Requirements
- **CMake** - (3.14 or higher)
- **OpenGl** - (3.0 or higher)
- **C++ Compiler**

## Platform Compatibility
- **Windows** - Supported 
- **macOs** - Needs Testing/ Minor Adjustments
- **Linux** - Needs Testing/ Minor Adjustments

## Installation 
Note: I have not tested SteganoPass on linux or macOS yet.
1. Clone the repository
> git clone https://github.com/psdoood/SteganoPass.git
- **For Windows Users** - I have provided batch files in the `batch` diretory to simplify the building process.
 
    2. **build.bat** - Either double click or execute this file to start the build process. After it is complete an exe can be found in `build/Release` called `SteganoPass.exe`.
  
    3. **run.bat** - Alternatively, you can execute this to run the gtest files and the executable.
  
    4. **clean.bat** - Execute this if you want to clean the build directory. 
2. Navigate to cloned repository
> cd SteganoPass
3. Create a build directory and navigate to it
> mkdir build && cd build
4. Generate the build files with CMake
> cmake ..
5. Build the project
> cmake --build .
6. Launch SteganoPass with the executable found in `build/Release` called `SteganoPass`.

## Usage
1. Launch SteganoPass
2. Use the File Explorer on the top left to load an image.
3. Enter the master key and a password you wish to hide in an image in the control window.
4. Click "Hide Data in Image" to hide the data in the loaded image.
5. Now save the image using the options in the settings window.
6. To retrieve password, select image containing the hidden data, enter the key, and click "Extract Data from Image".

## Acknowledgements
- [ImGui](https://github.com/ocornut/imgui) for the GUI elements.
- [stb](https://github.com/nothings/stb) for image loading and writing.
- [tinyaes](https://github.com/kokke/tiny-AES-c) for simple AES encryption.

## License
SteganoPass is licensed under the MIT [License.](LICENSE)

## Disclaimer
This code was a personal project. Use trusted and reputable password managers for sensitive information. 


# AURAVideoSync
Change AURA colors based on video input.

Logic behind the program: 
1. Take screenshot of the current video feed using Windows GDI (Graphic Design Interface)
2. Analyze the pixel color and compute the histogram
3. Set the color LEDs using AuraSDK based on the dominant color (currently the higher pixel count), but never turn off the LEDs just use a dimmer with light
4. Sleep thread for 100 ms so it won't overload the CPU

![video-demo](demo.gif)

The above GIF was made on my PC (i7-7700K 4.2GHz, Asus STRIX Z270F, Corsair Vengeance LPX 16GB, Gigabyte GTX 1070, Thermaltake Suppressor F31, compiled w/ MSVC2017 CE) and was never tested outside this enviorment.

## Resources:

Aura SDK can be found at the following [Link](https://www.asus.com/campaign/aura/us/SDK.html)

Information about Windows GDI [Link](https://docs.microsoft.com/en-us/windows/desktop/gdi/windows-gdi)


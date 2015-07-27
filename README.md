# ScenesManager
C++ project for a university course that provides a simple and intuitive interface to mark scenes separation inside a video.

### MARKERS
Markers are represented with the frame start number and the frame end number of the scene. Markers of a video will be saved in a file by following this simple structure:
* Each scene marker goes on a new line
* Scene marker start and end time are separated by a comma and a space

Example:
```
5, 10                 	// Scene 1 goes from frame 5 to frame 10 included
12, 25
50, 110
```

### VIDEOS
Videos are loaded using the famous ffmpeg library for C++.

### INITIAL GOALS
Those are the minimum goals that we have to achieve before starting the further goals:
* Implement utilities to manage/decode video files: FFMPEG
  1. Load a video
  2. Play & stop
  3. Random position access
  4. Fast forward & backward
* Create the interface: QT
  - Home
    1. Actual image viewer
    2. Snapshots of images around the current one
    3. Buttons for the management of the video (play, stop ecc)
    4. Scenes markers viewer
    5. Buttons for CRUD of markers
    6. Slider (jump to a random position in the video)
  - Help
  - About
* CRUD of scenes markers
  - Read markers file
  - Add & remove marker
  - Update markers
 
### FURTHER GOALS
Advanced features that must be done only after all first goals are marked as completed:
* Markers comparison (2 files)
* Interface: QT
  - Help
  - About
 
### CONTRIBUTORS
We would like to thank Daniel Roggen for the QTFFmpegWrapper that allowed us to easily 
decode videos.


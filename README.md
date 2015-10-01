# Shot Manager
Shot Manager is a C++ project that shows a simple and intuitive interface to mark shots changes in a file video.

The application was created using **[Qt](http://www.qt.io/download-open-source/)**, a cross-platform application framework that is widely used for developing application software that can be run on various software and hardware platforms with little or no change in the underlying codebase. An interesting feature of Qt is that allows the use of CSS to stylize the entire application.

The core of the application was realized using **[ffmpeg](https://www.ffmpeg.org/)**, a free software project that produces libraries and programs for handling multimedia data.


## 1. HOW TO USE
### 1.1 Windows
#### 1.1.1 Compile
We compiled it with **Qt 5.5.0 MSVC2013** and **64bit Release** as Build Kit.

It works even under Visual Studio 2013 with the Visual Studio Add-in for Qt5 that you can find **[here](http://www.qt.io/download-open-source/#section-2)** or if the link is dead just look in the download page into the Qt website.

#### 1.1.2 Binaries
In the subfolder **Binary x64** you can find the compiled version for 64 bit windows system.

If you make changes to the application and compile it, you can simply replace the current .exe with the new one. If you used new Qt Libraries that are not included in the **Binary x64** folder you have to copy them there as well.


### 1.2 Linux
You have to install ffmpeg and x264 locally by following one of the guides that you can find online, for example:
https://trac.ffmpeg.org/wiki/How%20to%20quickly%20compile%20FFmpeg%20with%20libx264%20(x264,%20H.264)

Once installed, properly change unix folders paths in the **.pro** file so that they link correctly to your ffmpeg installation folders (libraries and includes).

## 2. COMPONENTS 
### 2.1 Marker
A Marker is represented by a **start number** and an **end number**, both refers to the frame unique number/position in the entire video. Frame number starts from value 0.

They are stored with the following structure:
* Each marker (relating to a shot) must be in a new line;
* Start and end number of a marker are separeted by a space.

Example:
```
5 10                // Shot 1 goes from frame 5 to frame 10
12 25
50 110
```

### 2.2 Video file and formats
Videos are decode using **[qtffmpegwrapper](https://code.google.com/p/qtffmpegwrapper/)**, a simple library that uses ffmpeg primitives to create high-level methods, like accessing a particular frame inside a video stream or moving between previous and next frames from a given one.

We had to extend this library because the number of allowed video formats was really low. Actually it is possible to open: **avi, asf, mpg, wmv, mkv and mp4**.

Moreover the library was based on 2011 ffmpeg methods, so we had to replace all deprecated methods with new ones.

### 2.3 Interface
The interface is really simple and intuitive, it’s composed by 3 widgets:
* **PlayerWidget**, for video playback;
* **PreviewsWidget**, for frames visualization;
* **MarkerWidget**, for markers management.

Widget are disposed in a logical manner with the PlayerW besides the MarkerW while the PreviewsW is under both.

Once that the video has been paused, the PreviewsWidget will show some frames before and after the current one displayed in the PlayerWidget.

### 2.4 Compare markers file
Markers files can be compared to highlight differences between markers. 

Differences are marked using background colors:
* **White background**, this line and the one on the other file are identical;
* **Colored background**, this line and the contiguous ones with the same background color (on the same file) has differences with the lines on the other file with the same background.


## 3. BASE CLASSES
The application consists of 11 base classes (the ones bold will be explained better later):
* **QVideoDecoder and ImagesBuffer**, these are the core classes of the application because the first one allows us to decode single frames from the video, and the second one manages a buffer of frames used by all the other classes;
* **PreviewsWidget, MarkersWidget and PlayerWidget**, already presented before in this document, respectively serve for the management of snapshots, the management of markers and the playback of the video;
* **CompareMarkersDialog**, already explained before;
* **MainWindow**, create and setup the window and implements all other classes;
* **MenuBar, TitleBar, HoverMoveFilter and WindowTitleFilter**, they allow us to recreate functions that are not present in FrameLessWindow (a window without the default edges of the operating system).

### 3.1 ImagesBuffer
Requests of access to specific frames must pass through the ImagesBuffer. When the requested frame isn’t found in the buffer, the ImagesBuffer will demand to the QVideoDecoder to decode a certain number of frames (actually fixed to 30) around the requested one. We made this choice because that was inline with the PreviewsWidget’s needs.

Moreover, before asking for frames to the QVideoDecoder, we check if there are any overlaps between the current buffer and the one that will be created, this way we can maintain some frames in the buffer and save some time in decoding.

### 3.2 PreviewsWidget
It obtains some frames from the ImagesBuffer by keeping the current frame at the center.
The number of frames is calculated on runtime based on both the video frame size and the window dimensions.
The previews won’t be updated while the video is playing because it could cause problems in rendering and slow the playback.

### 3.3 PlayerWidget
Implements a player for the playback of frames obtained from the ImagesBuffer. The playback frame-rate has an upper limit of 30 fps, when the decoding requires more time the frame-rate automatically scales.
You can go forward and backward frame by frame.

### 3.4 MarkersWidget
It allows to create, modify, delete Markers and save/load them to/from a file. Markers are automatically ordered based on the start number and then by the end number.
Moreover, the overlaps between markers's range are highlighted with a red background color.


## 4. CODERS
* Luca Gallinari
* Dario Stabili
* Marco Ravazzini

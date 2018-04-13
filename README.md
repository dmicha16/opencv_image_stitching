# opencv_image_stitching
OpenCV image stitching application

@channel okay guys I refactored our code, and sent invites to the repo, into a nice and fancy class hierarchy where we have Features class, CameraParameters class and Warping and whatever comes later. We also have a Pipeline class which is the baseclass for all. Pipeline currently reads and uploads images, you can access this function very easily to have your images loaded by it. (hence the hierarchy) Ive done it this way so if anything comes later, such as resizing images, instead of writing that each time we could just access one of Pipeline future functions.
```
Pipeline pipeline;
string path = "C:/photos/BLADE/*.JPG";	
vector<Mat> images = pipeline.readImages(path);
``` 
All the classes are in different files, both .h and .cpp, which means we can now all put our code into the project separately. If you run the program it will just do what it does right now, which I showed you all the other days.

So to test your code, what you can do is comment in and out the class object you would not like to use in image_stitching.cpp. I recommend passing the images through your class constructor and working the logic further from there, instead of calling each function in image_stitching.cpp. Like so:
```
Features features(images);
//Warping warping;
```
Our new int main is in image_stitching.cpp and the code will start to compile from there. The opencv_image_stiching.cpp file is *deprecated*, do *not* uncomment anything from there. I left it there so we can copy code if we need to. If you make changes in image_stitching.cpp, please make sure to not push it to the github repo. You can do so by right clicking on the file, when you want to commit, and clicking "ignore this file", this will add it into the .gitignore, which is exactly what we want right now. Later we can remove it once we are making the whole thing work together. 

You can also log stuff anywhere in the project by calling this macro:
```
CLOG("message", Verbosity::INFO);
```
The log file is in the opencv_image_stitching folder, where you store your local repo. File's called clogging.log. There is a lot more functionality in my logging class so if you want to utalize all that, refer to here: https://github.com/dmicha16/clogging. I hope the readme is clear enough.

Also every time you start working on something make sure to PULL first to avoid any merge issues. If we have any tell me and Ill fix it right away.


[The project structure. (just a handdrawn pic for now)](project_structure.jpg)

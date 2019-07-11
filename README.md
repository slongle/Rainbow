# Rainbow
My renderer for learning  

## Building Rainbow
### Dependencies
All tiny third-part libraries, including `filesystem`, `halton`, `pcg32`, `stb_images`, `tinyformat`, `tinyobjloader` and `pugixml`, are in the `src/ext/` folder.    
Except that, Rainbow need `embree3`, `tbb` and `openvdb`.  
If you don't know how to add the three libraries, I recommend you use some package manager like `vcpkg`.  
```bash
 > vcpkg install embree3 tbb openvdb : x64-windows  
 > vcpkg integrate install 
```

### Build
Now I just test building it on Windows.   
Please build as x64 platform like
```bash
 > mkdir build
 > cd build
 > cmake -G "Visual Studio 15 2017 Win64" ..
```
Or use `./setup_builds.bat`. 

[Project Website](https://slongle.github.io/projects/rainbow)

![homogeneous](https://i.postimg.cc/ryHf24pD/0002.png)  
Homogeneous Medium(BDPT WIP)  
![heterogeneous](https://i.postimg.cc/9MJN7NVb/0001.png)  
Heterogeneous Medium  
Rendered using volumetric pathtracing.

# mms_master_msgs
To use these, add `mms_master_msgs` to your CMakeLists.txt and package.xml

```xml
<depend>mms_master_msgs</depend>
```

```cmake
find_package(catkin REQUIRED COMPONENTS 
    .... # Just add the following line
    mms_master_msgs
)
```

And also add the message generation step as a dependency 
```cmake
add_dependencies(TARGET mms_master_msgs_generate_messages_cpp)
```

Make sure this is _**after**_ `target_link_libraries`.
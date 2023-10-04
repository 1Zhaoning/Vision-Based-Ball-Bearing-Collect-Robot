# botbot_common

This implements code that is shared across multiple nodes. It also provides documentation for writing and executing different tests. 

All code here is placed under the `mms` namespace. No other code (such as application / node code) should add anything to this namespace.

*There are currently many places where this rule is broken, and will be rectified in the future*.

## How do I use this in my node?
Add:
```
find_package(catkin REQUIRED COMPONENTS
  botbot_common
)
```
To the "CMakeLists.txt" file, and 
```
<depend>botbot_common</depend>
```
to the package.xml file.

Then you can use `#include "botbot_common/some_header.hpp"`

## Current modules:

**For more detailed documentation please refer to the doxygen documentation**

### mms::VectorXY
A vector class based upon Eigen, for the fixed size [2x1] vector. In addition to the many [Eigen methods available](https://eigen.tuxfamily.org/dox/group__QuickRefPage.html), it also extends the capabilites by adding functions such as:

 - in_polygon
 - find_closest
 - add_XY_noise
 - perpendicular_projection
 - parallel_projection
 - rotate
 - angle_diff

You can acces the `x` and `y` members by calling the functions `.x()` and `.y()`, in order to modify the underlying data you must use `(0)` and `(1)`. In general, prefer to do opertations with vectors rather than assigning the x and y individually.

For the full list of functionality, please refer to the linked Eigen reference to veiw inherited functions. To see additional capabilities of the class generate the doxygen documentation. 


### mms::VehicleParams
Contains intrinsic vehicle dimensions such as mass, COG distances and sensor offset etc...

### mms::MathUtils
Some basic mathematical utility functions such as gaussian noise and converting degrees to radians

### TicToc (mms::profiling)
This is a profiling timer interface. 

Call `TIC("helpful description")` to start a timer and `TOC("helpful description")` to stop it.

Calling `LATENCY_LOG("helpful description", ros::Time)`, will calculate and log the difference between the given time and now.

Calling `FPS_LOG("helpful description")`, will determine (and log) the rate at which it is called at. eg: This will give you the actual rate your function is being called at, rather than guessing it from `1/latency`. <br>
*This can sometimes give a different rate than what `rostopic hz` will, it is not really clear why.*

To see a summary of the logged data, call  `TIC_TOC_SUMMARY("my_node_name")`



### mms::SignalFilter
For creating and applying low pass, high pass or band pass filters to a time domain signal.
This class make use of the boost circular buffer, so you may need to add:
```
find_package(Boost REQUIRED COMPONENTS system)
include_directories( ${Boost_INCLUDE_DIRS} )
```
to your CMakeLists.txt file.

### mms::ThreadPool

A class for creating a threadpool of a given size, with a queue of functions for the workers to run.


### mms::CsvWriter
A low latency csv writing class that pushes io and formatting to another thread. By default the filename will be the current date and time.

```c++
mms::CsvWriter<int, int, std::optional<double>> writer{{"a", "b", "c"}, "%Y.%m.%d_%H.%M_test.csv"};
writer.log_row(1, 5, {});
```
```c++
mms::CsvVectorWriter<int, 2> writer{{"a", "b"}};
writer.log_row(1, 2);
writer.log_vector_row(std::vector{1, 2});
```

# Unit Testing
Unit tests are small tests that only operate on a single function or class. When writing unit tests some example functionality to test is checking correctness when:

 - the inputs are really small (is there a divide by zero error?)
 - inputs are really big
 - angles are not wrapped
 - etc ...

We use [**google-test**](https://github.com/google/googletest/blob/master/googletest/docs/primer.md) as the framework for our unit tests. 
Unit tests should be in a file called *unit_tests.cpp* and be located at: `<your_node>/test/unit_tests.cpp`.

The *Hello World* equivalent of unit tests is:
```c++
#include <gtest/gtest.h>

TEST(TestSetName test_name_1)
{
    ASSERT_TRUE(true);
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

There are many other types of test assertions other than `ASSERT_TRUE`, which you can view in the google documentation linked above.
The docs also give some advice about how to write good unit tests. For how to compile and run the unit tests, refer to the *CMake Modules* section below.

If you call `catkin <xxx>_test` in the command line, you should see the results of the unit tests printed in the terminal after they compile.


# CMake Modules
In order to unify the commpile commands, optimzation options, compiler warnings and interface, there are several CMake modules which will simpy handle all this for you. They will also optionally link eigen (automatically aware of CPU type) and build and run unit tests if they exist. In order to use these modules you need to include some lines in your **CMakeLists.txt** file.

```cmake
# Determine path to botbot_common to include files
execute_process(COMMAND rospack find botbot_common
    OUTPUT_VARIABLE MMS_CMAKE_DIR
)
string(STRIP ${MMS_CMAKE_DIR} MMS_CMAKE_DIR)
string(APPEND MMS_CMAKE_DIR "/cmake")

include(${MMS_CMAKE_DIR}/project_settings.cmake)

include_eigen() # optional

.
. Find packages and declare executable
.

link_eigen(${TARGET_NAME}) # optional

# expects test source code to be in test/unit_tests.cpp
add_unit_tests(${LIBRARY or EXE Target Name})
```

In order to run all the different compile modes, there are certain flags you need type when calling `catkin build`, however this is somewhat annoying so I have created some shorcuts that handle this for you:
```bash
catkin b # catkin build, release with debug mode
caktin bt_rel #catkin build this in release mode
catkin bto_deb_test # catkin build this only (no dependencies) in debug mode and run tests

# There are also variours combinations of the above commands
```
These can be installed through the *install_catkin_verbs.sh* script.

# Clang How-To
Install the clang ecosystem:
```
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
sudo add-apt-repository 'deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic main'
sudo apt update
sudo apt install -y clangd clang-format clang-tidy
```

Install the **vscode-clangd** extension, and don't forget to disable your current linter.

In order for clang to correctly parse your included files, it needs a list of compile commands that are used to build the project. The easiest way to obtain this to use the standard cmake modules system from here, which will handle it automatically. Alternatively: 
```
catkin config --cmake-args -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```
before you `catkin build`, and then create a symlink in `your_node/` that points to `catkin_ws/build/your_node/compile_commands.json`.

eg, whilst in `.../YOUR_NODE/` :

```
ln -s ${HOME}/catkin_ws/build/YOUR_NODE/compile_commands.json .
ln -s ${HOME}/catkin_ws/src/mms_master/botbot_common/.clang-format .
ln -s ${HOME}/catkin_ws/src/mms_master/botbot_common/.clang-tidy .
```

Create symlinks in `your_node/` that point to the `.clang-tidy` and `.clang-format` files here.

The easiest way to do this is to simply run the script: ```scripts/clang_links.sh```

Don't forget to add `compile_commands.json` to your `.gitignore`

**clang-tidy** will run better if you add the following commands the settings of the vscode extension:

 - --suggest-missing-includes
 - --background-index
 - -j=4

# FlexLog
Flexible and Extensible Logging Utility for C Applications

### Preliminary
Logging is essential for all applications. Unlike simple `printf` approaches, a good logging utility provides the same level of easiness, yet output more useful information with less codes involved.

FlexLog sits in a middle ground between other logging utilities. It is small and simple like most simple logging tools, while also being flexible and extensible like some larger logging suites. FlexLog will satisfy most programming demands, and leaves a bit of extras for those who aren't 100% satisfied.


### Integration

#### Standalone
You can include FlexLog directly into your projects. It is recommended that you copy all the files under the `src` folder to a say `flexlog` folder under your project directory. Then, integrating FlexLog is as simple as adding a line in your codes:

``` c
#include "flexlog/log.h"
```

#### Shared library
You can also compile FlexLog to a shared library and use it among many projects.

``` bash
make lib
sudo make install
```

By default, the header files will be located under the `/usr/include/flexlog/` folder. You will be able to include FlexLog in the following form:

``` c
#include <flexlog/log.h>
```

Remember to append `-lflexlog` option to your link command, so that the shared library can be linked.


### Usage

FlexLog prints logs to the console by default. After you include the headers, FlexLog is ready. There's no need to make extra function calls if you are satisfied with the default settings.

#### Logging

There are currently 6 function-like macros for logging. They are pretty straight-forward and can be used just like `printf`:

|    Macros    | Descriptions                                                                                |
|--------------|---------------------------------------------------------------------------------------------|
| `FATAL(...)` | Fatal events. Catastrophic errors, the program will exit.                                   |
| `ERROR(...)` | Error events. The program is unable to complete the current task.                           |
| `WARN(...)`  | Warning events. Inform user against special behaviors or some minor errors with workaround. |
| `INFO(...)`  | Informative events. Routine outputs.                                                        |
| `DEBUG(...)` | Debug events. Diagnostics outputs. Informations that can't be shown too frequently.         |
| `TRACE(...)` | Trace events. Verbose outputs. Provide debugging info for rare issues.                      |

#### Change log time format

By default, FlexLog outputs a monotonic timestamp. Monotonic timestamp is better for debugging purposes, because it is shorter, more accurate, and does not change with NTP updates. You can change the log time format to output wall clock time by updating the following line in `LogTimeToStr()`:

``` c
static clockid_t log_clock = CLOCK_REALTIME;
```

#### Change the maximum level of log types to print

By default, FlexLog have `LOG_TYPE_INFO` as the maximum level of printable log type. This can be changed by calling:

``` c
SetLogLevel(LOGGER_CONSOLE, LOG_TYPE_TRACE);
```


### License

FlexLog is distributed with MIT License.

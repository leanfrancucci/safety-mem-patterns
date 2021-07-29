# Safety memory patterns
This repository contains the source code and unit test cases explainded in 
the article [Protecting your embedded software against memory corruption](x).

[Config.alt1/](Config.alt1), [Config.alt2/](Config.alt2) and 
[Config.recovery/](Config.recovery) directories are three alternatives to 
implement the module `Config` according to the proposed method.
Config.alt1 and Config.alt2 are similar but the last one does not check the 
data set stored in RAM every time a configuration option is accessed by set 
and get functions, whereas the alternative Config.recovery is derived from 
Config.alt2 but includes the recovery mechanism.
Each of these directories are arranged in four sub-directories, `inc/`, `src/`, 
`test/` and `build/`. The directories inc/ and src/ contain the header and 
source code files, whereas the directory `test/` the unit test cases that were 
implemented using [Ceedling](https://github.com/ThrowTheSwitch/Ceedling), 
[Unity](https://github.com/ThrowTheSwitch/Unity) and 
[Cmock](https://github.com/ThrowTheSwitch/CMock) tools.

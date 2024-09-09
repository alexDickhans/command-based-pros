# Installation

Download the zip file from the [most recent release](https://github.com/alexDickhans/command-based-pros/releases/latest). 

Then fetch the libcommand zip

```bash
pros c fetch libcommand@VERSION.zip
```

Apply the libcommand zip file to your project

```bash
cd YOUR_PROJECT_BASE
pros c apply libcommand
```

Add includes to your projects `main.h`

```c++
#include "command/includes.h"
```

You're all set, take a look at the [Annotated Intake Example](intakeExample.md) next!

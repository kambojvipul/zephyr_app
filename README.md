The Firmware Take Home Project:

The file architecture is same as required. There are two more files below in boards folder:

1) Devicetree Overlays: xiao_esp32c3.overlay
    -All device nodes are mentioned in above overlay file.

2) Board Specific Conf File: xiao_esp32c3.conf
    - All configs for the project is in above file, proj.conf is empty.

Main Features of the Project:

1) ADC Read: 
    -Read Analog voltage on Pin GPIO2 ADC Channel 2: Done
    -Chnage in analog Voltage at Pin is tested and readable: Done 
    -The k_work_delayable thread is used to execute the task: Done
    -The sample interval time is configurable by customized Node App: Done

2) Ble :
    -The ble is advertise successfully with the named FW-Challenge: Done
    -Customized 128 bits UUID are created for both characterstics with service: Donel
    -It transmits the two characterstics: voltage_mv (readable, notifiable) and sample_interval_ms (readable): Done
    -Notify the voltage_mv at regulat interval i.e. sample_interval_ms: Done
    -The k_work_delayable thread is used to execute the task of Notify: Done
    -COnnection, Disconnection, Notification Enable/Disable Callback: Done

3) Button:
    - The button is enabled using Device Tree in Interrupt Mode: Done
    - The button is configured as Push Button with Software Debounce Code: Done
    - On button press, it turns ON/OFF (Toggle) the BLE Notification of voltage_mv: Done

4) PWD Led:
    - The PWM Led is configured using device tree with Pinctrl Node : Done
    - The PWM is used to lower the brightness of the Led: Done
    - The Led is designed as in 
        A) ADC_Idle_Mode: Slow Bink: Done
        B) ADC_Sampling_Mode: Brief Double Blink: Done
        C) ADC_ERROR_Mode: Rapi Blink: Done

    - The Preemtive Thread is used for above function with High Priority: Done

5) Nomral Led:
    - The Normal Led is configured same as above with PWM.

6) Custom App Node:
    - The custom App Node with DT_PROP, COMPAT is created in Overlay as required: Done
    - The Read of the Properties is successful in main.c: Done
    - The property i.e. sample-interval-ms is used for below threads as loop interval:  
        -Call ADC Thread regularly: Done
        -Call BLE Notification in loop : Done
        -Transmit through BLE CHaracterstics : Done
    - Hence, the changing the app properties will cause the change in above threads
    like change in loop interval, transit via ble: Done

7) Customized Kconfig:
    - The customized Kconfig () is created as desired with yaml file: Done
    - Displayable in Menuconfig and configurable: Done
    - FW Challenge Options are added in the bottom of main menu.
    - Access in main.c and pring using LOG_INF: Done
    Note: Only Custom App Node in Overlay will change the threads like sample_interval, enable_ble.

    Comparison: Kconfig file is more flexible as configuration can be changed in menuconfig, while
    chaning config in Customized App Node, syntax and format has to taken cared.

8) Logging:
    - LOG_INF is used of normal logging and LOG_ERR for errorL: Done

9) The program is error free, buildable and running on Esp32 Borad. The demonstration video of running project on youtube is as follows:

    https://youtu.be/8TXV3R41snU
   
11) The commands are as follows:

  

        Board: xiao_esp32c3

        Note: the path of project in my computer as follows: /home/vipul/zephyrproject/zephyr/apps/zephyr_app

        0) Go to Main Zephyr Folder and activate the virtual environment created during installation:
        cd /home/vipul/zephyrproject/zephyr
        source ~/zephyrproject/.venv/bin/activate

        1) Run Menuconfig: to run customized Kconfig
        west build -p always -b xiao_esp32c3 --sysbuild apps/zephyr_app -t menuconfig -- -DCONF_FILE="boards/xiao_esp32c3.conf"

        2) build the project: (Kconfig will be lost)
        west build -p always -b xiao_esp32c3 --sysbuild apps/zephyr_app -- -DCONF_FILE="boards/xiao_esp32c3.conf"
        
        2A) Without build clean (it restores the Kconfig)
        west build -b xiao_esp32c3 --sysbuild apps/zephyr_app -- -DCONF_FILE="boards/xiao_esp32c3.conf"

        3) Provide Permisson to /dev/ttyACM0 to access esp32c3
        sudo chmod 666 /dev/ttyACM0

        4) Flash the project
        west flash

        5) Monitor and Logging
        west espressif monitor


Note: For the development of the project, the help is taken as below: 
    -Digikey Zephyr Tutorial, Nordic Zephyr Tutorial, Sample Code by Zephyr
    -AI is used to troubleshoot and understand the error.
    -Installation of the Zephyr is done as detalied on its website on my linux computer 
     and run under virtual environment:
        -https://docs.zephyrproject.org/latest/develop/getting_started/index.html



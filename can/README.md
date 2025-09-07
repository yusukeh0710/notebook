# Can

### Setup a virtual can device for loopback connection

* Install packages
  ```bash
  sudo apt install -y can-utils
  ```

* [Optional] Create a loopback device if there is no connection to physical CAN devices  

  ```bash
  ./activate_vcan.sh vcan0
  ```

* Try to listen CAN connection (Terminal#1)
  ```bash
  candump vcan0
  ```

* Try to send CAN signals (Teriminal#2)
  ```bash
  cansend vcan0 123#DEADBEEF
  ```

Then, if you can see the input in the Terminal#1, the setup is completed.



### Try sample code

* Build the code
  (Classic CAN)

  ```
  cd sample
  mkdir build
  cd build
  cmake ../
  make 
  ```

  (CAN FD)
  ```
  cd sample
  mkdir build
  cd build
  cmake -DUSE_CANFD=ON ../
  make 
  ```

* Try to receive (Terminal#1)
  ```
  ./can_receiver [device_name]
  ```

  ※ The default device name: vcan0
  
* Try to send (Terminal#2)
  ```
  ./can_send [device_name] CHANNEL_ID DATA
  ```

  

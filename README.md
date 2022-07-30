# SM3_Attack
Group members: 刘天骏 202000460002

Introduction: Some simple attack of sm3 and Merkle Tree

Accomplished project: 

                      1. impl the naive birthaday atteck of reduced sm3 (32-bits)     //sm2_test()
                      
![image](https://user-images.githubusercontent.com/87689532/181863253-0b337b1c-61ea-4d30-9abe-39389f66bf5a.png)
                      
                      2. impl the rho method of reduced sm3 (48-bits)

The size of digest can be extend to 64-bits, which will take much longer time to find a collision.

![image](https://user-images.githubusercontent.com/87689532/181867219-7e7478aa-d26a-460d-ad48-748e0ab1ed92.png)

                      3. impl length extension attack for sm3
                      
![image](https://user-images.githubusercontent.com/87689532/181867400-fd0988c9-5eee-4fc3-9fae-c4f3132dd310.png)

(The same hash result)

                     4. impl Merkle Tree following RFC6962

![image](https://user-images.githubusercontent.com/87689532/181867546-c1187460-f746-474b-b563-00e2d6867b5d.png)

(The hash result of 23 random array of char)

Have trouble dealing with real network communication, several other projects are unfinished.

To run above codes, you'll need WinNTL-11_5_1 functions.

Be aware of "//" sign to run all functions.

All codes are wrtien by myself.

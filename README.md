# BMP_Secret_Word

### 簡介

將秘密檔案儲存至bmp圖檔中，將檔案位元存在bmp中的RGB位元中，RGB資訊各存8bits共24bits，使用者可選擇要存在BMP中的這各8bits的後1~7bits，並且儲存的位元會由Secret Data 從MSB轉存至LSB(如下圖)。

若儲存的訊息超過bmp圖檔的能儲存的範圍則會出現error 錯誤訊息。

<img src="C:\Users\grace\Desktop\剪貼簿-7.jpg" alt="剪貼簿-7" style="zoom:70%;" />



### Code 執行方式 

```
$ make
```

./img_hide [option_1]  [option_2]  [N] [the bytes of secret_data] [cover_bmp] [secret_data]

option_1:

-w,  Write the secret data to the cover_bmp

-e, Extract the secret data from the cover_bmp to the secret_data

option_2:

-b, bits==N, use last N bits. N is form 1 to 8

> -b  (佔 bmp檔中 bgr的後幾個位元 (1~7不得超過 7)) (secret_data總 byte數 )

範例 :

+ 將secret data寫進 bmp檔案中

  ```
  $ ./img_hide -w -b 2 10 b.bmp b.txt
  ```

+ 將secret data從bmp 中拿出

  ```
  $ ./img_hide -e -b 2 10 b.bmp d.txt
  ```

